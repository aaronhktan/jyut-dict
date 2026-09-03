# Release checklist

## macOS

### Install
- [ ] Build install .app bundle.
- [ ] Run macdeployqt on .app bundle.
- [ ] Codesign install .app bundle.
- [ ] Generate .dmg.
- [ ] Codesign .dmg.
- [ ] Notarize .dmg.
- [ ] Staple notarization to .dmg.

### Portable
- [ ] Build portable .app bundle.
- [ ] Run macdeployqt on .app bundle.
- [ ] Codesign portable .app bundle.
- [ ] Notarize .app bundle.
- [ ] Staple notarization to .app bundle.
- [ ] Compress .app bundle using the .zip format.

## Windows

### Install
- [ ] Build 32-bit executable.
- [ ] Move executable to platform/windows/installer data folder.
- [ ] Run windeployqt on the executable.
- [ ] Copy OpenSSL .dlls to the data folder.
- [ ] Run binarycreator to generate installer file.
- [ ] Repeat for 64-bit.

### Portable
- [ ] Build 32-bit executable.
- [ ] Run windeployqt on the executable.
- [ ] Copy OpenSSL .dlls to the data folder.
- [ ] Compress using the .zip format.
- [ ] Repeat for 64-bit.

## Linux

### AppImage
- [ ] Build the binary using the Release configuration with APPIMAGE defined on Ubuntu 16.04.
- [ ] See list of files to copy in the `linux` subfolder.
- [ ] Run linuxdeployqt.
