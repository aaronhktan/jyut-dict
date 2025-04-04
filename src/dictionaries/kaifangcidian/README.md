#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- Run the scripts from the `dictionaries` folder, e.g. `python3 -m kaifangcidian.parse <database filename>`.

#### Scripts:
- `parse.py`
  - This script generates a SQLite database file for all the words contained in the traditional and simplified source files from Kaifangcidian.
  - The purpose of this script is to generate a database file from the Traditional + Yale romanisation file (cidian_zhyue-ft-kfcd-ylshu-2019623.txt) and Simplified + Jyutping romanisation file (cidian_zhyue-jt-kfcd-yp-2019623.txt), so that all entries have a simplified + traditional script version, as well as simplified + traditional definitions in Mandarin.
  - Run `python3 -m kaifangcidian.parse help` to view instructions.
