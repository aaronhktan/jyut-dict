#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Run the scripts in the following order:
  - `setup_data.py`
  - `scrape.py`
  - `parse.py`
  - `fixup.py`
  - `parse.py`
- Specific usage instructions for each script are provided by the scripts themselves.
- **Run the scripts from the `dictionaries` folder, e.g. `python3 -m cantodict.setup_data <character URL output file> <compound word URL output file> <sentence URL output file>`.**

#### Scripts:
- `setup_data.py`
  - This script downloads the list of characters, words, and sentence URLs that will be scraped.
- `scrape.py`
  - This script downloads every webpage for each URL in the files created by `setup_data.py`.
- `parse.py`
  - This script generates a SQLite database from the webpages downloaded by `scrape.py`.
- `fixup.py`
  - This script attempts to re-download compound word webpages whose text failed to parse (usually compound words starting with roman characters, e.g. AA制)