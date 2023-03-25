from collections import namedtuple
from dataclasses import dataclass, field
from typing import List

SourceTuple = namedtuple(
    "Source",
    [
        "name",
        "shortname",
        "version",
        "description",
        "legal",
        "link",
        "update_url",
        "other",
    ],
)


# Option 1 for representing an entry: this class gives a basic structure for an object with
# traditional, simplified, pinyin, jyutping, and definitions, with pinyin/jyutping/definitions
# able to be set later via add_pinyin/add_jyutping/add_defs functions.
class Entry(object):
    def __init__(self, trad="", simp="", pin="", jyut="", freq=0.0, defs=None):
        self.traditional = trad
        self.simplified = simp
        self.pinyin = pin
        self.jyutping = jyut  # An exact match
        self.fuzzy_jyutping = (
            ""  # A "fuzzy" match (i.e. jyutping matches traditional but not pinyin)
        )
        self.freq = freq
        self.definitions = defs if defs is not None else []

    def __str__(self):
        return (
            f"T:{self.traditional}; S:{self.simplified}; "
            f"P:{self.pinyin}; J:{self.jyutping}; "
            f"FJ:{self.fuzzy_jyutping}; F:{self.freq}; "
            f"D:{self.definitions}"
        )

    def __eq__(self, other):
        return (
            self.traditional == other.traditional
            and self.simplified == other.simplified
            and self.pinyin == other.pinyin
            and self.jyutping == other.jyutping
            and self.fuzzy_jyutping == other.fuzzy_jyutping
            and self.freq == other.freq
            and self.definitions == other.definitions
        )

    def __hash__(self):
        return hash(self.__str__())

    def add_traditional(self, trad):
        self.traditional = trad

    def add_simplified(self, simp):
        self.simplified = simp

    def add_pinyin(self, pin):
        self.pinyin = pin

    def add_jyutping(self, jyut):
        self.jyutping = jyut

    def add_fuzzy_jyutping(self, jyut):
        if self.fuzzy_jyutping == "":
            self.fuzzy_jyutping = jyut
        elif self.fuzzy_jyutping.find(jyut) == -1:
            self.fuzzy_jyutping += ", " + jyut

    def add_freq(self, freq):
        self.freq = freq

    def add_defs(self, defs):
        self.definitions = defs

    def append_to_defs(self, item):
        if isinstance(self.definitions, list):
            self.definitions.append(item)
        elif isinstance(self.definitions, set):
            if item in self.definitions:
                return False
            self.definitions.add(item)
            return True


# Option 2: same as option 1, but with separate definitions from a
# Mandarin source (cedict_eng) and a Cantonese source (canto_eng)
class EntryWithCantoneseAndMandarin(Entry):
    def __init__(
        self,
        trad="",
        simp="",
        pin="",
        jyut="",
        freq=0.0,
        cedict_eng=None,
        canto_eng=None,
    ):
        super(EntryWithCantoneseAndMandarin, self).__init__(
            trad=trad, simp=simp, pin=pin, jyut=jyut, freq=freq
        )
        self.cedict_english = cedict_eng if cedict_eng is not None else []
        self.canto_english = canto_eng if canto_eng is not None else []

    def add_canto_eng(self, canto_eng):
        self.canto_english.append(canto_eng)


# Basic structure for definitions with examples:
# one or more examples per DefinitionTuple,
# multiple DefinitionTuples in the defs array for the Entry object defined above, in this file
DefinitionTuple = namedtuple("Definition", ["definition", "label", "examples"])

# Basic stucture for an example:
# - field for language of this example (ISO 693-3), e.g. "cmn" for Mandarin
# - field for one pronunciation type (either Pinyin or Jyutping, not both)
# - field for content (either simplified or traditional, not both)
ExampleTuple = namedtuple("ExampleTuple", ["lang", "pron", "content"])

# More complicated structure for an example:
# - specifies an ID + may contain pronunciation for Mandarin and Cantonese +
#   may contain traditional and simplified variants
# - may be linked to one or more NonChineseSentence tuples
ChineseSentence = namedtuple(
    "ChineseSentence",
    [
        "id",
        "traditional",
        "simplified",
        "pinyin",
        "jyutping",
        "language",
    ],
)

NonChineseSentence = namedtuple(
    "NonChineseSentence",
    [
        "id",
        "sentence",
        "language",
    ],
)


@dataclass
class Example:
    lang: str = ""
    pron: str = ""
    content: str = ""

    def __hash__(self):
        return hash((self.lang, self.pron, self.content))

    def __eq__(self, other):
        if not isinstance(other, type(self)): return NotImplemented
        return self.lang == other.lang and self.pron == other.pron and self.content == other.content


@dataclass
class Definition:
    definition: str = ""
    label: str = ""
    examples: List[List[Example]] = field(default_factory=list)

    def __hash__(self):
        return hash((self.definition, self.label))

    def __eq__(self, other):
        if not isinstance(other, type(self)): return NotImplemented
        return self.definition == other.definition and self.label == other.label
