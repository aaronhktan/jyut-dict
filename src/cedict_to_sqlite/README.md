#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Data files for CC-CEDICT/CC-CANTO and Cantonese Readings from Pleco are located in the `data` subdirectory.
- Specific usage instructions for each script are provided by the scripts themselves.

#### Scripts:
- `generate-readings.py`
  - This script outputs a file that contains Cantonese pronunciations for every term in CC-CANTO *and* the Cantonese readings file provided by Pleco.
  - The purpose of the outputted Cantonese pronunciation file is to provide Cantonese readings for other dictionaries, such as CFDICT or HanDeDict, that do not come with Cantonese pronunciations.
- `merge.py`
  - This script outputs a SQLite database file that contains all the entries and definitions from two input databases.
  - The purpose of this script is to merge multiple databases into a single file for distribution (such as CEDICT and CC-CANTO, which are designed to be used together).
- `script-individual.py`
  - This script generates a SQLite database file from the entries/definitions contained in a CEDICT-compatible format and readings from a Cantonese readings file created by `generate_readings.py`.
  - The purpose of this script is to generate a database file for CFDict, HanDeDict, etc. where there is only one file containing entries and definitions, and no Cantonese readings are provided.
- `script-set.py`
  - This script generates a SQLite database file from:
    - the entries/definitions contained in CEDICT (or a CEDICT-compatible format)
    - the entries/definitions contained in CC-CANTO (or a CC-CANTO-compatible format)
    - the readings provided by Pleco (or a Pleco-readings-compatible format)
  - The purpose of this script is to generate a database file where there is one file that contains Mandarin entries/definitions (e.g. CEDICT), another file that contains Cantonese entries/definitions (e.g. CC-CANTO), and another file that contains Cantonese pronunciations for Mandarin words (e.g. the Pleco readings file).
