package com.aaronhktan.cantonesedictionary

import com.aaronhktan.cantonesedictionary.logic.entry.Definition
import com.aaronhktan.cantonesedictionary.logic.entry.DefinitionsSet
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class DefinitionsSetUnitTest {
    @Test
    fun definitionsSet_isEmpty_simple() {
        val ds = DefinitionsSet("CC-CANTO", "CCY", listOf())
        assertEquals(true, ds.isEmpty())
    }

    @Test
    fun definitionsSet_isEmpty_false() {
        val definition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "",
            examples = listOf()
        )
        val ds = DefinitionsSet("CC-CANTO", "CCY", listOf(definition))
        assertEquals(false, ds.isEmpty())
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

        assertFalse(definitionsSet === secondDefinitionsSet)
        assertTrue(definitionsSet == secondDefinitionsSet)
        assertTrue(definitionsSet.hashCode() == secondDefinitionsSet.hashCode())

        val secondDefinition = Definition(
            "(historical) Qingyuan (a former county of Guangdong, China)",
            label = "noun",
            examples = listOf()
        )
        secondDefinitionsSet.appendDefinition(secondDefinition)
        assertFalse(definitionsSet == secondDefinitionsSet)
        assertFalse(definitionsSet.hashCode() == secondDefinitionsSet.hashCode())
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

        assertFalse(definitionsSet === secondDefinitionsSet)
        assertTrue(definitionsSet == secondDefinitionsSet)

        val thirdDefinitionsSet = definitionsSet.clone()
        assertFalse(definitionsSet === thirdDefinitionsSet)
        assertTrue(definitionsSet == thirdDefinitionsSet)
    }

    @Test
    fun definitionsSet_equals_false() {
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
            "(historical) Qingyuan (a former county of Guangdong, China)",
            label = "",
            examples = listOf()
        )
        val secondDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                secondDefinition
            )
        )

        assertFalse(definitionsSet === secondDefinitionsSet)
        assertFalse(definitionsSet == secondDefinitionsSet)

        val thirdDefinition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "noun",
            examples = listOf()
        )
        val thirdDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                thirdDefinition
            )
        )
        assertFalse(definitionsSet === thirdDefinitionsSet)
        assertFalse(definitionsSet == thirdDefinitionsSet)
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

        assertFalse(definitionsSet === secondDefinitionsSet)
        assertTrue(definitionsSet.hashCode() == secondDefinitionsSet.hashCode())
    }

    @Test
    fun definitionsSet_hashCode_false() {
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
            "(historical) Qingyuan (a former county of Guangdong, China)",
            label = "",
            examples = listOf()
        )
        val secondDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                secondDefinition
            )
        )

        assertFalse(definitionsSet === secondDefinitionsSet)
        assertFalse(definitionsSet.hashCode() == secondDefinitionsSet.hashCode())

        val thirdDefinition = Definition(
            "Qingyuan, prefecture-level city in Guangdong",
            label = "noun",
            examples = listOf()
        )
        val thirdDefinitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf(
                thirdDefinition
            )
        )
        assertFalse(definitionsSet === thirdDefinitionsSet)
        assertFalse(definitionsSet.hashCode() == thirdDefinitionsSet.hashCode())
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

        assertTrue(definitionList == definitionsSet.definitions)
        assertFalse(definitionList === definitionsSet.definitions)
        assertFalse(definitionsSet.definitions === definitionsSet.definitions)
    }

    @Test
    fun definitionsSet_source_simple() {
        val definitionsSet = DefinitionsSet(
            "CC-CANTO", "CCY", listOf()
        )

        assertEquals("CC-CANTO", definitionsSet.source)
        assertEquals("CCY", definitionsSet.sourceShortString)
    }
}