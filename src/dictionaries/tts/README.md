- Trim with dBpoweramp Music Converter > Music Converter > DSP Effects > Trim Silence.
- Configure to trim -50dB, 1ms timing.
- Bit rate 96kbps.

- For Yue_C in Google TTS, pitch up 4.5.
- For all voices, play at 0.8x speed.
- Delete duplicates with `rm -rf $(find . -type f -exec md5sum {} + | grep $(md5sum * | sort -k1 | awk '{print $1}' | uniq -d) | awk '{print $2}')`