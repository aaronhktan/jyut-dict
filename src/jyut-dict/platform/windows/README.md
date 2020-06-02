To build for Windows:

- Build using the RELEASE configuration on Windows.
- Copy the executable, dict.db, and user.db to `installer/packages/com.aaronhktan.cantonesedictionary/data`.
- Run WinDeployQt on the executable. Make sure to run WinDeployQt with the command prompt installed in the Start Menu or run QtEnv2.bat, so that appropriate paths are added to the system path.
- Download and install the appropriate OpenSSL 1.0.2 file (32-bit and 64-bit). Copy libeay32.dll and ssleay32.dll to the data folder.
- Run binarycreator.exe (from Qt Installer Framework Tools), passing `--offline-only -c <path-to-config/config.xml> -p <path-to-packages> <name-of-executable>`.