DROP TABLE IF EXISTS testcase;
CREATE TABLE `testcase` (
    `id` INTEGER PRIMARY KEY,
    -- Contains an SQL pattern expression.
    -- The SQL pattern uses the printf convention. Literal '%' shall be escaped with '%%'.
    -- The first occurence of %s is replaced by the ID of the testreport.
    -- In case of crash test, the expression may crash.
    -- Returns: a scalar value that will be stored in testreport.report_value.
    -- Example:
    --      '%s LIKE ''1%%'''   -- return 1 if the testreport ID starts with a 1
    `execute_sql` TEXT NOT NULL,
    -- Contains an SQL pattern expression.
    -- The SQL pattern uses the printf convention. Literal '%' shall be escaped with '%%'.
    -- The first occurence of %s is replaced by the ID of the testreport.
    -- The expression shall not crash.
    -- Returns: a scalar boolean value that will be stored in testreport.evaluation
    -- Example:
    --      '%s LIKE ''1%%'''   -- return 1 if the testreport ID starts with a 1
    `evaluate_sql` TEXT NOT NULL,
    -- Name of the table that contains more information about the testcase.
    `src_table` TEXT,
    -- ID in the table src_table linking to the row that originates the testcase.
    `src_id`,
    `description` TEXT
);

DROP TABLE IF EXISTS testcampaign;
CREATE TABLE `testcampaign` (
    id INTEGER PRIMARY KEY,
    -- Julian day that identifies the begining of the test campaign.
    started_at FLOAT,
    -- Julian day that identifies the end of the test campaign.
    ended_at FLOAT,
    -- Text that identify the library that was used for the test.
    lib TEXT
);

DROP TABLE IF EXISTS testreport;
CREATE TABLE testreport (
    `id` INTEGER PRIMARY KEY,
    `testcase` INTEGER NOT NULL REFERENCES testcase,
    `testcampaign` INTEGER NOT NULL REFERENCES testcampaign,
    -- Standard error that is returned by SQLite after the execution of testcase.execute_sql
    `stderr`,
    -- Exit code that is returned by SQLite after the execution of testcase.execute_sql
    `exit_code` INTEGER,
    -- Value that is returned by the execution of testcase.execute_sql.
    `report_value`,
    -- Value that is returned by the execution of testcase.evaluate_sql.
    `evaluation` INTEGER,
    UNIQUE (testcase, testcampaign)
);
