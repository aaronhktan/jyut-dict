//
//  await InputMethodDetectorTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-25.
//

import Testing

@testable import CantoneseDictionary

struct InputMethodDetectorTests {
  @Test func simplifiedChinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "语言") == true
    )
  }

  @Test func noSimplifiedChinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "incomplete") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "預") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "個個person") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "cingjyun2") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasSimplified(text: "yongyuan") == false
    )
  }

  @Test func traditionalChinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "語言") == true
    )
  }

  @Test func noTraditionalChinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "incomplete") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "预") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "个个person") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "cingjyun2") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasTraditional(text: "yongyuan") == false
    )
  }

  @Test func chinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasHanCharacters(text: "叮叮") == true
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasHanCharacters(text: "一二三四五") == true
    )
  }

  @Test func noChinese() async throws {
    #expect(
      await InputMethodDetector(searcher: nil).hasHanCharacters(text: "mto") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasHanCharacters(text: "guiren") == false
    )

    #expect(
      await InputMethodDetector(searcher: nil).hasHanCharacters(text: "notgood") == false
    )
  }
}
