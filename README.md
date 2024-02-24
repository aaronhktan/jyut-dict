# <img src="/docs/icon/icon.png?raw=true" height="48"> Jyut Dictionary - A free, open-source, offline Cantonese dictionary

/jyːt ˈdɪkʃənɛɹi/

Look up words from multiple dictionaries in Cantonese or Mandarin, with Traditional Chinese, Simplified Chinese, Jyutping, Pinyin, and English input.

### [Download now!](https://jyutdictionary.com/#download-program)

Available for macOS, Windows, and Ubuntu.

## Features

### Vast number of entries.
Jyut Dictionary gives you access to CEDICT, CC-CANTO, words.hk 粵典, the Unihan database, 開放詞典, and Chinese sentences from Tatoeba, all completely offline. Plus, you can download and add more dictionaries to the program. That's over 200,000 entries and 100,000 sentences to search from!

<img src="/docs/screenshots/mac/search.png?raw=true" style="margin: 5px">

### Search quickly.
Results appear in a list as you type, so it's faster and easier to find what you're looking for. Plus, your search history is saved if you want to go back to a word you've looked up before.

<img src="/docs/screenshots/mac/search_animated.gif?raw=true" style="margin: 5px">

### Search with your preferred input method.
Jyut Dictionary supports entry with Traditional Chinese, Simplified Chinese, Jyutping, Pinyin, and English.

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

This folder contains several Python3 scripts that convert the various online Cantonese/Written Chinese dictionaries into the dictionary format used by Jyut Dictionary. However, for copyright reasons, data for these sources may not be included in this repository. Selected sources include:
- CC-CEDICT and CC-CEDICT-compatible dictionaries (in .u8 and .xml format), such as:
  - **[CC-CEDICT](https://cc-cedict.org/editor/editor.php?handler=Download)**
  - **[CC-CANTO](https://cantonese.org/download.html)**
  - **[CFDICT](https://chine.in/mandarin/dictionnaire/)**
  - **[HanDeDict](https://handedict.zydeo.net/de)**
- **[ABC Chinese-English Dictionary](https://wenlin.com/abc)**
- **[ABC Cantonese-English Dictionary](https://wenlin.co/wow/Project:Jyut)**
- **[CUHK 現代標準漢語與粵語對照資料庫](https://apps.itsc.cuhk.edu.hk/hanyu/Page/Cover.aspx)**
- **[CantoDict](https://www.cantonese.sheik.co.uk/)**
- **[Cross-Straits Language Database (兩岸詞典)](http://www.chinese-linguipedia.org/)**
  - **[兩岸三地生活差異詞語彙編](https://github.com/g0v/moedict-data-csld/blob/master/%E5%85%A9%E5%B2%B8%E4%B8%89%E5%9C%B0%E7%94%9F%E6%B4%BB%E5%B7%AE%E7%95%B0%E8%A9%9E%E8%AA%9E%E5%BD%99%E7%B7%A8-%E5%90%8C%E5%90%8D%E7%95%B0%E5%AF%A6.csv)**
- **[廣州話正音字典](https://github.com/jyutnet/cantonese-books-data/tree/master/2004_%E5%BB%A3%E5%B7%9E%E8%A9%B1%E6%AD%A3%E9%9F%B3%E5%AD%97%E5%85%B8)**
- **[HSK 3.0 (汉语水平考试 3.0)](https://github.com/elkmovie/hsk30)**
- **[Kaifangcidian 粵語詞典 - 開放詞典](https://www.kaifangcidian.com/han/yue)**
- **[MoEDict (重編國語辭典修訂本)](http://dict.revised.moe.edu.tw/cbdic/)**
- **[Tatoeba](https://tatoeba.org/eng/downloads)**
- **[Unihan Database](https://www.unicode.org/reports/tr38/)**
- **[Wikipedia Article Titles](https://wikipedia.org)**
- **[Wiktionary - English](https://en.wiktionary.org/wiki/Wiktionary:Main_Page)**
- **[words.hk 粵典](https://words.hk/)**

### jyut-dict

This folder contains the source code for the program, and a Qt Creator project file. Files are divided into several subdirectories:
- `components`: UI components, such as the list view or search bar.
- `dialogs`: dialogs, such as the "update available" notification dialog.
- `logic`: definitions for search and entry classes, as well as other backend logic.
- `platform`: platform-specific files, such as `Info.plist` for the macOS application bundle.
- `resources`: databases, icons, and images.
- `windows`: UI windows of the program, such as the main window.

## Build and run

This project requires Qt 5.15.

**Before building the application, you must provide a dictionary database. Download one from [the website](https://jyutdictionary.com/#download-addon), or build the dictionary database using `parse-set.py` (for CEDICT + CC-CANTO) or `parse-individual.py` (for CFDICT/HanDeDict).**

Place the database, named `dict.db`, in `src/jyut-dict/resources/db/`

### macOS, Windows: Craft + Qt Creator
1. Install [Craft](https://community.kde.org/Craft).
2. Install various dependencies using Craft:
```
craft libs/qt/multimedia
craft libs/qt/qtspeech
craft karchive
```
3. Set up Qt Creator with a kit from Craft, following instructions [here](https://community.kde.org/Craft#Using_Craft_with_an_IDE).
4. Open Jyut Dictionary in Qt Creator, and add DEFINES+="PORTABLE" to the QMake configuration if you would like to isolate your debug build from any system files.
5. Compile and run!

#### Ubuntu: Manual Git clone + Qt Creator
This guide assumes you have already installed Qt 5.15.2 using the online installer to `~/Qt`, with the Qt Multimedia and Qt Speech plugins.
1. Clone KArchive and check out `tags/v5.114.0`. This is the last version of KArchive that is compatible with Qt 5.
```
git clone https://github.com/KDE/karchive.git
cd karchive
git checkout tags/v5.114.0
```
2. Build according to KArchive instructions:
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=~/Qt/5.15.2/gcc_64
make
sudo make install
```
3. Open Jyut Dictionary in Qt Creator, and add DEFINES+="PORTABLE" to the QMake configuration if you would like to isolate your debug build from any system files.
4. Compile and run!

## Packaging for release

See the READMEs for each platform under `src/jyut-dict/platform/<platform>` for instructions on packaging the built executable.

## On the roadmap
See the Github issues and projects for more information!
