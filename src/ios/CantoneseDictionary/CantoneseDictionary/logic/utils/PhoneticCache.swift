//
//  PhoneticCache.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-21.
//

import Foundation

nonisolated final class PhoneticCache: @unchecked Sendable {
  static let shared = PhoneticCache()

  private let cache = NSCache<NSString, NSString>()

  private init(maxEntries: Int = 1000) {
    cache.countLimit = maxEntries
  }

  func value(
    for type: CantoneseOptions,
    jyutping: String,
    generatePhonetic: () -> String
  ) -> String {
    let key = "c|\(type.rawValue)|\(jyutping)" as NSString

    if let cached = cache.object(forKey: key) {
      return cached as String
    }

    let result = generatePhonetic()
    cache.setObject(
      result as NSString,
      forKey: key,
      cost: result.utf16.count
    )
    return result
  }

  func value(
    for type: MandarinOptions,
    pinyin: String,
    generatePhonetic: () -> String
  ) -> String {
    let key = "m|\(type.rawValue)|\(pinyin)" as NSString

    if let cached = cache.object(forKey: key) {
      return cached as String
    }

    let result = generatePhonetic()
    cache.setObject(
      result as NSString,
      forKey: key,
      cost: result.utf16.count
    )
    return result
  }
}
