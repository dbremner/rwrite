/*  -*- c -*-
 *
 * $RCSfile: match.h,v $
 * ----------------------------------------------------------------------
 * Public header for wildcard string matcher by tri.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 20 10:34:43 1994 tri
 * Last modified: Tue Sep 20 11:26:16 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.3 $
 * $State: Exp $
 * $Date: 1994/12/12 15:58:41 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: match.h,v $
 * Revision 1.3  1994/12/12 15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.2  1994/09/20  08:26:36  tri
 * Minor fix.
 *
 * Revision 1.1  1994/09/20  08:24:13  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Timo J. Rinne <tri@cirion.fi> and Cirion oy.
 * 
 * Address: Cirion oy, PO-BOX 250, 00121 HELSINKI, Finland
 * 
 * Even though this code is copyrighted property of the author, it can
 * still be used for any purpose under following conditions:
 * 
 *     1) This copyright notice is not removed.
 *     2) Source code follows any distribution of the software
 *        if possible.
 *     3) Copyright notice above is found in the documentation
 *        of the distributed software.
 * 
 * Any express or implied warranties are disclaimed.  In no event
 * shall the author be liable for any damages caused (directly or
 * otherwise) by the use of this software.
 * ----------------------------------------------------------------------
 */
/* Headers are included only once. */
#ifndef __MATCH_H__
#define __MATCH_H__ 1
/*
** Wildcards for any single character and any string.
*/
#define MATCH_WILD_CHAR   '?'
#define MATCH_WILD_STRING '*'
/*
** Return values.
*/
#define RETVAL_LESS    (-1) /* No match and str is less or unknown. */
#define RETVAL_MATCH   (0)  /* Match. */
#define RETVAL_GREATER (1)  /* No match and str is known to be greater. */
/*
** Proto.
*/
int StrMatch(char *str, char *cnd);

#endif /* not __MATCH_H__ */
/* EOF (match.h) */
