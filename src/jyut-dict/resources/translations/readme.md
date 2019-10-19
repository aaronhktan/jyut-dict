## Notes on translations

- Before doing `lupdate`: Change yue_CN and yue_HK to zh_CN and zh_HK respectively, since lupdate does not recognize Cantonese and will not insert the new translations.
- After doing `lupdate`: lupdate will erase everything with MAC_APPLICATION_MENU and some other useful translations by marking them as "vanished". Do a find and replace ` type="vanished"` (with the space at the beginning) => empty to re-enable these translations.