//
//  SearchParameters.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-12.
//

enum SearchParameters: Int, CaseIterable, Identifiable {
    case simplified = 0
    case traditional = 1
    
    case jyutping = 2
    case fuzzyJyutping = 3
    case pinyin = 4
    case fuzzyPinyin = 5

    case english = 6
    
    case autoDetect = 1000

    var id: Int { rawValue }
}
