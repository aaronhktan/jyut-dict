//
//  MandarinUtilsTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-16.
//

import Testing

@testable import CantoneseDictionary

struct MandarinUtilsTests {
    @Test func prettyPinyinSimple() async throws {
        #expect(
            createPrettyPinyin(pinyin: "shuai4 ge1") == "shuài gē"
        )
    }
    
    @Test func prettyPinyinRejectNoTone() async throws {
        #expect(
            createPrettyPinyin(pinyin: "ba") == "ba"
        )
        
        #expect(
            createPrettyPinyin(pinyin: "nu") == "nu"
        )
    }
    
    @Test func prettyPinyinRejectSingleLetter() async throws {
        #expect(
            createPrettyPinyin(pinyin: "a") == "a"
        )
    }
    
    @Test func prettyPinyinRejectSpecialCharacter() async throws {
        #expect(
            createPrettyPinyin(pinyin: "-") == "-"
        )
    }
    
    @Test func prettyPinyinSecondaryVowel() async throws {
        #expect(
            createPrettyPinyin(pinyin: "hui4 tu2") == "huì tú"
        )
    }
    
    @Test func prettyPinyinUmlaut() async throws {
        #expect(
            createPrettyPinyin(pinyin: "nu:3 hai2") == "nǚ hái"
        )
    }
    
    @Test func prettyPinyinTones() async throws {
        #expect(
            createPrettyPinyin(pinyin: "ma1 ma2 ma3 ma4 ma5") == "mā má mǎ mà ma"
        )
    }
    
    @Test func numberedPinyinSimple() async throws {
        #expect(
            createNumberedPinyin(pinyin: "nu:3 hai2") == "nü3 hai2"
        )
    }
    
    @Test func pinyinWithVSimple() async throws {
        #expect(
            createPinyinWithV(pinyin: "nu:3 hai2") == "nv3 hai2"
        )
    }
    
    @Test func pinyinToZhuyinSimple() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "ba1 da2 tong1") == "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ"
        )
    }
    
    @Test func pinyinToZhuyinRejectNoTone() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "ba") == "ba"
        )
    }
    
    @Test func pinyinToZhuyinRejectSingleLetter() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "a") == "a"
        )
    }
    
    @Test func pinyinToZhuyinRejectSpecialCharacter() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "-") == "-"
        )
    }
    
    @Test func pinyinToZhuyinNoSpaces() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "ba1da2tong1") == "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ"
        )
    }
    
    @Test func pinyinToZhuyinSpacesToSegment() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "ba1 da2 tong1", useSpacesToSegment: true) == "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ"
        )
    }
    
    @Test func pinyinToZhuyinSpecialInitial() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "qu4") == "ㄑㄩˋ"
        )
        
        #expect(
            convertPinyinToZhuyin(pinyin: "chi1") == "ㄔ"
        )
        
        #expect(
            convertPinyinToZhuyin(pinyin: "ri4") == "ㄖˋ"
        )
    }
    
    @Test func pinyinToZhuyinSpecialFinals() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "hm5") == "˙ㄏㄇ"
        )
        
        #expect(
            convertPinyinToZhuyin(pinyin: "hng5") == "˙ㄏㄫ"
        )
        
        #expect(
            convertPinyinToZhuyin(pinyin: "er2") == "ㄦˊ"
        )
    }
    
    @Test func pinyinToZhuyinErhua() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "quanr1") == "ㄑㄩㄢㄦ"
        )
    }
    
    @Test func pinyinToZhuyinMalformed() async throws {
        #expect(
            convertPinyinToZhuyin(pinyin: "chzng2 quanr1") == "chzng2 ㄑㄩㄢㄦ"
        )
    }
    
    @Test func segmentPinyinSimple() async throws {
        #expect(
            segmentPinyin(text: "guang3 dong1") == (true, ["guang3", "dong1"])
        )
    }
    
    @Test func segmentPinyinNoDigits() async throws {
        #expect(
            segmentPinyin(text: "guang dong") == (true, ["guang", "dong"])
        )
    }
    
    @Test func segmentPinyinNoSpaces() async throws {
        #expect(
            segmentPinyin(text: "guang3dong1") == (true, ["guang3", "dong1"])
        )
    }
    
    @Test func segmentPinyinNoDigitsNoSpaces() async throws {
        #expect(
            segmentPinyin(text: "guangdong") == (true, ["guang", "dong"])
        )
    }
    
    @Test func segmentPinyinNoDigitsApostrophe() async throws {
        #expect(
            segmentPinyin(text: "xi'an") == (true, ["xi", "an"])
        )
    }
    
    @Test func segmentPinyinDigitsApostrophe() async throws {
        #expect(
            segmentPinyin(text: "xi1'an") == (true, ["xi1", "an"])
        )
    }
    
    @Test func segmentPinyinRemoveSpecialCharacters() async throws {
        #expect(
            segmentPinyin(text: "guang。dong？") == (true, ["guang", "dong"])
        )
    }
    
    @Test func segmentPinyinKeepGlobCharacters() async throws {
        #expect(
            segmentPinyin(text: "guang* dong?", removeGlobCharacters: false) == (true, ["guang", "* ", "dong", "?"])
        )
    }
    
    @Test func segmentPinyinKeepGlobCharactersNoWhitespace() async throws {
        #expect(
            segmentPinyin(text: "guang*dong?", removeGlobCharacters: false) == (true, ["guang", "*", "dong", "?"])
        )
    }
    
    @Test func segmentPinyinKeepMultipleGlobCharactersNoWhitespace() async throws {
        #expect(
            segmentPinyin(text: "guang?* dong?", removeGlobCharacters: false) == (true, ["guang", "?", "* ", "dong", "?"])
        )
    }
    
    @Test func segmentPinyinKeepMultipleGlobCharactersWhitespace() async throws {
        #expect(
            segmentPinyin(text: "guang? * dong", removeGlobCharacters: false) == (true, ["guang", "? ", "* ", "dong"])
        )
    }
    
    @Test func segmentPinyinKeepMultipleGlobCharactersWhitespaceSurround() async throws {
        #expect(
            segmentPinyin(text: "guang ? * dong", removeGlobCharacters: false) == (true, ["guang", " ? ", "* ", "dong"])
        )
    }
    
    @Test func segmentPinyinGlobCharactersTrimWhitespace() async throws {
        #expect(
            segmentPinyin(text: "guang  ?            *      dong", removeGlobCharacters: false) == (true, ["guang", " ? ", "* ", "dong"])
        )
        
        #expect(
            segmentPinyin(text: "guang?* ?????", removeGlobCharacters: false) == (true, ["guang", "?", "* ", "?", "?", "?", "?", "?"])
        )
        
        #expect(
            segmentPinyin(text: "guang * ????*", removeGlobCharacters: false) == (true, ["guang", " * ", "?", "?", "?", "?", "*"])
        )
        
        #expect(
            segmentPinyin(text: "guang? dong*", removeGlobCharacters: false) == (true, ["guang", "? ", "dong", "*"])
        )
    }
    
    @Test func segmentPinyinKeepSpecialCharacters() async throws {
        #expect(
            segmentPinyin(text: "guang？ dong1", removeSpecialCharacters: false) == (true, ["guang", "？", "dong1"])
        )
    }
    
    @Test func segmentPinyinRemoveWhitespace() async throws {
        #expect(
            segmentPinyin(text: "  guang                           dong      ") == (true, ["guang", "dong"])
        )
    }
    
    @Test func segmentPinyinLower() async throws {
        #expect(
            segmentPinyin(text: "gUanGdOnG") == (true, ["guang", "dong"])
        )
    }
    
    @Test func segmentPinyinLowerWithDigits() async throws {
        #expect(
            segmentPinyin(text: "gUanG3dOnG1") == (true, ["guang3", "dong1"])
        )
    }
    
    @Test func segmentPinyinMultipleFinalsVowelsOnly() async throws {
        #expect(
            segmentPinyin(text: "ee") == (true, ["e", "e"])
        )
    }
    
    @Test func segmentPinyinMultipleFinals() async throws {
        #expect(
            segmentPinyin(text: "angang") == (true, ["ang", "ang"])
        )
    }
    
    @Test func segmentPinyinInvalidTone() async throws {
        #expect(
            segmentPinyin(text: "heng0") == (false, ["heng0"])
        )
        
        #expect(
            segmentPinyin(text: "heng6") == (false, ["heng6"])
        )
    }
    
    @Test func segmentPinyinGarbage() async throws {
        #expect(
            segmentPinyin(text: "kljnxclkjvnl") == (false, ["kljnxclkjvnl"])
        )
    }
    
    @Test func soundChangeZ() async throws {
        #expect(
            pinyinSoundChanges(text: ["zuan"]) == ["z(h)!uang!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["zuan3"]) == ["z(h)!uang!3"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["zuan?"]) == ["z(h)!uang!?"]
        )
    }
    
    @Test func soundChangeC() async throws {
        #expect(
            pinyinSoundChanges(text: ["cong"]) == ["c(h)!ong"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["cong1"]) == ["c(h)!ong1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["chong?"]) == ["c(h)!ong?"]
        )
    }
    
    @Test func soundChangeS() async throws {
        #expect(
            pinyinSoundChanges(text: ["se"]) == ["s(h)!e"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["se2"]) == ["s(h)!e2"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["se?"]) == ["s(h)!e?"]
        )
    }
    
    @Test func soundChangeN() async throws {
        #expect(
            pinyinSoundChanges(text: ["ni"]) == ["(n|l)i"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ni3"]) == ["(n|l)i3"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ni?"]) == ["(n|l)i?"]
        )
    }
    
    @Test func soundChangeR() async throws {
        #expect(
            pinyinSoundChanges(text: ["re"]) == ["(l|r)e"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["re4"]) == ["(l|r)e4"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["re?"]) == ["(l|r)e?"]
        )
    }
    
    @Test func soundChangeLN() async throws {
        #expect(
            pinyinSoundChanges(text: ["li"]) == ["(l|n)i"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lie4"]) == ["(l|n)ie4"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["liao?"]) == ["(l|n)iao?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["liu"]) == ["(l|n)iu"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lian2"]) == ["(l|n)iang!2"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lin?"]) == ["(l|n)ing!?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["liang"]) == ["(l|n)iang!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ling1"]) == ["(l|n)ing!1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lu:?"]) == ["(l|n)u:?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lu:e"]) == ["(l|n)u:e"]
        )
    }
    
    @Test func soundChangeLNR() async throws {
        #expect(
            pinyinSoundChanges(text: ["lang"]) == ["(l|n|r)ang!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lang4"]) == ["(l|n|r)ang!4"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["lang?"]) == ["(l|n|r)ang!?"]
        )
    }
    
    @Test func soundChangeAng() async throws {
        #expect(
            pinyinSoundChanges(text: ["bang"]) == ["bang!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["bang1"]) == ["bang!1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["bang?"]) == ["bang!?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ban"]) == ["bang!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ban1"]) == ["bang!1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["ban?"]) == ["bang!?"]
        )
    }
    
    @Test func soundChangeEng() async throws {
        #expect(
            pinyinSoundChanges(text: ["peng"]) == ["peng!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["peng4"]) == ["peng!4"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["peng?"]) == ["peng!?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pen"]) == ["peng!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pen1"]) == ["peng!1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pen?"]) == ["peng!?"]
        )
    }
    
    @Test func soundChangeIng() async throws {
        #expect(
            pinyinSoundChanges(text: ["bing"]) == ["bing!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["bing3"]) == ["bing!3"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["bing?"]) == ["bing!?"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pin"]) == ["ping!"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pin1"]) == ["ping!1"]
        )
        
        #expect(
            pinyinSoundChanges(text: ["pin?"]) == ["ping!?"]
        )
    }
}
