import requests

import logging
import os
import re
import sqlite3
import sys

JYUTPING_REGEX = re.compile(r"(\w*?)(\d)")
AUTH_URL = "https://eastus.api.cognitive.microsoft.com/sts/v1.0/issueToken"
TTS_URL = "https://eastus.tts.speech.microsoft.com/cognitiveservices/v1"


def fetch_key():
    headers = {
        "Ocp-Apim-Subscription-Key": os.environ["AZURE_KEY"],
        "Host": "eastus.api.cognitive.microsoft.com",
        "Content-Type": "application/x-www-form-urlencoded",
    }
    response = requests.post(AUTH_URL, headers=headers)
    return response.text


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
        "Content-Type": "application/ssml+xml",
        "Authorization": f"Bearer {key}",
        "X-Microsoft-OutputFormat": "audio-24khz-48kbitrate-mono-mp3",
        "User-Agent": "Jyut_Dictionary",
    }

    data = (
        "<speak version='1.0' xml:lang='zh-HK'>"
        "<voice xml:lang='zh-HK' xml:gender='Male' name='zh-HK-WanLungNeural'>"
        "<prosody rate='0.8'>"
        f"<phoneme alphabet='sapi' ph='{initial_final} {tone}'>嗝</phoneme>"
        "</prosody>"
        "</voice></speak>"
    )

    response = requests.post(TTS_URL, headers=headers, data=data.encode())
    if response.status_code != 200:
        logging.error(
            f"There was an error fetching audio for syllable {syllable}"
            f", status code {response.status_code}"
        )
        return response.status_code

    if len(response.content) > 0:
        with open(f"{output_path}{syllable}.mp3", "wb") as binary_file:
            binary_file.write(response.content)
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
