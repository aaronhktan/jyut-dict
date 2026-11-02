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
    logger.info("Populating Jyutping cache...")
    do {
      let results = try await pool.read { db in
        return try String.fetchAll(db, sql: fetchAllJyutpingQuery)
      }
      logger.info("Populated Jyutping cache")
      return .success(results)
    } catch {
      logger.error("Failed to populate Jyutping cache, error: \(error)")
      return .failure(error)
    }
  }

  private func populatePinyinCache() async -> Result<[String], Error> {
    logger.info("Populating Pinyin cache")
    do {
      let results = try await pool.read { db in
        return try String.fetchAll(db, sql: fetchAllPinyinQuery)
      }
      logger.info("Populated Pinyin cache")
      return .success(results)
    } catch {
      logger.error("Failed to populate Pinyin cache, error: \(error)")
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
      logger.error("Failed to fetch Jyutping with prefix, error: \(error)")
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
      logger.error("Failed to fetch Pinyin with prefix, error: \(error)")
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
        if case .success(let cache) = await populateJyutpingCache() {
          self.jyutpingCache = (hash, cache)
        }
      }
    } else {
      if case .success(let cache) = await populateJyutpingCache() {
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
        if case .success(let cache) = await populatePinyinCache() {
          self.pinyinCache = (hash, cache)
        }
      }
    } else {
      if case .success(let cache) = await populatePinyinCache() {
        self.pinyinCache = (hash, cache)
      }
    }
    return self.pinyinCache?.cache ?? []
  }

  private func allRowIds() async -> [Int] {
    // This is called rarely enough that it's not necessary to cache.
    do {
      let results = try await pool.read { db in
        return try Int.fetchAll(db, sql: fetchAllRowId)
      }
      return results
    } catch {
      logger.error("Failed to fetch all rowIds, error: \(error)")
    }
    return []
  }

  private func rowIdFilteredByJyutping(prefix: String) async -> [Int] {
    do {
      return try await pool.read { db in
        try Int.fetchAll(
          db,
          sql: fetchRowIdWithJyutpingPrefixQuery,
          arguments: ["\(prefix)*"]
        )
      }
    } catch {
      logger.error("Failed to fetch rowIds with prefix Jyutping, error: \(error)")
    }
    return []
  }

  private func rowIdFilteredByPinyin(prefix: String) async -> [Int] {
    do {
      return try await pool.read { db in
        try Int.fetchAll(
          db,
          sql: fetchRowIdWithPinyinPrefixQuery,
          arguments: ["\(prefix)*"]
        )
      }
    } catch {
      logger.error("Failed to fetch rowIds with prefix Pinyin, error: \(error)")
    }
    return []
  }

  nonisolated private func literalPrefixes(of pattern: String) -> [String] {
    // Truncate string to right before a second "?" or "(", whichever occurs
    // earlier. (The limitation of unfoldBasicRegex is that we only support
    // one set of parentheses and one question mark.)
    var endIdx = pattern.endIndex
    if let first = pattern.firstIndex(of: "(") {
      let afterFirst = pattern.index(after: first)
      if let second = pattern[afterFirst...].firstIndex(of: "(") {
        endIdx = second
      }
    }
    if let first = pattern.firstIndex(of: "?") {
      let afterFirst = pattern.index(after: first)
      if let second = pattern[afterFirst...].firstIndex(of: "?"), second < endIdx {
        endIdx = second
      }
    }

    let possibilities = unfoldBasicRegex(for: String(pattern[..<endIdx]))

    var prefixes: Set<String> = []
    for p in possibilities {
      var prefix = ""
      for character in p {
        guard character.isLetter || character.isNumber else {
          break
        }
        prefix.append(character)
      }

      guard let next = pattern.dropFirst(prefix.count).first else {
        if !prefix.isEmpty {
          prefixes.insert(prefix)
        }
        continue
      }

      switch next {
      case "?", "*":
        prefix.removeLast()
      default:
        break
      }

      if !prefix.isEmpty {
        prefixes.insert(prefix)
      }
    }

    return Array(prefixes)
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
    logger.info(
      "searchJyutpingExistence pattern: '\(pattern)', useFuzzyJyutping: \(useFuzzyJyutping)")

    guard useFuzzyJyutping else {
      var result = false
      do {
        result = try await pool.read { db in
          let rows = try Row.fetchAll(
            db,
            sql: jyutpingExistenceQuery,
            arguments: [pattern]
          )
          return parseExistenceRecords(rows: rows)
        }
      } catch {
        logger.error("Error happened when trying to read from db: \(error)")
      }

      logger.info("Found non-fuzzy Jyutping match: \(result)")
      return result
    }

    // General idea: use GLOB to filter out as many rows as possible before
    // running REGEXP on those rows. GLOB can take advantage of SQLITE's
    // indices, whereas REGEXP, being a user-defined function, can't.
    guard let regex = try? NSRegularExpression(pattern: pattern) else {
      logger.error("Failed to compile regex in Jyutping existence check")
      return false
    }
    // GLOB can't handle the parentheses that we use in the regex patterns,
    // so unfold them as much as possible.
    let prefixes = literalPrefixes(of: pattern)
    logger.info("Jyutping prefixes for pattern \(pattern): \(prefixes)")
    var candidates: [String] = []
    if prefixes.isEmpty {
      candidates = await allJyutping()
    } else {
      for p in prefixes {
        candidates += await jyutpingFilteredBy(prefix: p)
      }
    }
    guard !Task.isCancelled else {
      return false
    }
    for (index, text) in candidates.enumerated() {
      if index % 25000 == 0 {
        guard !Task.isCancelled else {
          return false
        }
      }
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

    var results: [Entry] = []
    guard useFuzzyJyutping else {
      do {
        results = try await pool.read { db in
          let rows = try Row.fetchAll(
            db,
            sql: searchJyutpingGlobQuery,
            arguments: [pattern]
          )
          return parseReturnedEntryRecords(rows: rows)
        }
      } catch {
        logger.error("Error happened when trying to read from db: \(error)")
      }

      return results
    }

    let prefixes = literalPrefixes(of: pattern)
    var candidates: [Int] = []
    if prefixes.isEmpty {
      candidates = await allRowIds()
    } else {
      for p in prefixes {
        candidates += await rowIdFilteredByJyutping(prefix: p)
      }
    }

    // Using JSON lets us get around the bound parameter limit in SQLITE:
    // https://sqlite.org/c3ref/c_limit_attached.html#sqlitelimitvariablenumber
    guard let data = try? JSONEncoder().encode(candidates),
      let json = String(data: data, encoding: .utf8)
    else {
      logger.error("Couldn't turn candidates into JSON!")
      return []
    }

    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchJyutpingRegexQuery,
          arguments: [json, pattern]
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
    logger.info("searchPinyinExistence pattern: '\(pattern)', useFuzzyPinyin: \(useFuzzyPinyin)")

    guard useFuzzyPinyin else {
      let query = String(
        format: pinyinExistenceQuery,
        arguments: [GLOB_OPERATOR]
      )

      var result = false
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

      return result
    }

    guard let regex = try? NSRegularExpression(pattern: pattern) else {
      logger.error("Failed to compile regex in Pinyin existence check")
      return false
    }
    let prefixes = literalPrefixes(of: pattern)
    var candidates: [String] = []
    if prefixes.isEmpty {
      candidates = await allPinyin()
    } else {
      for p in prefixes {
        candidates += await pinyinFilteredBy(prefix: p)
      }
    }
    guard !Task.isCancelled else {
      return false
    }
    for (index, text) in candidates.enumerated() {
      if index % 25000 == 0 {
        guard !Task.isCancelled else {
          return false
        }
      }
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
    logger.info(
      "Failed to fuzzy Pinyin match for pattern \(pattern) in \(candidates.count) candidates")
    return false
  }

  @concurrent func searchPinyin(searchTerm: String, useFuzzyPinyin: Bool) async -> [Entry] {
    let pattern = preparePinyinBindValues(
      pinyin: searchTerm,
      useFuzzyPinyin: useFuzzyPinyin
    )
    logger.info("searchPinyin pattern: '\(pattern)'")

    guard useFuzzyPinyin else {
      var results: [Entry] = []
      do {
        results = try await pool.read { db in
          let rows = try Row.fetchAll(
            db,
            sql: searchPinyinGlobQuery,
            arguments: [pattern]
          )
          return parseReturnedEntryRecords(rows: rows)
        }
      } catch {
        logger.error("Error happened when trying to read from db: \(error)")
      }
      return results
    }

    let prefixes = literalPrefixes(of: pattern)
    logger.info("Pinyin prefixes: '\(prefixes)'")
    var candidates: [Int] = []
    if prefixes.isEmpty {
      candidates = await allRowIds()
    } else {
      for p in prefixes {
        candidates += await rowIdFilteredByPinyin(prefix: p)
      }
    }

    guard let data = try? JSONEncoder().encode(candidates),
      let json = String(data: data, encoding: .utf8)
    else {
      logger.error("Couldn't turn candidates into JSON!")
      return []
    }

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchPinyinRegexQuery,
          arguments: [json, pattern]
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
