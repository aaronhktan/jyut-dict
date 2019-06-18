#
# Regular cron jobs for the jyutdict package
#
0 4	* * *	root	[ -x /usr/bin/jyutdict_maintenance ] && /usr/bin/jyutdict_maintenance
