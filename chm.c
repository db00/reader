/* $Id: test_chmLib.c,v 1.5 2002/10/09 12:38:12 jedwin Exp $ */
/***************************************************************************
  gcc -D test_chm -Wall chm.c chm_lib.c lzx.c && ./a.out gl2.chm /gl/glReadPixels.htm
  gcc -D test_chm -Wall chm.c chm_lib.c lzx.c && ./a.out gl2.chm
 *          test_chmLib.c - CHM archive test driver                        *
 *                           -------------------                           *
 *                                                                         *
 *  author:     Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  notes:      This is the quick-and-dirty test driver for the chm lib    *
 *              routines.  The program takes as its inputs the path to a   *
 *              .chm file, a path within the .chm file, and a destination  *
 *              path.  It attempts to open the .chm file, locate the       *
 *              desired file in the archive, and extract to the specified  *
 *              destination.                                               *
 *                                                                         *
 *              It is not included as a particularly useful program, but   *
 *              rather as a sort of "simplest possible" example of how to  *
 *              use the resolve/retrieve portion of the API.               *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "chm_lib.h"

#ifdef WIN32
#include <malloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct chmfile{
	char * filename;	
	char * data;	
	int len;	
};

/*
 * callback function for enumerate API
 */
static int _print_ui(struct chmFile *h,
		struct chmUnitInfo *ui,
		void *context)
{
	static char szBuf[128];
	memset(szBuf, 0, 128);
	if(ui->flags & CHM_ENUMERATE_NORMAL)
		strcpy(szBuf, "normal ");
	else if(ui->flags & CHM_ENUMERATE_SPECIAL)
		strcpy(szBuf, "special ");
	else if(ui->flags & CHM_ENUMERATE_META)
		strcpy(szBuf, "meta ");

	if(ui->flags & CHM_ENUMERATE_DIRS)
		strcat(szBuf, "dir");
	else if(ui->flags & CHM_ENUMERATE_FILES)
		strcat(szBuf, "file");

	struct chmfile * f = (struct chmfile*)context;
	if(f && f->filename && strlen(f->filename)) {
		if(strcmp(f->filename,ui->path)==0) {
			//printf("resolving %s\n", (char*)f->filename);
			if (CHM_RESOLVE_SUCCESS == chm_resolve_object(h, 
						f->filename,
						ui))
			{
				f->data = malloc(ui->length+1);
				memset(f->data ,0,ui->length+1);
				/*
				   printf("    object: <%d, %lu, %lu>\n",
				   ui->space,
				   (unsigned long)ui->start,
				   (unsigned long)ui->length);
				   */

				chm_retrieve_object(h, ui, (unsigned char*)f->data, 0, ui->length);
				f->len = ui->length+1;
				return CHM_ENUMERATOR_SUCCESS;
			}
		}
	}else{
		char out[1024];
		memset(out,0,sizeof(1024));
		sprintf(out,"   %1d %8d %8d   %s\t\t%s\n",
				(int)ui->space,
				(int)ui->start,
				(int)ui->length,
				szBuf,
				ui->path);
		if(f->data==NULL){
			f->data = malloc(strlen(out)+1);
			memset(f->data,0,strlen(out)+1);
			f->len = strlen(out)+1;
		}else{
			f->data = realloc(f->data,f->len+strlen(out));
			f->len += strlen(out);
		}
		//sprintf(f->data+strlen(f->data),"%s",out);
		strcat(f->data,out);
	}
	return CHM_ENUMERATOR_CONTINUE;
}

char * chm_parse(char * chmdata,int chmdatalen,char * filename,int * outlen)
{
	struct chmFile *h = chm_mem(chmdata,chmdatalen);
	if (h == NULL)
	{
		fprintf(stderr, "failed to open %s\n", filename);
		exit(1);
	}
	struct chmfile getfile;
	memset(&getfile,0,sizeof(struct chmfile));
	getfile.filename = filename;
	if (! chm_enumerate(h,
				CHM_ENUMERATE_ALL,
				_print_ui,
				&getfile))
	{
		printf("   *** ERROR ***\n");
	}
	*outlen = getfile.len;
	chm_close(h);
	//printf("%s",getfile.data);
	return getfile.data;
}


#ifdef test_chm
int main(int c, char **v)
{
	FILE * file = fopen(v[1],"rb");
	int fileLen = fseek(file,0,SEEK_END);
	fileLen = ftell(file);
	rewind(file);
	//printf("%s,%d\n",filename,fileLen);

	char * data = malloc(fileLen);
	if(data==NULL)
	{
		fclose(file);
		return -1;
	}
	fread(data,1,fileLen,file);
	fclose(file);

	int outlen=0;
	char * out = chm_parse(data,fileLen,v[2],&outlen);
	//char * out = chm_parse(data,fileLen,NULL,&outlen);
	if(out && outlen>0)
	{
		printf("%s",out);
		free(out);
		out = NULL;
	}
	return 0;
}
#endif
