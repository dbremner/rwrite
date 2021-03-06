/*  -*- c -*-
 *
 * $RCSfile: match.c,v $
 * ----------------------------------------------------------------------
 * Wildcard string matcher by tri.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 20 10:27:32 1994 tri
 * Last modified: Sat Apr 18 23:36:45 1998 tri
 * ----------------------------------------------------------------------
 * Copyright � 1994-1998
 * Timo J. Rinne <tri@iki.fi>
 * All rights reserved.  See file COPYRIGHT for details.
 *
 * Address: Cirion oy, PO-BOX 250, 00121 Helsinki, Finland
 * ----------------------------------------------------------------------
 * Any express or implied warranties are disclaimed.  In no event
 * shall the author be liable for any damages caused (directly or
 * otherwise) by the use of this software.
 *
 * Please, send your patches to <tri@iki.fi>.
 * ----------------------------------------------------------------------
 * $Revision: 1.3 $
 * $State: Exp $
 * $Date: 1998/04/18 20:52:28 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: match.c,v $
 * Revision 1.3  1998/04/18 20:52:28  tri
 * New copyright in COPYRIGHT.
 *
 * Revision 1.2  1994/12/12 15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.1  1994/09/20  08:24:13  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 */
#define __MATCH_C__ 1
#ifndef lint
static char *RCS_id = "$Id: match.c,v 1.3 1998/04/18 20:52:28 tri Exp $";
#endif /* not lint */
/*
** match.c
**
** Weird return values are simply done for b+-tree matching for mte.
** To get sane boolean values out of this use macro 
** #define BoolMatch(str, con) (StrMatch(str, cnd) ? 0 : 1)
*/
#include "match.h"
/*
** toupper() ja tolower() in <ctype.h> don't work with scands and they
** don't work properly with non-letters.
*/
#define AlphaToLower(ch) (((ch) >= 65 && (ch) <= 95)  ? ((ch) + 32) : (ch))
#define AlphaToUpper(ch) (((ch) >= 97 && (ch) <= 127) ? ((ch) - 32) : (ch))
/*
** StrMatch(char *str, char *cnd) compares string str and string cnd
** case insensitively. String cnd could contain wildcards.
*/
int StrMatch(char *str, char *cnd)
{
    unsigned char *s, *c;
    int wc = 0;

    /* We use unsigned chars internally, since 8-bit is coming to town */
    s = (unsigned char *)str;
    c = (unsigned char *)cnd;
    while(*c) {
	if(AlphaToLower(*c) != AlphaToLower(*s) &&
	   *c != MATCH_WILD_CHAR && *c != MATCH_WILD_STRING) {
	    /* 
	    ** No absolute match or wildcard, so piss off.
	    */
	    return ((((AlphaToLower(*s) > AlphaToLower(*c))) && (!(wc))) ?
		    RETVAL_GREATER : RETVAL_LESS);
	} else if(*c == MATCH_WILD_STRING) {
	    wc = 1;
	    while(*c == MATCH_WILD_STRING)
		c++;
	    while(*s) {
		if(!(StrMatch((char *)(s), (char *)(c))))
		    return RETVAL_MATCH; /* We found matching combination. */
		s++;
            }
	    if(StrMatch((char *)(s), (char *)(c)))
		return RETVAL_LESS; /* No match, so it simply doesn't match. */
	    else
		return RETVAL_MATCH; /* Wildcard standed for nothing but... */
	} else {
	    if(*c == MATCH_WILD_CHAR)
		wc = 1;
	    s++; /* Scroll the str and cond strings one step forward... */
	    c++; /* ...and continue loop. */
	}
    }
    /* If str string has ended too, retuern true, else it doesn't match. */
    return(!(*s) ? RETVAL_MATCH : (wc ? RETVAL_LESS : RETVAL_GREATER));
}

/* EOF (match.c) */
