from collections import namedtuple

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

    def add_jyutping(self, jyut):
        self.jyutping = jyut

    def add_fuzzy_jyutping(self, jyut):
        if self.fuzzy_jyutping == "":
            self.fuzzy_jyutping = jyut
        elif self.fuzzy_jyutping.find(jyut) == -1:
            self.fuzzy_jyutping += ", " + jyut

    def add_freq(self, freq):
        self.freq = freq


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
