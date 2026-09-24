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

  public init(pool: DatabasePool) {
    self.pool = pool
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
        return parseReturnedRecords(rows: rows)[0]
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db")
    }

    return result
  }

  @concurrent func searchTraditional(searchTerm: String) async -> [Entry] {
    let globTerm = prepareCharacterBindValues(
      characters: searchTerm
    )
    print("globTerm: '\(globTerm)'")

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchTraditionalQuery,
          arguments: [globTerm]
        )
        return parseReturnedRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db")
    }

    return results
  }

  @concurrent func searchSimplified(searchTerm: String) async -> [Entry] {
    let globTerm = prepareCharacterBindValues(
      characters: searchTerm
    )
    print("globTerm: '\(globTerm)'")

    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchSimplifiedQuery,
          arguments: [globTerm]
        )
        return parseReturnedRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db")
    }

    return results
  }

  @concurrent func searchJyutpingExistence(searchTerm: String, useFuzzyJyutping: Bool) async -> Bool
  {
    let unsafeFuzzyJyutping = false

    let globTerm = await prepareJyutpingBindValues(
      jyutping: searchTerm,
      useFuzzyJyutping: useFuzzyJyutping
    )
    print("globTerm: '\(globTerm)'")
    let query = String(
      format: jyutpingExistenceQuery,
      arguments: [useFuzzyJyutping ? REGEXP_OPERATOR : GLOB_OPERATOR]
    )

    var result: Bool = false
    do {
      result = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: query,
          arguments: [globTerm]
        )
        return parseExistenceRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return result
  }

  @concurrent func searchJyutping(searchTerm: String, useFuzzyJyutping: Bool) async
    -> [Entry]
  {
    // TODO: Actually implement checking for option
    let unsafeFuzzyJyutping = false

    let globTerm = await prepareJyutpingBindValues(
      jyutping: searchTerm,
      useFuzzyJyutping: useFuzzyJyutping
    )
    print("globTerm: '\(globTerm)'")
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
          arguments: [globTerm]
        )
        return parseReturnedRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchPinyinExistence(searchTerm: String, useFuzzyPinyin: Bool) async -> Bool {
    let globTerm = await preparePinyinBindValues(
      pinyin: searchTerm,
      useFuzzyPinyin: useFuzzyPinyin
    )
    print("globTerm: '\(globTerm)'")
    let query = String(
      format: pinyinExistenceQuery,
      arguments: [useFuzzyPinyin ? REGEXP_OPERATOR : GLOB_OPERATOR]
    )

    var result: Bool = false
    do {
      result = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: query,
          arguments: [globTerm]
        )
        return parseExistenceRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return result
  }

  @concurrent func searchPinyin(searchTerm: String, useFuzzyPinyin: Bool) async -> [Entry] {
    let globTerm = await preparePinyinBindValues(
      pinyin: searchTerm,
      useFuzzyPinyin: useFuzzyPinyin
    )
    print("globTerm: '\(globTerm)'")
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
          arguments: [globTerm]
        )
        return parseReturnedRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }

  @concurrent func searchEnglish(searchTerm: String) async -> [Entry] {
    let (ftsParam, likeParam) = await prepareEnglishBindValues(english: searchTerm)
    var results: [Entry] = []
    do {
      results = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: searchEnglishQuery,
          arguments: [ftsParam, likeParam]
        )
        return parseReturnedRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read from db: \(error)")
    }

    return results
  }
}
