import requests

import base64
import logging
import os
import re
import sqlite3
import sys

JYUTPING_REGEX = re.compile(r"(\w*?)(\d)")
TTS_URL = "https://texttospeech.googleapis.com/v1/text:synthesize"


def fetch_key():
    return os.environ["GCLOUD_KEY"]


def fetch_pronunciation(output_path, syllable, key):
    match = JYUTPING_REGEX.fullmatch(syllable)

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
        f'<phoneme alphabet="jyutping" ph="{syllable}">饭</phoneme>'
        "</speak>"
    )

    data = (
        "{"
        "  'input': {"
        f"    'ssml': '{ssml}'"
        "  },"
        "  'voice': {"
        "    'languageCode': 'yue-HK',"
        "    'name': 'yue-HK-Standard-C',"
        "  },"
        "  'audioConfig': {"
        "    'audioEncoding': 'MP3',"
        "    'effectsProfileId': ["
        "      'large-home-entertainment-class-device'"
        "    ],"
        "    'speakingRate': 0.8,"
        "    'pitch': 4.5,"
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
    db_path = sys.argv[1]
    output_path = sys.argv[2]

    jyutping_syllables = set()

    db = sqlite3.connect(db_path)
    c = db.cursor()

    c.execute("SELECT jyutping FROM entries")
    rows = c.fetchall()

    for row in rows:
        syllables = row[0].split()
        [jyutping_syllables.add(x) for x in syllables]

    key = fetch_key()

    for syllable in jyutping_syllables:
        status_code = fetch_pronunciation(output_path, syllable, key)
        while status_code == 401:
            key = fetch_key()
            status_code = fetch_pronunciation(output_path, syllable, key)
