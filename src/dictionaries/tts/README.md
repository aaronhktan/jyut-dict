#### Usage:
- Run the script (`python3 google.py`), using the instructions from the script for parameters.
- The `azure.py` script also works, but is currently hard-coded to only download a Hong Kong Cantonese voice.

### Post-Processing (Remove Duplicates)
- Delete duplicates with `rm -rf $(find . -type f -exec md5sum {} + | grep $(md5sum * | sort -k1 | awk '{print $1}' | uniq -d) | awk '{print $2}')`

### Creating the ZIP file
The directory structure should look like the following:
```
Google/
├─ Chinese_China/
│  ├─ CMN_1/
│  │  ├─ ...
│  │  ├─ a1.mp3
│  │  ├─ a2.mp3
├─ Chinese_Hong Kong/
│  ├─ YUE_1/
│  │  ├─ ...
│  │  ├─ aa1.mp3
│  │  ├─ aa2.mp3
├─ Chinese_Taiwan/
│  ├─ CMN_1/
│  │  ├─ ...
│  │  ├─ a1.mp3
│  │  ├─ a2.mp3
```

On macOS, you'll want to zip up the directory using `zip -rX Google.zip ./Google` (note the `-X` option to remove the various hidden `.DS_Store` and `__MACOSX` files).