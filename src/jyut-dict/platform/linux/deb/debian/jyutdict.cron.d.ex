#
# Regular cron jobs for the jyut-dict package
#
0 4	* * *	root	[ -x /usr/bin/jyut-dict_maintenance ] && /usr/bin/jyut-dict_maintenance
