#!/bin/sh
# $Id: showtree.sh,v 11.1 2010/07/05 20:47:51 tom Exp $
# test-script for RCS baseline utility
#
echo '** resulting tree:'

LANG=C; export LANG
LC_ALL=C; export LC_ALL
LC_TIME=C; export LC_TIME
LC_CTYPE=C; export LC_CTYPE
LANGUAGE=C; export LANGUAGE
LC_COLLATE=C; export LC_COLLATE
LC_NUMERIC=C; export LC_NUMERIC
LC_MESSAGES=C; export LC_MESSAGES

find "$@" -print | sort
