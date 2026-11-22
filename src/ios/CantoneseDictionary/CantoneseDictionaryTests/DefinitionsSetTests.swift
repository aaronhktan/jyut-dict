//
//  DefinitionsSetTests.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-21.
//

import Foundation
import SwiftUI
import Testing

@testable import CantoneseDictionary

struct DefinitionsSetTests {
  @Test func isEmptySimple() async throws {
    let ds = DefinitionsSet(source: "CC-CANTO", sourceShortString: "CCY", definitions: [])
    #expect(ds.isEmpty == true)
  }

  @Test func cloneSimple() async throws {
    var definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])

    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let secondDefinitionsSet = definitionsSet
    #expect(definitionsSet == secondDefinitionsSet)

    definition.definitionContent = "(historical) Qingyuan (a former county of Guangdong, China)"
    #expect(definitionsSet == secondDefinitionsSet)
    #expect(
      definitionsSet.definitions[0].definitionContent
        == "Qingyuan, prefecture-level city in Guangdong")
    #expect(
      secondDefinitionsSet.definitions[0].definitionContent
        == "Qingyuan, prefecture-level city in Guangdong")
  }

  @Test func equalsSimple() async throws {
    let definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])
    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    let secondDefinition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])
    let secondDefinitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [secondDefinition])

    // In Swift, equality is based on just the ID, and those should differ even
    // if the contents are the same
    #expect(definitionsSet != secondDefinitionsSet)
  }

  @Test func definitionSnippetSimple() async throws {
    let definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])
    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    #expect(definitionsSet.definitionSnippet == "Qingyuan, prefecture-level city in Guangdong")
  }

  @Test func definitionSnippetEmpty() async throws {
    let definition = Definition(
      definitionContent: "", label: "", examples: [])
    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [definition])

    #expect(definitionsSet.definitionSnippet == "")
  }

  @Test func definitionsCopy() async throws {
    var definition = Definition(
      definitionContent: "Qingyuan, prefecture-level city in Guangdong", label: "", examples: [])
    let definitionList = [definition]
    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: definitionList)
    #expect(definitionsSet.definitions == definitionList)

    definition.definitionContent = "(historical) Qingyuan (a former county of Guangdong, China)"
    #expect(definitionList[0] != definition)
    #expect(definitionsSet.definitions == definitionList)
  }

  @Test func sourceSimple() async throws {
    let definitionsSet = DefinitionsSet(
      source: "CC-CANTO", sourceShortString: "CCY", definitions: [])
    #expect(definitionsSet.source == "CC-CANTO")
    #expect(definitionsSet.sourceShortString == "CCY")
  }
}
