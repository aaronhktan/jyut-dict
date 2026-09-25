//
//  ChineseUtilsTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-15.
//

import Testing
import SwiftUI

@testable import CantoneseDictionary

struct ChineseUtilsTests {
  @Test func applyColoursJyutping() async throws {
    var firstChar = AttributedString("唔")
    firstChar.foregroundColor = defaultJyutpingToneColours[4]
    var secondChar = AttributedString("係")
    secondChar.foregroundColor = defaultJyutpingToneColours[6]

    let text = "唔係"
    let tones = [4, 6]
    let expected = firstChar + secondChar
    #expect(
      applyColours(
        text: text,
        tones: tones,
        jyutpingToneColours: defaultJyutpingToneColours,
        pinyinToneColours: [],
        type: .cantonese
      ) == expected
    )
  }

  @Test func applyColoursPinyin() async throws {
    var firstChar = AttributedString("不")
    firstChar.foregroundColor = defaultPinyinToneColours[2]
    var secondChar = AttributedString("是")
    secondChar.foregroundColor = defaultPinyinToneColours[4]

    let text = "不是"
    let tones = [2, 4]
    let expected = firstChar + secondChar
    #expect(
      applyColours(
        text: text,
        tones: tones,
        jyutpingToneColours: [],
        pinyinToneColours: defaultPinyinToneColours,
        type: .mandarin
      ) == expected
    )
  }

  @Test func compareStringSimple() async throws {
    #expect(
      compareStrings(original: "語言藝術", comparison: "语言艺术")
        == "语" + sameCharacterString + "艺术"
    )
  }

  @Test func compareStringsSingleMultibyteGrapheme() async throws {
    #expect(
      compareStrings(original: "賵", comparison: "赗") == "赗"
    )
  }

  @Test func compareStringsMultipleMultibyteGraphemes() async throws {
    #expect(
      compareStrings(original: "齮齕", comparison: "𬺈龁") == "𬺈龁"
    )
  }

  @Test func compareStringsMultibyteGraphemesWithAlpha() async throws {
    #expect(
      compareStrings(original: "齮aaaa齕", comparison: "𬺈aaaa龁") == "𬺈" + sameCharacterString
        + sameCharacterString + sameCharacterString + sameCharacterString + "龁"
    )
  }

  @Test func compareStringsCompatibilityVariantNormalization() async throws {
    #expect(
      compareStrings(original: "響", comparison: "響") == sameCharacterString
    )
  }

  @Test func constructRomanisationQuerySingleSyllable() async throws {
    #expect(
      constructRomanisationQuery(syllables: ["se"], delimiter: "?") == "se?"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se2"], delimiter: "?") == "se2"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se*"], delimiter: "?") == "se*?"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se?"], delimiter: "?") == "se??"
    )
  }

  @Test func constructRomanisationQueryMultiSyllable() async throws {
    #expect(
      constructRomanisationQuery(syllables: ["se", "dak"], delimiter: "?") == "se? dak?"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se2", "dak1"], delimiter: "?") == "se2 dak1"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se*", "dak*"], delimiter: "?") == "se*? dak*?"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se?", "dak?"], delimiter: "?") == "se?? dak??"
    )
  }

  @Test func constructRomanisationQueryGlobCharacters() async throws {
    #expect(
      constructRomanisationQuery(syllables: ["se", " *"], delimiter: "?") == "se? *"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se", " ?", "?", "?"], delimiter: "?") == "se? ???"
    )

    #expect(
      constructRomanisationQuery(syllables: ["se", " ?", "?", "? ", "dak"], delimiter: "?")
        == "se? ??? dak?"
    )
  }

  @Test func constructRomanisationQueryOnlyGlobCharacters() async throws {
    #expect(
      constructRomanisationQuery(
        syllables: ["?", "?", "?", "?", "? ", "?", "?", "?", "?"], delimiter: "?") == "????? ????"
    )
  }
}
