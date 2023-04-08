#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- Run the scripts from the `dictionaries` folder, e.g. `python3 -m moedict.parse <database filename> <dict-revised.json>`.

#### Scripts:
- `parse.py`
  - This script generates a SQLite database file for all entries provided by the dict-revised.json file downloaded from https://github.com/g0v/moedict-data.
  - Run `python3 -m moedict.parse help` to view instructions.
