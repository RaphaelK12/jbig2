/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -  This software is distributed in the hope that it will be
 -  useful, but with NO WARRANTY OF ANY KIND.
 -  No author or distributor accepts responsibility to anyone for the
 -  consequences of using this software, or for whether it serves any
 -  particular purpose or works at all, unless he or she says so in
 -  writing.  Everyone is granted permission to copy, modify and
 -  redistribute this source code, for commercial or non-commercial
 -  purposes, with the following restrictions: (1) the origin of this
 -  source code must not be misrepresented; (2) modified versions must
 -  be plainly marked as such; and (3) this notice may not be removed
 -  or altered from any source or modified source distribution.
 *====================================================================*/

/*
 * xtractprotos.c
 *
 *   This program accepts a list of C files on the command line
 *   and outputs the C prototypes to stdout.  It uses cpp to
 *   handle the preprocessor macros, and then parses the cpp output.
 *   In use, it is convenient to redirect stdout to a file.
 *
 *   Update the version number when making a new version.
 *
 *   For simple C prototype extraction, xtractprotos has essentially
 *   the same functionality as Adam Bryant's cextract, but the latter
 *   has not been officially supported for over 10 years, has been
 *   patched numerous times, and currently doesn't work with
 *   sys/sysmacros.h for 64 bit architecture.
 *
 *   This is used to extract all prototypes in leptonlib, in the
 *   file leptprotos.h.  The function that does all the work is
 *   parseForProtos(), which takes as input the output from cpp.
 *   To avoid including the very large leptprotos.h in the input
 *   from each file, cpp runs here with -DNO_PROTOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allheaders.h"

static const char *tempfile = "/tmp/temp_cpp_output";
static const l_int32  L_BUF_SIZE = 512;
static const char *version = "1.3";

main(int    argc,
     char **argv)
{
char        *filename, *filein, *str;
char         buf[L_BUF_SIZE];
l_int32      i, len, ret;
SARRAY      *sa;
static char  mainName[] = "xtractprotos";

        /* Output extern C head */
    sa = sarrayCreate(0);
    sarrayAddString(sa, "/*", 1);
    snprintf(buf, L_BUF_SIZE,
             " *  This file was autogen'd by xtractprotos, v. %s", version);
    sarrayAddString(sa, buf, 1);
    sarrayAddString(sa, " */", 1);
    sarrayAddString(sa, "#ifdef __cplusplus", 1);
    sarrayAddString(sa, "extern \"C\" {", 1);
    sarrayAddString(sa, "#endif  /* __cplusplus */\n", 1);
    str = sarrayToString(sa, 1);
    fprintf(stdout, str);
    sarrayDestroy(&sa);
    FREE(str);

    for (i = 1; i < argc; i++) {
        filein = argv[i];
	len = strlen(filein);
	if (filein[len - 1] == 'h')
	    continue;
	snprintf(buf, L_BUF_SIZE, "cpp -ansi -DNO_PROTOS %s %s",
	         filein, tempfile);
	ret = system(buf);
	if (ret) {
            fprintf(stderr, "cpp failure for %s; continuing\n", filein);
	    continue;
	}

#ifndef _CYGWIN_ENVIRON
        filename = stringNew(tempfile);
#else
        filename = stringJoin(tempfile, ".exe");
#endif  /* ~ _CYGWIN_ENVIRON */

	if ((str = parseForProtos(filename)) == NULL) {
            fprintf(stderr, "parse failure for %s; continuing\n", filein);
	    continue;
	}
	if (strlen(str) > 1) {  /* strlen == 1 is a file without protos */
            fprintf(stdout, str);
            FREE(str);
        }
	FREE(filename);
    }

        /* Output extern C tail */
    sa = sarrayCreate(0);
    sarrayAddString(sa, "\n#ifdef __cplusplus", 1);
    sarrayAddString(sa, "}", 1);
    sarrayAddString(sa, "#endif  /* __cplusplus */", 1);
    str = sarrayToString(sa, 1);
    fprintf(stdout, str);
    sarrayDestroy(&sa);
    FREE(str);

    return 0;
}


