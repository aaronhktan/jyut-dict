#!/bin/bash
# This script is designed to increment the build number consistently across all
# targets.
# Borrowed from https://medium.com/@mateuszsiatrak/automating-build-number-increments-in-xcode-with-custom-format-a-practical-guide-bcc90a19f716

set -euo pipefail

# Navigating to the 'CantoneseDictionary' directory inside the source root.
cd "$SRCROOT/$PRODUCT_NAME"

# Get the current date in the format "YYMMDD".
current_date=$(date "+%y%m%d")

# Parse the 'Config.xcconfig' file to retrieve the previous build number.
# The 'awk' command is used to find the line containing "BUILD_NUMBER"
# and the 'tr' command is used to remove any spaces.
previous_build_number=$(awk -F "=" '/BUILD_NUMBER/ {print $2}' Config.xcconfig | tr -d ' ')

# If the current date matches the date from the previous build number,
# increment the counter. Otherwise, reset the counter to 1.
current_date_no_dots="${current_date//./}"
previous_date="${previous_build_number:0:6}"
previous_date_no_dots="${previous_date//./}"
new_build_number=$((current_date_no_dots == previous_date_no_dots ? previous_build_number + 1 : 1))

# Use 'sed' command to replace the previous build number with the new build
# number in the 'Config.xcconfig' file.
sed -i -e "/BUILD_NUMBER =/ s/= .*/= $new_build_number/" Config.xcconfig
rm -f Config.xcconfig-e
