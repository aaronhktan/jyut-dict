//
//  DatabaseManager.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
import SwiftUI
import os

import GRDB

@Observable
class DatabaseManager {
    private let fileManager = FileManager.default
    private let bundleURL = Bundle.main.url(forResource: "dict", withExtension: "db")
    private let localURL = URL.applicationSupportDirectory.appending(components: "dictionaries", "dict.db")
    
    private var pool: DatabasePool? = nil
    
    public init() {
        if fileManager.fileExists(atPath: localURL.absoluteString) {
            return
        }
        
        do {
            try fileManager.createDirectory(
                at: localURL.deletingLastPathComponent(),
                withIntermediateDirectories: true,
                attributes: nil
            )
            
            try fileManager.copyItem(at: bundleURL!, to: localURL)
        } catch {
            logger.error("Could not copy dictionary database to \(self.localURL.absoluteString)!")
            return
        }
        
        logger.info("Successfully copied dictionary database to \(self.localURL.absoluteString)!")
        
        do {
            pool = try DatabasePool(path: localURL.absoluteString)
        } catch {
            logger.error("Could not open database pool for db \(self.localURL.absoluteString)!")
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
