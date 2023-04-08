#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- Run the scripts from the `dictionaries` folder, e.g. `python3 -m two_shores_three_places.parse <database filename> <dict-同實異名.json>`.

#### Scripts:
- `parse.py`
  - This script generates a SQLite database file for all entries provided by the 兩岸三地生活差異詞語彙編-同實異名.csv and 兩岸三地生活差異詞語彙編-同名異實.csv files downloaded from https://github.com/g0v/moedict-data-csld.
  - Run `python3 -m two_shores_three_places.parse help` to view instructions.
