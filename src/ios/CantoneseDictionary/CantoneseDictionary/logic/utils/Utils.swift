//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import Foundation

// 
extension String {
    func firstIndex(of: String, at: String.Index) -> String.Index? {
        return self[at...].range(of: of)?.lowerBound
    }
}
