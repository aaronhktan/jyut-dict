## Notes on translations

- Before doing `lupdate`: Change yue_CN and yue_HK to zh_CN and zh_HK respectively, since lupdate does not recognize Cantonese and will not insert the new translations. The affected files are `yue_Hans.ts`, `yue_Hant.ts`, `zh_Hans_HK.ts`, and `zh_Hant_HK.ts`.
- After doing `lupdate`: lupdate will erase everything with MAC_APPLICATION_MENU and some other useful translations by marking them as "vanished". Do a find and replace ` type="vanished"` (with the space at the beginning) => empty to re-enable these translations.

- Useful resources:
  - https://pontoon.mozilla.org/zh-TW/all-projects/all-resources/
  - https://pontoon.mozilla.org/zh-CN/all-projects/all-resources/
  - https://pontoon.mozilla.org/fr/all-projects/all-resources/
