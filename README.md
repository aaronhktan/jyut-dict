# <img src="/docs/icon/icon.png?raw=true" height="48"> Jyut Dictionary - An offline Cantonese dictionary

/jyːt ˈdɪkʃənɛɹi/

A program to look up words in Mandarin or Cantonese, with Simplified Chinese, Traditional Chinese, Pinyin, Jyutping, and English input.

Available for macOS, Windows, and Ubuntu.

[Download now!](https://github.com/aaronhktan/jyut-dict/releases)

## Features

### Vast number of entries.
Jyut Dictionary uses CEDICT and CC-CANTO as sources for its dictionary and Tatoeba as source for its sentences, giving it over 135,000 entries and 60,000 sentences to search from!

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
Prefer to only Traditional Chinese first? Maybe hide Pinyin? Change the colours of the words or disable them altogether? Do that with a plethora of settings options!

<img src="/docs/screenshots/mac/settings.png?raw=true" style="margin: 5px">

## Project structure

The project contains two subdirectories under `src`: `dictionaries`, and `jyut-dict`.

### dictionaries

This folder contains several Python3 scripts that convert the various online Cantonese dictionaries into the dictionary format used by Jyut Dictionary. Some sources include:
- CC-CEDICT and CC-CEDICT-compatible dictionaries, such as:
  - **[CC-CEDICT](https://cc-cedict.org/editor/editor.php?handler=Download)**
  - **[CC-CANTO](https://cantonese.org/download.html)**
  - **[CFDICT](https://chine.in/mandarin/dictionnaire/)**
  - **[HanDeDict](https://handedict.zydeo.net/de)**
- **[Kaifangcidian 粵語詞典 - 開放詞典](https://www.kaifangcidian.com/han/yue)**
- **[Tatoeba](https://tatoeba.org/eng/downloads)**
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

This project uses Qt 5.12. It has been verified to also compile on Qt 5.9.5 on Ubuntu, but there are no guarantees of other compatible Qt versions.

**Before building the application, you must build the dictionary database using `script-set.py` (for CEDICT + CC-CANTO) or `script-individual` (for CFDICT/HanDeDict).** Read the README in `src/cedict_to_sqlite` for instructions, then place the generated database, named `dict.db`, in `src/jyut-dict/resources/db/`.

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
