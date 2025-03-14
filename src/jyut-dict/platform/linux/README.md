To build for Linux:

- Build the **AppImage** using Ubuntu 22.04.
	- Build using the Release configuration, with DEFINES+="APPIMAGE".
	- Copy the directory structure under "appimage" to another folder, called `<folder>` from here on out.
	- Copy files to `<folder>`:
		- Copy the built executable to `<folder>`/usr/bin and rename it to "jyut-dict".
		- Copy dict.db to `<folder>`/usr/share/jyut-dict/dictionaries/
		- Copy user.db to `<folder>`/usr/share/jyut-dict/dictionaries/
	- Run `linuxdeployqt <folder>/usr/share/applications/jyut-dict.desktop -appimage`.
	- You should have an AppImage that works now.
