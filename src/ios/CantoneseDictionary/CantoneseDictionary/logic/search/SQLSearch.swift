//
//  SQLSearch.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-21.
//

import Foundation

import GRDB

actor SQLSearch {
    private var pool: DatabasePool
    
    public init(pool: DatabasePool) {
        self.pool = pool
    }
    
    func searchJyutping(searchTerm: String) async -> [Entry] {
        let searchTask = Task {
            // TODO: Actually implement checking for option
            let fuzzyJyutping = true
            let unsafeFuzzyJyutping = true
            
            let globTerm = prepareJyutpingBindValues(jyutping: searchTerm, useFuzzyJyutping: fuzzyJyutping)
            
            var results: [Entry] = []
            do {
                try pool.read { db in
                    let rows = try Row.fetchCursor(db, sql: searchJyutpingQuery, arguments: [globTerm])
                    while let row = try rows.next() {
                        // TODO: Process rows
                    }
                }
            } catch {
                // TODO: Handle errors
            }
            
            return results
        }
        
        return await searchTask.value
    }
}
