/*  -*- c -*-
 *
 * $RCSfile: rwrite.h,v $
 * ----------------------------------------------------------------------
 * Header file of the rwrite and rwrited programs that implement
 * the RWP protocol.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:58 1994 tri
 * Last modified: Tue Sep 20 22:08:30 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.7 $
 * $State: Exp $
 * $Date: 1994/09/20 19:08:57 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrite.h,v $
 * Revision 1.7  1994/09/20 19:08:57  tri
 * Added a configuration option for rwrited ran
 * without tty setgid.
 *
 * Revision 1.6  1994/09/20  08:24:13  tri
 * Support for .rwrite-allow and .rwrite-deny files.
 *
 * Revision 1.5  1994/09/19  22:40:37  tri
 * TOOK replaced by VRFY and made some considerable
 * cleanup.
 *
 * Revision 1.4  1994/09/15  20:14:42  tri
 * Completed the support of RWP version 1.0.
 *
 * Revision 1.3  1994/09/14  16:04:50  tri
 * Nothing really.
 *
 * Revision 1.2  1994/09/14  14:59:48  tri
 * Added a few codes.
 *
 * Revision 1.1  1994/09/13  12:32:13  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Timo Rinne <tri@cirion.fi> and Cirion oy.
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
#ifndef __RWRITE_H__
#define __RWRITE_H__ 1

/*
 * If DO_NOT_TELL_USERS is defined, server gives "user not in" notification
 * instead of "no such user".
 */
/* #define DO_NOT_TELL_USERS 1 */
/*
 * If rwrited program is not going to be ran as a tty-group setgid
 * process you should define NO_TERMINAL_SGID to ensure that VRFY
 * command works about right.
 */
/* #define NO_TERMINAL_SGID 1 */

#define RWP_VERSION_NUMBER	"1.0"		/* Protocol version */
#define RWRITED_VERSION_NUMBER	"1.0b"		/* Server version   */
#define RWRITE_VERSION_NUMBER	"1.0b"		/* Client version   */
/*
 * User definitions are in the following files.
 */
#define RWRITE_FILE_DENY	".rwrite-deny"
#define RWRITE_FILE_ALLOW	".rwrite-allow"
#define RWRITE_FILE_FORWARD	".rwrite-forward"	/* Not implemented */
#define RWRITE_FILE_AGENT	".rwrite-agent"		/* Not implemented */

/*************************************************/
/*************************************************/
/********* END OF THE USER CONFIGURATION *********/
/*************************************************/
/*************************************************/

/*
 * #
 * # Entry to enable rwrite service in /etc/services.
 * # The port number is about to change in near future.
 * #
 * rwrite		2801/tcp			# rwrite
 */
#define RWRITE_DEFAULT_PORT	2801

#define RWRITE_FWD_LIMIT	32
/*
 * These response codes follow the RWP version 1.0 RFC.
 */
/*
 * Success codes.
 */
#define RWRITE_READY		100
#define RWRITE_BYE		101
#define RWRITE_DELIVERY_OK	103
#define RWRITE_DELIVERY_FORWARDED	104
#define RWRITE_SENDER_OK	105
#define RWRITE_RCPT_OK		106
#define RWRITE_MSG_OK		107
#define RWRITE_RCPT_OK_TO_SEND	108
#define RWRITE_RSET_OK		109
#define RWRITE_RCPT_OK_TO_FWD	110
#define RWRITE_FHST_OK		111
#define RWRITE_QUOTE_OK		112
/*
 * Server readu to receive message body.
 */
#define RWRITE_GETMSG		200
/*
 * Informational responses.
 */
#define RWRITE_HELO		500
#define RWRITE_VER		501
#define RWRITE_PROT		502
#define RWRITE_HELP		510
#define RWRITE_INFO		511 /* Stuff for client to ignore. */
/*
 * Error codes.
 */
#define RWRITE_ERR_FATAL	666
#define RWRITE_ERR_SYNTAX	668
#define RWRITE_ERR_PERMISSION_DENIED	669
#define RWRITE_ERR_USER_NOT_IN	670
#define RWRITE_ERR_NO_SUCH_USER	671
#define RWRITE_ERR_NO_MESSAGE	672
#define RWRITE_ERR_NO_SENDER	673
#define RWRITE_ERR_NO_ADDRESS	674
#define RWRITE_ERR_NO_DATA	675
#define RWRITE_ERR_FWD_LIMIT_EXCEEDED	676
#define RWRITE_ERR_FWD_FAILED	677
#define RWRITE_ERR_QUOTE_CMD_FAILED	678
#define RWRITE_ERR_QUOTE_CMD_UNKNOWN	679
#define RWRITE_ERR_INTERNAL	698
#define RWRITE_ERR_UNKNOWN	699

#endif /* not __RWRITE_H__ */
/* EOF (rwrite.h) */
