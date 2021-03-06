/*
 * ----------------------------------------------------------------------
 * Created      : Sat Dec 10 17:27:21 1994 toka
 * Last modified: Sat Apr 18 23:36:15 1998 tri
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
 * $Revision: 1.10 $
 * $State: Exp $
 * $Date: 1998/04/18 20:52:28 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: mkrwritebox.c,v $
 * Revision 1.10  1998/04/18 20:52:28  tri
 * New copyright in COPYRIGHT.
 *
 * Revision 1.9  1994/12/14 03:36:38  tri
 * Added -version flag and version number :).
 *
 * Revision 1.8  1994/12/14  00:46:16  tri
 * Fixed for configure system.
 *
 * Revision 1.7  1994/12/13  20:28:57  tri
 * Preparation for autoconfig and tcp-port change.
 *
 * Revision 1.6  1994/12/12  15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.5  1994/12/12  11:03:42  tri
 * Added compatibility fixes from toka.
 *
 * Revision 1.4  1994/12/11  18:48:49  cirion
 * Fixed potential security bug. Minor cleanup.
 *
 * Revision 1.3  1994/12/11  18:40:28  tri
 * Now makerules work like the lavatory in the train
 * and portability is as good as I can make it.
 *
 * Revision 1.2  1994/12/11  14:56:13  tri
 * Minor fix.
 *
 * Revision 1.1  1994/12/10  15:38:22  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 */
#define __MKRWRITEBOX_C__ 1
#ifndef lint
static char *RCS_id = "$Id: mkrwritebox.c,v 1.10 1998/04/18 20:52:28 tri Exp $";
#endif /* not lint */

#define MKRWRITEBOX_VERSION_NUMBER	"1.0"	/* Program version   */

#include <stdio.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <string.h>
#include <sys/stat.h>
#include <sys/param.h>

#ifndef MAXPATHLEN
#  define MAXPATHLEN PATH_MAX
#endif

int main(int argc, char **argv) {
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    char path[MAXPATHLEN + 1];
    char *file = NULL;
    char *t = NULL;
    int f = -1;
    int uid = -1;
    int ttygid = -1;

    *path = '\000';

    if((argc == 2) && (!(strcmp("-version", argv[1])))) {
	fprintf(stderr, "Mkrwritebox version %s.\n", 
		MKRWRITEBOX_VERSION_NUMBER);
	exit(0);
    }
    if(2 != argc) {
	fprintf(stderr, "Usage: mkrwritebox filename\n");
	exit(1);
    }
    file = argv[1];

#define BADFNAMEXIT()                                       \
    fputs("mkrwritebox: Bad filename. Exiting.\n", stderr); \
    exit(2);

    if(!(*file) || !(strcmp("..", file)) || !(strcmp(".", file))) {
	BADFNAMEXIT();
    }

    for(t = file; t && *t ; t++) {
	if(*t == '/') {
	    BADFNAMEXIT();
	}
    }

    pwd = getpwuid(uid = getuid());
    grp = getgrnam(TTY_GROUP_NAME);
    if(!pwd || !grp) {
	fputs("mkrwritebox: Cannot get user or group.\n", stderr);
	exit(3);
    }
    ttygid = grp->gr_gid;
    strcpy(path, pwd->pw_dir);
    strcat(path, "/");
    strcat(path, file);
    (void)umask(0);
    if(0 > (f = open(path, O_CREAT | O_EXCL, 0620))) {
	perror("mkrwritebox");
	exit(4);
    }
#ifdef HAVE_FCHOWN
    if(0 > (fchown(f, uid, ttygid)))
#else
    /*
     * Chown supposedly clears sgids, in case some Fast Eddie
     * managed to set it after open(). 
     */
    if(0 > (chown(path, uid, ttygid)))
#endif
	{
	    perror("mkrwritebox");
	    (void)unlink(path);
	    exit(5);
	}
    (void)close(f);
    fprintf(stdout, "Created \"%s\".\n", path);
    return(0);
}

/* EOF (mkrwritebox.c) */
