To build for Linux:

- Build the **AppImage** using Ubuntu 20.04.
	- Build using the Release configuration, with DEFINES+="APPIMAGE".
	- Copy the directory structure under "appimage" to another folder, called `<folder>` from here on out.
	- Copy files to `<folder>`:
		- Copy the built executable to `<folder>`/usr/bin and rename it to "jyut-dict".
		- Copy dict.db to `<folder>`/usr/share/jyut-dict/dictionaries/
		- Copy user.db to `<folder>`/usr/share/jyut-dict/dictionaries/
	- Run `linuxdeployqt <folder>/usr/share/applications/jyut-dict.desktop -appimage`.
	- You should have an AppImage that works now.

- Build the **.deb** using Ubuntu 20.04.
	- Add repository to install Qt 5.15: `sudo add-apt-repository ppa:beineri/opt-qt-5.15.2-focal; sudo apt-get update`
	- Install dependencies: `sudo apt install qt515base qt515speech qt515svg`
	- Add `export DEBEMAIL="your.email.address@example.org"` and `export DEBFULLNAME="Firstname Lastname"` to your `~/.bashrc` (or `.zshrc` or whatever shell is being used).
	- Generate a GPG key: `gpg --default-new-key-algo rsa4096 --gen-key`
	- Run `create-deb.sh` in the deb subdirectory.
