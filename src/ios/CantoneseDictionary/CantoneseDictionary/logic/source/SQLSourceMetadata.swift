//
//  SQLSourceMetadata.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import Foundation
import GRDB
import SwiftUI
import os

nonisolated let sourceQuery = """
  SELECT sourcename, version, description, legal, link, update_url, other
  FROM sources
  """

actor SQLSourceMetadata {
  private var pool: DatabasePool

  public init(pool: DatabasePool) {
    self.pool = pool
  }

  @concurrent func getSources() async -> [SourceMetadata] {
    var result: [SourceMetadata] = []
    do {
      result = try await pool.read { db in
        let rows = try Row.fetchAll(
          db,
          sql: sourceQuery
        )
        return parseSourceRecords(rows: rows)
      }
    } catch {
      // TODO: Handle errors
      logger.error("Error happened when trying to read sources from db: \(error)")
    }

    return result
  }
}

struct SourceMetadata : Identifiable {
  let id = UUID()

  var name: String
  var version: String
  var description: String
  var legal: String
  var link: String
  var updateURL: String
  var other: String
}
