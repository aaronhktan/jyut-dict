//
//  SQLSearch.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
import GRDB
import SwiftUI
import os

actor SQLSearch {
  private var pool: DatabasePool

  private var jyutpingCache: (sourceHash: Int, cache: [String])? = nil
  private var pinyinCache: (sourceHash: Int, cache: [String])? = nil

  public init(pool: DatabasePool) {
    self.pool = pool
  }

  private func populateJyutpingCache() async -> Result<[String], Error> {
    logger.info("Populating jyutping cache...")
    do {
      let results = try await pool.read { db in
        return try String.fetchAll(db, sql: fetchAllJyutpingQuery)
      }
      logger.info("Populated jyutping cache")
      return .success(results)
    } catch {
      logger.error("Failed to populate jyutping cache, error: \(error)")
      return .failure(error)
    }
  }

  private func populatePinyinCache() async -> Result<[String], Error> {
    logger.info("Populating pinyin cache")
    do {
      let results = try await pool.read { db in
        return try String.fetchAll(db, sql: fetchAllPinyinQuery)
      }
      logger.info("Populated pinyin cache")
      return .success(results)
    } catch {
      logger.error("Failed to populate pinyin cache, error: \(error)")
      return .failure(error)
    }
  }

  private func jyutpingFilteredBy(prefix: String) async -> [String] {
    do {
      return try await pool.read { db in
        try String.fetchAll(
          db,
          sql: fetchJyutpingWithPrefixQuery,
          arguments: ["\(prefix)*"]
        )
      }
    } catch {
      logger.error("Failed to fetch jyutping with prefix, error: \(error)")
    }
    return []
  }

  private func pinyinFilteredBy(prefix: String) async -> [String] {
    do {
      return try await pool.read { db in
        try String.fetchAll(
          db,
          sql: fetchPinyinWithPrefixQuery,
          arguments: ["\(prefix)*"]
        )
      }
    } catch {
      logger.error("Failed to fetch pinyin with prefix, error: \(error)")
    }
    return []
  }

  private func allJyutping() async -> [String] {
    let searcher = SQLSourceMetadata(pool: pool)
    let sources = await searcher.getSources()
    let hash = sources.reduce(into: Hasher()) { $0.combine($1) }.finalize()
    if let jyutpingCache = self.jyutpingCache {
      if hash != jyutpingCache.sourceHash {
        // The only time the list of all Jyutping changes is when a source
        // is added or removed.
        if case let .success(cache) = await populateJyutpingCache() {
          self.jyutpingCache = (hash, cache)
        }
      }
    } else {
      if case let .success(cache) = await populateJyutpingCache() {
        self.jyutpingCache = (hash, cache)
      }
    }
    return self.jyutpingCache?.cache ?? []
  }

  private func allPinyin() async -> [String] {
    let searcher = SQLSourceMetadata(pool: pool)
    let sources = await searcher.getSources()
    let hash = sources.reduce(into: Hasher()) { $0.combine($1) }.finalize()
    if let pinyinCache = self.pinyinCache {
      if hash != pinyinCache.sourceHash {
        if case let .success(cache) = await populatePinyinCache() {
          self.pinyinCache = (hash, cache)
        }
      }
    } else {
      if case let .success(cache) = await populatePinyinCache() {
        self.pinyinCache = (hash, cache)
      }
    }
    return self.pinyinCache?.cache ?? []
  }

  nonisolated private func literalPrefix(ofRegex pattern: String) -> String {
    var prefix = ""
    for character in pattern {
      guard character.isLetter || character.isNumber else {
        break
      }
      prefix.append(character)
    }

    guard let next = pattern.dropFirst(prefix.count).first else {
      return prefix
    }

    switch next {
    case "?", "*":
      prefix.removeLast()
    default:
      break
    }
    return prefix
  }

  @concurrent func searchByRowId(rowid: String) async -> Entry? {
    var result: Entry?
    do {
      result = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: getEntryByRowId,
          arguments: [rowid]
        )
        return parseReturnedEntryRecords(rows: rows)[0]
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return result
  }

  @concurrent func searchTraditional(searchTerm: String) async -> [Entry] {
    let pattern = prepareCharacterBindValues(
      characters: searchTerm
    )
    logger.info("searchTraditional pattern: '\(pattern)'")

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchTraditionalQuery,
          arguments: [pattern]
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchSimplified(searchTerm: String) async -> [Entry] {
    let pattern = prepareCharacterBindValues(
      characters: searchTerm
    )
    logger.info("searchSimplified pattern: '\(pattern)'")

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchSimplifiedQuery,
          arguments: [pattern]
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchJyutpingExistence(
    searchTerm: String, useFuzzyJyutping: Bool, unsafeFuzzyJyutping: Bool
  ) async -> Bool {
    let pattern = prepareJyutpingBindValues(
      jyutping: searchTerm,
      useFuzzyJyutping: useFuzzyJyutping,
      unsafeFuzzyJyutping: unsafeFuzzyJyutping
    )
    logger.info("searchJyutpingExistence pattern: '\(pattern)', useFuzzyJyutping: \(useFuzzyJyutping)")

    if !useFuzzyJyutping {
      let query = String(
        format: jyutpingExistenceQuery,
        arguments: [GLOB_OPERATOR]
      )

      var result: Bool = false
      do {
        result = try await pool.read { db in
          let rows = try Row.fetchAll(
            db,
            sql: query,
            arguments: [pattern]
          )
          return parseExistenceRecords(rows: rows)
        }
      } catch {
        logger.error("Error happened when trying to read from db: \(error)")
      }

      logger.info("glob Jyutping match result: \(result)")
      return result
    }

    guard let regex = try? NSRegularExpression(pattern: pattern) else {
      logger.error("Failed to compile regex in Jyutping existence check")
      return false
    }
    let prefix = literalPrefix(ofRegex: pattern)
    logger.info("prefix: \(prefix)")
    var candidates: [String]
    if prefix.isEmpty {
      candidates = await allJyutping()
    } else {
      candidates = await jyutpingFilteredBy(prefix: prefix)
    }
    for text in candidates {
      // If this is updated, you will likely want to update the implementation
      // in DatabaseManager!
      let range = NSRange(location: 0, length: text.utf16.count)
      if regex.rangeOfFirstMatch(
        in: text,
        options: [.anchored],
        range: range
      ).location != NSNotFound {
        logger.info("Found fuzzy Jyutping match for pattern \(pattern): \(text)")
        return true
      }
    }
    logger.info("Failed to fuzzy Jyutping match for pattern \(pattern)")
    return false
  }

  @concurrent func searchJyutping(
    searchTerm: String, useFuzzyJyutping: Bool, unsafeFuzzyJyutping: Bool
  ) async
    -> [Entry]
  {
    let pattern = prepareJyutpingBindValues(
      jyutping: searchTerm,
      useFuzzyJyutping: useFuzzyJyutping,
      unsafeFuzzyJyutping: unsafeFuzzyJyutping
    )
    logger.info("searchJyutping pattern: '\(pattern)'")
    let query = String(
      format: searchJyutpingQuery,
      arguments: [useFuzzyJyutping ? REGEXP_OPERATOR : GLOB_OPERATOR]
    )

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: query,
          arguments: [pattern]
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchPinyinExistence(searchTerm: String, useFuzzyPinyin: Bool) async -> Bool {
    let pattern = preparePinyinBindValues(
      pinyin: searchTerm,
      useFuzzyPinyin: useFuzzyPinyin
    )
    logger.info("searchPinyinExistence pattern: '\(pattern)'")

    if !useFuzzyPinyin {
      let query = String(
        format: pinyinExistenceQuery,
        arguments: [GLOB_OPERATOR]
      )

      var result: Bool = false
      do {
        result = try await pool.read { db in
          let rows = try Row.fetchAll(
            db,
            sql: query,
            arguments: [pattern]
          )
          return parseExistenceRecords(rows: rows)
        }
      } catch {
        // TODO: Handle errors
        logger.error("Error happened when trying to read from db: \(error)")
      }

      return result
    }

    let prefix = literalPrefix(ofRegex: pattern)
    guard let regex = try? NSRegularExpression(pattern: pattern) else {
      logger.error("Failed to compile regex in pinyin existence check")
      return false
    }
    var candidates: [String]
    if prefix.isEmpty {
      candidates = await allPinyin()
    } else {
      candidates = await pinyinFilteredBy(prefix: prefix)
    }
    for text in candidates {
      let range = NSRange(location: 0, length: text.utf16.count)
      if regex.rangeOfFirstMatch(
        in: text,
        options: [.anchored],
        range: range
      ).location != NSNotFound {
        logger.info("Found fuzzy Pinyin match for pattern \(pattern): \(text)")
        return true
      }
    }
    logger.info("Failed to fuzzy Pinyin match for pattern \(pattern) in \(candidates.count) candidates")
    return false
  }

  @concurrent func searchPinyin(searchTerm: String, useFuzzyPinyin: Bool) async -> [Entry] {
    let pattern = preparePinyinBindValues(
      pinyin: searchTerm,
      useFuzzyPinyin: useFuzzyPinyin
    )
    logger.info("searchPinyin pattern: '\(pattern)'")
    let query = String(
      format: searchPinyinQuery,
      arguments: [useFuzzyPinyin ? REGEXP_OPERATOR : GLOB_OPERATOR]
    )

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: query,
          arguments: [pattern]
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchEnglish(searchTerm: String) async -> [Entry] {
    let (ftsParam, likeParam) = prepareEnglishBindValues(english: searchTerm)
    logger.info("searchEnglish patterns: '\(ftsParam)', '\(likeParam)'")
    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchEnglishQuery,
          arguments: [ftsParam, likeParam]
        )
        return parseReturnedEntryRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchExamplesByTraditional(searchTerm: String) async -> [Example] {
    var results: [Example] = []
    let likeTerm = "%\(searchTerm)%"
    logger.info("searchExamplesByTraditional pattern: '\(likeTerm)'")

    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchExamplesQuery,
          arguments: [likeTerm]
        )
        return parseReturnedExampleRecords(rows: rows)
      }
    } catch {
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }
}
