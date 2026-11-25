# Lib that contains the functions used for the test.
set -e
TESTDIR="$( cd "$( dirname "$0" )" && pwd )"
SQLITE=sqlite3
DIR="$(dirname "$TESTDIR")"
DB="$TESTDIR/db.sqlite3"

# If overriden: the sql_call_stdin and sql_call_args will load this library first.
# Note: the library should be provided with a path (`./filename`) and not just
# the filename otherwise only an installed library will be searched (see
# `man dlopen`).
SQL_LOAD_LIB=""


sql_escape() {
    # Escape the string for sqlite
    #
    # Example:
    #
    #       sql_escape "isn't it?"
    #       # => "'isn''t it?'"
    local sedstr='s/'"'"'/'"''"'/g;1s/^/'"'"'/;$s/$/'"'"'/'
    if [ "$#" -eq 0 ]
    then
        sed "$sedstr"
    else
        echo "$1" | sed "$sedstr"
    fi
}


sql_printf() {
    # Like the printf function, but escape the parameter for SQL.
    #
    # Example:
    #
    #       sql_printf "select * from t where col=%s" 1
    #       # => "select * from t where col='1'"
    local arg
    local cmd
    local lvar
    local i=1
    for arg in "$@"
    do
        if [ "$i" -eq 1 ]
        then
            cmd="\"\$1\""
        else
            lvar="L$i"
            eval "local $lvar="'"$(sql_escape "$arg")"'
            cmd="$cmd \"\$$lvar\""
        fi
        i="$((i+1))"
    done
    eval "printf $cmd"
}


id_escape() {
    # Escape the string for SQL identifiers.
    #
    # Example:
    #
    #       id_escape "isn`t it?"
    #       # => "`isn``t it?`"
    local sedstr='s/`/``/g;1s/^/`/;$s/$/`/'
    if [ "$#" -eq 0 ]
    then
        sed "$sedstr"
    else
        echo "$1" | sed "$sedstr"
    fi
}


printf_escape() {
    # Escape the string for printf function.
    #
    # Example:
    #
    #       id_escape "10% \ 1"
    #       # => '10%% \\ 1'
    local sedstr='s/[%\\]/\0\0/g'
    if [ "$#" -eq 0 ]
    then
        sed "$sedstr"
    else
        echo "$1" | sed "$sedstr"
    fi
}


sql_build_include() {
    if [ ! -z "$SQL_LOAD_LIB" ]
    then
        echo ".load $SQL_LOAD_LIB"
    fi
}


sql_call_file() {
    local "include=$(sql_build_include)"
    if [ -z "$include" ]
    then
        cat "$@" | $SQLITE "$DB" \
            || log "based on file $*"
    else
        cat "$@" | $SQLITE "$DB" -cmd "$include" \
            || log "based on file $*"
    fi
}


sql_call_args() {
    local sql
    local "include=$(sql_build_include)"
    local exit_status
    if [ $# -eq 0 ]
    then
        log 'No arguments to sql_call_args'
        exit 1
    elif [ $# -eq 1 ]
    then
        sql="$1"
    else
        sql=`sql_printf "$@"`
    fi
    if [ -z "$include" ]
    then
        $SQLITE "$DB" "$sql" \
            || log "Based on SQL command: $sql"
    else
        $SQLITE "$DB" -cmd "$include" "$sql" \
            || log "Based on SQL command: $sql"
    fi
}


log() {
    local "last_return=$?"
    if [ "$NOLOG" -ne 1 ]
    then
        echo "$@" 1>&2
    fi
    return "$last_return"
}
