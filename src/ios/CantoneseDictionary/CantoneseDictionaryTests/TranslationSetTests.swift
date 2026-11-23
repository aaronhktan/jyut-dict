//
//  TranslationSetTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-22.
//

import Testing

@testable import CantoneseDictionary

struct TranslationSetTests {
  @Test func isEmptySimple() async throws {
    let ts = TranslationSet(source: "CC-CANTO", translations: [])
    #expect(ts.isEmpty())
  }

  @Test func isEmptyFalse() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    #expect(translationSet.isEmpty() == false)
  }

  @Test func cloneSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let secondTranslationSet = translationSet
    #expect(translationSet == secondTranslationSet)
  }

  @Test func equalsSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])

    let secondTranslation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let secondTranslationSet = TranslationSet(source: "粵典—words.hk", translations: [secondTranslation])

    // In Swift, equality is based on just the ID, and those should differ even
    // if the contents are the same
    #expect(translationSet != secondTranslationSet)
  }

  @Test func getTranslationSnippetSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])

    #expect(translationSet.getTranslationSnippet() == [translation])
  }

  @Test func getTranslationSnippetEmpty() async throws {
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [])
    #expect(translationSet.getTranslationSnippet() == [])
  }

  @Test func translationCopy() async throws {
    var translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])

    #expect(translationSet.translations == [translation])

    translation.content = "small egg"
    #expect(translationSet.translations != [translation])
    #expect(translationSet.translations[0].content == "egg waffle")
  }

  @Test func sourceSimple() async throws {
    var translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    #expect(translationSet.source == "粵典—words.hk")
  }
}
