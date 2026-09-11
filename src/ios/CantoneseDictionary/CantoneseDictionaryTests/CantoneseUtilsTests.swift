//
//  CantoneseUtilsTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-10.
//

import Testing

@testable import CantoneseDictionary

struct CantoneseUtilsTests {
    @Test func jyutpingToYaleSimple() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "si1 zi2 saan1") == "sī jí sāan"
        )
    }
    
    @Test func jyutpingToYaleRejectNoTone() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "joeng") == "joeng"
        )
    }
    
    @Test func jyutpingToYaleRejectSingleLetter() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "a") == "a"
        )
    }
    
    @Test func jyutpingToYaleRejectSpecialCharacter() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "-") == "x"
        )
    }
    
    @Test func jyutpingToYaleNoSpaces() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "si1zi2saan1") == "sī jí sāan"
        )
    }
    
    @Test func jyutpingToYaleSpacesToSegment() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "si1 zi2 saan1", useSpacesToSegment: true) == "sī jí sāan"
        )
    }
    
    @Test func jyutpingToYaleSpecialFinal() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "goek3jyun5") == "geuk yúhn"
        )
    }
    
    @Test func jyutpingToYaleLightTone() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "lok6 jyu5") == "lohk yúh"
        )
    }
    
    @Test func jyutpingToYaleSpecialSyllable() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "m4 hai6") == "m̀h haih"
        )
    }
    
    @Test func jyutpingToYaleTones() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6") == "sāam gáu sei lìhng ńgh yih chāt baat luhk"
        )
    }
    
    @Test func jyutpingToYaleNoTone() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "mit") == "mit"
        )
    }
    
    @Test func jyutpingToYaleMalformedTone() async throws {
        #expect(
            convertJyutpingToYale(jyutping: "mat 7") == "x"
        )
    }

    @Test func segmentJyutpingSimple() async throws {
        #expect(segmentJyutping(text: "m4 goi1") == (true, ["m4", "goi1"]))
    }

    @Test func segmentJyutpingNoDigits() async throws {
        #expect(segmentJyutping(text: "m goi") == (true, ["m", "goi"]))
    }

    @Test func segmentJyutpingNoSpaces() async throws {
        #expect(segmentJyutping(text: "m4goi1") == (true, ["m4", "goi1"]))
    }

    @Test func segmentJyutpingNoDigitsNoSpaces() async throws {
        #expect(segmentJyutping(text: "mgoi") == (true, ["m", "goi"]))
    }

    @Test func segmentJyutpingNoDigitsApostrophe() async throws {
        #expect(segmentJyutping(text: "m'aam") == (true, ["m", "aam"]))
    }

    @Test func segmentJyutpingDigitsApostrophe() async throws {
        #expect(segmentJyutping(text: "m4'aam") == (true, ["m4", "aam"]))
    }

    @Test func segmentJyutpingRemoveSpecialCharacters() async throws {
        #expect(segmentJyutping(text: "m*aam") == (true, ["m", "aam"]))
    }

    @Test func segmentJyutpingKeepGlobCharacters() async throws {
        #expect(
            segmentJyutping(
                text: "m* goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "* ", "goi"])
        )
    }

    @Test func segmentJyutpingKeepGlobCharactersNoWhitespace() async throws {
        #expect(
            segmentJyutping(
                text: "m*goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "*", "goi"])
        )
    }

    @Test func segmentJyutpingKeepMultipleGlobCharacters() async throws {
        #expect(
            segmentJyutping(
                text: "m?* goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "?", "* ", "goi"])
        )
    }

    @Test func segmentJyutpingKeepMultipleGlobCharactersWhitespace()
        async throws
    {
        #expect(
            segmentJyutping(
                text: "m? * goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "? ", "* ", "goi"])
        )
    }

    @Test func segmentJyutpingKeepMultipleGlobCharactersWhitespaceSurround()
        async throws
    {
        #expect(
            segmentJyutping(
                text: "m ? * goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", " ? ", "* ", "goi"])
        )
    }

    @Test func segmentJyutpingGlobCharactersTrimWhitespace() async throws {
        #expect(
            segmentJyutping(
                text: "m  ?            *      goi",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", " ? ", "* ", "goi"])
        )

        #expect(
            segmentJyutping(
                text: "m?* ????",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "?", "* ", "?", "?", "?", "?"])
        )

        #expect(
            segmentJyutping(
                text: "m * ????*",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", " * ", "?", "?", "?", "?", "*"])
        )

        #expect(
            segmentJyutping(
                text: "m? goi*",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "? ", "goi", "*"])
        )
    }

    @Test func segmentJyutpingKeepSpecialCharacters() async throws {
        #expect(
            segmentJyutping(text: "m？ goi", removeSpecialCharacters: false) == (
                true, ["m", "？", "goi"]
            )
        )
    }

    @Test func segmentJyutpingRemoveWhitespace() async throws {
        #expect(
            segmentJyutping(
                text: "  m                           goi      ",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["m", "goi"])
        )
    }

    @Test func segmentJyutpingLower() async throws {
        #expect(segmentJyutping(text: "mGoI") == (true, ["m", "goi"]))
    }

    @Test func segmentJyutpingLowerWithDigits() async throws {
        #expect(segmentJyutping(text: "m4GoI1") == (true, ["m4", "goi1"]))
    }

    @Test func segmentJyutpingMultipleFinalsVowelsOnly() async throws {
        #expect(
            segmentJyutping(
                text: "aaaa",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["aa", "aa"])
        )
    }

    @Test func segmentJyutpingMultipleFinalsConsonantsOnly() async throws {
        #expect(
            segmentJyutping(
                text: "ngng",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["ng", "ng"])
        )
    }

    @Test func segmentJyutpingMultipleFinals() async throws {
        #expect(
            segmentJyutping(
                text: "amam",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (true, ["am", "am"])
        )
    }

    @Test func segmentJyutpingInvalidTone() async throws {
        #expect(
            segmentJyutping(
                text: "hang0",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (false, ["hang0"])
        )

        #expect(
            segmentJyutping(
                text: "hang7",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (false, ["hang7"])
        )
    }

    @Test func segmentJyutpingGarbage() async throws {
        #expect(
            segmentJyutping(
                text: "kljnxclkjvnl",
                removeSpecialCharacters: true,
                removeGlobCharacters: false
            ) == (false, ["kljnxclkjvnl"])
        )
    }
}
