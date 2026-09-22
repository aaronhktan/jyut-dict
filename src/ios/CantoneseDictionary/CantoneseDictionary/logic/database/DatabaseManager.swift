//
//  DatabaseManager.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
import GRDB
import SwiftUI
import os

@Observable
class DatabaseManager {
    private let fileManager = FileManager.default
    private let bundleURL = Bundle.main.url(
        forResource: "dict",
        withExtension: "db"
    )
    private let localURL = URL.applicationSupportDirectory.appending(
        components: "dictionaries",
        "dict.db"
    )

    private var pool: DatabasePool? = nil

    public init() {
        if !fileManager.fileExists(atPath: self.localURL.path) {
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
                try fileManager.copyItem(at: self.bundleURL!, to: self.localURL)
                logger.info(
                    "Successfully copied dictionary database to \(self.localURL.absoluteString)!"
                )
            } catch {
                logger.error(
                    "Could not copy dictionary database to \(self.localURL.absoluteString), error: \(error)!"
                )
            }
        }
        
        let regexpFunction = DatabaseFunction("REGEXP", argumentCount: 2, pure: true) { values in
            guard let pattern = String.fromDatabaseValue(values[0]),
                  let text = String.fromDatabaseValue(values[1]) else {
                return false
            }
            guard let regex = try? NSRegularExpression(pattern: pattern) else {
                return false
            }
            let range = NSRange(location: 0, length: (text as NSString).length)
            return regex.firstMatch(in: text, options: [.anchored], range: range) != nil
        }
        
        var config = Configuration()
        config.prepareDatabase { db in
            db.add(function: regexpFunction)
        }

        do {
            self.pool = try DatabasePool(path: localURL.absoluteString, configuration: config)
        } catch {
            logger.error(
                "Could not open database pool for db \(self.localURL.absoluteString)!"
            )
            return
        }
    }

    var dbURL: URL {
        return localURL
    }

    var dbPool: DatabasePool? {
        return pool
    }
}
