/*  -*- c -*-
 *
 * $RCSfile: rwrite.h,v $
 * ----------------------------------------------------------------------
 * <Description of this file and its contents>
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:58 1994 tri
 * Last modified: Tue Sep 13 20:18:27 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.2 $
 * $State: Exp $
 * $Date: 1994/09/14 14:59:48 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrite.h,v $
 * Revision 1.2  1994/09/14 14:59:48  tri
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

#define RWP_VERSION_NUMBER	"1.0"		/* Protocol version */
#define RWRITED_VERSION_NUMBER	"0.9.1a"	/* Server version   */
#define RWRITE_VERSION_NUMBER	"0.0"		/* Client version   */

#define RWRITE_READY		100
#define RWRITE_BYE		101
#define RWRITE_DELIVERY_OK	103
#define RWRITE_DELIVERY_FORWARDED	104
#define RWRITE_SENDER_OK	105
#define RWRITE_RCPT_OK		106
#define RWRITE_MSG_OK		107
#define RWRITE_RCPT_OK_TO_SEND	108
#define RWRITE_RSET_OK		109

#define RWRITE_GETMSG		200

#define RWRITE_HELO		500
#define RWRITE_VER		501
#define RWRITE_PROT		502
#define RWRITE_HELP		510
#define RWRITE_INFO		511 /* Stuff for client to ignore. */

#define RWRITE_ERR_FATAL	666
#define RWRITE_ERR_EPERM	667
#define RWRITE_ERR_SYNTAX	668
#define RWRITE_ERR_PERMISSION_DENIED	669
#define RWRITE_ERR_USER_NOT_IN	670
#define RWRITE_ERR_NO_SUCH_USER	671
#define RWRITE_ERR_NO_MESSAGE	672
#define RWRITE_ERR_NO_SENDER	673
#define RWRITE_ERR_NO_ADDRESS	674
#define RWRITE_ERR_NO_DATA	675

#define RWRITE_ERR_INTERNAL	698
#define RWRITE_ERR_UNKNOWN	699

#endif /* not __RWRITE_H__ */
/* EOF (rwrite.h) */
