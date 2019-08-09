# <img src="/docs/icon/icon.png?raw=true" height="48"> Jyut Dictionary - An offline Cantonese dictionary

/jyːt ˈdɪkʃənɛɹi/

A program to look up words in Chinese, with Simplified Chinese, Traditional Chinese, Pinyin, Jyutping, and English input. Currently on macOS, Windows, and Ubuntu.

[Download now!](https://github.com/aaronhktan/jyut-dict/releases)

## Features

### Vast number of entries.
Jyut Dictionary uses CEDICT and CC-CANTO as sources for its dictionary, giving it over 135,000 entries to search from!

<img src="/docs/screenshots/mac/search_jyutping.png?raw=true" style="margin: 5px">

### Search quickly.
Results appear in a list as you type, so it's faster and easier to find what you're looking for.

<img src="/docs/screenshots/mac/search_animated.gif?raw=true" style="margin: 5px">

### Search with your preferred input method.
Jyut Dictionary supports entry with Simplified Chinese, Traditional Chinese, Pinyin, Jyutping, and English. More to come, so stayed tuned!

<img src="/docs/screenshots/mac/search_simplified.png?raw=true" style="margin: 5px">

### Supports dark mode.
Dark mode is love. Dark mode is life.

<img src="/docs/screenshots/mac/dark.png?raw=true" style="margin: 5px">

## Project structure

The project contains two main subdirectories, `cedict_to_sqlite` and `jyut-dict`.

### cedict_to_sqlite

This folder contains a Python 3 script that takes three source files and spits out a full-text search SQLITE3 database.

### jyut-dict

This folder contains the source code for the program, and a Qt Creator project file. Files are divided into several subdirectories:
- `windows`: UI windows of the program, such as the main window.
- `components`: UI components, such as the list view or search bar.
- `logic`: definitions for search and entry classes, as well as any other backend logic.
- `platform`: platform-specific files, such as `Info.plist` for the macOS application bundle.
- `resources`: databases, icons, and images.

## Build and run

### cedict_to_sqlite

The three text files the script requires are:
1. CEDICT
2. CC-CANTO
3. CC-CANTO readings for CEDICT

In addition, the SQLITE driver must support full-text search on your machine.

Usage:
- To install required packages: `pip install -r requirements.txt`
- To generate database: `python3 script.py <output database filename> <CC_CEDICT file> <CC_CANTO file> <CC_CANTO readings file>`

### jyut-dict

This project uses Qt 5.12.

Before building the application, you must build the database. See the section above for instructions, then place the generated database, named `dict.db`, in `resources/db/`.

#### macOS

##### Command line
1. Generate a Makefile from `jyut-dict.pro`: `qmake jyut-dict.pro`
  - Note: you may need the full path of qmake. Search in Spotlight to find the appropriate version, which on 64-bit Macs is the qmake binary under in the `clang_64` folder.
2. Make with `make`. A `.app` application bundle will appear in the directory where you ran this command, which you can double click to run.

##### Qt Creator
Import the project to Qt Creator, then run.

#### Windows

#### Qt Creator
Import the project to Qt Creator, then run. If packaging for release, select the release option in the bottom left corner of the IDE.

#### Ubuntu

##### Command line
1. Generate a Makefile from `jyut-dict.pro`: `qmake jyut-dict.pro`
2. Make with `make`. A Linux executable will appear in the directory where you ran this command.

##### Qt Creator
Import the project to Qt Creator, then run.

## Packaging for release

### macOS

1. **Generate the deployment .app**: Qt has a handy-dandy application called macdeployqt that packages all the frameworks, resources and other dependencies into the Mac bundle file.
  - Run it with `macdeployqt ./Jyut\ Dictionary.app` in the directory where the program was built.
  - Note: you may need the full path of macdeployqt. Search in Spotlight to find the appropriate version, which on 64-bit Macs is the macdeployqt binary in the `clang_64` folder.

2. **Create a .dmg file**: Mac users expect a .dmg file where they can drag and drop the application bundle into the `~/Applications` folder.
  - To generate the .dmg for distribution, you will need Node.js and create-dmg installed. If you do not have create-dmg installed, get it with ```npm install --global create-dmg```.
  - Create the dmg. ```create-dmg 'Jyut\ Dictionary.app'```

### Windows

1. **Generate the deployment .exe**: Qt has a handy-dandy application called windeployqt that packages all the .dlls, resources and other dependencies into the same folder as the .exe.
  - Run it with `windeployqt ./jyut-dict.exe` in the directory where the program was built. (Here, use the release folder.)
  - Note: you may need the full path of windeployqt, and set the appropriate environment variables. Find the appropriate version by looking in the folder where you installed Qt, which on Windows may be located in the `msvc_<year>` folder if using Microsoft Visual C++, or `mingw<version>_64` if using MinGW/GCC for Windows.

2. **Create a .msi file**: Windows users expect an installer that automatically places frameworks and executables in the right places, as well as setting registry keys as necessary.
  - To generate the .msi for distribution, you will need Advanced Installer.
  - Add the .dlls, folders, database files, and executable generated by Qt Creator and windeployqt to the list of files and folders in Advanced Installer, then click 'Build' to generate a .msi file.

### Ubuntu

1. **Generate the AppImage**: A third-party application called linuxdeployqt can generate an AppImage for this application. See that repository for more information.

2. **Create a .deb file**: Ubuntu users also expect to be able to install with a .deb file. This can be generated with the `create-deb.sh` script under `platform/linux`.

## On the roadmap
See the Github bugs and project for more information!
