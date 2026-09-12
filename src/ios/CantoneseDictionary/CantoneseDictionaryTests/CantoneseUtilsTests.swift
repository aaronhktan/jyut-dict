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
            convertJyutpingToYale(
                jyutping: "si1 zi2 saan1",
                useSpacesToSegment: true
            ) == "sī jí sāan"
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
            convertJyutpingToYale(
                jyutping: "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6"
            ) == "sāam gáu sei lìhng ńgh yih chāt baat luhk"
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

    @Test func jyutpingToIPASimple() async throws {
        #expect(
            convertJyutpingToIPA(jyutping: "joeng4 sing4") == "jœ̽ːŋ˨˩ sɪŋ˨˩"
        )
    }

    @Test func jyutpingToIPARejectNoTone() async throws {
        #expect(convertJyutpingToIPA(jyutping: "joeng") == "joeng")
    }

    @Test func jyutpingToIPARejectSingleLetter() async throws {
        #expect(convertJyutpingToIPA(jyutping: "a") == "a")
    }

    @Test func jyutpingToIPARejectSpecialCharacter() async throws {
        #expect(convertJyutpingToIPA(jyutping: "-") == "x")
    }

    @Test func jyutpingToIPANoSpaces() async throws {
        #expect(convertJyutpingToIPA(jyutping: "faa1sing4") == "fäː˥ sɪŋ˨˩")
    }

    @Test func jyutpingToIPASpacesToSegment() async throws {
        #expect(
            convertJyutpingToIPA(
                jyutping: "joeng4 sing4",
                useSpacesToSegment: true
            ) == "jœ̽ːŋ˨˩ sɪŋ˨˩"
        )
    }

    @Test func jyutpingToIPAPreprocessInitial() async throws {
        #expect(convertJyutpingToIPA(jyutping: "zyu2 sung3") == "t͡ʃyː˧˥ sʊŋ˧")
    }

    @Test func jyutpingToIPASpecialSyllable() async throws {
        #expect(convertJyutpingToIPA(jyutping: "m4") == "m̩˨˩")
    }

    @Test func jyutpingToIPACheckedTone() async throws {
        #expect(
            convertJyutpingToIPA(jyutping: "sik6 si2 o1 faan6")
                == "sɪk̚˨ siː˧˥ ɔː˥ fäːn˨"
        )
    }

    @Test func jyutpingToIPASpecialFinal() async throws {
        #expect(
            convertJyutpingToIPA(jyutping: "uk1 kei2 jan4")
                == "ʊk̚˥ kʰei̯˧˥ jɐn˨˩"
        )
    }

    @Test func jyutpingToIPATones() async throws {
        #expect(
            convertJyutpingToIPA(
                jyutping: "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6"
            ) == "säːm˥ kɐu̯˧˥ sei̯˧ lɪŋ˨˩ ŋ̍˩˧ jiː˨ t͡sʰɐt̚˥ päːt̚˧ lʊk̚˨"
        )
    }

    @Test func jyutpingToIPANoTone() async throws {
        #expect(convertJyutpingToIPA(jyutping: "mok") == "mok")
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

    @Test func autocorrectJyutpingHui() async throws {
        #expect(
            jyutpingAutocorrect(text: "hui") == "heoi"
        )

        #expect(
            jyutpingAutocorrect(text: "hui", unsafeSubstitutions: true)
                == "heoi"
        )

        #expect(
            jyutpingAutocorrect(text: "shui") == "seoi"
        )

        #expect(
            jyutpingAutocorrect(text: "shui", unsafeSubstitutions: true)
                == "seoi"
        )
    }

    @Test func autocorrectJyutpingCu() async throws {
        #expect(
            jyutpingAutocorrect(text: "cui") == "(k)(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "cui", unsafeSubstitutions: true)
                == "(k)(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "cum") == "(k)am"
        )

        #expect(
            jyutpingAutocorrect(text: "cum", unsafeSubstitutions: true)
                == "(k)am"
        )
    }

    @Test func autocorrectJyutpingX() async throws {
        #expect(
            jyutpingAutocorrect(text: "xuet") == "s(yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "xuet", unsafeSubstitutions: true)
                == "s(yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "xui") == "s(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "xui", unsafeSubstitutions: true)
                == "s(eo|u)i"
        )
    }

    @Test func autocorrectJyutpingCh() async throws {
        #expect(
            jyutpingAutocorrect(text: "choeng") == "coeng"
        )

        #expect(
            jyutpingAutocorrect(text: "choeng", unsafeSubstitutions: true)
                == "coeng"
        )

        #expect(
            jyutpingAutocorrect(text: "chi") == "ci"
        )

        #expect(
            jyutpingAutocorrect(text: "chi", unsafeSubstitutions: true) == "ci"
        )
    }

    @Test func autocorrectJyutpingSh() async throws {
        #expect(
            jyutpingAutocorrect(text: "shoeng") == "soeng"
        )

        #expect(
            jyutpingAutocorrect(text: "shoeng", unsafeSubstitutions: true)
                == "soeng"
        )

        #expect(
            jyutpingAutocorrect(text: "shi") == "si"
        )

        #expect(
            jyutpingAutocorrect(text: "shi", unsafeSubstitutions: true) == "si"
        )
    }

    @Test func autocorrectJyutpingZh() async throws {
        #expect(
            jyutpingAutocorrect(text: "zhoeng") == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zhoeng", unsafeSubstitutions: true)
                == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zhi") == "zi"
        )

        #expect(
            jyutpingAutocorrect(text: "zhi", unsafeSubstitutions: true) == "zi"
        )
    }

    @Test func autocorrectJyutpingEungErngEong() async throws {
        #expect(
            jyutpingAutocorrect(text: "zeung") == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zeung", unsafeSubstitutions: true)
                == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zerng") == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zerng", unsafeSubstitutions: true)
                == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zeong") == "zoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "zeong", unsafeSubstitutions: true)
                == "zoeng"
        )
    }

    @Test func autocorrectJyutpingEui() async throws {
        #expect(
            jyutpingAutocorrect(text: "zeui") == "zeoi"
        )

        #expect(
            jyutpingAutocorrect(text: "zeui", unsafeSubstitutions: true)
                == "zeoi"
        )
    }

    @Test func autocorrectJyutpingEuk() async throws {
        #expect(
            jyutpingAutocorrect(text: "seuk") == "soek"
        )

        #expect(
            jyutpingAutocorrect(text: "seuk", unsafeSubstitutions: true)
                == "soek"
        )
    }

    @Test func autocorrectJyutpingEun() async throws {
        #expect(
            jyutpingAutocorrect(text: "ceun") == "c(eo|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "ceun", unsafeSubstitutions: true)
                == "c(eo|yu)n"
        )
    }

    @Test func autocorrectJyutpingEut() async throws {
        #expect(
            jyutpingAutocorrect(text: "seut") == "s(eo|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "seut", unsafeSubstitutions: true)
                == "s(eo|yu)t"
        )
    }

    @Test func autocorrectJyutpingEu() async throws {
        #expect(
            jyutpingAutocorrect(text: "zeu") == "z(e|y)u"
        )

        #expect(
            jyutpingAutocorrect(text: "zeu", unsafeSubstitutions: true)
                == "z(e|y)u"
        )
    }

    @Test func autocorrectJyutpingErn() async throws {
        #expect(
            jyutpingAutocorrect(text: "zern") == "zeon"
        )

        #expect(
            jyutpingAutocorrect(text: "zern", unsafeSubstitutions: true)
                == "zeon"
        )
    }

    @Test func autocorrectJyutpingOen() async throws {
        #expect(
            jyutpingAutocorrect(text: "zoen") == "zeon"
        )

        #expect(
            jyutpingAutocorrect(text: "zoen", unsafeSubstitutions: true)
                == "zeon"
        )
    }

    @Test func autocorrectJyutpingAo() async throws {
        #expect(
            jyutpingAutocorrect(text: "gao") == "gau"
        )

        #expect(
            jyutpingAutocorrect(text: "gao", unsafeSubstitutions: true) == "gau"
        )

        #expect(
            jyutpingAutocorrect(text: "gaolyun") == "ga(ou!)l(ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "gaolyun", unsafeSubstitutions: true)
                == "gaul(ja|jyu|yu)n"
        )
    }

    @Test func autocorrectJyutpingAr() async throws {
        #expect(
            jyutpingAutocorrect(text: "char") == "caa"
        )

        #expect(
            jyutpingAutocorrect(text: "char", unsafeSubstitutions: true)
                == "caa"
        )
    }

    @Test func autocorrectJyutpingEe() async throws {
        #expect(
            jyutpingAutocorrect(text: "see") == "si"
        )

        #expect(
            jyutpingAutocorrect(text: "see", unsafeSubstitutions: true) == "si"
        )
    }

    @Test func autocorrectJyutpingAy() async throws {
        #expect(
            jyutpingAutocorrect(text: "hay") == "hei"
        )

        #expect(
            jyutpingAutocorrect(text: "hay", unsafeSubstitutions: true) == "hei"
        )
    }

    @Test func autocorrectJyutpingOy() async throws {
        #expect(
            jyutpingAutocorrect(text: "choy") == "coi"
        )

        #expect(
            jyutpingAutocorrect(text: "choy", unsafeSubstitutions: true)
                == "coi"
        )
    }

    @Test func autocorrectJyutpingOo() async throws {
        #expect(
            jyutpingAutocorrect(text: "soot") == "s(y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: "soot", unsafeSubstitutions: true)
                == "s(y!u)t"
        )
    }

    @Test func autocorrectJyutpingOng() async throws {
        #expect(
            jyutpingAutocorrect(text: "fong") == "f(o|u)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "fong", unsafeSubstitutions: true)
                == "f(o|u)ng"
        )
    }

    @Test func autocorrectJyutpingYoung() async throws {
        #expect(
            jyutpingAutocorrect(text: "young") == "j(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "young", unsafeSubstitutions: true)
                == "j(y!u|a|eo)ng"
        )
    }

    @Test func autocorrectJyutpingYue() async throws {
        #expect(
            jyutpingAutocorrect(text: "yuet") == "(j)(yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "yuet", unsafeSubstitutions: true)
                == "(j)(yu)t"
        )
    }

    @Test func autocorrectJyutpingUe() async throws {
        #expect(
            jyutpingAutocorrect(text: "tsuen") == "c(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "tsuen", unsafeSubstitutions: true)
                == "c(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "wantsuen") == "wants(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "wantsuen", unsafeSubstitutions: true)
                == "wanc(yu)n"
        )
    }

    @Test func autocorrectJyutpingTsz() async throws {
        #expect(
            jyutpingAutocorrect(text: "tsz") == "zi"
        )

        #expect(
            jyutpingAutocorrect(text: "tsz", unsafeSubstitutions: true) == "zi"
        )
    }

    @Test func autocorrectJyutpingCk() async throws {
        #expect(
            jyutpingAutocorrect(text: "back") == "bak"
        )

        #expect(
            jyutpingAutocorrect(text: "back", unsafeSubstitutions: true)
                == "bak"
        )
    }

    @Test func autocorrectJyutpingEy() async throws {
        #expect(
            jyutpingAutocorrect(text: "gey ") == "gei "
        )

        #expect(
            jyutpingAutocorrect(text: "gey ", unsafeSubstitutions: true)
                == "gei "
        )

        #expect(
            jyutpingAutocorrect(text: "gey'") == "gei'"
        )

        #expect(
            jyutpingAutocorrect(text: "gey'", unsafeSubstitutions: true)
                == "gei'"
        )

        #expect(
            jyutpingAutocorrect(text: "gey") == "gei"
        )

        #expect(
            jyutpingAutocorrect(text: "gey", unsafeSubstitutions: true) == "gei"
        )

        #expect(
            jyutpingAutocorrect(text: "geyhey") == "geihei"
        )

        #expect(
            jyutpingAutocorrect(text: "geyhey", unsafeSubstitutions: true)
                == "geihei"
        )

        // Test with an initial that is only valid for the -ei combo
        #expect(
            jyutpingAutocorrect(text: "peylou") == "peilou"
        )

        #expect(
            jyutpingAutocorrect(text: "peylou", unsafeSubstitutions: true)
                == "peilou"
        )

        // Test with an initial that is only valid for the -e j- combo
        #expect(
            jyutpingAutocorrect(text: "yeye") == "(j)e (j)e"
        )

        #expect(
            jyutpingAutocorrect(text: "yeye", unsafeSubstitutions: true)
                == "(j)e (j)e"
        )

        // Test with ambiguous initial + initial after the "ey"
        #expect(
            jyutpingAutocorrect(text: "beycaam") == "beicaam"
        )

        #expect(
            jyutpingAutocorrect(text: "beycaam", unsafeSubstitutions: true)
                == "beicaam"
        )

        // Test with ambiguous initial + non-initial after the "ey"
        #expect(
            jyutpingAutocorrect(text: "geye") == "ge (j)e"
        )

        #expect(
            jyutpingAutocorrect(text: "geye", unsafeSubstitutions: true)
                == "ge (j)e"
        )

        #expect(
            jyutpingAutocorrect(
                text: "geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey"
            )
                == "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei"
        )

        #expect(
            jyutpingAutocorrect(
                text: "geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey",
                unsafeSubstitutions: true
            )
                == "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei"
        )
    }

    @Test func autocorrectJyutpingOh() async throws {
        #expect(
            jyutpingAutocorrect(text: "moh ") == "mou "
        )

        #expect(
            jyutpingAutocorrect(text: "moh ", unsafeSubstitutions: true)
                == "mou "
        )

        #expect(
            jyutpingAutocorrect(text: "moh'") == "mou'"
        )

        #expect(
            jyutpingAutocorrect(text: "moh'", unsafeSubstitutions: true)
                == "mou'"
        )

        #expect(
            jyutpingAutocorrect(text: "moh") == "mou"
        )

        #expect(
            jyutpingAutocorrect(text: "moh", unsafeSubstitutions: true) == "mou"
        )

        #expect(
            jyutpingAutocorrect(text: "ohoh") == "ouou"
        )

        #expect(
            jyutpingAutocorrect(text: "ohoh", unsafeSubstitutions: true)
                == "ouou"
        )

        // Test with an initial that is only valid for the -ou combo
        #expect(
            jyutpingAutocorrect(text: "nohdoi") == "noudoi"
        )

        #expect(
            jyutpingAutocorrect(text: "nohdoi", unsafeSubstitutions: true)
                == "noudoi"
        )

        // Test with ambiguous initial + initial after the "oh"
        #expect(
            jyutpingAutocorrect(text: "lohjan") == "loujan"
        )

        #expect(
            jyutpingAutocorrect(text: "lohjan", unsafeSubstitutions: true)
                == "loujan"
        )

        // Test with ambiguous initial + non-initial after the "oh"
        #expect(
            jyutpingAutocorrect(text: "lohon") == "l(ou!) hon"
        )

        #expect(
            jyutpingAutocorrect(text: "lohon", unsafeSubstitutions: true)
                == "l(ou!) hon"
        )

        #expect(
            jyutpingAutocorrect(text: "mohmohmohlohonlohonlohjannohdoimoh moh")
                == "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou"
        )

        #expect(
            jyutpingAutocorrect(
                text: "mohmohmohlohonlohonlohjannohdoimoh moh",
                unsafeSubstitutions: true
            ) == "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou"
        )
    }

    @Test func autocorrectJyutpingOw() async throws {
        #expect(
            jyutpingAutocorrect(text: "gow ") == "gau "
        )

        #expect(
            jyutpingAutocorrect(text: "gow ", unsafeSubstitutions: true)
                == "gau "
        )

        #expect(
            jyutpingAutocorrect(text: "gow'") == "gau'"
        )

        #expect(
            jyutpingAutocorrect(text: "gow'", unsafeSubstitutions: true)
                == "gau'"
        )

        #expect(
            jyutpingAutocorrect(text: "gow") == "gau"
        )

        #expect(
            jyutpingAutocorrect(text: "gow", unsafeSubstitutions: true) == "gau"
        )

        // Test with an initial that is only valid for the -au combo
        #expect(
            jyutpingAutocorrect(text: "mow") == "mau"
        )

        #expect(
            jyutpingAutocorrect(text: "mow", unsafeSubstitutions: true) == "mau"
        )

        // Test with ambiguous initial + initial after the "ow"
        #expect(
            jyutpingAutocorrect(text: "towgai") == "taugai"
        )

        #expect(
            jyutpingAutocorrect(text: "towgai", unsafeSubstitutions: true)
                == "taugai"
        )

        #expect(
            jyutpingAutocorrect(text: "gowcat") == "gaucat"
        )

        #expect(
            jyutpingAutocorrect(text: "gowcat", unsafeSubstitutions: true)
                == "gaucat"
        )

        // Test with ambiguous initial + non-initial after the "ow"
        #expect(
            jyutpingAutocorrect(text: "howu") == "h(ou!) wu"
        )

        #expect(
            jyutpingAutocorrect(text: "ho wu", unsafeSubstitutions: true)
                == "h(ou!) wu"
        )

        #expect(
            jyutpingAutocorrect(text: "mowmowmowhowuho wu towgai")
                == "maumaumauh(ou!) wuh(ou!) wu taugai"
        )

        #expect(
            jyutpingAutocorrect(
                text: "mowmowmowhowuho wu towgai",
                unsafeSubstitutions: true
            ) == "maumaumauh(ou!) wuh(ou!) wu taugai"
        )
    }

    @Test func autocorrectJyutpingUm() async throws {
        #expect(
            jyutpingAutocorrect(text: "gum ") == "gam "
        )

        #expect(
            jyutpingAutocorrect(text: "gum ", unsafeSubstitutions: true)
                == "gam "
        )

        #expect(
            jyutpingAutocorrect(text: "gum'") == "gam'"
        )

        #expect(
            jyutpingAutocorrect(text: "gum'", unsafeSubstitutions: true)
                == "gam'"
        )

        #expect(
            jyutpingAutocorrect(text: "gum") == "gam"
        )

        #expect(
            jyutpingAutocorrect(text: "gum", unsafeSubstitutions: true) == "gam"
        )

        // Test with an initial that is only valid for the -am combo
        #expect(
            jyutpingAutocorrect(text: "bum") == "bam"
        )

        #expect(
            jyutpingAutocorrect(text: "bum", unsafeSubstitutions: true) == "bam"
        )

        #expect(
            jyutpingAutocorrect(text: "bumbumbumbum", unsafeSubstitutions: true)
                == "bambambambam"
        )

        // Test with an initial that is only valid for the -u m- combo
        #expect(
            jyutpingAutocorrect(text: "wumit") == "wumit"
        )

        #expect(
            jyutpingAutocorrect(text: "wumit", unsafeSubstitutions: true)
                == "wumit"
        )

        // Test with ambiguous initial + initial after the "um"
        #expect(
            jyutpingAutocorrect(text: "gumzau") == "gamzau"
        )

        #expect(
            jyutpingAutocorrect(text: "gumzau", unsafeSubstitutions: true)
                == "gamzau"
        )

        // Test with ambiguous initial + non-initial after the "um"
        #expect(
            jyutpingAutocorrect(text: "guman") == "gu man"
        )

        #expect(
            jyutpingAutocorrect(text: "guman", unsafeSubstitutions: true)
                == "gu man"
        )

        #expect(
            jyutpingAutocorrect(text: "gumangumzauwumitbumbumbumbum")
                == "gu mangamzauwumitbambambambam"
        )

        #expect(
            jyutpingAutocorrect(
                text: "gumangumzauwumitbumbumbumbum",
                unsafeSubstitutions: true
            ) == "gu mangamzauwumitbambambambam"
        )
    }

    @Test func autocorrectJyutpingYum() async throws {
        #expect(
            jyutpingAutocorrect(text: "yum ") == "(j)am "
        )

        #expect(
            jyutpingAutocorrect(text: "yum ", unsafeSubstitutions: true)
                == "(j)am "
        )

        #expect(
            jyutpingAutocorrect(text: "yum'") == "(j)am'"
        )

        #expect(
            jyutpingAutocorrect(text: "yum'", unsafeSubstitutions: true)
                == "(j)am'"
        )

        #expect(
            jyutpingAutocorrect(text: "yum") == "(j)am"
        )

        #expect(
            jyutpingAutocorrect(text: "yum", unsafeSubstitutions: true)
                == "(j)am"
        )

        // Test with an initial that is valid for the -yu m- combo
        #expect(
            jyutpingAutocorrect(text: "cyumat") == "cyu mat"
        )

        #expect(
            jyutpingAutocorrect(text: "cyumat", unsafeSubstitutions: true)
                == "cyu mat"
        )

        // Test with an initial that is only valid for the jam combo
        #expect(
            jyutpingAutocorrect(text: "syuyum") == "syu(j)am"
        )

        #expect(
            jyutpingAutocorrect(text: "syuyam", unsafeSubstitutions: true)
                == "syu(j)am"
        )

        #expect(
            jyutpingAutocorrect(text: "cyumatcyumatcyumatsyuyumyumyum")
                == "cyu matcyu matcyu matsyu(j)am(j)am(j)am"
        )

        #expect(
            jyutpingAutocorrect(
                text: "cyumatcyumatcyumatsyuyumyumyum",
                unsafeSubstitutions: true
            ) == "cyu matcyu matcyu macyu(j)am(j)am(j)am"
        )
    }

    @Test func autocorrectJyutpingYup() async throws {
        #expect(
            jyutpingAutocorrect(text: "yup ") == "(j)ap "
        )

        #expect(
            jyutpingAutocorrect(text: "yup ", unsafeSubstitutions: true)
                == "(j)ap "
        )

        #expect(
            jyutpingAutocorrect(text: "yup'") == "(j)ap'"
        )

        #expect(
            jyutpingAutocorrect(text: "yup'", unsafeSubstitutions: true)
                == "(j)ap'"
        )

        #expect(
            jyutpingAutocorrect(text: "yup") == "(j)ap"
        )

        #expect(
            jyutpingAutocorrect(text: "yup", unsafeSubstitutions: true)
                == "(j)ap"
        )

        // Test with an initial that is valid for the -yu p- combo
        #expect(
            jyutpingAutocorrect(text: "syupei") == "syu pei"
        )

        #expect(
            jyutpingAutocorrect(text: "syupei", unsafeSubstitutions: true)
                == "syu pei"
        )

        // Test with an initial that is only valid for the jap combo
        #expect(
            jyutpingAutocorrect(text: "zeonyup") == "zeon(j)ap"
        )

        #expect(
            jyutpingAutocorrect(text: "zeonyup", unsafeSubstitutions: true)
                == "zeon(j)ap"
        )

        #expect(
            jyutpingAutocorrect(text: "zeonyupzeonyupsyupeisyupeiyupyupzeonyup")
                == "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap"
        )

        #expect(
            jyutpingAutocorrect(
                text: "zeonyupzeonyupsyupeisyupeiyupyupzeonyup",
                unsafeSubstitutions: true
            ) == "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap"
        )
    }

    @Test func autocorrectJyutpingYuk() async throws {
        #expect(
            jyutpingAutocorrect(text: "yuk") == "(j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "yuk", unsafeSubstitutions: true)
                == "(j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: " yuk") == " (j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: " yuk", unsafeSubstitutions: true)
                == " (j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "geyyuk") == "gei(j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "geyyuk", unsafeSubstitutions: true)
                == "gei(j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "gey yuk") == "gei (j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "gey yuk", unsafeSubstitutions: true)
                == "gei (j)uk"
        )

        #expect(
            jyutpingAutocorrect(text: "jyukap") == "jyu kap"
        )

        #expect(
            jyutpingAutocorrect(text: "jyukap", unsafeSubstitutions: true)
                == "jyu kap"
        )

        #expect(
            jyutpingAutocorrect(text: "jyu kap") == "jyu kap"
        )

        #expect(
            jyutpingAutocorrect(text: "jyu kap", unsafeSubstitutions: true)
                == "jyu kap"
        )

        #expect(
            jyutpingAutocorrect(text: "jyu kapgey yukgeyyukyukyukyukyuk yuk")
                == "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk"
        )

        #expect(
            jyutpingAutocorrect(
                text: "jyu kapgey yukgeyyukyukyukyukyuk yuk",
                unsafeSubstitutions: true
            ) == "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk"
        )
    }

    @Test func autocorrectJyutpingYung() async throws {
        #expect(
            jyutpingAutocorrect(text: "yung") == "(j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "yung", unsafeSubstitutions: true)
                == "(j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: " yung") == " (j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: " yung", unsafeSubstitutions: true)
                == " (j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "gumyung") == "gam(j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "gumyung", unsafeSubstitutions: true)
                == "gam(j)(y!u|a|eo)ng"
        )

        #expect(
            jyutpingAutocorrect(text: "zyungaa") == "z(yu)n gaa"
        )

        #expect(
            jyutpingAutocorrect(text: "zyungaa", unsafeSubstitutions: true)
                == "z(yu)n gaa"
        )

        #expect(
            jyutpingAutocorrect(text: "jyungin") == "j(yu)n gin"
        )

        #expect(
            jyutpingAutocorrect(text: "jyungin", unsafeSubstitutions: true)
                == "j(yu)n gin"
        )

        #expect(
            jyutpingAutocorrect(text: "jyungingumyung yungyungyungzyungaa")
                == "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa"
        )

        #expect(
            jyutpingAutocorrect(
                text: "jyungingumyung yungyungyungzyungaa",
                unsafeSubstitutions: true
            )
                == "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa"
        )
    }

    @Test func autocorrectJyutpingYun() async throws {
        #expect(
            jyutpingAutocorrect(text: "yun") == "(j)(a|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "yun", unsafeSubstitutions: true)
                == "(j)(a|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: " yun") == " (ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: " yun", unsafeSubstitutions: true)
                == " (ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "gumyun") == "gam(ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "gumyun", unsafeSubstitutions: true)
                == "gam(ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "syuntau") == "sy(y!u|a|eo)ntau"
        )

        #expect(
            jyutpingAutocorrect(text: "syuntau", unsafeSubstitutions: true)
                == "sy(y!u|a|eo)ntau"
        )

        #expect(
            jyutpingAutocorrect(text: "syuntausyuntaugumyunyun yunyunyun")
                == "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n"
        )

        #expect(
            jyutpingAutocorrect(
                text: "syuntausyuntaugumyunyun yunyunyun",
                unsafeSubstitutions: true
            )
                == "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n"
        )
    }

    @Test func autocorrectJyutpingYut() async throws {
        #expect(
            jyutpingAutocorrect(text: "yut") == "(j)(a|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "yut", unsafeSubstitutions: true)
                == "(j)(a|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: " yut") == " (ja|jyu|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: " yut", unsafeSubstitutions: true)
                == " (ja|jyu|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "gamyut") == "gam(ja|jyu|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "gamyut", unsafeSubstitutions: true)
                == "gam(ja|jyu|yu)t"
        )

        #expect(
            jyutpingAutocorrect(text: "jyutjyu") == "j(yu)tjyu"
        )

        #expect(
            jyutpingAutocorrect(text: "jyutjyu", unsafeSubstitutions: true)
                == "j(yu)tjyu"
        )

        #expect(
            jyutpingAutocorrect(text: "zyutai") == "z(yu)tai"
        )

        #expect(
            jyutpingAutocorrect(text: "zyutai", unsafeSubstitutions: true)
                == "z(yu)tai"
        )

        #expect(
            jyutpingAutocorrect(text: "zyutaijyutjyugamyut yut yutyutyut")
                == "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t"
        )

        #expect(
            jyutpingAutocorrect(
                text: "zyutaijyutjyugamyut yut yutyutyut",
                unsafeSubstitutions: true
            )
                == "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t"
        )
    }

    @Test func autocorrectJyutpingUnsafe() async throws {
        #expect(
            jyutpingAutocorrect(text: "kwun") == "(g|k)w!(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "kwun", unsafeSubstitutions: true)
                == "(g|k)w!(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "baakwun") == "baakw(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "baakwun", unsafeSubstitutions: true)
                == "baa(g|k)w!(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "tsuen") == "c(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "tsuen", unsafeSubstitutions: true)
                == "c(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "wantsuen") == "wants(yu)n"
        )

        #expect(
            jyutpingAutocorrect(text: "wantsuen", unsafeSubstitutions: true)
                == "wanc(yu)n"
        )
    }

    @Test func autocorrectJyutpingY() async throws {
        #expect(
            jyutpingAutocorrect(text: "yaang") == "(j)aang"
        )

        #expect(
            jyutpingAutocorrect(text: "yaang", unsafeSubstitutions: true)
                == "(j)aang"
        )

        #expect(
            jyutpingAutocorrect(text: "yeng") == "(j)eng"
        )

        #expect(
            jyutpingAutocorrect(text: "yeng", unsafeSubstitutions: true)
                == "(j)eng"
        )

        #expect(
            jyutpingAutocorrect(text: "yuen") == "(j)y(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "yuen", unsafeSubstitutions: true)
                == "(j)y(y!u|a|eo)n"
        )
    }

    @Test func autocorrectJyutpingUi() async throws {
        #expect(
            jyutpingAutocorrect(text: "gui") == "g(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "gui", unsafeSubstitutions: true)
                == "g(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: " gui") == " g(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: " gui", unsafeSubstitutions: true)
                == " g(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "yumsui") == "(j)ams(eo|u)i"
        )

        #expect(
            jyutpingAutocorrect(text: "yumsui", unsafeSubstitutions: true)
                == "(j)ams(eo|u)i"
        )
    }

    @Test func autocorrectJyutpingUn() async throws {
        #expect(
            jyutpingAutocorrect(text: "gun") == "g(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "gun", unsafeSubstitutions: true)
                == "g(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: " gun") == " g(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: " gun", unsafeSubstitutions: true)
                == " g(y!u|a|eo)n"
        )

        #expect(
            jyutpingAutocorrect(text: "gunzoeng") == "g(y!u|a|eo)nzoeng"
        )

        #expect(
            jyutpingAutocorrect(text: "gunzoeng", unsafeSubstitutions: true)
                == "g(y!u|a|eo)nzoeng"
        )
    }

    @Test func autocorrectJyutpingUt() async throws {
        #expect(
            jyutpingAutocorrect(text: "gut") == "g(a|y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: "gut", unsafeSubstitutions: true)
                == "g(a|y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: " gut") == " g(a|y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: " gut", unsafeSubstitutions: true)
                == " g(a|y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: "gumgut") == "gamg(a|y!u)t"
        )

        #expect(
            jyutpingAutocorrect(text: "gumgut", unsafeSubstitutions: true)
                == "gamg(a|y!u)t"
        )
    }

    @Test func soundChangeNg() async throws {
        #expect(
            jyutpingSoundChanges(text: ["ng"]) == ["(ng|m)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ng4"]) == ["(ng|m)4"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ng?"]) == ["(ng|m)?"]
        )
    }

    @Test func soundChangeM() async throws {
        #expect(
            jyutpingSoundChanges(text: ["m"]) == ["(ng|m)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["m4"]) == ["(ng|m)4"]
        )

        #expect(
            jyutpingSoundChanges(text: ["m?"]) == ["(ng|m)?"]
        )
    }

    @Test func soundChangeNgInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["ngo"]) == ["(ng)!o"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ngo5"]) == ["(ng)!o5"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ngo?"]) == ["(ng)!o?"]
        )
    }

    @Test func soundChangeNullInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["o"]) == ["(ng)!o"]
        )

        #expect(
            jyutpingSoundChanges(text: ["o5"]) == ["(ng)!o5"]
        )

        #expect(
            jyutpingSoundChanges(text: ["o?"]) == ["(ng)!o?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ang"]) == ["(ng)!aa!ng!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ang2"]) == ["(ng)!aa!ng!2"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ang?"]) == ["(ng)!aa!ng!?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["uk"]) == ["(ng)!uk"]
        )

        #expect(
            jyutpingSoundChanges(text: ["uk1"]) == ["(ng)!uk1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["uk?"]) == ["(ng)!uk?"]
        )
    }

    @Test func soundChangeNInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["nei"]) == ["(n|l)ei"]
        )

        #expect(
            jyutpingSoundChanges(text: ["nei5"]) == ["(n|l)ei5"]
        )

        #expect(
            jyutpingSoundChanges(text: ["nei?"]) == ["(n|l)ei?"]
        )
    }

    @Test func soundChangeLInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["lei"]) == ["(n|l)ei"]
        )

        #expect(
            jyutpingSoundChanges(text: ["lei5"]) == ["(n|l)ei5"]
        )

        #expect(
            jyutpingSoundChanges(text: ["lei?"]) == ["(n|l)ei?"]
        )
    }

    @Test func soundChangeGwInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["gok"]) == ["(g|k)w!o(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["gok3"]) == ["(g|k)w!o(k|t)3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["gok?"]) == ["(g|k)w!o(k|t)?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["g(o|u)ng"]) == ["(g|k)w!(o|u)ng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["g(o|u)ng3"]) == ["(g|k)w!(o|u)ng3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["g(o|u)ng?"]) == ["(g|k)w!(o|u)ng?"]
        )
    }

    @Test func soundChangeKwInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["kok"]) == ["(g|k)w!o(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["kok3"]) == ["(g|k)w!o(k|t)3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["kok?"]) == ["(g|k)w!o(k|t)?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["k(o|u)ng"]) == ["(g|k)w!(o|u)ng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["k(o|u)ng3"]) == ["(g|k)w!(o|u)ng3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["k(o|u)ng?"]) == ["(g|k)w!(o|u)ng?"]
        )
    }

    @Test func soundChangeDInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["deng"]) == ["(d|t)eng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["deng1"]) == ["(d|t)eng1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["deng?"]) == ["(d|t)eng?"]
        )
    }

    @Test func soundChangeTInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["teng"]) == ["(d|t)eng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["teng1"]) == ["(d|t)eng1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["teng?"]) == ["(d|t)eng?"]
        )
    }

    @Test func soundChangeC() async throws {
        #expect(
            jyutpingSoundChanges(text: ["ceng"]) == ["(c|z)eng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ceng2"]) == ["(c|z)eng2"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ceng?"]) == ["(c|z)eng?"]
        )
    }

    @Test func soundChangeZ() async throws {
        #expect(
            jyutpingSoundChanges(text: ["zeng"]) == ["(c|z)eng"]
        )

        #expect(
            jyutpingSoundChanges(text: ["zeng2"]) == ["(c|z)eng2"]
        )

        #expect(
            jyutpingSoundChanges(text: ["zeng?"]) == ["(c|z)eng?"]
        )
    }

    @Test func soundChangeG() async throws {
        #expect(
            jyutpingSoundChanges(text: ["ging"]) == ["(g|k)ing"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ging1"]) == ["(g|k)ing1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["ging?"]) == ["(g|k)ing?"]
        )
    }

    @Test func soundChangeKInitial() async throws {
        #expect(
            jyutpingSoundChanges(text: ["king"]) == ["(g|k)ing"]
        )

        #expect(
            jyutpingSoundChanges(text: ["king1"]) == ["(g|k)ing1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["king?"]) == ["(g|k)ing?"]
        )
    }

    @Test func soundChangeAa() async throws {
        #expect(
            jyutpingSoundChanges(text: ["mak"]) == ["maa!(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mak6"]) == ["maa!(k|t)6"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mak?"]) == ["maa!(k|t)?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maak"]) == ["maa!(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maak1"]) == ["maa!(k|t)1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maak?"]) == ["maa!(k|t)?"]
        )
    }

    @Test func soundChangeAng() async throws {
        #expect(
            jyutpingSoundChanges(text: ["maang"]) == ["maa!ng!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maang4"]) == ["maa!ng!4"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maang?"]) == ["maa!ng!?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mang"]) == ["maa!ng!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mang1"]) == ["maa!ng!1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mang?"]) == ["maa!ng!?"]
        )
    }

    @Test func soundChangeOng() async throws {
        #expect(
            jyutpingSoundChanges(text: ["bong"]) == ["bong!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bong2"]) == ["bong!2"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bong?"]) == ["bong!?"]
        )
    }

    @Test func soundChangeAn() async throws {
        #expect(
            jyutpingSoundChanges(text: ["maan"]) == ["maa!ng!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maan4"]) == ["maa!ng!4"]
        )

        #expect(
            jyutpingSoundChanges(text: ["maan?"]) == ["maa!ng!?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["man"]) == ["maa!ng!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["man1"]) == ["maa!ng!1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["man?"]) == ["maa!ng!?"]
        )
    }

    @Test func soundChangeOn() async throws {
        #expect(
            jyutpingSoundChanges(text: ["mon"]) == ["mong!"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mon1"]) == ["mong!1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["mon?"]) == ["mong!?"]
        )
    }

    @Test func soundChangeTFinal() async throws {
        #expect(
            jyutpingSoundChanges(text: ["got"]) == ["(g|k)w!o(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["got3"]) == ["(g|k)w!o(k|t)3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["got?"]) == ["(g|k)w!o(k|t)?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bit"]) == ["bit"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bit6"]) == ["bit6"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bit?"]) == ["bit?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["but"]) == ["but"]
        )

        #expect(
            jyutpingSoundChanges(text: ["but6"]) == ["but6"]
        )

        #expect(
            jyutpingSoundChanges(text: ["but?"]) == ["but?"]
        )
    }

    @Test func soundChangeKFinal() async throws {
        #expect(
            jyutpingSoundChanges(text: ["gok"]) == ["(g|k)w!o(k|t)"]
        )

        #expect(
            jyutpingSoundChanges(text: ["gok3"]) == ["(g|k)w!o(k|t)3"]
        )

        #expect(
            jyutpingSoundChanges(text: ["gok?"]) == ["(g|k)w!o(k|t)?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bik"]) == ["bik"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bik1"]) == ["bik1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["bik?"]) == ["bik?"]
        )

        #expect(
            jyutpingSoundChanges(text: ["buk"]) == ["buk"]
        )

        #expect(
            jyutpingSoundChanges(text: ["buk1"]) == ["buk1"]
        )

        #expect(
            jyutpingSoundChanges(text: ["buk?"]) == ["buk?"]
        )
    }

    @Test func testCasesClarence() async throws {
        let inputExpected: [String: [String]] = [
            "seut goh": ["s(eo|yu)(k|t)", "(g|k)w!ou"],
            "gongyeun": ["(g|k)w!(o|u)ng", "(j)(eo|yu)n"],
            "tsum4yut6": ["(c|z)aa!m4", "(jaa!|jyu|yu)(k|t)6"],
            "tsum yut": ["(c|z)aa!m", "(jaa!|jyu|yu)(k|t)"],
            "gum yut": ["(g|k)aa!m", "(jaa!|jyu|yu)(k|t)"],
            "gum man": ["(g|k)aa!m", "maa!ng!"],
            "sum": ["saa!m"],
            "sun": ["s(y!u|aa!|eo)n"],
            "cheung": ["(c|z)oeng"],
            "chun": ["(c|z)(y!u|aa!|eo)n"],
            "hui": ["heoi"],
            "yutback": ["(j)(aa!|yu)(k|t)", "baa!(k|t)"],
        ]

        for (input, expected) in inputExpected {
            let intermediate = jyutpingAutocorrect(
                text: input,
                unsafeSubstitutions: true
            )
            let (validJyutping, segmented) =
                segmentJyutping(
                    text: intermediate,
                    removeSpecialCharacters: true,
                    removeGlobCharacters: false,
                    removeRegexCharacters: false
                )
            let result = jyutpingSoundChanges(text: segmented)
            #expect(result == expected)
        }
    }
    
    @Test func testCasesMichelle() async throws {
        let inputExpected: [String: [String]] = [
            "xuet go": ["s(yu)(k|t)", "(g|k)w!(ou!)"],
            "gong yuen": ["(g|k)w!(o|u)ng", "(j)y(y!u|aa!|eo)n"],
            "kum yut": ["(g|k)aa!m", "(jaa!|jyu|yu)(k|t)"],
            "gai suen gay": ["(g|k)aa!i", "s(yu)n", "(g|k)ei"],
            "yut bak": ["(j)(aa!|yu)(k|t)", "baa!(k|t)"],
        ]

        for (input, expected) in inputExpected {
            let intermediate = jyutpingAutocorrect(
                text: input,
                unsafeSubstitutions: true
            )
            let (validJyutping, segmented) =
                segmentJyutping(
                    text: intermediate,
                    removeSpecialCharacters: true,
                    removeGlobCharacters: false,
                    removeRegexCharacters: false
                )
            let result = jyutpingSoundChanges(text: segmented)
            #expect(result == expected)
        }
    }
    
    @Test func testCasesYvonne() async throws {
        let inputExpected: [String: [String]] = [
            "shyut go": ["s(yu)(k|t)", "(g|k)w!(ou!)"],
            "gong yun": ["(g|k)w!(o|u)ng", "(jaa!|jyu|yu)n"],
            "cum yut": ["(k)aa!m", "(jaa!|jyu|yu)(k|t)"],
            "cheun": ["(c|z)(eo|yu)n"],
            "gai syun gei": ["(g|k)aa!i", "sy(y!u|aa!|eo)n", "(g|k)ei"],
            "yut baat": ["(j)(aa!|yu)(k|t)", "baa!(k|t)"],
        ]

        for (input, expected) in inputExpected {
            let intermediate = jyutpingAutocorrect(
                text: input,
                unsafeSubstitutions: true
            )
            let (validJyutping, segmented) =
                segmentJyutping(
                    text: intermediate,
                    removeSpecialCharacters: true,
                    removeGlobCharacters: false,
                    removeRegexCharacters: false
                )
            let result = jyutpingSoundChanges(text: segmented)
            #expect(result == expected)
        }
    }
    
    @Test func testCasesAaron() async throws {
        let inputExpected: [String: [String]] = [
            "hagao": ["haa!", "(g|k)aa!u"],
            "josun": ["(j|z)(ou!)", "s(y!u|aa!|eo)n"],
            "sun": ["s(y!u|aa!|eo)n"],
            "jo ye": ["(j|z)(ou!)", "(j)e"],
            "choysum": ["(c|z)oi", "saa!m"],
            "shiumai": ["siu", "maa!i"],
            "fongzow": ["f(o|u)ng", "(c|z)aa!u"],
            "jesungdoongmut":
            ["(j|z)e", "s(y!u|aa!|eo)ng", "(d|t)(y!u)ng", "m(aa!|y!u)(k|t)"],
        ]

        for (input, expected) in inputExpected {
            let intermediate = jyutpingAutocorrect(
                text: input,
                unsafeSubstitutions: true
            )
            let (validJyutping, segmented) =
                segmentJyutping(
                    text: intermediate,
                    removeSpecialCharacters: true,
                    removeGlobCharacters: false,
                    removeRegexCharacters: false
                )
            let result = jyutpingSoundChanges(text: segmented)
            #expect(result == expected)
        }
    }
}
