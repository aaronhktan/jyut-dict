#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Specific usage instructions for the script are provided by the script itself.
- **Run the scripts from the `dictionaries` folder, e.g. `python3 -m wiki.parse <database filename> <page.db file> <langlinks.db file> <source language> <destination language> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source contents>`.**

#### Scripts:
- `parse.py`
  - This script generates a SQLite database from dumped `langlinks` and `page` SQL files provided by Wikimedia. For example, the dumped files for the Cantonese Wikipedia are available [here](https://dumps.wikimedia.org/zh_yuewiki/20240201/). The langlinks file is called `zh_yuewiki-<DATE>-langlinks.sql.gz`, and the page file is called `zh_yuewiki-<DATE>-page.sql.gz`. These files must be extracted, then converted from MySQL to SQLite3 format using something like [mysql2sqlite](https://github.com/dumblob/mysql2sqlite). **Remove all unique constraints from the generated SQLite3 command**, otherwise your will be missing a significant number of articles!
  - Run `python3 -m wiki.parse help` to view instructions.
