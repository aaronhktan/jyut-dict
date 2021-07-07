#### Usage:
- Run the scripts from the `dictionaries` folder, e.g. `python3 -m database.merge <output database> <input database 1> <input database 2>`.

#### Scripts:
- `merge.py`
  - This script outputs a SQLite database file that contains all the entries and definitions from two input databases.
  - The purpose of this script is to merge multiple databases into a single file for distribution (such as CEDICT and CC-CANTO, which are designed to be used together).