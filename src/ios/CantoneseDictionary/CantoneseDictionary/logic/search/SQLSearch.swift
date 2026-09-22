//
//  SQLSearch.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation
import os
import SwiftUI

import GRDB

actor SQLSearch {
    private var pool: DatabasePool
    
    public init(pool: DatabasePool) {
        self.pool = pool
    }
    
    func searchJyutping(searchTerm: String) async -> [Entry] {
        // TODO: Actually implement checking for option
        let fuzzyJyutping = true
        let unsafeFuzzyJyutping = true
        
        let globTerm = prepareJyutpingBindValues(jyutping: searchTerm, useFuzzyJyutping: fuzzyJyutping)
        print("globTerm: '\(globTerm)'")
        
        var results: [Entry] = []
        do {
            results = try await pool.read { db in
                let rows = try Row.fetchAll(db, sql: searchJyutpingQuery, arguments: [globTerm])
                return parseReturnedRecords(rows: rows)
            }
        } catch {
            // TODO: Handle errors
            logger.error("Error happened when trying to read from db")
        }
        
        return results
    }
    
    func searchPinyin(searchTerm: String) async -> [Entry] {
        // TODO: Actually implement checking for option
        let fuzzyPinyin = true
        
        let globTerm = preparePinyinBindValues(pinyin: searchTerm, useFuzzyPinyin: fuzzyPinyin)
        print("globTerm: '\(globTerm)'")
        
        var results: [Entry] = []
        do {
            results = try await pool.read { db in
                let rows = try Row.fetchAll(db, sql: searchPinyinQuery, arguments: [globTerm])
                return parseReturnedRecords(rows: rows)
            }
        } catch {
            // TODO: Handle errors
            logger.error("Error happened when trying to read from db")
        }
        
        return results
    }
}
