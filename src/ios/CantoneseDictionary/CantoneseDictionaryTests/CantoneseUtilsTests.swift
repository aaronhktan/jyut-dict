//
//  CantoneseUtilsTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-10.
//

import Testing
@testable import CantoneseDictionary

struct CantoneseUtilsTests {

    @Test func example() async throws {
        #expect(segmentJyutping(text:"m4 goi1") == (true, ["m4", "goi1"]))
    }

}
