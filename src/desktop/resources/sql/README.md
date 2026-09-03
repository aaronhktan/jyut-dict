To format SQL queries:
- Install the dependencies in package-lock.json using NPM.
- Remove all quotes from the multi-line strings.
- Replace placeholder values ("?") with empty strings ("").
- Run prettier: `prettier in.sql --plugin=prettier-plugin-sql-cst --print-width 60 > out.sql`.
- Surround the output with quotes: in Sublime Text, `Ctrl+Shift+L`, `"`, `→+Space`.
- Copy and paste the output into `searchqueries.h`.
