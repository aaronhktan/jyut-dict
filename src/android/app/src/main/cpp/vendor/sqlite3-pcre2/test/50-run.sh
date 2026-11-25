#!/usr/bin/env sh
# This script execute the test.
#
# The test are based on two input script arguments:
# 1) The database. It contains the test specification.
# 2) The library to be loaded.
#
# Note: the library should be provided with a path (`./filename`) and not just
# the filename otherwise only an installed library will be searched (see
# `man dlopen`).


# Preamble of all the shell functions
set -e
TESTDIR="$( cd "$( dirname "$0" )" && pwd )"
. "$TESTDIR/lib.sh"

if [ ! -z "$1" ]
then
    DB="$1"
fi
if [ ! -z "$2" ]
then
    INCLUDE_LIB="$2"
fi

testcampaign=`sql_call_args '
    INSERT INTO testcampaign (started_at, lib) VALUES (julianday(), %s);
    SELECT last_insert_rowid();
' "$INCLUDE_LIB"`

while true
do
    # Fetch test data into testcase and execute_sql
    testcase=`sql_call_args '
        SELECT testcase.id FROM testcase
        LEFT JOIN testreport ON
            testreport.testcase = testcase.id
            AND testcampaign = %s
        WHERE testreport.id IS NULL
        LIMIT 1
    ' "$testcampaign"`
    if [ -z "$testcase" ]
    then
        break
    fi
    execute_sql=`sql_call_args '
        SELECT execute_sql
        FROM testcase
        WHERE id = %s
    ' "$testcase"`

    # Create testreport record
    testreport=`sql_call_args "
        INSERT INTO testreport
        (testcase, testcampaign)
        VALUES (%s, %s);
        SELECT last_insert_rowid();
    " "$testcase" "$testcampaign"`

    # Execute test. Record exit_code and stderr.
    # logdebug "Update test $testcase with execute_sql='$execute_sql'"
    set +e
    stderr=`SQL_LOAD_LIB="$INCLUDE_LIB" NOLOG=1 sql_call_args "
        UPDATE testreport
        SET report_value = (
            $(sql_printf "$execute_sql" "$testreport" | printf_escape)
        )
        WHERE id = %s;
    " "$testreport" 2>&1 1>/dev/null`
    exit_code=$?
    set -e

    # logdebug "exit_code: '$exit_code', stderr: '$stderr'"
    sql_call_args 'UPDATE testreport SET stderr=%s, exit_code=%s WHERE id=%s' \
        "$stderr" "$exit_code" "$testreport"

    evaluate_sql=`sql_call_args 'SELECT evaluate_sql FROM testcase WHERE id=%s' "$testcase"`
    if [ -z evaluate_sql ]
    then
        evaluate_sql='NULL'
    fi
    # logdebug "Update test $testcase with evaluate_sql='$evaluate_sql'"
    sql_call_args "
        UPDATE testreport
        SET evaluation = (
            $(sql_printf "$evaluate_sql" "$testreport" | printf_escape)
        )
        WHERE id=%s
    " "$testreport"
done

sql_call_args 'UPDATE testcampaign SET ended_at=julianday() WHERE id = %s' "$testcampaign"
