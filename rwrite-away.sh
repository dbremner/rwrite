#!/bin/sh
#
# $RCSfile: rwrite-away.sh,v $
# ----------------------------------------------------------------------
# away(1)  -  handle automagically ~/.rwrite-autoreply files.
# ----------------------------------------------------------------------
# Created      : Mon Dec 12 16:18:23 1994 tri
# Last modified: Mon Dec 12 18:44:25 1994 tri
# ----------------------------------------------------------------------
# $Revision: 1.4 $
# $State: Exp $
# $Date: 1994/12/12 16:44:58 $
# $Author: tri $
# ----------------------------------------------------------------------
# $Log: rwrite-away.sh,v $
# Revision 1.4  1994/12/12 16:44:58  tri
# Added -q option.
#
# Revision 1.3  1994/12/12  15:58:41  tri
# Copyright fixed a bit.
#
# Revision 1.2  1994/12/12  15:15:06  tri
# # is now comment in ~/.rwrite-away file but only
# if it's the first character on the line.
#
# Revision 1.1  1994/12/12  14:43:46  tri
# Initial revision
#
# ----------------------------------------------------------------------
# Copyright 1994, Timo J. Rinne <tri@cirion.fi> and Cirion oy.
# 
# Address: Cirion oy, PO-BOX 250, 00121 HELSINKI, Finland
# 
# Even though this code is copyrighted property of the author, it can
# still be used for any purpose under following conditions:
# 
#     1) This copyright notice is not removed.
#     2) Source code follows any distribution of the software
#        if possible.
#     3) Copyright notice above is found in the documentation
#        of the distributed software.
# 
# Any express or implied warranties are disclaimed.  In no event
# shall the author be liable for any damages caused (directly or
# otherwise) by the use of this software.
# ----------------------------------------------------------------------
#

# Needs grep(1), awk(1), sed(1) and working /bin/sh.

if [ "x$RWRITE_AWAY_FILE" = "x" ]
then
    RWRITE_AWAY_FILE="$HOME/.rwrite-away"    
fi

if [ "x$RWRITE_AUTOREPLY_FILE" = "x" ]
then
    RWRITE_AUTOREPLY_FILE="$HOME/.rwrite-autoreply"
fi

if [ -r "$RWRITE_AWAY_FILE" ]
then
    if [ "$1" = "-b" ]
    then
        if [ -f "$HOME/.rwrite-autoreply" ]
        then
            rm -f "$HOME/.rwrite-autoreply"
	    echo "away: You are no longer marked as being away."
            exit 0
	else
            echo "away: Cannot see $RWRITE_AUTOREPLY_FILE.  You are not marked as being away."
	    exit 1
        fi
    fi
    if [ "$1" = "-q" ]
    then
        if [ -f "$HOME/.rwrite-autoreply" ]
        then
	    cat "$HOME/.rwrite-autoreply" | sed '1s/^I/You/'
	else
            echo "away: You are not marked as being away."
        fi
	exit 0
    fi
    if [ -f "$HOME/.rwrite-autoreply" ]
    then
        echo "away: There already is $RWRITE_AUTOREPLY_FILE.  Use away -b to remove it."
	exit 1
    fi
    if [ "x$1" = "x" ]
    then
        echo "Type in the away message.  EOF terminates the input."
	away_body="`cat`"
    else
        away_body="`grep -v '^#' < $RWRITE_AWAY_FILE | awk 'BEGIN { mode = 1 } { if (mode == 1 && match($0, "::'$1'::")) { mode = 2 } else if (mode == 2 && match($0, "^::.*::$")) { mode = 3 } else if (mode == 2) { print $0 } }'`"
    fi
    if [ "x$away_body" = "x" ]
    then
        echo "away: No away message."
        exit 1
    fi
    away_hdr="I have been marked as being away at `date`."
    away_msg=`echo "$away_hdr"; echo ""; echo "$away_body"`
    echo "$away_msg" | sed '1s/^I/You/'
    echo "$away_msg" > $RWRITE_AUTOREPLY_FILE
    exit 0
else
    echo "away: Cannot access $RWRITE_AWAY_FILE."
    exit 1
fi

exit 0
# EOF (away)
