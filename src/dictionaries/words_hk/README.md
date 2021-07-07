#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Run the scripts in the following order:
  - `setup_data.py`
  - `scrape.py`
  - `parse.py`
- Specific usage instructions for each script are provided by the scripts themselves.
- Run the scripts from the `dictionaries` folder, e.g. `python3 -m cedict.parse-set <database filename> <CC_CEDICT file> <CC_CANTO file> <Cantonese Readings file>`.

#### Scripts:
- `setup_data.py`
  - This script downloads the list of words that will be scraped. Since some words are hidden behind a user login, the script also opens a Selenium browser window so that the user can log in to words.hk.
- `scrape.py`
  - This script downloads every webpage for each words in the list of words previously downloaded by `setup_data.py`.
- `parse.py`
  - This script generates a SQLite database from the webpages downloaded by `scrape.py`. Note that this script only parses words.hk webpages from **July 2020** - the format has since changed but this script has not yet been updated!