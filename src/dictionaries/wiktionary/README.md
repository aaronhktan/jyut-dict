#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- **Run the scripts from the `dictionaries` folder, e.g. `python3 -m wiktionary.parse <database filename> <wiktionary.json file> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source other>`.**

#### Scripts:
- `parse.py`
  - This script generates a SQLite database from the `wiktionary.json` file provided by Kaikki's wiktextract project. See [Kaikki's webpage](https://kaikki.org/dictionary/Chinese/nonsenses.html) or the [Github project](https://github.com/tatuylonen/wiktextract) for more details.
  - Run `python3 -m wiktionary.parse help` to view instructions.
