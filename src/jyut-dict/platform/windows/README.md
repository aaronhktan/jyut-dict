To build for Windows:

- Build using the RELEASE configuration on Windows.
- Copy the executable, FLOW_LAYOUT_LICENSE.txt, dict.db, and user.db to `installer/packages/com.aaronhktan.cantonesedictionary/data`.
- Run WinDeployQt on the executable. Make sure to run WinDeployQt with the command prompt installed in the Start Menu or run QtEnv2.bat, so that appropriate paths are added to the system path.
- Manually copy over libarchive, libbzip2, libfreetype, libharfbuzz, libKF5Archive, liblzma, libpcre2-16, libpng, libsqlite, libz, and libzstd dlls to the data directory.
- Download and install the appropriate OpenSSL 3 file (32-bit and 64-bit). Copy libssl-3.dll, libcrypto-3.dll to the data folder.
- Run binarycreator.exe (from Qt Installer Framework Tools), passing `--offline-only -c <path-to-config/config.xml> -p <path-to-packages> <name-of-executable>`. Note that there are different `config.xml` files for 32-bit and 64-bit installers!

Defender SmartScreen notes:
- Submit the installer .exe and the portable .exe to Microsoft's malware analysis: https://www.microsoft.com/en-us/wdsi/filesubmission.
