# <img src="/docs/icon/icon.png?raw=true" height="48"> Jyut Dictionary - A free, open-source, offline Cantonese dictionary

/jyːt ˈdɪkʃənɛɹi/

Look up words from multiple dictionaries in Cantonese or Mandarin, with Traditional Chinese, Simplified Chinese, Jyutping, Pinyin, and English input.

### [Download now!](https://github.com/aaronhktan/jyut-dict/releases)

Available for macOS, Windows, and Ubuntu.

## Features

### Vast number of entries.
Jyut Dictionary gives you access to CEDICT, CC-CANTO, words.hk 粵典, the Unihan database, 開放詞典, and Chinese sentences from Tatoeba, all completely offline. That means over 200,000 entries and 100,000 sentences to search from!

<img src="/docs/screenshots/mac/search.png?raw=true" style="margin: 5px">

### Search quickly.
Results appear in a list as you type, so it's faster and easier to find what you're looking for. Plus, your search history is saved if you want to go back to a word you've looked up before.

<img src="/docs/screenshots/mac/search_animated.gif?raw=true" style="margin: 5px">

### Search with your preferred input method.
Jyut Dictionary supports entry with Simplified Chinese, Traditional Chinese, Pinyin, Jyutping, and English.

<img src="/docs/screenshots/mac/search_options.gif?raw=true" style="margin: 5px">

### Localized.
Use the dictionary in English, French, Simplified and Traditional Cantonese, or Simplified and Traditional Chinese.

<img src="/docs/screenshots/mac/search_localization.gif?raw=true" style="margin: 5px">

### Customizable.
Prefer to see only Traditional Chinese? Maybe hide Pinyin? Change the colours of the words or disable them altogether? Do that with a plethora of settings options!

<img src="/docs/screenshots/mac/settings.png?raw=true" style="margin: 5px">

## Project structure

The project contains two subdirectories under `src`: `dictionaries`, and `jyut-dict`.

### dictionaries

This folder contains several Python3 scripts that convert the various online Cantonese/Written Chinese dictionaries into the dictionary format used by Jyut Dictionary. Some sources include:
- CC-CEDICT and CC-CEDICT-compatible dictionaries, such as:
  - **[CC-CEDICT](https://cc-cedict.org/editor/editor.php?handler=Download)**
  - **[CC-CANTO](https://cantonese.org/download.html)**
  - **[CFDICT](https://chine.in/mandarin/dictionnaire/)**
  - **[HanDeDict](https://handedict.zydeo.net/de)**
- **[CUHK 現代標準漢語與粵語對照資料庫](https://apps.itsc.cuhk.edu.hk/hanyu/Page/Cover.aspx)**
- **[CantoDict](https://www.cantonese.sheik.co.uk/)**
- **[Cross-Straits Language Database (兩岸詞典)](http://www.chinese-linguipedia.org/)**
  - **[兩岸三地生活差異詞語彙編](https://github.com/g0v/moedict-data-csld/blob/master/%E5%85%A9%E5%B2%B8%E4%B8%89%E5%9C%B0%E7%94%9F%E6%B4%BB%E5%B7%AE%E7%95%B0%E8%A9%9E%E8%AA%9E%E5%BD%99%E7%B7%A8-%E5%90%8C%E5%90%8D%E7%95%B0%E5%AF%A6.csv)**
- **[Kaifangcidian 粵語詞典 - 開放詞典](https://www.kaifangcidian.com/han/yue)**
- **[MoEDict (重編國語辭典修訂本)](http://dict.revised.moe.edu.tw/cbdic/)**
- **[Tatoeba](https://tatoeba.org/eng/downloads)**
- **[Unihan Database](https://www.unicode.org/reports/tr38/)**
- **[words.hk 粵典](https://words.hk/)**

### jyut-dict

This folder contains the source code for the program, and a Qt Creator project file. Files are divided into several subdirectories:
- `components`: UI components, such as the list view or search bar.
- `dialogs`: dialogs, such as the update available notification dialog.
- `logic`: definitions for search and entry classes, as well as any other backend logic.
- `platform`: platform-specific files, such as `Info.plist` for the macOS application bundle.
- `resources`: databases, icons, and images.
- `windows`: UI windows of the program, such as the main window.

## Build and run

This project requires Qt 5.15.

**Before building the application, you must build the dictionary database using `parse-set.py` (for CEDICT + CC-CANTO) or `parse-individual.py` (for CFDICT/HanDeDict).** Read the README in `src/dictionaries/cedict` for instructions, then place the generated database, named `dict.db`, in `src/jyut-dict/resources/db/`.

#### Qt Creator (macOS, Ubuntu, Windows)
Import the project to Qt Creator, then run. Add DEFINES+="PORTABLE" to the QMake configuration if you would like to isolate your debug build from any system files.

#### Command line (macOS, Ubuntu)

##### macOS
1. Generate a Makefile from `jyut-dict.pro`: `qmake jyut-dict.pro`
  - Note: you may need the full path of qmake. Search in Spotlight to find the appropriate version, which on 64-bit Macs is the qmake binary under in the `clang_64` folder.
2. Make with `make`. A `.app` application bundle will appear in the directory where you ran this command, which you can use `open` to run.

##### Ubuntu
1. Generate a Makefile from `jyut-dict.pro`: `qmake jyut-dict.pro`
2. Make with `make`. A Linux executable will appear in the directory where you ran this command.

## Packaging for release

See the READMEs for each platform under `src/jyut-dict/platform/<platform>` for instructions on packaging the built executable.

## On the roadmap
See the Github bugs and project for more information!
