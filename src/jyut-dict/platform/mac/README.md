To build for Mac:

- Build the executable using the RELEASE configuration.
- Run MacDeployQt on the generated .app bundle.
- Run `create-dmg 'Jyut Dictionary.app' --overwrite` to generate the .dmg.

Codesigning and Notarizing
(See https://ahmad.ltd/How-To-Deploy-Your-Qt-macOS-App-Properly/ for instructions)
- Use XCode to generate a Developer ID certificate: Preferences > Accounts > Manage Certificates... > + (lower left corner) > Developer ID Application.
- BEFORE GENERATING THE DMG: codesign the generated .app bundle: `codesign -f --deep -v --options runtime -s 'Developer ID Application: {name of developer ID}' {app_name}.app`. You can find the name of the developer ID in Keychain, under `Certificates`; it'll have the format `Developer ID Application: <name> (<alphanumeric string>)`.
- AFTER GENERATING THE DMG: codesign the .dmg: `codesign -f --deep -v --options runtime -s 'Developer ID Application: {name of developer ID}' {app_name}.dmg`.
- Upload the codesigned .dmg file: `xcrun altool --notarize-app --primary-bundle-id '{bundle_ID}' --username '{apple_username}' --password '{apple_password}' --file {app_name}.dmg`.
- Staple the notarization info to the .dmg: `xcrun stapler staple {app_name}.dmg`.
- For the portable version, you can just codesign the .app bundle, upload the zipped version, and then staple the .app bundle before zipping again. Stapling does not work on .zips.

Common issues:
- `Warning: unable to build chain to self-signed root for signer`: Open XCode, go to Preferences > Accounts, and click "Download Manual Profiles". You may also need to go to Keychain Access and make sure all the Apple Certificates (e.g. `Apple Root CA`, `Apple Worldwide Developer Relations Certification Authority`, `Developer ID Certification Authority`, etc.) are set to `Use System Default`, NOT `Always Trust`!
- When uploading the codesigned .dmg, you may need an app-specific password (especially if you have MFA enabled on your Apple ID). See [here](https://support.apple.com/en-us/HT204397) for how to generate one.
