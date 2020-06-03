#!/bin/bash

# Requires root to clean the build-dir
# Snippet from https://serverfault.com/questions/37829/bash-scripting-require-script-to-be-run-as-root-or-with-sudo
if [[ $(/usr/bin/id -u) -ne 0 ]]; then
    echo "Not running as root"
    exit
fi

# Create directory to build in
rm -rf ../../../../jyut-dict-flatpak
mkdir ../../../../jyut-dict-flatpak

# Copy manifest into that directory
cp com.aaronhktan.cantonesedictionary.json ../../../../jyut-dict-flatpak

# Switch into that directory
cd ../../../../jyut-dict-flatpak

# Build according to manifest, putting output repository into repo/
flatpak-builder --repo=repo build-dir com.aaronhktan.cantonesedictionary.json --force-clean --user

# Build a bundle from the repo/ directory
flatpak build-bundle ./repo jyut-dict.flatpak com.aaronhktan.cantonesedictionary