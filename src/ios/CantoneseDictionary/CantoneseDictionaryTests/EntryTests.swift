//
//  EntryTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-21.
//

import Foundation
import SwiftUI
import Testing

@testable import CantoneseDictionary

struct EntryTests {
  @Test func getCharactersSimpleCantonese() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    let entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharacters(
        options: .preferTraditional, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(traditional) [\(compareStrings(original: traditional, comparison: simplified))]")
    )
    #expect(
      entry.getCharacters(
        options: .preferSimplified, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(simplified) [\(compareStrings(original: simplified, comparison: traditional))]")
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlySimplified, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferTraditional, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferSimplified, useColours: false, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
  }

  @Test func getCharactersSimpleMandarin() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    let entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharacters(
        options: .preferTraditional, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(traditional) [\(compareStrings(original: traditional, comparison: simplified))]")
    )
    #expect(
      entry.getCharacters(
        options: .preferSimplified, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(simplified) [\(compareStrings(original: simplified, comparison: traditional))]")
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlySimplified, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferTraditional, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferSimplified, useColours: false, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
  }

  @Test func getCharactersColourCantonese() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    let entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    var colouredTraditionalString: AttributedString = ""
    var colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredTraditionalString.append(colouredCurrentString)

    var colouredSimplifiedString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredSimplifiedString.append(colouredCurrentString)

    var colouredPreferTraditionalString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredPreferTraditionalString.append(AttributedString(" [" + sameCharacterString))
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredPreferTraditionalString.append(AttributedString("]"))

    var colouredPreferSimplifiedString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredPreferSimplifiedString.append(AttributedString(" [" + sameCharacterString))
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredPreferSimplifiedString.append(AttributedString("]"))

    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
    #expect(
      entry.getCharacters(
        options: .preferTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredPreferTraditionalString
    )
    #expect(
      entry.getCharacters(
        options: .preferSimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredPreferSimplifiedString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlySimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferSimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
  }

  @Test func getCharactersColourMandarin() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    let entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    var colouredTraditionalString: AttributedString = ""
    var colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[1]
    colouredTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredTraditionalString.append(colouredCurrentString)

    var colouredSimplifiedString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[1]
    colouredSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredSimplifiedString.append(colouredCurrentString)

    var colouredPreferTraditionalString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[1]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredPreferTraditionalString.append(AttributedString(" [" + sameCharacterString))
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredPreferTraditionalString.append(colouredCurrentString)
    colouredPreferTraditionalString.append(AttributedString("]"))

    var colouredPreferSimplifiedString: AttributedString = ""
    colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[1]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredPreferSimplifiedString.append(AttributedString(" [" + sameCharacterString))
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultPinyinToneColours[3]
    colouredPreferSimplifiedString.append(colouredCurrentString)
    colouredPreferSimplifiedString.append(AttributedString("]"))

    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
    #expect(
      entry.getCharacters(
        options: .preferTraditional, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredPreferTraditionalString
    )
    #expect(
      entry.getCharacters(
        options: .preferSimplified, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredPreferSimplifiedString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlySimplified, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferTraditional, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferSimplified, useColours: true, colourPhoneticType: .mandarin,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )
  }

  @Test func getCharactersSimpleNone() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    let entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharacters(
        options: .preferTraditional, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(traditional) [\(compareStrings(original: traditional, comparison: simplified))]")
    )
    #expect(
      entry.getCharacters(
        options: .preferSimplified, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      )
        == AttributedString(
          "\(simplified) [\(compareStrings(original: simplified, comparison: traditional))]")
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlyTraditional, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .onlySimplified, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferTraditional, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(traditional)
    )
    #expect(
      entry.getCharactersNoSecondary(
        options: .preferSimplified, useColours: false, colourPhoneticType: .none,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == AttributedString(simplified)
    )
  }

  @Test func traditionalGetSet() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    var entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    var colouredTraditionalString: AttributedString = ""
    var colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("遠")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredTraditionalString.append(colouredCurrentString)

    #expect(entry.traditional == traditional)
    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredTraditionalString
    )

    let newTraditional = "廣州"
    entry.traditional = newTraditional
    entry.jyutping = "gwong2 zau1"
    var colouredNewTraditionalString: AttributedString = ""
    colouredCurrentString = AttributedString("廣")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[2]
    colouredNewTraditionalString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("州")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredNewTraditionalString.append(colouredCurrentString)

    #expect(entry.traditional == newTraditional)
    #expect(
      entry.getCharacters(
        options: .onlyTraditional, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredNewTraditionalString
    )
  }

  @Test func simplifiedGetSet() async throws {
    let traditional = "清遠"
    let simplified = "清远"
    var entry = Entry(
      rowid: 1, traditional: traditional, simplified: simplified, jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    var colouredSimplifiedString: AttributedString = ""
    var colouredCurrentString = AttributedString("清")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("远")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[5]
    colouredSimplifiedString.append(colouredCurrentString)

    #expect(entry.simplified == simplified)
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredSimplifiedString
    )

    let newSimplified = "广州"
    entry.simplified = newSimplified
    entry.jyutping = "gwong2 zau1"
    var colouredNewSimplifiedString: AttributedString = ""
    colouredCurrentString = AttributedString("广")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[2]
    colouredNewSimplifiedString.append(colouredCurrentString)
    colouredCurrentString = AttributedString("州")
    colouredCurrentString.foregroundColor = defaultJyutpingToneColours[1]
    colouredNewSimplifiedString.append(colouredCurrentString)

    #expect(entry.simplified == newSimplified)
    #expect(
      entry.getCharacters(
        options: .onlySimplified, useColours: true, colourPhoneticType: .cantonese,
        jyutpingToneColours: defaultJyutpingToneColours, pinyinToneColours: defaultPinyinToneColours
      ) == colouredNewSimplifiedString
    )
  }

  @Test func jyutpingGetSet() async throws {
    let jyutping = "cing1 jyun5"
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: jyutping,
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.jyutping == jyutping)
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == jyutping
    )
    let yale = "chīng yúhn"
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == yale
    )
    let cantoneseIPA = "t͡sʰɪŋ˥ jyːn˩˧"
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == cantoneseIPA
    )

    let newSimplified = "广州"
    entry.simplified = newSimplified
    let newJyutping = "gWoNg2 ZaU1"
    let normalizedJyutping = "gwong2 zau1"
    entry.jyutping = newJyutping

    #expect(entry.jyutping == normalizedJyutping)
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == normalizedJyutping
    )
    let newYale = "gwóng jāu"
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == newYale
    )
    let newCantoneseIPA = "kʷɔːŋ˧˥ t͡sɐu̯˥"
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == newCantoneseIPA
    )
  }

  @Test func pinyinGetSet() async throws {
    let pinyin = "qing1 yuan3"
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: pinyin, definitions: [])

    #expect(entry.pinyin == pinyin)
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == pinyin
    )
    let prettyPinyin = "qīng yuǎn"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == prettyPinyin
    )
    let numberedPinyin = "qing1 yuan3"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .numberedPinyin)
        == numberedPinyin
    )
    let zhuyin = "ㄑㄧㄥ ㄩㄢˇ"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == zhuyin
    )
    let mandarinIPA = "t͡ɕʰiŋ˥˥ ɥɛn˨˩˦꜕꜖(꜓)"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == mandarinIPA
    )
    let rawPinyinWithV = "qing1 yuan3"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyinWithV)
        == rawPinyinWithV
    )

    let newSimplified = "广州"
    entry.simplified = newSimplified
    let newPinyin = "gUaNg3 ZhOu1"
    let normalizedPinyin = "guang3 zhou1"
    entry.pinyin = newPinyin

    #expect(entry.pinyin == normalizedPinyin)
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == normalizedPinyin
    )
    let newPrettyPinyin = "guǎng zhōu"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == newPrettyPinyin
    )
    let newNumberedPinyin = "guang3 zhou1"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .numberedPinyin)
        == newNumberedPinyin
    )
    let newZhuyin = "ㄍㄨㄤˇ ㄓㄡ"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == newZhuyin
    )
    let newMandarinIPA = "kwɑŋ˨˩˦꜕꜖꜖ ʈ͡ʂoʊ̯˥˥"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == newMandarinIPA
    )
    let newRawPinyinWithV = "guang3 zhou1"
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyinWithV)
        == newRawPinyinWithV
    )
  }

  @Test func getPhoneticCantonese() async throws {
    let jyutping = "cing1 jyun5"
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: jyutping,
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.jyutping == jyutping)
    #expect(
      entry.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == jyutping
    )
    var phoneticString = "chīng yúhn (qing1 yuan3)"
    #expect(
      entry.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == phoneticString
    )
    phoneticString = "t͡sʰɪŋ˥ jyːn˩˧ (qing1 yuan3)"
    #expect(
      entry.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == phoneticString
    )

    entry.pinyin = ""
    phoneticString = "chīng yúhn"
    #expect(
      entry.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == phoneticString
    )

    entry.jyutping = ""
    entry.pinyin = "qing1 yuan3"
    phoneticString = "(qing1 yuan3)"
    #expect(
      entry.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == phoneticString
    )
  }

  @Test func getPhoneticMandarin() async throws {
    let pinyin = "qing1 yuan3"
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: pinyin, definitions: [])

    #expect(entry.pinyin == pinyin)
    #expect(
      entry.getPhonetic(
        options: .onlyMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == pinyin
    )
    var phoneticString = "qīng yuǎn (cing1 jyun5)"
    #expect(
      entry.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == phoneticString
    )
    phoneticString = "ㄑㄧㄥ ㄩㄢˇ (cing1 jyun5)"
    #expect(
      entry.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == phoneticString
    )
    phoneticString = "t͡ɕʰiŋ˥˥ ɥɛn˨˩˦꜕꜖(꜓) (cing1 jyun5)"
    #expect(
      entry.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == phoneticString
    )

    entry.jyutping = ""
    phoneticString = "qing1 yuan3"
    #expect(
      entry.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .numberedPinyin)
        == phoneticString
    )

    entry.pinyin = ""
    entry.jyutping = "cing1 jyun5"
    phoneticString = "(cing1 jyun5)"
    #expect(
      entry.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == phoneticString
    )
  }

  @Test func getJyutpingNumbersSimple() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.getJyutpingNumbers() == [1, 5])
  }

  @Test func getJyutpingNumbersInvalidTones() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 cing7 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.getJyutpingNumbers() == [1, 5])
  }

  @Test func getJyutpingNumbersResetJyutping() async throws {
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.getJyutpingNumbers() == [1, 5])

    entry.jyutping = "gwong2 zau1"
    #expect(entry.getJyutpingNumbers() == [2, 1])
  }

  @Test func getPinyinNumbersSimple() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.getPinyinNumbers() == [1, 3])
  }

  @Test func getPinyinNumbersInvalidTones() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 qing6 yuan3", definitions: [])

    #expect(entry.getPinyinNumbers() == [1, 3])
  }

  @Test func getPinyinNumbersResetJyutping() async throws {
    var entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.getPinyinNumbers() == [1, 3])

    entry.pinyin = "guang3 zhou1"
    #expect(entry.getPinyinNumbers() == [3, 1])
  }

  @Test func getDefinitionsSetsSimple() async throws {
    let definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])

    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [definitionsSet])

    #expect(entry.getDefinitionsSets()! == [definitionsSet])
  }

  @Test func modifyDefinitionsSetsSimple() async throws {
    var definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])

    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [definitionsSet])

    #expect(entry.getDefinitionsSets()! == [definitionsSet])

    definition.definitionContent = "(historical) Qingyuan (a former county of Guangdong, China)"
    // DefinitionsSet should be copied into entry, so modifications outside should not affect it
    #expect(
      definitionsSet.definitions[0].definitionContent
        == "Qingyuan, prefecture-level city in Guangdong")

    // The sets returned by getDefinitionsSets should be a copy, not a reference to the internal sets
    #expect(entry.getDefinitionsSets()!.count == 1)
    #expect(
      entry.getDefinitionsSets()![0].definitions[0].definitionContent
        == "Qingyuan, prefecture-level city in Guangdong")
  }

  @Test func getDefinitionSnippetSimple() async throws {
    let definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])

    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [definitionsSet])

    #expect(entry.definitionSnippet == "Qingyuan, prefecture-level city in Guangdong")
  }

  @Test func getDefinitionSnippetEmptyDefinitionSet() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.definitionSnippet == "")
  }

  @Test func getDefinitionSnippetEmptyDefinition() async throws {
    let definition = Definition(
      definitionContent: "", label: "", examples: [])

    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [definitionsSet])

    #expect(entry.definitionSnippet == "")
  }

  @Test func getIdSimple() async throws {
    let entry = Entry(
      rowid: 1, traditional: "清遠", simplified: "清远", jyutping: "cing1 jyun5",
      pinyin: "qing1 yuan3", definitions: [])

    #expect(entry.id == 1)
  }
}
