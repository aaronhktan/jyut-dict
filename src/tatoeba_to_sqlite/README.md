##### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for each script are provided by the scripts themselves.

##### Scripts:
- `tatoeba.py`
  - This script generates a SQLite database file for all sentences and translations between a language pair specified by their ISO-639-3 language codes (e.g. eng for English, fra for French, cmn for Mandarin, yue for Cantonese).
  - The purpose of this script is to generate a database file from a sentences.csv file (showing sentences themselves) and links.csv (translations between those sentences). It also traverses the sentence graph to find sentences that are one indirect translation away (e.g. Cantonese -> English -> French).
- `merge.py`
  - This script generates a SQLite database file that contains all sentences from two input databases.
  - The purpose of this script is to merge multiple databases into a single file for distribution.
