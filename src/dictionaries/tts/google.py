import requests

import base64
import logging
import os
import re
import sqlite3
import subprocess
import sys

TTS_URL = "https://texttospeech.googleapis.com/v1/text:synthesize"

language_settings = {
    "yue-HK": {
        "alphabet": "jyutping",
        "languageCode": "yue-HK",
        "regex": re.compile(r"(\w*?)(\d)"),
        "name": "yue-HK-Standard-C",
    },
    "cmn-CN": {
        "alphabet": "pinyin",
        "languageCode": "cmn-Hans-CN",
        "regex": re.compile(r"(\w*?)(\d)"),
        "name": "cmn-CN-Standard-C",
    },
    "cmn-TW": {
        "alphabet": "pinyin",
        "languageCode": "cmn-Hant-TW",
        "regex": re.compile(r"(\w*?)(\d)"),
        "name": "cmn-TW-Standard-B",
    },
}


def fetch_key():
    key = (
        subprocess.Popen(
            ["gcloud auth print-access-token"], shell=True, stdout=subprocess.PIPE
        )
        .stdout.read()
        .strip()
        .decode("utf-8")
    )
    logging.info(f"Fetched new key: {key}")
    return key


def fetch_pronunciation(output_path, syllable, lang, key):
    match = language_settings[lang]["regex"].fullmatch(syllable)

    if not match:
        logging.debug(f"Ignoring {syllable}")
        return 403

    if os.path.isfile(f"{output_path}{syllable}.mp3"):
        logging.debug(f"{syllable} already fetched, skipping")
        return 304

    logging.info(f"Getting {syllable}")

    initial_final = match.group(1)
    tone = match.group(2)

    headers = {
        "Content-Type": "application/json; charset=utf-8",
        "Authorization": f"Bearer {key}",
        "x-goog-user-project": "jyut-dictionary",
    }

    ssml = (
        "<speak>"
        f"<phoneme alphabet=\"{language_settings[lang]['alphabet']}\" ph=\"{syllable}\">饭</phoneme>"
        "</speak>"
    )

    data = (
        "{"
        "  'input': {"
        f"    'ssml': '{ssml}'"
        "  },"
        "  'voice': {"
        f"    'languageCode': '{language_settings[lang]['languageCode']}',"
        f"    'name': '{language_settings[lang]['name']}'"
        "  },"
        "  'audioConfig': {"
        "    'audioEncoding': 'MP3',"
        "    'effectsProfileId': ["
        "      'large-home-entertainment-class-device'"
        "    ],"
        "    'speakingRate': 0.8"
        "  }"
        "}"
    )

    response = requests.post(TTS_URL, headers=headers, data=data.encode())
    if response.status_code != 200:
        logging.error(
            f"There was an error fetching audio for syllable {syllable}"
            f", status code {response.status_code}"
        )
        logging.error(response.content)
        return response.status_code

    if len(response.content) > 0:
        content = response.json()
        audio_bytes = base64.b64decode(content["audioContent"])
        with open(f"{output_path}{syllable}.mp3", "wb") as binary_file:
            binary_file.write(audio_bytes)
            logging.info(f"Wrote {syllable}")

    return response.status_code


if __name__ == "__main__":
    logging.basicConfig(level="INFO")

    if len(sys.argv) != 4:
        print(
            (
                "Usage: python3 google.py <database filename> "
                "<output folder> <language code>"
            )
        )
        print("e.g. python3 google.py ./data/dict.db" "./data/files/")
        sys.exit(1)

    db_path = sys.argv[1]
    output_path = sys.argv[2]
    lang = sys.argv[3]

    if lang not in language_settings.keys():
        logging.error(
            f"Language not found, choose between [{', '.join(language_settings.keys())}]"
        )
        sys.exit(1)

    syllable_set = set()

    db = sqlite3.connect(db_path)
    c = db.cursor()

    c.execute(f"SELECT {language_settings[lang]['alphabet']} FROM entries")
    rows = c.fetchall()

    for row in rows:
        syllables = row[0].split()
        [syllable_set.add(x.replace("u:", "ü")) for x in syllables]

    key = fetch_key()

    for syllable in syllable_set:
        status_code = fetch_pronunciation(output_path, syllable, lang, key)
        while status_code == 400:
            key = fetch_key()
            status_code = fetch_pronunciation(output_path, syllable, lang, key)
