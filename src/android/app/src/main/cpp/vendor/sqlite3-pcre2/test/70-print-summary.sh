#!/usr/bin/env sh


set -e
TESTDIR="$( cd "$( dirname "$0" )" && pwd )"
. "$TESTDIR/lib.sh"



testcampaign=`sql_call_args 'SELECT id FROM testcampaign ORDER BY id DESC LIMIT 1'`

sql_call_args "
    SELECT 'Test campaign #' || testcampaign
        || ' executed in ' || ((ended_at - started_at) * 3600 * 24) || ' seconds'
        || ' with '
        || SUM(NOT evaluation) || ' tests failure'
        || ', ' || SUM(evaluation IS NULL) || ' tests that cannot be evaluated'
        || ', and ' || SUM(evaluation) || ' tests successfully passed'

    FROM testreport
    JOIN testcampaign ON testcampaign.id = testreport.testcampaign
    WHERE testreport.testcampaign = %s
    GROUP BY testreport.testcampaign
    " "$testcampaign"


error_count=`sql_call_args 'SELECT COUNT(*) FROM testreport WHERE testreport.testcampaign = %s AND NOT IFNULL(testreport.evaluation, FALSE)' "$testcampaign"`
if [ "$error_count" -ne 0 ]
then
    sql_call_args "
.header on
            SELECT *
            FROM testreport
            JOIN testcase ON testcase.id = testreport.testcase
            WHERE testreport.testcampaign = %s AND NOT IFNULL(testreport.evaluation, FALSE)
        " "$testcampaign"
fi
