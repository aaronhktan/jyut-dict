package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Test

class DefinitionsSetUnitTest {
    @Test
    fun definitionsSet_isEmpty_simple() {
        val ds = DefinitionsSet("CC-CANTO", "CCY", listOf())
        assertEquals(true, ds.isEmpty())
    }

    @Test
    fun definitionsSet_clone_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val secondDefinitionsSet = definitionsSet.clone()

        assertEquals(false, definitionsSet === secondDefinitionsSet)
        assertEquals(true, definitionsSet == secondDefinitionsSet)
        assertEquals(true, definitionsSet.hashCode() == secondDefinitionsSet.hashCode())

        val secondDefinition = Definition(
            "(historical) Qingyuan (a former county of Guangdong, China)",
            label = "noun",
            examples = listOf()
        )
        secondDefinitionsSet.appendDefinition(secondDefinition)
        assertEquals(false, definitionsSet == secondDefinitionsSet)
        assertEquals(false, definitionsSet.hashCode() == secondDefinitionsSet.hashCode())
    }

    @Test
    fun definitionsSet_equals_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val secondDefinition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val secondDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                secondDefinition
            )
        )

        val definitionsSetRef = definitionsSet
        assertEquals(true, definitionsSet.equals(definitionsSetRef))
        assertEquals(false, definitionsSet.equals(1))

        assertEquals(false, definitionsSet === secondDefinitionsSet)
        assertEquals(true, definitionsSet == secondDefinitionsSet)
    }

    @Test
    fun definitionsSet_hashCode_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        val secondDefinition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val secondDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                secondDefinition
            )
        )

        assertEquals(false, definitionsSet === secondDefinitionsSet)
        assertEquals(true, definitionsSet.hashCode() == secondDefinitionsSet.hashCode())
    }

    @Test
    fun definitionsSet_getDefinitionsSnippet_simple() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                definition
            )
        )

        assertEquals(
            "Qingyuan, prefecture-level city in Guangdong",
            definitionsSet.getDefinitionSnippet()
        )
    }

    @Test
    fun definitionsSet_getDefinitionsSnippet_empty() {
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf()
        )

        assertEquals(true, definitionsSet.isEmpty())
        assertEquals("", definitionsSet.getDefinitionSnippet())
    }

    @Test
    fun definitionsSet_definitions_copy() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val definitionList = listOf(
            definition
        )
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", definitionList
        )

        assertEquals(definitionList, definitionsSet.definitions)
        assertFalse(definitionList === definitionsSet.definitions)
        assertFalse(definitionsSet.definitions === definitionsSet.definitions)
    }
}