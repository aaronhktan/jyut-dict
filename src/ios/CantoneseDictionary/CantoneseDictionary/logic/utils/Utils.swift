//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import Foundation
import os

nonisolated let sameCharacterString = "－"
nonisolated let logger = Logger()

// Nabbed from https://forums.swift.org/t/find-multiple-substrings-from-specified-index/28667/4
extension String {
    nonisolated func firstIndex(of: String, at: String.Index) -> String.Index? {
        return self[at...].range(of: of)?.lowerBound
    }
}
