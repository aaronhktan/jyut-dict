#### Usage:
- To install required packages: `pip install -r requirements.txt`
- Data files for Cantonese Readings from Pleco are located in the `cedict/data` subdirectory – I recommend using `FULLREADINGS.txt`, as it contains Cantonese pronunciations from both `READINGS.txt` and `CC-CANTO.txt`.
- Specific usage instructions for each script are provided by the scripts themselves.
- **Run the script from the `dictionaries` folder, e.g. `python3 -m cfdict-xml.parse <database filename> <CFDICT.xml file> <Cantonese readings file> <source name> <source short name> <source version> <source description> <source legal> <source link> <source update url> <source contents>`.**

#### Scripts:
- `parse.py`
  - This script generates a SQLite database file from the entries/definitions contained in a CEDICT-compatible XML format and readings from a Cantonese readings file created by `generate_readings.py`.
  - The purpose of this script is to generate a database file for CFDICT where there is only one XML file containing entries and definitions, and no Cantonese readings are provided.
  - **This script has been tested only with CFDICT. If you need to parse CEDICT, CC-CANTO, or HanDeDict, see instructions in the `cedict` directory.**
  - A copy of CFDICT.xml may be downloaded from this site: https://chine.in/mandarin/dictionnaire/CFDICT/
