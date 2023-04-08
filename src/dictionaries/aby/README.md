#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- **Run the scripts from the `dictionaries` folder, e.g. `python3 -m aby.parse <database filename> <jyut.u8 file> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source other>`.**

#### Scripts:
- `parse.py`
  - This script generates a SQLite database from the `jyut.u8` file provided by Wenlin's Developer Program. See [Wenlin's webpage](https://wenlin.com/developers) for more details.
  - Run `python3 -m aby.parse help` to view instructions.
