//
//  ExampleTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-22.
//

import Testing

@testable import CantoneseDictionary

struct ExampleTests {
  @Test func cloneSimple() async throws {
    var translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    let secondExample = example
    #expect(example == secondExample)
    #expect(example.getTranslationSets()[0].translations[0].content == "egg waffle")
    #expect(
      example.getTranslationSets()[0].translations[0].content
        == secondExample.getTranslationSets()[0].translations[0].content)

    translation.content = ""
    #expect(example.getTranslationSets()[0].translations[0].content == "egg waffle")
    #expect(
      example.getTranslationSets()[0].translations[0].content
        == secondExample.getTranslationSets()[0].translations[0].content)
  }

  @Test func equalsSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    let secondTranslation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let secondTranslationSet = TranslationSet(
      source: "粵典—words.hk", translations: [secondTranslation])
    let secondExample = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [secondTranslationSet])

    // In Swift, equality is based on just the ID, and those should differ even
    // if the contents are the same
    #expect(example != secondExample)
    #expect(example.getTranslationSets()[0].translations[0].content == "egg waffle")
    #expect(
      example.getTranslationSets()[0].translations[0].content
        == secondExample.getTranslationSets()[0].translations[0].content)
  }

  @Test func sourceLanguageSimple() async throws {
    let example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.sourceLanguage == "yue")
  }

  @Test func getCharactersTraditional() async throws {
    let traditional = "臺山話"
    let example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.traditional == traditional)
    #expect(example.getCharacters(options: .onlyTraditional) == traditional)
    #expect(example.getCharacters(options: .preferTraditional) == traditional)
  }

  @Test func getCharactersSimplified() async throws {
    let simplified = "台山话"
    let example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.simplified == simplified)
    #expect(example.getCharacters(options: .onlySimplified) == simplified)
    #expect(example.getCharacters(options: .preferSimplified) == simplified)
  }

  @Test func traditionalGetSet() async throws {
    let traditional = "臺山話"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.traditional == traditional)

    let newTraditional = "四邑話"
    example.traditional = newTraditional
    #expect(example.traditional == newTraditional)
    #expect(example.getCharacters(options: .onlyTraditional) == newTraditional)
    #expect(example.getCharacters(options: .preferTraditional) == newTraditional)
  }

  @Test func simplifiedGetSet() async throws {
    let simplified = "台山话"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.simplified == simplified)

    let newSimplified = "四邑话"
    example.simplified = newSimplified
    #expect(example.simplified == newSimplified)
    #expect(example.getCharacters(options: .onlySimplified) == newSimplified)
    #expect(example.getCharacters(options: .preferSimplified) == newSimplified)
  }

  @Test func getPhoneticCantonese() async throws {
    let jyutping = "toi4 saan1 waa2"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.jyutping == jyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == jyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == "tòih sāan wá")
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == "tʰɔːi̯˨˩ säːn˥ wäː˧˥")

    example.pinyin = ""
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == "tòih sāan wá")
    example.jyutping = ""
    example.pinyin = "tai2 shan1 hua4"
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == "")
  }

  @Test func getPhoneticMandarin() async throws {
    let pinyin = "tai2 shan1 hua4"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.pinyin == pinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == pinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == "tái shān huà")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == "tʰaɪ̯˧˥ ʂän˥˥ xwä˥˩")

    example.jyutping = ""
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == "tai2 shan1 hua4")
    example.pinyin = ""
    example.jyutping = "toi4 saan1 waa2"
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == "")
  }

  @Test func jyutpingGetSet() async throws {
    let jyutping = "toi4 saan1 waa2"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.jyutping == jyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == jyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == "tòih sāan wá")
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == "tʰɔːi̯˨˩ säːn˥ wäː˧˥")

    example.jyutping = "SeI3 jAp1 WaA2"
    let normalizedNewJyutping = "sei3 jap1 waa2"
    #expect(example.jyutping == normalizedNewJyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == normalizedNewJyutping)
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .prettyYale, mandarinOptions: .rawPinyin)
        == "sei yāp wá")
    #expect(
      example.getPhonetic(
        options: .preferCantonese, cantoneseOptions: .cantoneseIPA, mandarinOptions: .rawPinyin)
        == "sei̯˧ jɐp̚˥ wäː˧˥")
  }

  @Test func pinyinGetSet() async throws {
    let pinyin = "tai2 shan1 hua4"
    var example = Example(
      sourceLanguage: "yue", traditional: "臺山話", simplified: "台山话", jyutping: "toi4 saan1 waa2",
      pinyin: "tai2 shan1 hua4", translations: [])

    #expect(example.pinyin == pinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == pinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == "tái shān huà")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .numberedPinyin)
        == "tai2 shan1 hua4")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == "tʰaɪ̯˧˥ ʂän˥˥ xwä˥˩")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyinWithV)
        == "tai2 shan1 hua4")

    example.pinyin = "Si4 yi4 HuA4"
    let normalizedNewPinyin = "si4 yi4 hua4"
    #expect(example.pinyin == normalizedNewPinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyin)
        == normalizedNewPinyin)
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .prettyPinyin)
        == "sì yì huà")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .numberedPinyin)
        == "si4 yi4 hua4")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .zhuyin)
        == "ㄙˋ ㄧˋ ㄏㄨㄚˋ")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .mandarinIPA)
        == "sz̩˥˩꜒꜔ i˥˩꜒꜔ xwä˥˩")
    #expect(
      example.getPhonetic(
        options: .preferMandarin, cantoneseOptions: .rawJyutping, mandarinOptions: .rawPinyinWithV)
        == "si4 yi4 hua4")
  }

  @Test func getTranslationSetsSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    #expect(example.getTranslationSets() == [translationSet])
  }

  @Test func getTranslationSnippetSimple() async throws {
    let translation = Translation(content: "egg waffle", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    #expect(example.getTranslationSnippet() == "egg waffle")
  }

  @Test func getTranslationSnippetEmptyTranslations() async throws {
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [])

    #expect(example.getTranslationSnippet() == "")
  }

  @Test func getTranslationSnippetEmptyTranslationSet() async throws {
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    #expect(example.getTranslationSnippet() == "")
  }

  @Test func getTranslationSnippetEmptyTranslationContent() async throws {
    let translation = Translation(content: "", language: "eng", directTarget: true)
    let translationSet = TranslationSet(source: "粵典—words.hk", translations: [translation])
    let example = Example(
      sourceLanguage: "yue", traditional: "雞蛋仔", simplified: "鸡蛋仔", jyutping: "gai1 daan2 zai2",
      pinyin: "ji1 dan4 zai3", translations: [translationSet])

    #expect(example.getTranslationSnippet() == "")
  }
}
