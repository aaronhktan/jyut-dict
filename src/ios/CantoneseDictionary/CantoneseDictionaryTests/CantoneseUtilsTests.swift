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
    
    @Test func jyutpingToIPASimple() async throws {
        #expect(convertJyutpingToIPA(jyutping: "joeng4 sing4") == "jœ̽ːŋ˨˩ sɪŋ˨˩")
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
        #expect(convertJyutpingToIPA(jyutping: "joeng4 sing4", useSpacesToSegment: true) == "jœ̽ːŋ˨˩ sɪŋ˨˩")
    }
    
    @Test func jyutpingToIPAPreprocessInitial() async throws {
        #expect(convertJyutpingToIPA(jyutping: "zyu2 sung3") == "t͡ʃyː˧˥ sʊŋ˧")
    }
    
    @Test func jyutpingToIPASpecialSyllable() async throws {
        #expect(convertJyutpingToIPA(jyutping: "m4") == "m̩˨˩")
    }
    
    @Test func jyutpingToIPACheckedTone() async throws {
        #expect(convertJyutpingToIPA(jyutping: "sik6 si2 o1 faan6") == "sɪk̚˨ siː˧˥ ɔː˥ fäːn˨")
    }
    
    @Test func jyutpingToIPASpecialFinal() async throws {
        #expect(convertJyutpingToIPA(jyutping: "uk1 kei2 jan4") == "ʊk̚˥ kʰei̯˧˥ jɐn˨˩")
    }
    
    @Test func jyutpingToIPATones() async throws {
        #expect(convertJyutpingToIPA(jyutping: "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6") == "säːm˥ kɐu̯˧˥ sei̯˧ lɪŋ˨˩ ŋ̍˩˧ jiː˨ t͡sʰɐt̚˥ päːt̚˧ lʊk̚˨")
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
            jyutpingAutocorrect(text: "hui", unsafeSubstitutions: true) == "heoi"
        )
        
        #expect(
            jyutpingAutocorrect(text: "shui") == "seoi"
        )
        
        #expect(
            jyutpingAutocorrect(text: "shui", unsafeSubstitutions: true) == "seoi"
        )
    }
    
    @Test func autocorrectJyutpingCu() async throws {
        #expect(
            jyutpingAutocorrect(text: "cui") == "(k)(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cui", unsafeSubstitutions: true) == "(k)(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cum") == "(k)am"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cum", unsafeSubstitutions: true) == "(k)am"
        )
    }
    
    @Test func autocorrectJyutpingX() async throws {
        #expect(
            jyutpingAutocorrect(text: "xuet") == "s(yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "xuet", unsafeSubstitutions: true) == "s(yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "xui") == "s(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "xui", unsafeSubstitutions: true) == "s(eo|u)i"
        )
    }
    
    @Test func autocorrectJyutpingCh() async throws {
        #expect(
            jyutpingAutocorrect(text: "choeng") == "coeng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "choeng", unsafeSubstitutions: true) == "coeng"
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
            jyutpingAutocorrect(text: "shoeng", unsafeSubstitutions: true) == "soeng"
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
            jyutpingAutocorrect(text: "zhoeng", unsafeSubstitutions: true) == "zoeng"
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
            jyutpingAutocorrect(text: "zeung", unsafeSubstitutions: true) == "zoeng"
        )
    
        #expect(
            jyutpingAutocorrect(text: "zerng") == "zoeng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zerng", unsafeSubstitutions: true) == "zoeng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeong") == "zoeng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeong", unsafeSubstitutions: true) == "zoeng"
        )
    }
    
    @Test func autocorrectJyutpingEui() async throws {
        #expect(
            jyutpingAutocorrect(text: "zeui") == "zeoi"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeui", unsafeSubstitutions: true) == "zeoi"
        )
    }
    
    @Test func autocorrectJyutpingEuk() async throws {
        #expect(
            jyutpingAutocorrect(text: "seuk") == "soek"
        )
        
        #expect(
            jyutpingAutocorrect(text: "seuk", unsafeSubstitutions: true) == "soek"
        )
    }
    
    @Test func autocorrectJyutpingEun() async throws {
        #expect(
            jyutpingAutocorrect(text: "ceun") == "c(eo|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "ceun", unsafeSubstitutions: true) == "c(eo|yu)n"
        )
    }
    
    @Test func autocorrectJyutpingEut() async throws {
        #expect(
            jyutpingAutocorrect(text: "seut") == "s(eo|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "seut", unsafeSubstitutions: true) == "s(eo|yu)t"
        )
    }
    
    @Test func autocorrectJyutpingEu() async throws {
        #expect(
            jyutpingAutocorrect(text: "zeu") == "z(e|y)u"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeu", unsafeSubstitutions: true) == "z(e|y)u"
        )
    }
    
    @Test func autocorrectJyutpingErn() async throws {
        #expect(
            jyutpingAutocorrect(text: "zern") == "zeon"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zern", unsafeSubstitutions: true) == "zeon"
        )
    }
    
    @Test func autocorrectJyutpingOen() async throws {
        #expect(
            jyutpingAutocorrect(text: "zoen") == "zeon"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zoen", unsafeSubstitutions: true) == "zeon"
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
            jyutpingAutocorrect(text: "gaolyun", unsafeSubstitutions: true) == "gaul(ja|jyu|yu)n"
        )
    }
    
    @Test func autocorrectJyutpingAr() async throws {
        #expect(
            jyutpingAutocorrect(text: "char") == "caa"
        )
        
        #expect(
            jyutpingAutocorrect(text: "char", unsafeSubstitutions: true) == "caa"
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
            jyutpingAutocorrect(text: "choy", unsafeSubstitutions: true) == "coi"
        )
    }
    
    @Test func autocorrectJyutpingOo() async throws {
        #expect(
            jyutpingAutocorrect(text: "soot") == "s(y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "soot", unsafeSubstitutions: true) == "s(y!u)t"
        )
    }
    
    @Test func autocorrectJyutpingOng() async throws {
        #expect(
            jyutpingAutocorrect(text: "fong") == "f(o|u)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "fong", unsafeSubstitutions: true) == "f(o|u)ng"
        )
    }
    
    @Test func autocorrectJyutpingYoung() async throws {
        #expect(
            jyutpingAutocorrect(text: "young") == "j(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "young", unsafeSubstitutions: true) == "j(y!u|a|eo)ng"
        )
    }
    
    @Test func autocorrectJyutpingYue() async throws {
        #expect(
            jyutpingAutocorrect(text: "yuet") == "(j)(yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yuet", unsafeSubstitutions: true) == "(j)(yu)t"
        )
    }
    
    @Test func autocorrectJyutpingUe() async throws {
        #expect(
            jyutpingAutocorrect(text: "tsuen") == "c(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "tsuen", unsafeSubstitutions: true) == "c(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "wantsuen") == "wants(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "wantsuen", unsafeSubstitutions: true) == "wanc(yu)n"
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
            jyutpingAutocorrect(text: "back", unsafeSubstitutions: true) == "bak"
        )
    }
    
    @Test func autocorrectJyutpingEy() async throws {
        #expect(
            jyutpingAutocorrect(text: "gey ") == "gei "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gey ", unsafeSubstitutions: true) == "gei "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gey'") == "gei'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gey'", unsafeSubstitutions: true) == "gei'"
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
            jyutpingAutocorrect(text: "geyhey", unsafeSubstitutions: true) == "geihei"
        )
        
        // Test with an initial that is only valid for the -ei combo
        #expect(
            jyutpingAutocorrect(text: "peylou") == "peilou"
        )
        
        #expect(
            jyutpingAutocorrect(text: "peylou", unsafeSubstitutions: true) == "peilou"
        )
        
        // Test with an initial that is only valid for the -e j- combo
        #expect(
            jyutpingAutocorrect(text: "yeye") == "(j)e (j)e"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yeye", unsafeSubstitutions: true) == "(j)e (j)e"
        )
        
        // Test with ambiguous initial + initial after the "ey"
        #expect(
            jyutpingAutocorrect(text: "beycaam") == "beicaam"
        )
        
        #expect(
            jyutpingAutocorrect(text: "beycaam", unsafeSubstitutions: true) == "beicaam"
        )
        
        // Test with ambiguous initial + non-initial after the "ey"
        #expect(
            jyutpingAutocorrect(text: "geye") == "ge (j)e"
        )
        
        #expect(
            jyutpingAutocorrect(text: "geye", unsafeSubstitutions: true) == "ge (j)e"
        )
        
        #expect(
            jyutpingAutocorrect(text: "geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey") == "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei"
        )
        
        #expect(
            jyutpingAutocorrect(text: "geyegeye beycaamyeyeyeyeyeyeyepeylougeyheygeygey'gey", unsafeSubstitutions: true) == "ge (j)ege (j)e beicaam(j)e (j)e (j)e (j)e (j)e (j)e (j)epeilougeiheigeigei'gei"
        )
    }
    
    @Test func autocorrectJyutpingOh() async throws {
        #expect(
            jyutpingAutocorrect(text: "moh ") == "mou "
        )
        
        #expect(
            jyutpingAutocorrect(text: "moh ", unsafeSubstitutions: true) == "mou "
        )
        
        #expect(
            jyutpingAutocorrect(text: "moh'") == "mou'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "moh'", unsafeSubstitutions: true) == "mou'"
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
            jyutpingAutocorrect(text: "ohoh", unsafeSubstitutions: true) == "ouou"
        )
        
        // Test with an initial that is only valid for the -ou combo
        #expect(
            jyutpingAutocorrect(text: "nohdoi") == "noudoi"
        )
        
        #expect(
            jyutpingAutocorrect(text: "nohdoi", unsafeSubstitutions: true) == "noudoi"
        )
        
        // Test with ambiguous initial + initial after the "oh"
        #expect(
            jyutpingAutocorrect(text: "lohjan") == "loujan"
        )
        
        #expect(
            jyutpingAutocorrect(text: "lohjan", unsafeSubstitutions: true) == "loujan"
        )
        
        // Test with ambiguous initial + non-initial after the "oh"
        #expect(
            jyutpingAutocorrect(text: "lohon") == "l(ou!) hon"
        )
        
        #expect(
            jyutpingAutocorrect(text: "lohon", unsafeSubstitutions: true) == "l(ou!) hon"
        )
        
        #expect(
            jyutpingAutocorrect(text: "mohmohmohlohonlohonlohjannohdoimoh moh") == "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou"
        )
        
        #expect(
            jyutpingAutocorrect(text: "mohmohmohlohonlohonlohjannohdoimoh moh", unsafeSubstitutions: true) == "moumoumoul(ou!) honl(ou!) honloujannoudoimou mou"
        )
    }
    
    @Test func autocorrectJyutpingOw() async throws {
        #expect(
            jyutpingAutocorrect(text: "gow ") == "gau "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gow ", unsafeSubstitutions: true) == "gau "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gow'") == "gau'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gow'", unsafeSubstitutions: true) == "gau'"
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
            jyutpingAutocorrect(text: "towgai", unsafeSubstitutions: true) == "taugai"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gowcat") == "gaucat"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gowcat", unsafeSubstitutions: true) == "gaucat"
        )
        
        // Test with ambiguous initial + non-initial after the "ow"
        #expect(
            jyutpingAutocorrect(text: "howu") == "h(ou!) wu"
        )
        
        #expect(
            jyutpingAutocorrect(text: "ho wu", unsafeSubstitutions: true) == "h(ou!) wu"
        )
        
        #expect(
            jyutpingAutocorrect(text: "mowmowmowhowuho wu towgai") == "maumaumauh(ou!) wuh(ou!) wu taugai"
        )
        
        #expect(
            jyutpingAutocorrect(text: "mowmowmowhowuho wu towgai", unsafeSubstitutions: true) == "maumaumauh(ou!) wuh(ou!) wu taugai"
        )
    }
    
    @Test func autocorrectJyutpingUm() async throws {
        #expect(
            jyutpingAutocorrect(text: "gum ") == "gam "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gum ", unsafeSubstitutions: true) == "gam "
        )
        
        #expect(
            jyutpingAutocorrect(text: "gum'") == "gam'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gum'", unsafeSubstitutions: true) == "gam'"
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
            jyutpingAutocorrect(text: "bumbumbumbum", unsafeSubstitutions: true) == "bambambambam"
        )
        
        // Test with an initial that is only valid for the -u m- combo
        #expect(
            jyutpingAutocorrect(text: "wumit") == "wumit"
        )
        
        #expect(
            jyutpingAutocorrect(text: "wumit", unsafeSubstitutions: true) == "wumit"
        )
        
        // Test with ambiguous initial + initial after the "um"
        #expect(
            jyutpingAutocorrect(text: "gumzau") == "gamzau"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumzau", unsafeSubstitutions: true) == "gamzau"
        )
        
        // Test with ambiguous initial + non-initial after the "um"
        #expect(
            jyutpingAutocorrect(text: "guman") == "gu man"
        )
        
        #expect(
            jyutpingAutocorrect(text: "guman", unsafeSubstitutions: true) == "gu man"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumangumzauwumitbumbumbumbum") == "gu mangamzauwumitbambambambam"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumangumzauwumitbumbumbumbum", unsafeSubstitutions: true) == "gu mangamzauwumitbambambambam"
        )
    }
    
    @Test func autocorrectJyutpingYum() async throws {
        #expect(
            jyutpingAutocorrect(text: "yum ") == "(j)am "
        )
        
        #expect(
            jyutpingAutocorrect(text: "yum ", unsafeSubstitutions: true) == "(j)am "
        )
        
        #expect(
            jyutpingAutocorrect(text: "yum'") == "(j)am'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yum'", unsafeSubstitutions: true) == "(j)am'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yum") == "(j)am"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yum", unsafeSubstitutions: true) == "(j)am"
        )
        
        // Test with an initial that is valid for the -yu m- combo
        #expect(
            jyutpingAutocorrect(text: "cyumat") == "cyu mat"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cyumat", unsafeSubstitutions: true) == "cyu mat"
        )
        
        // Test with an initial that is only valid for the jam combo
        #expect(
            jyutpingAutocorrect(text: "syuyum") == "syu(j)am"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syuyam", unsafeSubstitutions: true) == "syu(j)am"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cyumatcyumatcyumatsyuyumyumyum") == "cyu matcyu matcyu matsyu(j)am(j)am(j)am"
        )
        
        #expect(
            jyutpingAutocorrect(text: "cyumatcyumatcyumatsyuyumyumyum", unsafeSubstitutions: true) == "cyu matcyu matcyu macyu(j)am(j)am(j)am"
        )
    }
    
    @Test func autocorrectJyutpingYup() async throws {
        #expect(
            jyutpingAutocorrect(text: "yup ") == "(j)ap "
        )
        
        #expect(
            jyutpingAutocorrect(text: "yup ", unsafeSubstitutions: true) == "(j)ap "
        )
        
        #expect(
            jyutpingAutocorrect(text: "yup'") == "(j)ap'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yup'", unsafeSubstitutions: true) == "(j)ap'"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yup") == "(j)ap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yup", unsafeSubstitutions: true) == "(j)ap"
        )
        
        // Test with an initial that is valid for the -yu p- combo
        #expect(
            jyutpingAutocorrect(text: "syupei") == "syu pei"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syupei", unsafeSubstitutions: true) == "syu pei"
        )
        
        // Test with an initial that is only valid for the jap combo
        #expect(
            jyutpingAutocorrect(text: "zeonyup") == "zeon(j)ap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeonyup", unsafeSubstitutions: true) == "zeon(j)ap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeonyupzeonyupsyupeisyupeiyupyupzeonyup") == "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zeonyupzeonyupsyupeisyupeiyupyupzeonyup", unsafeSubstitutions: true) == "zeon(j)apzeon(j)apsyu peisyu pei(j)ap(j)apzeon(j)ap"
        )
    }
    
    @Test func autocorrectJyutpingYuk() async throws {
        #expect(
            jyutpingAutocorrect(text: "yuk") == "(j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yuk", unsafeSubstitutions: true) == "(j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yuk") == " (j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yuk", unsafeSubstitutions: true) == " (j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "geyyuk") == "gei(j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "geyyuk", unsafeSubstitutions: true) == "gei(j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gey yuk") == "gei (j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gey yuk", unsafeSubstitutions: true) == "gei (j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyukap") == "jyu kap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyukap", unsafeSubstitutions: true) == "jyu kap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyu kap") == "jyu kap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyu kap", unsafeSubstitutions: true) == "jyu kap"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyu kapgey yukgeyyukyukyukyukyuk yuk") == "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyu kapgey yukgeyyukyukyukyukyuk yuk", unsafeSubstitutions: true) == "jyu kapgei (j)ukgei(j)uk(j)uk(j)uk(j)uk(j)uk (j)uk"
        )
    }
    
    @Test func autocorrectJyutpingYung() async throws {
        #expect(
            jyutpingAutocorrect(text: "yung") == "(j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yung", unsafeSubstitutions: true) == "(j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yung") == " (j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yung", unsafeSubstitutions: true) == " (j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumyung") == "gam(j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumyung", unsafeSubstitutions: true) == "gam(j)(y!u|a|eo)ng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyungaa") == "z(yu)n gaa"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyungaa", unsafeSubstitutions: true) == "z(yu)n gaa"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyungin") == "j(yu)n gin"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyungin", unsafeSubstitutions: true) == "j(yu)n gin"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyungingumyung yungyungyungzyungaa") == "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyungingumyung yungyungyungzyungaa", unsafeSubstitutions: true) == "j(yu)n gingam(j)(y!u|a|eo)ng (j)(y!u|a|eo)ng(j)(y!u|a|eo)ng(j)(y!u|a|eo)ngz(yu)n gaa"
        )
    }
    
    @Test func autocorrectJyutpingYun() async throws {
        #expect(
            jyutpingAutocorrect(text: "yun") == "(j)(a|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yun", unsafeSubstitutions: true) == "(j)(a|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yun") == " (ja|jyu|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yun", unsafeSubstitutions: true) == " (ja|jyu|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumyun") == "gam(ja|jyu|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumyun", unsafeSubstitutions: true) == "gam(ja|jyu|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syuntau") == "sy(y!u|a|eo)ntau"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syuntau", unsafeSubstitutions: true) == "sy(y!u|a|eo)ntau"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syuntausyuntaugumyunyun yunyunyun") == "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "syuntausyuntaugumyunyun yunyunyun", unsafeSubstitutions: true) == "sy(y!u|a|eo)ntausy(y!u|a|eo)ntaugam(ja|jyu|yu)n(ja|jyu|yu)n (ja|jyu|yu)n(ja|jyu|yu)n(ja|jyu|yu)n"
        )
    }
    
    @Test func autocorrectJyutpingYut() async throws {
        #expect(
            jyutpingAutocorrect(text: "yut") == "(j)(a|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yut", unsafeSubstitutions: true) == "(j)(a|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yut") == " (ja|jyu|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: " yut", unsafeSubstitutions: true) == " (ja|jyu|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gamyut") == "gam(ja|jyu|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gamyut", unsafeSubstitutions: true) == "gam(ja|jyu|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyutjyu") == "j(yu)tjyu"
        )
        
        #expect(
            jyutpingAutocorrect(text: "jyutjyu", unsafeSubstitutions: true) == "j(yu)tjyu"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyutai") == "z(yu)tai"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyutai", unsafeSubstitutions: true) == "z(yu)tai"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyutaijyutjyugamyut yut yutyutyut") == "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "zyutaijyutjyugamyut yut yutyutyut", unsafeSubstitutions: true) == "z(yu)taij(yu)tjyugam(ja|jyu|yu)t (ja|jyu|yu)t (ja|jyu|yu)t(ja|jyu|yu)t(ja|jyu|yu)t"
        )
    }
    
    @Test func autocorrectJyutpingUnsafe() async throws {
        #expect(
            jyutpingAutocorrect(text: "kwun") == "(g|k)w!(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "kwun", unsafeSubstitutions: true) == "(g|k)w!(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "baakwun") == "baakw(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "baakwun", unsafeSubstitutions: true) == "baa(g|k)w!(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "tsuen") == "c(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "tsuen", unsafeSubstitutions: true) == "c(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "wantsuen") == "wants(yu)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "wantsuen", unsafeSubstitutions: true) == "wanc(yu)n"
        )
    }
    
    @Test func autocorrectJyutpingY() async throws {
        #expect(
            jyutpingAutocorrect(text: "yaang") == "(j)aang"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yaang", unsafeSubstitutions: true) == "(j)aang"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yeng") == "(j)eng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yeng", unsafeSubstitutions: true) == "(j)eng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yuen") == "(j)y(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yuen", unsafeSubstitutions: true) == "(j)y(y!u|a|eo)n"
        )
    }
    
    @Test func autocorrectJyutpingUi() async throws {
        #expect(
            jyutpingAutocorrect(text: "gui") == "g(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gui", unsafeSubstitutions: true) == "g(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gui") == " g(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gui", unsafeSubstitutions: true) == " g(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yumsui") == "(j)ams(eo|u)i"
        )
        
        #expect(
            jyutpingAutocorrect(text: "yumsui", unsafeSubstitutions: true) == "(j)ams(eo|u)i"
        )
    }
    
    @Test func autocorrectJyutpingUn() async throws {
        #expect(
            jyutpingAutocorrect(text: "gun") == "g(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gun", unsafeSubstitutions: true) == "g(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gun") == " g(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gun", unsafeSubstitutions: true) == " g(y!u|a|eo)n"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gunzoeng") == "g(y!u|a|eo)nzoeng"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gunzoeng", unsafeSubstitutions: true) == "g(y!u|a|eo)nzoeng"
        )
    }
    
    @Test func autocorrectJyutpingUt() async throws {
        #expect(
            jyutpingAutocorrect(text: "gut") == "g(a|y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gut", unsafeSubstitutions: true) == "g(a|y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gut") == " g(a|y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: " gut", unsafeSubstitutions: true) == " g(a|y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumgut") == "gamg(a|y!u)t"
        )
        
        #expect(
            jyutpingAutocorrect(text: "gumgut", unsafeSubstitutions: true) == "gamg(a|y!u)t"
        )
    }
}
