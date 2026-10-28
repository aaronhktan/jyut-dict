//
//  DatabaseManager.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import AppleArchive
import Foundation
import GRDB
import SwiftUI
import System
import os

nonisolated let currentDatabaseVersion = 4

nonisolated private final class RegexCache: @unchecked Sendable {
  private let cache = NSCache<NSString, NSRegularExpression>()

  init(maxEntries: Int = 64) {
    cache.countLimit = maxEntries
  }

  func regex(for pattern: String) -> NSRegularExpression? {
    let key = pattern as NSString
    if let cached = cache.object(forKey: key) {
      return cached
    }
    guard let newRegex = try? NSRegularExpression(pattern: pattern) else {
      return nil
    }
    cache.setObject(newRegex, forKey: key)
    return newRegex
  }
}

@Observable
class DatabaseManager {
  private let fileManager = FileManager.default
  private let bundleURL = Bundle.main.url(
    forResource: "dict.db",
    withExtension: "aar"
  )
  private let localURL = URL.applicationSupportDirectory.appending(
    components: "dictionaries",
    "dict.db"
  )
  private let bundleUserURL = Bundle.main.url(
    forResource: "user",
    withExtension: "db"
  )
  private let localUserURL = URL.applicationSupportDirectory.appending(
    components: "dictionaries",
    "user.db"
  )

  private var pool: DatabasePool? = nil
  var databaseExists = false

  public init() {
    if fileManager.fileExists(atPath: self.localURL.path) {
      databaseExists = true
    }

    if !fileManager.fileExists(atPath: self.localUserURL.path) {
      do {
        try fileManager.createDirectory(
          at: self.localURL.deletingLastPathComponent(),
          withIntermediateDirectories: true,
          attributes: nil
        )
      } catch {
        logger.error(
          "Could not create directory at \(self.localURL.deletingLastPathComponent().absoluteString)!"
        )
      }

      do {
        if let bundleUserURL = self.bundleUserURL {
          try fileManager.copyItem(at: bundleUserURL, to: self.localUserURL)
          logger.info(
            "Successfully copied user database to \(self.localUserURL.absoluteString)!"
          )
        } else {
          logger.warning("No bundle URL found, skipping user database copy!")
        }
      } catch {
        logger.error(
          "Could not copy user database to \(self.localUserURL.absoluteString), error: \(error)!"
        )
      }
    }
  }

  var dbURL: URL {
    return localURL
  }

  var userURL: URL {
    return localUserURL
  }

  var dbPool: DatabasePool? {
    return pool
  }

  var databaseVersion: Int? {
    // Must have already called setupDatabase() before trying to access this variable!
    guard self.pool != nil else {
      return nil
    }
    do {
      return try self.pool?.read { db in
        return try Int.fetchOne(db, sql: "PRAGMA user_version")!
      }
    } catch {
      logger.error(
        "Could get user_version for \(self.localURL.absoluteString), error: \(error)!"
      )
    }

    return nil
  }

  @concurrent func checkpointDatabase() async {
    do {
      try await self.dbPool?.writeWithoutTransaction { db in
        guard
          let row = try Row.fetchOne(
            db,
            sql: "PRAGMA wal_checkpoint(TRUNCATE)"
          )
        else {
          logger.error("Could not checkpoint user database!")
          return
        }
        logger.info(
          "busy: \(row["busy"] as! Int64), log: \(row["log"] as! Int64), checkpointed: \(row["checkpointed"] as! Int64)"
        )
      }
    } catch {
      logger.error("Could not checkpoint database!")
    }
  }

  @concurrent func deleteDatabase() async {
    do {
      await MainActor.run {
        do {
          self.pool?.interrupt()
          try self.pool?.close()
          self.pool = nil
        } catch {
          logger.error("Could not close database pool!")
        }
      }
      let base = self.localURL.path
      for suffix in ["", "-wal", "-shm", "-journal"] {
        let path = base + suffix
        if FileManager.default.fileExists(atPath: path) {
          try FileManager.default.removeItem(atPath: path)
        }
      }
      await MainActor.run {
        databaseExists = false
      }
      logger.info("Deleted database from filesystem!")
    } catch {
      logger.error("Could not delete database from filesystem!")
    }
  }

  @concurrent func importDatabase(from: URL) async {
    if FileManager.default.fileExists(atPath: self.localURL.path) {
      logger.warning("Database import attempted when database already exists, no-op!")
      return
    }

    do {
      try FileManager.default.copyItem(atPath: from.path, toPath: self.localURL.path)
      Task { @MainActor in
        databaseExists = true
      }
    } catch {
      logger.error("Database import failed: \(error)")
    }
    logger.info("Imported database from filesystem: \(from.absoluteString)")
  }

  @concurrent func copyDatabase() async {
    // In order to generate the file, run `aa archive -i dict.db -o dict.db.aar -a lzfse -b 1m`
    do {
      guard let bundleURL = self.bundleURL else {
        logger.warning("No bundle URL found, skipping database copy!")
        return
      }

      guard
        let readFileStream = ArchiveByteStream.fileStream(
          path: FilePath(bundleURL.path),
          mode: .readOnly,
          options: [],
          permissions: FilePermissions(rawValue: 0o644))
      else {
        logger.error("Unable to create read stream")
        return
      }
      defer {
        try? readFileStream.close()
      }

      guard
        let writeFileStream = ArchiveByteStream.fileStream(
          path: FilePath(self.localURL.path),
          mode: .writeOnly,
          options: [.create, .truncate],
          permissions: FilePermissions(rawValue: 0o644))
      else {
        logger.error("Unable to create write stream")
        return
      }
      defer {
        try? writeFileStream.close()
      }

      guard
        let decompressStream = ArchiveByteStream.decompressionStream(readingFrom: readFileStream)
      else {
        logger.error("Unable to create compress stream")
        return
      }
      defer {
        try? decompressStream.close()
      }

      let decoded = ArchiveStream.decodeStream(readingFrom: decompressStream)!
      let extracted = ArchiveStream.extractStream(
        extractingTo: FilePath(self.localURL.deletingLastPathComponent().path)
      )!

      let i = try ArchiveStream.process(
        readingFrom: decoded,
        writingTo: extracted)

      if FileManager.default.fileExists(atPath: self.localURL.path) {
        let attr = try FileManager.default.attributesOfItem(atPath: self.localURL.path)
        let fileSize = attr[FileAttributeKey.size] as! UInt64
        logger.info("Expanded dictionary database is \(fileSize) bytes")
        logger.info(
          "Successfully copied dictionary database from \(i) bytes to \(fileSize) bytes at \(self.localURL.absoluteString)!"
        )
        Task { @MainActor in
          databaseExists = true
        }
      } else {
        logger.info("Database does not exist after copy!!")
      }
    } catch {
      logger.error(
        "Could not copy dictionary database to \(self.localURL.absoluteString), error: \(error)!"
      )
    }
  }

  @concurrent func checkpointUserDatabase() async {
    do {
      try await self.dbPool?.writeWithoutTransaction { db in
        guard
          let row = try Row.fetchOne(
            db,
            sql: "PRAGMA user.wal_checkpoint(TRUNCATE)"
          )
        else {
          logger.error("Could not checkpoint user database!")
          return
        }
        logger.info(
          "busy: \(row["busy"] as! Int64), log: \(row["log"] as! Int64), checkpointed: \(row["checkpointed"] as! Int64)"
        )
      }
    } catch {
      logger.error("Could not checkpoint user database, error: \(error)")
    }
  }

  @concurrent func deleteUserDatabase() async {
    do {
      await MainActor.run {
        do {
          self.pool?.interrupt()
          try self.pool?.close()
          self.pool = nil
        } catch {
          logger.error("Could not close database pool!")
        }
      }
      let base = self.localUserURL.path
      for suffix in ["", "-wal", "-shm", "-journal"] {
        let path = base + suffix
        if FileManager.default.fileExists(atPath: path) {
          try FileManager.default.removeItem(atPath: path)
        }
      }
      logger.info("Deleted user database from filesystem!")
    } catch {
      logger.error("Could not delete user database from filesystem!")
    }
  }

  @concurrent func importUserDatabase(from: URL) async {
    if FileManager.default.fileExists(atPath: self.localUserURL.path) {
      logger.warning("User database import attempted when user database already exists, no-op!")
      return
    }

    do {
      try FileManager.default.copyItem(atPath: from.path, toPath: self.localUserURL.path)
    } catch {
      logger.error("User database import failed: \(error)")
    }
    logger.info("Imported user database from filesystem: \(from.absoluteString)")
  }

  func setupDatabase() {
    let regexCache = RegexCache()
    let regexpFunction = DatabaseFunction("REGEXP", argumentCount: 2, pure: true) { values in
      guard let pattern = String.fromDatabaseValue(values[0]),
        let text = String.fromDatabaseValue(values[1])
      else {
        return false
      }

      guard let regex = regexCache.regex(for: pattern) else {
        return false
      }

      let range = NSRange(location: 0, length: text.utf16.count)
      return regex.rangeOfFirstMatch(in: text, options: [.anchored], range: range).location
        != NSNotFound
    }

    var config = Configuration()
    config.prepareDatabase { db in
      db.add(function: regexpFunction)
      try db.execute(
        sql: """
          ATTACH DATABASE ? AS user
          """, arguments: [self.localUserURL.path])
      try db.execute(sql: "PRAGMA cache_size = -64000")
      try db.execute(sql: "PRAGMA mmap_size = 268435456")
      try db.execute(sql: "PRAGMA temp_store = MEMORY")
      try db.execute(sql: "PRAGMA foreign_keys = ON")
      try db.execute(sql: "PRAGMA synchronous = NORMAL")
    }

    do {
      self.pool = try DatabasePool(path: localURL.absoluteString, configuration: config)
    } catch {
      logger.error(
        "Could not open database pool for db \(self.localURL.absoluteString), error: \(error)!"
      )
      return
    }
  }
}
