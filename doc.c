/**
 * @file doc.c
 gcc doc.c ole.c bytearray.c -D test_doc -lm && ./a.out
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-10-22
 */
#include"doc.h"
FibBase * FibBase_read(ByteArray *bytearray)
{
	int position = bytearray->position;
	FibBase * fibbase = malloc(sizeof(FibBase));
	memset(fibbase,0,sizeof(FibBase));
	fibbase->wIdent = ByteArray_readInt16(bytearray);
	if(fibbase->wIdent!=0xA5EC)
	{
		printf("not a WORD file!\n");
		free(fibbase);
		return NULL;
	}
	fibbase->nFib= ByteArray_readInt16(bytearray);
	fibbase->unused= ByteArray_readInt16(bytearray);
	fibbase->lid= ByteArray_readInt16(bytearray);
	fibbase->pnNext= ByteArray_readInt16(bytearray);
	//fibbase->flag = ByteArray_readInt16(bytearray);
	ByteArray_readBytes(bytearray,2,(char*)&(fibbase->flag));
	fibbase->nFibBack = ByteArray_readInt16(bytearray);
	if(fibbase->nFibBack != 0xbf && fibbase->nFibBack != 0xc1)
	{
		printf("nFibBack error! %x\n",fibbase->nFibBack);
		free(fibbase);
		return NULL;
	}
	fibbase->lKey = ByteArray_readInt32(bytearray);
	fibbase->envr = ByteArray_readByte(bytearray);
	//fibbase->flag2= ByteArray_readByte(bytearray);
	ByteArray_readBytes(bytearray,1,(char*)&(fibbase->flag2));
	bytearray->position += 12;//reserved 12 bytes
	printf("FibBase (%d bytes == 32) read ok!\n",bytearray->position-position);
	return fibbase;
}
FibRgLw97 * FibRgLW97_read(ByteArray *bytearray)
{
	int position = bytearray->position;

	FibRgLw97 * fibRgLw = malloc(sizeof(FibRgLw97));
	memset(fibRgLw,0,sizeof(FibRgLw97));
	fibRgLw->cbMac = ByteArray_readInt32(bytearray);
	bytearray->position+=8;
	fibRgLw->ccpText= ByteArray_readInt32(bytearray);
	fibRgLw->ccpFtn= ByteArray_readInt32(bytearray);
	fibRgLw->ccpHdd= ByteArray_readInt32(bytearray);
	bytearray->position+=4;
	fibRgLw->ccpAtn= ByteArray_readInt32(bytearray);
	fibRgLw->ccpEdn= ByteArray_readInt32(bytearray);
	fibRgLw->ccpTxbx= ByteArray_readInt32(bytearray);
	fibRgLw->ccpHdrTxbx= ByteArray_readInt32(bytearray);
	bytearray->position+=44;
	printf("fibRgLw cbMac:%d, ccpText:%d, ccpFtn:%d, ccpHdd:%d, ccpAtn:%d, ccpEdn:%d, ccpTxbx:%d, ccpHdrTxbx:%d, (%d)\n"
			,fibRgLw->cbMac
			,fibRgLw->ccpText
			,fibRgLw->ccpFtn
			,fibRgLw->ccpHdd
			,fibRgLw->ccpAtn
			,fibRgLw->ccpEdn
			,fibRgLw->ccpTxbx
			,fibRgLw->ccpHdrTxbx
			,(int)sizeof(FibRgLw97)
		  );

	bytearray->position = position + 88;
	printf("FibRgLW97_read(%d bytes == 88) read ok!\n",bytearray->position-position);
	return fibRgLw;
}


FibRgFcLcb * FibRgFcLcb_read(ByteArray*bytearray)
{
	//int position = bytearray->position;
	FibRgFcLcb* fibrgfclcb = malloc(sizeof(FibRgFcLcb));
	memset(fibrgfclcb,0,sizeof(FibRgFcLcb));

	int i = 0;
	fibrgfclcb->rgFcLcb97 = malloc(744);
	memset(fibrgfclcb->rgFcLcb97,0,744);
	while(i<744)
	{
		int * p = (int*)((char*)fibrgfclcb->rgFcLcb97+ i);
		*p = ByteArray_readInt32(bytearray);
		i+= 4;
	}
	//printf("[0x%x],%d==744 == %d\n",position,sizeof(FibRgFcLcb97),bytearray->position-position);
	fibrgfclcb->rgFcLcb2000 = malloc(864-744);
	memset(fibrgfclcb->rgFcLcb2000,0,864-744);
	while(i<864)
	{
		int * p = ((int*)((char*)fibrgfclcb->rgFcLcb2000+ i));
		*p = ByteArray_readInt32(bytearray);
		i+= 4;
	}
	fibrgfclcb->rgFcLcb2002 = malloc(1088-864);
	memset(fibrgfclcb->rgFcLcb2002,0,1088-864);
	while(i<1088)
	{
		int * p = (int*)((char*)fibrgfclcb->rgFcLcb2002+ i);
		*p = ByteArray_readInt32(bytearray);
		i+= 4;
	}
	fibrgfclcb->rgFcLcb2003 = malloc(1312-1088);
	memset(fibrgfclcb->rgFcLcb2003,0,1312-1088);
	while(i<1312)
	{
		int * p = (int*)((char*)fibrgfclcb->rgFcLcb2003+ i);
		*p = ByteArray_readInt32(bytearray);
		i+= 4;
	}
	//printf("[0x%x],%d = 1312-1088 = %d\n",position,sizeof(FibRgFcLcb2003),bytearray->position-position - position);
	fibrgfclcb->rgFcLcb2007 = malloc(sizeof(FibRgFcLcb2007));
	memset(fibrgfclcb->rgFcLcb2007,0,sizeof(FibRgFcLcb2007));
	while(i<1312 + sizeof(FibRgFcLcb2007))
	{
		int * p = (int*)((char*)fibrgfclcb->rgFcLcb2007+ i);
		*p = ByteArray_readInt32(bytearray);
		i+= 4;
	}
	//printf("[0x%x],%d = %d\n",position,sizeof(FibRgFcLcb2007),bytearray->position-position - position);

	printf("FibRgFcLcb (%d)\n" , (int)sizeof(FibRgFcLcb));
	return fibrgfclcb;
}


void FIB_free(FIB * fib)
{
	if(fib)
	{
		if(fib->base){
			free(fib->base);
		}
		free(fib);
	}
}

FibRgW97 * FibRgW97_read(ByteArray*bytearray)
{//28 bytes;
	int position = bytearray->position;
	FibRgW97 * fibrgw = malloc(sizeof(FibRgW97));
	memset(fibrgw,0,sizeof(FibRgW97));
	bytearray->position = position + 28;
	//printf("FibRgW97_read (%d bytes == 28) read ok!\n",bytearray->position-position);
	return fibrgw;
}
FibRgCswNew * FibRgCswNew_read(ByteArray * bytearray)
{
	//printf("FibRgCswNew_read\n");
	FibRgCswNew * fibrgcswnew  = malloc(sizeof(FibRgCswNew ));
	memset(fibrgcswnew,0,sizeof(FibRgCswNew ));
	fibrgcswnew->nFibNew = ByteArray_readInt16(bytearray);// (2 bytes): An unsigned integer that specifies the version number of the file format that is used. This value MUST be one of the following.  Value
	switch(fibrgcswnew->nFibNew)
	{
		case 0x00D9 :
			//fibrgcswnew->rgCswNewData[0] = ByteArray_readInt16(bytearray);
			break;
		case 0x0101 :
			//fibrgcswnew->rgCswNewData[0] = ByteArray_readInt16(bytearray);
			break;
		case 0x010C :
			//fibrgcswnew->rgCswNewData[0] = ByteArray_readInt16(bytearray);
			break;
		case 0x0112 :
			//fibrgcswnew->rgCswNewData[0] = ByteArray_readInt16(bytearray);
			//fibrgcswnew->rgCswNewData[2] = ByteArray_readInt16(bytearray);
			//fibrgcswnew->rgCswNewData[4] = ByteArray_readInt16(bytearray);
			//fibrgcswnew->rgCswNewData[6] = ByteArray_readInt16(bytearray);
			break;
		default:
			printf("fibrgcswnew ERROR!");
			break;
	}
	return  fibrgcswnew ;
}

FIB * DocFile_readFib(DocFile *file)
{/*{{{*/
	if(file->fib)
		return file->fib;

	ByteArray * bytearray = file->bytearray;
	StorageEntry * entry = Ole_getEntryByName(file->header,"WordDocument");
	if(entry)
	{
		bytearray->position = Ole_getEntryPostion(file->header,entry);
	}else{
		printf("{WordDocument not found}\n");
		return 0;
	}

	FIB * fib = malloc(sizeof(FIB));
	memset(fib,0,sizeof(FIB));
	//printf("read fib at[0x%x]\n ",bytearray->position);
	fib->base = FibBase_read(bytearray);//32 bytes
	fib->csw = ByteArray_readInt16(bytearray);//csw (2 bytes): An unsigned integer that specifies the count of 16-bit values corresponding to fibRgW that follow. MUST be 0x000E.
	if(fib->csw!=0xe)
	{
		printf("error fibrgw number!\n");
		FIB_free(fib);
		return NULL;
	}
	fib->fibrgw = FibRgW97_read(bytearray);//28 bytes
	fib->cslw = ByteArray_readInt16(bytearray);//cslw (2 bytes): An unsigned integer that specifies the count of 32-bit values corresponding to fibRgLw that follow. MUST be 0x0016.
	//printf("read cslw at[0x%x]\n ",bytearray->position);
	if(fib->cslw != 0x16)
	{
		printf("error cslw number!\n");
		FIB_free(fib);
		return NULL;
	}
	fib->fibRgLw = FibRgLW97_read(bytearray); //fibRgLw (88 bytes): The FibRgLw97.
	fib->cbRgFcLcb = ByteArray_readInt16(bytearray);//cbRgFcLcb (2 bytes): An unsigned integer that specifies the count of 64-bit values corresponding to fibRgFcLcbBlob that follow. This MUST be one of the following values, depending on the value of nFib.
	//printf("read fibRgLw at[0x%x]\n ",bytearray->position);
	//0x00C1 fibRgFcLcb97
	switch(fib->cbRgFcLcb)
	{
		case 0x005D : // case 0x00C1 :
			printf("%x,%x\n",0x00C1,fib->base->nFib);
			fib->base->nFib = 0x00C1;
			printf("FibRgFcLcb97,");
			break;
		case 0x006C :// case 0x00D9 :
			printf("%x,%x\n",0x00D9,fib->base->nFib);
			fib->base->nFib = 0x00D9;
			printf("FibRgFcLcb2000,");
			break;
		case 0x0088 :// case 0x0101 :
			printf("%x,%x\n",0x0101,fib->base->nFib);
			fib->base->nFib = 0x0101;
			printf("FibRgFcLcb2002,");
			break;
		case 0x00A4 :// case 0x010C :
			printf("%x,%x\n",0x010C,fib->base->nFib);
			fib->base->nFib = 0x010C;
			printf("FibRgFcLcb2003,");
			break;
		case 0x00B7 :// case 0x0112:
			printf("%x,%x\n",0x0112,fib->base->nFib);
			fib->base->nFib = 0x0112;
			printf("FibRgFcLcb2007,");
			break;
		default:
			printf("cbRgFcLcb ERROR!!!!\n");
			FIB_free(fib);
			return NULL;
	}
	//printf("cbRgFcLcb :%x,%x\n",fib->cbRgFcLcb,fib->base->nFib);
	//return NULL;
	//char * fibRgFcLcbBlob = malloc(fib->cbRgFcLcb*8);
	//ByteArray_readBytes(bytearray,fib->cbRgFcLcb*8,fibRgFcLcbBlob); //fibRgFcLcbBlob (variable): The FibRgFcLcb.
	//fib->fibRgFcLcbBlob = (FibRgFcLcb*)fibRgFcLcbBlob;
	int position = bytearray->position;
	fib->fibRgFcLcbBlob = FibRgFcLcb_read(bytearray);
	switch(fib->base->nFib)
	{
		case 0x00c1:
			bytearray->position = position + 744;
			break;
		case 0x00D9:
			bytearray->position = position + 864;
			break;
		case 0x0101:
			bytearray->position = position + 1088;
			break;
		case 0x010C:
			bytearray->position = position + 1312;
			break;
		case 0x0112:
			bytearray->position = position + 1350;
			break;
	}
	//bytearray->position = position + fib->cbRgFcLcb * 8;

	fib->cswNew = ByteArray_readInt16(bytearray);//cswNew (2 bytes): An unsigned integer that specifies the count of 16-bit values corresponding to fibRgCswNew that follow. This MUST be one of the following values, depending on the value of nFib.  Value of nFib cswNew
	switch(fib->cswNew)
	{
		case 0x02 :
			break;
		case 0x05 :
			break;
		case 0:
			break;
		default:
			printf("cswNew ERROR!!!!\n");
			FIB_free(fib);
			return NULL;

	}
	//fibRgCswNew (variable): If cswNew is nonzero, this is fibRgCswNew. Otherwise, it is not present in the file.
	if(fib->cswNew)
	{
		fib->fibRgCswNew = FibRgCswNew_read(bytearray);
		fib->nFib = fib->fibRgCswNew->nFibNew;
	}else{//cswNew == 0;
		fib->nFib = fib->base->nFib;
	}
	//printf("nFib:0x%x\n",fib->nFib);
	printf("read fib end at[0x%x]\n ",bytearray->position);
	return fib;
}/*}}}*/

Clx * Clx_read(DocFile * file)
{
	if(file->fib==NULL)
		file->fib = DocFile_readFib(file);
	if(file->fib==NULL)
	{
		return NULL;
	}

	StorageEntry * entry = NULL;
	if(file->fib->base->flag.G)//fWhichTblStm;
	{
		entry = Ole_getEntryByName(file->header,"1Table");
	}else{
		entry = Ole_getEntryByName(file->header,"0Table");
	}
	if(entry == NULL)
	{
		printf("Table Stream not found");
		return 0;
	}
	ByteArray * bytearray = file->bytearray;
	unsigned int offset = file->fib->fibRgFcLcbBlob->rgFcLcb97->fcClx;
	unsigned int size = file->fib->fibRgFcLcbBlob->rgFcLcb97->lcbClx;
	int position = Ole_getEntryPostion(file->header,entry);
	//printf("read clx at[0x%x]\n ",position);
	bytearray->position = position + offset;
	Clx * clx = malloc(sizeof(Clx));
	memset(clx,0,sizeof(Clx));
	position = bytearray->position;
	printf("-------------read clx at[0x%x],%d,\n ",position,size);

	while(bytearray->position - position < size){
		Prc * prc  = malloc(sizeof(Prc));
		prc->clxt = ByteArray_readByte(bytearray);
		if(prc->clxt==0x02)
		{
			Pcdt * pcdt = malloc(sizeof(Pcdt));
			clx->pcdt = pcdt;

			pcdt->clxt = 0x02;
			pcdt->lcb = ByteArray_readInt32(bytearray);
			printf("Pcdt size:%d\n",pcdt->lcb);

			pcdt->plcpcd.aCP = malloc((pcdt->lcb+8)/2);

			unsigned int ccpText = file->fib->fibRgLw->ccpText;
			unsigned int ccpFtn = file->fib->fibRgLw->ccpFtn;
			unsigned int ccpHdd = file->fib->fibRgLw->ccpHdd;
			//unsigned int ccpMcr= file->fib->fibRgLw->ccpMcr;
			unsigned int ccpAtn= file->fib->fibRgLw->ccpAtn;
			unsigned int ccpEdn= file->fib->fibRgLw->ccpEdn;
			unsigned int ccpTxbx= file->fib->fibRgLw->ccpTxbx;
			unsigned int ccpHdrTxbx = file->fib->fibRgLw->ccpHdrTxbx;
			//int sum = ccpFtn + ccpHdd + ccpMcr + ccpAtn + ccpEdn + ccpTxbx + ccpHdrTxbx;
			int sum = ccpFtn + ccpHdd + ccpAtn + ccpEdn + ccpTxbx + ccpHdrTxbx;

			int i = 0;
			int numCP = 0;
			while(i<(pcdt->lcb))
			{
				unsigned int acp = ByteArray_readInt32(bytearray);
				if(acp>0x7fffffff)
				{
					printf("ERROR acp\n");
					return 0;
				}
				pcdt->plcpcd.aCP[numCP] = acp;
				//printf("(%d)0x%x %x,",i, pcdt->plcpcd.aCP[numCP],1<<30);
				//printf("(%d)0x%x ,",i, pcdt->plcpcd.aCP[numCP]);
				i+= 4;
				numCP++;
				if((sum > 0 && acp == ccpText + sum + 1) || acp == ccpText) {
					break;
				}
			}
			//bytearray->position -= 4;
			clx->numCP = numCP;
			int len = pcdt->lcb - i;
			clx->numPcd = len/8;
			pcdt->plcpcd.aPcd = malloc(len);
			memset(pcdt->plcpcd.aPcd,0,len);
			ByteArray_readBytes(bytearray,len,(char*)pcdt->plcpcd.aPcd);
			printf("\nccpText : 0x%x, numCP :%d, numPcd:%d\n",ccpText,clx->numCP,clx->numPcd);
			i = 0;
			while(i<clx->numPcd)
			{
				Pcd * pcd = &(pcdt->plcpcd.aPcd[i]);
				FcCompressed * fc = (FcCompressed*)&(pcd->fc);

				if(pcd->C)
					printf("XXXXXXXXXXXXXXXXXXXx\n");

				if(fc->B ){
					printf("XXXXXXXXXXXXXXXXXXXx%d,%x,%x,\n",i,fc->A,fc->fc);
					//fc->fc &= 0x7fffffff;
				}
				++i;
			}
			break;
		}else if(prc->clxt==0x01){
			printf("Prc size:%d\n",prc->clxt);
			short cbGrpprl = ByteArray_readInt16(bytearray);
			prc->GrpPrl = malloc(cbGrpprl+2);
			prc->cbGrpprl = cbGrpprl;
			printf("Prc size:%d!\n",prc->cbGrpprl);
			ByteArray_readBytes(bytearray,prc->cbGrpprl,prc->GrpPrl);
		}else{
			printf("ERROR Prc! XXXXXXXXXXXXXXXXXXXx\n");
			return NULL;
			break;
		}
	}
	return clx;
}


DocFile * DocFile_parse(ByteArray * bytearray)
{
	DocFile * file = malloc(sizeof(DocFile));
	memset(file,0,sizeof(DocFile));

	file->bytearray = bytearray;
	OleHeader * header = Ole_read(NULL,file->bytearray);
	if(header==NULL)
		return 0;
	file->header = header;
	file->clx = Clx_read(file);
	if(file->clx){
		PlcPcd * plcpcd = &(file->clx->pcdt->plcpcd);
		unsigned int * aCP = plcpcd->aCP;
		Pcd * aPcd = plcpcd->aPcd;
		int length = file->clx->numCP;

		StorageEntry * entry = Ole_getEntryByName(file->header,"WordDocument");
		if(entry==NULL){
			printf("{WordDocument not found}\n");
			return 0;
		}
		int position = Ole_getEntryPostion(file->header,entry);
		Pcd* pcd = NULL;
		int i=0;
		while(i<length-1)
		{
			pcd = &(aPcd[i]);
			FcCompressed * fc = (FcCompressed*)&(pcd->fc);
			if(fc->B)
				printf("XXXXXXXXXXXXXXXXXXXx%d,0x%x,%x==0,0x%x\n",i,fc->A,fc->B,fc->fc);
			unsigned int acp = aCP[i];//character position
			unsigned int acp2 = aCP[i+1];//character position
			unsigned int start = 0; 
			unsigned int end=0;
			if(fc->A){//fCompressed
				start = fc->fc/2 + position;
				end = fc->fc/2 + (acp2 - acp -1) + position;
				bytearray->position = start;
				//printf("u8 string [0x%x] (%x->%x):\n",bytearray->position,start,end);
				ByteArray_prints(bytearray,end-start);
			}else{
				start = fc->fc + position;
				end = fc->fc  + 2*(acp2-acp-1)+ position;
				bytearray->position = start;
				//printf("u16 string [0x%x] (%x->%x):\n",bytearray->position,start,end);
				ByteArray_print16(bytearray,(end-start)/2);
			}
			++i;
		}
	}
	return 0;
}
void PlcPcd_free(PlcPcd* plcpcd)
{
	if(plcpcd->aCP)
		free(plcpcd->aCP);
	if(plcpcd->aPcd)
		free(plcpcd->aPcd);
}

void Pcdt_free(Pcdt * pcdt)
{
	if(pcdt)
	{
		PlcPcd_free(&pcdt->plcpcd);
		free(pcdt);
	}
}
void Prc_free(Prc* reprc)
{
	if(reprc)
	{
		if(reprc->GrpPrl)
		{
			free(reprc->GrpPrl);
		}
		free(reprc);
	}
}

void Clx_free(Clx * clx)
{
	if(clx)
	{
		if(clx->RgPrc)
		{
			Prc_free(clx->RgPrc);
		}
		if(clx->pcdt)
		{
			Pcdt_free(clx->pcdt);
		}

		free(clx);
	}
}
void DocFile_free(DocFile * file)
{
	if(file)
	{
		if(file->header)
			Ole_free(file->header);
		if(file->fib)
			FIB_free(file->fib);
		if(file->clx)
			Clx_free(file->clx);
		free(file);
	}
}

#ifdef test_doc
int main()
{
	//char * filename = "test.doc";
	//char * filename = "excel_.doc";
	char * filename = "RAR.doc";
	//char * filename = "PDF.doc";
	FILE * _file = fopen(filename,"rb");
	int fileLen = fseek(_file,0,SEEK_END);
	fileLen = ftell(_file);
	rewind(_file);
	printf("%s,%d\n",filename,fileLen);

	ByteArray * bytearray = ByteArray_new(fileLen);
	fread(bytearray->data,1,fileLen,_file);
	fclose(_file);

	DocFile * file = DocFile_parse(bytearray);
	printf("\n%x",file);
	DocFile_free(file);
	ByteArray_free(bytearray);
	return 0;
}

#endif
