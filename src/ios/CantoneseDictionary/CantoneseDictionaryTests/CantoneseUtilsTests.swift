//
//  CantoneseUtilsTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-10.
//

import Testing

@testable import CantoneseDictionary

struct CantoneseUtilsTests {

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
