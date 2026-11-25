DROP TABLE IF EXISTS testregexp;
CREATE TABLE testregexp (
    `id` INTEGER PRIMARY KEY,
    `pattern` TEXT NOT NULL,  -- SQL representation of the pattern
    `subject` TEXT NOT NULL,  -- SQL representation of the subject
    `match` TEXT,    -- SQL representation of the match value
    `failmessage` TEXT,  -- Pattern of the failmessage, when the match operation shall crash
    `description` TEXT,
    CONSTRAINT match_xor_failmessage CHECK (
        (`match` IS NULL AND `failmessage` IS NOT NULL)
        OR (`match` IS NOT NULL AND `failmessage` IS NULL)
    )
);


-- Insert testregexp for checking the match value based on the function input
INSERT INTO testregexp
(`pattern`, `subject`, `match`, `description`)
VALUES
    -- pattern      | `subject`     | match | `description`                    |
    ('''(?i)^A''',   '''asdf''',     1,      NULL),
    ('''(?i)^A''',   '''bsdf''',     0,      NULL),
    ('''^A''',       '''asdf''',     0,      NULL),
    ('NULL',         '''asdf''',     'NULL', 'NULL pattern shall return NULL'),
    ('''^A''',       'NULL',         'NULL', 'NULL subject shall return NULL'),
    ('''''',         '''asdf''',     1,      'Blank pattern shall match'),
    ('''^A''',       '''''',         0,      'Blank subject'),
    (
        '''B''',
        'x''00''||''B''',            1,      'NULL character in subject'),
    (
        'x''00''||''B''',
        '''a''||x''00''||''B''',     1,      'NULL character in pattern')
;


-- Insert testregexp for checking the error message when the function crash
INSERT INTO testregexp
(`pattern`, `subject`,
    `failmessage`,
    `description`
)
VALUES
    ('''^(A''',     '''asdf''',
        '%Cannot compile pattern % missing closing parenthesis%',
        'Non-compilable regexp'
    )
;


-- Write the testcase corresponding to the test data
DELETE FROM testcase WHERE src_table = 'testregexp';
INSERT INTO `testcase`
(`src_table`, `src_id`, `execute_sql`, `evaluate_sql`, `description`)
SELECT
    'testregexp' AS src_table,
    id AS src_id,
    (
        'SELECT '||`subject`||' REGEXP '||`pattern`||' AS `value`'
    ) AS `execute_sql`,
    (
        CASE WHEN `failmessage` IS NULL
        THEN
            '`report_value`'||' IS '||`match`
        ELSE '`stderr` LIKE '''  || REPLACE(`failmessage`, '%', '%%') || ''''
        END
    ) AS `evaluate_sql`,
    (
        CASE WHEN `failmessage` IS NULL
        THEN
            `pattern`
            || ' shall '
            || (CASE WHEN `match` THEN 'match' ELSE 'not match' END)
            || ' '
            || `match`
            || "."
        ELSE
            'The evaluation of `'
            || `subject` || ' REGEXP ' || `pattern`
            || '` shall crash with failmessage '
            || '''' || `failmessage` || ''''
            || '.'
        END
    ) AS `description`
FROM testregexp
;
