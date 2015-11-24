/**
 * @file xls.c
 gcc -g -Wall date.c mystring.c xls.c doc.c ole.c bytearray.c -D debug_xls -lm && ./a.out
 * 
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2015-10-22
 */

#include "date.h"
#include "xls.h"

typedef unsigned long
#ifndef __LP64__
long
#endif
u64;

typedef struct FrtHeader
{//The FrtHeader structure specifies a future record type header.
	unsigned short rt;//(2 bytes): An unsigned integer that specifies the record type identifier. MUST be identical to the record type identifier of the containing record.
	unsigned short grbitFrt;//(2 bytes): An FrtFlags that specifies attributes for this record. The value of grbitFrt.fFrtRef MUST be zero. The value of grbitFrt.fFrtAlert MUST be zero.
	unsigned char reserved[8];//(8 bytes): MUST be zero, and MUST be ignored. 
}FrtHeader;
void FrtHeader_free(FrtHeader* frtHeader)
{
	if(frtHeader)
	{
		free(frtHeader);
	}
}

FrtHeader * FrtHeader_read(ByteArray * bytearray)
{
	FrtHeader * frtHeader = malloc(sizeof(FrtHeader));
	memset(frtHeader,0,sizeof(FrtHeader));
	frtHeader->rt = ByteArray_readInt16(bytearray);
	frtHeader->grbitFrt = ByteArray_readInt16(bytearray);
	ByteArray_readBytes(bytearray,8,(char*)frtHeader->reserved);
	return frtHeader;
}
typedef struct HeaderFooter
{
	FrtHeader* frtHeader;//(12 bytes): An FrtHeader structure. The frtHeader.rt field MUST be 0x089C.
	char guidSView[16];//(16 bytes): A GUID as specified by [MS-DTYP] that specifies the current sheet view. If it is zero it means the current sheet. Otherwise, this field MUST match the guid field of the preceding UserSViewBegin record.
	unsigned short A:1;// fHFDiffOddEven (1 bit): A bit that specifies whether the odd and even pages use a different header and footer. If the value is 1, the Header and Footer records specify the odd page header and footer, and strHeaderEven and strFooterEven specify the even page header and footer.
	unsigned short B:1;// fHFDiffFirst (1 bit): A bit that specifies whether the first page uses a different header and footer from the rest of the pages. If the value is 1, the Header and Footer records specify the header and footer of the rest of the pages, and strHeaderFirst and strFooterFirst specify the first page header and footer.
	unsigned short C:1;// fHFScaleWithDoc (1 bit): A bit that specifies whether the header and footer is scaled with the sheet.
	unsigned short D:1;// fHFAlignMargins (1 bit): A bit that specifies whether the left and right edges of the header and footer are lined up with the left and right margins of the sheet.
	unsigned short unused:12;//(12 bits): Undefined, and MUST be ignored.
	unsigned short cchHeaderEven;//(2 bytes): An unsigned integer that specifies the number of characters in strHeaderEven. MUST be less than or equal to 255. The value MUST be zero if fHFDiffOddEven is zero.
	unsigned short cchFooterEven;//(2 bytes): An unsigned integer that specifies the number of characters in strFooterEven. MUST be less than or equal to 255. The value MUST be zero if fHFDiffOddEven is zero.
	unsigned short cchHeaderFirst;//(2 bytes): An unsigned integer that specifies the number of characters in strHeaderFirst. MUST be less than or equal to 255. The value MUST be zero if fHFDiffFirst is zero.
	unsigned short cchFooterFirst;//(2 bytes): An unsigned integer that specifies the number of characters in strFooterFirst. MUST be less than or equal to 255. The value MUST be zero if fHFDiffFirst is zero.
	XLUnicodeString * strHeaderEven;//(variable): An XLUnicodeString structure that specifies the header text on the even pages. The number of characters in the string MUST be equal to cchHeaderEven. The string can contain special commands, for example a placeholder for the page number, current date or text formatting attributes. Refer to Header for more details about the string format.
	XLUnicodeString * strFooterEven;//(variable): An XLUnicodeString structure that specifies the footer text on the even pages. The number of characters in the string MUST be equal to cchFooterEven. The string can contain special commands, for example a placeholder for the page number, current date or text formatting attributes. Refer to Header for more details about the string format.
	XLUnicodeString *strHeaderFirst;//(variable): An XLUnicodeString structure that specifies the header text on the first page. The number of characters in the string MUST be equal to cchHeaderFirst. The string can contain special commands, for example a placeholder for the page number, current date or text formatting attributes. Refer to Header for more details about the string format.
	XLUnicodeString * strFooterFirst;//(variable): An XLUnicodeString structure that specifies the footer text on the first page. The number of characters in the string MUST be equal to cchFooterFirst. The string can contain special commands, for example a placeholder for the page number, current date or text formatting attributes. Refer to Header for more details about the string format.
}HeaderFooter;
void HeaderFooter_free(HeaderFooter * headerfooter)
{
	if(headerfooter)
	{
		//if(headerfooter->frtHeader) FrtHeader_free(headerfooter->frtHeader);
		if(headerfooter->cchHeaderEven)
			XLUnicodeString_free(headerfooter->strHeaderEven);
		if(headerfooter->cchFooterEven)
			XLUnicodeString_free(headerfooter->strFooterEven);
		if(headerfooter->cchHeaderFirst)
			XLUnicodeString_free(headerfooter->strHeaderFirst);
		if(headerfooter->cchFooterFirst)
			XLUnicodeString_free(headerfooter->strFooterFirst);

		free(headerfooter);
	}

}
HeaderFooter * HeaderFooter_read(ByteArray * bytearray)
{
	HeaderFooter * headerfooter = malloc(sizeof(HeaderFooter));
	memset(headerfooter,0,sizeof(HeaderFooter));
	headerfooter->frtHeader = FrtHeader_read(bytearray);
	ByteArray_readBytes(bytearray,26,(char*)headerfooter+4);

	/*
	   printf(" A:%d, B:%d, C:%d, D:%d,\t"
	   "cchHeaderEven:%d,cchFooterEven:%d,cchHeaderFirst:%d,cchFooterFirst:%d,"
	   ,headerfooter->A
	   ,headerfooter->B
	   ,headerfooter->C
	   ,headerfooter->D
	   ,headerfooter->strHeaderEven
	   ,headerfooter->strFooterEven
	   ,headerfooter->strHeaderFirst
	   ,headerfooter->strFooterFirst
	   );
	   */
	if(headerfooter->cchHeaderEven){
		headerfooter->strHeaderEven = XLUnicodeString_read(bytearray);
	}
	if(headerfooter->cchFooterEven){
		headerfooter->strFooterEven= XLUnicodeString_read(bytearray);
	}
	if(headerfooter->cchHeaderFirst){
		headerfooter->strHeaderFirst= XLUnicodeString_read(bytearray);
	}
	if(headerfooter->cchFooterFirst){
		headerfooter->strFooterFirst= XLUnicodeString_read(bytearray);
	}
	return headerfooter;
}
typedef struct Dimensions
{
	unsigned int rwMic;//(4 bytes): A RwLongU structure that specifies the first row in the sheet that contains a used cell.
	unsigned int rwMac;//(4 bytes): An unsigned integer that specifies the zero-based index of the row after the last row in the sheet that contains a used cell. MUST be less than or equal to 0x00010000. If this value is 0x00000000, no cells on the sheet are used cells.
	unsigned short colMic;//(2 bytes): A ColU structure that specifies the first column in the sheet that contains a used cell.
	unsigned short colMac;//(2 bytes): An unsigned integer that specifies the zero-based index of the column after the last column in the sheet that contains a used cell. MUST be less than or equal to 0x0100. If this value is 0x0000, no cells on the sheet are used cells.
	unsigned short reserved;//(2 bytes): MUST be zero, and MUST be ignored.
}Dimensions;
void Dimensions_free(Dimensions * dimensions)
{
	if(dimensions)
	{
		free(dimensions);
	}
}
Dimensions * Dimensions_read(ByteArray * bytearray)
{
	Dimensions * dimensions = malloc(sizeof(Dimensions));
	dimensions->rwMic = ByteArray_readInt32(bytearray);
	dimensions->rwMac = ByteArray_readInt32(bytearray);
	dimensions->colMic = ByteArray_readInt16(bytearray);
	dimensions->colMac = ByteArray_readInt16(bytearray);
	printf("rwMic:%d,rwMac:%d,colMic:%d,colMac:%d,"
			,dimensions->rwMic
			,dimensions->rwMac
			,dimensions->colMic
			,dimensions->colMac
		  );
	return dimensions;
}
typedef struct MulBlank
{//The MulBlank record specifies a series of blank cells in a sheet row. This record can store up to 256 IXFCell structures.
	unsigned short rw;//(2 bytes): An Rw structure that specifies a row containing the blank cells.
	unsigned short colFirst;//(2 bytes): A Col structure that specifies the first column in the series of blank cells within the sheet. The value of colFirst.col MUST be less than or equal to 254.
	unsigned short * rgixfe;//(variable): An array of IXFCell structures. Each element of this array contains an IXFCell structure corresponding to a blank cell in the series. The number of entries in the array MUST be equal to the value given by the following formula:
	//Number of entries in rgixfe = (colLast.col – colFirst.col +1)
	unsigned short colLast;//(2 bytes): A Col structure that specifies the last column in the series of blank cells within the sheet. This colLast.col value MUST be greater than colFirst.col value.
}MulBlank;
void MulBlank_free(MulBlank * mulblank)
{
	if(mulblank)
	{
		if(mulblank->rgixfe)
			free(mulblank->rgixfe);
		free(mulblank);
	}
}
MulBlank * MulBlank_read(ByteArray * bytearray,char * out)
{
	MulBlank * mulblank = malloc(sizeof(MulBlank));
	mulblank->rw = ByteArray_readInt16(bytearray);
	mulblank->colFirst = ByteArray_readInt16(bytearray);
	int len = bytearray->length - bytearray->position - 2;
	if(len>0){
		mulblank->rgixfe = malloc(len);
		ByteArray_readBytes(bytearray,len,(char*)mulblank->rgixfe);
	}
	mulblank->colLast= ByteArray_readInt16(bytearray);
	printf("rw:%d,",mulblank->rw);
	printf("colFirst:%d,",mulblank->colFirst);
	printf("colLast:%d,",mulblank->colLast);
	int i = 0;
	while(i<len/2)
	{
		unsigned short ixfe = mulblank->rgixfe[i];
		printf("%d,",ixfe);
		sprintf(out+strlen(out),"\t");
		i++;
	}
	return mulblank;
}

void BOF_free(BOF * bof)
{
	if(bof)
	{
		free(bof);
	}
}
typedef struct Cell 
{//The Cell structure specifies a cell in the current sheet.
	unsigned short rw;//(2 bytes): An Rw that specifies the row.
	unsigned short col;//(2 bytes): A Col that specifies the column.
	unsigned short ixfe;//(2 bytes): An IXFCell that specifies the XF record.
}Cell;
void Cell_free(Cell * cell)
{
	if(cell)
	{
		free(cell);
	}
}

typedef struct ISSTInf
{ //The ISSTInf structure is the array element used in the rgISSTinf field of the ExtSST record. ib and cbOffset provide a way to access the first string in the set of strings specified by this structure.
	unsigned int ib;//(4 bytes): A FilePointer as specified in [MS-OSHARED] section 2.2.1.5 that specifies the zero-based offset into the workbook stream where the first string in the set of strings starts.
	unsigned short cbOffset;//(2 bytes): An unsigned integer that specifies the zero-based offset into the SST or Continue record, in which the first string in the set of strings starts. MUST be less than ib. The size of the SST or Continue record is determined by reading the record header at the location specified by the following formula:
	//ib – cbOffset
	unsigned short reserved;//(2 bytes): MUST be zero, and MUST be ignored.
}ISSTInf;

typedef struct ExtSST
{ //The ExtSST record specifies the location of sets of strings within the shared string table, specified in the SST record. This record is used to perform a quick lookup of a string within the shared string table, given the string’s index into the table (as specified in LabelSst). To do that, first use the string’s index and the value of dsst to find the set the string is in, then use the corresponding element in rgISSTInf to find the beginning of that set, and finally search incrementally forward in that set to locate the string.
	unsigned short dsst;//(2 bytes): An unsigned integer that specifies the number of strings in each set specified by ISSTInf. Number of strings in each set except the last set MUST be equal to the value specified by the following formula:
	//max(((SST.cstUnique / 128) + 1),8)
	//Number of strings in the last set MUST be less than or equal to the value specified by the following formula:
	//max(((SST.cstUnique / 128) + 1),8)
	ISSTInf * rgISSTInf;//(variable): An array of ISSTInf structures. Each array element specifies the location of a set of strings within the SST record. The number of elements is determined by first evaluating the following formula<83>:
	//(SST.cstUnique mod ExtSST.dsst)
	//If the result of the previous formula is equal to 0, then the number of elements MUST be equal to the value as specified by the following formula: 
	//(SST.cstUnique / ExtSST.dsst)
	//Otherwise, the number of elements MUST be equal to the value as specified by the following formula:
	//(SST.cstUnique / ExtSST.dsst) + 1
}ExtSST;
void ExtSST_free(ExtSST * extsst)
{
	if(extsst)
	{
		if(extsst->rgISSTInf)
			free(extsst->rgISSTInf);
		free(extsst);
	}
}
ExtSST * ExtSST_read(ByteArray * bytearray)
{
	ExtSST * extsst = malloc(sizeof(ExtSST));
	memset(extsst,0,sizeof(ExtSST));
	extsst->dsst = ByteArray_readInt16(bytearray);
	printf("dsst:%d,",extsst->dsst);
	int len = bytearray->length-2;
	if(len>0)
	{
		extsst->rgISSTInf = malloc(len);
		ByteArray_readBytes(bytearray,len,(char*)extsst->rgISSTInf);
		int i=0;
		while(i<len/sizeof(ISSTInf))
		{
			ISSTInf * rgISSTInf = (ISSTInf*)&(extsst->rgISSTInf[i]);
			printf("ib:%d,",rgISSTInf->ib);
			printf("cbOffset:%d,",rgISSTInf->cbOffset);
			//printf("reserved:%d,",rgISSTInf->reserved);
			i+= 1;
		}
	}
	return extsst;
}
typedef struct Setup
{ //The Setup record specifies the page format settings used to print the current sheet.
	unsigned short iPaperSize;//(2 bytes): An unsigned integer that specifies the paper size. Refer to the following table for values. The value 0, or values greater than or equal to 256, specify custom printer paper sizes.  Values between 118 and 255 are reserved for future use. If fNoPls is 1, this value is undefined and MUST be ignored.
	/*
	   Value Meaning
	   1 US Letter 8 1/2 x 11 in
	   2 US Letter Small 8 1/2 x 11 in
	   3 US Tabloid 11 x 17 in
	   4 US Ledger 17 x 11 in
	   5 US Legal 8 1/2 x 14 in
	   6 US Statement 5 1/2 x 8 1/2 in
	   7 US Executive 7 1/4 x 10 1/2 in
	   8 A3 297 x 420 mm
	   9 A4 210 x 297 mm
	   10 A4 Small 210 x 297 mm
	   11 A5 148 x 210 mm
	   12 B4 (JIS) 250 x 354
	   13 B5 (JIS) 182 x 257 mm
	   14 Folio 8 1/2 x 13 in
	   15 Quarto 215 x 275 mm
	   16 10 x 14 in
	   17 11 x 17 in
	   18 US Note 8 1/2 x 11 in
	   19 US Envelope #9 3 7/8 x 8 7/8
	   20 US Envelope #10 4 1/8 x 9 1/2
	   21 US Envelope #11 4 1/2 x 10 3/8
	   22 US Envelope #12 4 \276 x 11
	   23 US Envelope #14 5 x 11 1/2
	   24 C size sheet
	   25 D size sheet
	   26 E size sheet
	   27 Envelope DL 110 x 220mm
	   28 Envelope C5 162 x 229 mm
	   29 Envelope C3 324 x 458 mm
	   30 Envelope C4 229 x 324 mm
	   31 Envelope C6 114 x 162 mm
	   32 Envelope C65 114 x 229 mm
	   33 Envelope B4 250 x 353 mm
	   34 Envelope B5 176 x 250 mm
	   35 Envelope B6 176 x 125 mm
	   36 Envelope 110 x 230 mm
	   37 US Envelope Monarch 3.875 x 7.5 in
	   38 6 3/4 US Envelope 3 5/8 x 6 1/2 in
	   39 US Std Fanfold 14 7/8 x 11 in
	   40 German Std Fanfold 8 1/2 x 12 in
	   41 German Legal Fanfold 8 1/2 x 13 in
	   42 B4 (ISO) 250 x 353 mm
	   43 Japanese Postcard 100 x 148 mm
	   44 9 x 11 in
	   45 10 x 11 in
	   46 15 x 11 in
	   47 Envelope Invite 220 x 220 mm
	   48 RESERVED--DO NOT USE
	   49 RESERVED--DO NOT USE
	   50 US Letter Extra 9 \275 x 12 in
	   51 US Legal Extra 9 \275 x 15 in
	   52 US Tabloid Extra 11.69 x 18 in
	   53 A4 Extra 9.27 x 12.69 in
	   54 Letter Transverse 8 \275 x 11 in
	   55 A4 Transverse 210 x 297 mm
	   56 Letter Extra Transverse 9\275 x 12 in
	   57 SuperA/SuperA/A4 227 x 356 mm
	   58 SuperB/SuperB/A3 305 x 487 mm
	   59 US Letter Plus 8.5 x 12.69 in
	   60 A4 Plus 210 x 330 mm
	   61 A5 Transverse 148 x 210 mm
	   62 B5 (JIS) Transverse 182 x 257 mm
	   63 A3 Extra 322 x 445 mm
	   64 A5 Extra 174 x 235 mm
	   65 B5 (ISO) Extra 201 x 276 mm
	   66 A2 420 x 594 mm
	   67 A3 Transverse 297 x 420 mm
	   68 A3 Extra Transverse 322 x 445 mm
	   69 Japanese Double Postcard 200 x 148 mm
	70 A6 105 x 148 mm
		71 Japanese Envelope Kaku #2
		72 Japanese Envelope Kaku #3
		73 Japanese Envelope Chou #3
		74 Japanese Envelope Chou #4
		75 Letter Rotated 11 x 8 1/2 11 in
		76 A3 Rotated 420 x 297 mm
		77 A4 Rotated 297 x 210 mm
		78 A5 Rotated 210 x 148 mm
		79 B4 (JIS) Rotated 364 x 257 mm
		80 B5 (JIS) Rotated 257 x 182 mm
		81 Japanese Postcard Rotated 148 x 100 mm
		82 Double Japanese Postcard Rotated 148 x
		200 mm
		83 A6 Rotated 148 x 105 mm
		84 Japanese Envelope Kaku #2 Rotated
		85 Japanese Envelope Kaku #3 Rotated
		86 Japanese Envelope Chou #3 Rotated
		87 Japanese Envelope Chou #4 Rotated
		88 B6 (JIS) 128 x 182 mm
		89 B6 (JIS) Rotated 182 x 128 mm
		90 12 x 11 in
		91 Japanese Envelope You #4
		92 Japanese Envelope You #4 Rotated
		93 PRC 16K 146 x 215 mm
		94 PRC 32K 97 x 151 mm
		95 PRC 32K(Big) 97 x 151 mm
		96 PRC Envelope #1 102 x 165 mm
		97 PRC Envelope #2 102 x 176 mm
		98 PRC Envelope #3 125 x 176 mm
		99 PRC Envelope #4 110 x 208 mm
		100 PRC Envelope #5 110 x 220 mm
		101 PRC Envelope #6 120 x 230 mm
		102 PRC Envelope #7 160 x 230 mm
		103 PRC Envelope #8 120 x 309 mm
		104 PRC Envelope #9 229 x 324 mm
		105 PRC Envelope #10 324 x 458 mm
		106 PRC 16K Rotated
		107 PRC 32K Rotated
		108 PRC 32K(Big) Rotated
		109 PRC Envelope #1 Rotated 165 x 102 mm
		110 PRC Envelope #2 Rotated 176 x 102 mm
		111 PRC Envelope #3 Rotated 176 x 125 mm
		112 PRC Envelope #4 Rotated 208 x 110 mm
		113 PRC Envelope #5 Rotated 220 x 110 mm
		114 PRC Envelope #6 Rotated 230 x 120 mm
		115 PRC Envelope #7 Rotated 230 x 160 mm
		116 PRC Envelope #8 Rotated 309 x 120 mm
		117 PRC Envelope #9 Rotated 324 x 229 mm
		118 PRC Envelope #10 Rotated 458 x 324 mm
		*/
		unsigned short iScale;//(2 bytes): An unsigned integer that specifies the scaling factor for printing as a percentage.  For example, if the value is 107 then the scaling factor is 107%. If fNoPls is 1, this value is undefined and MUST be ignored.
	unsigned short iPageStart;//(2 bytes): A signed integer that specifies the starting page number. If fUsePage is 0, MUST be ignored.
	unsigned short iFitWidth;//(2 bytes): An unsigned integer that specifies the number of pages the sheet width is fit to. MUST be less than or equal to 32767. The value 0 means use as many pages as necessary to print the columns in the sheet.
	unsigned short iFitHeight;//(2 bytes): An unsigned integer that specifies the number of pages the sheet height is fit to. MUST be less than or equal to 32767. The value 0 means use as many pages as necessary to print the rows of the sheet.
	unsigned short A:1;//( fLeftToRight (1 bit): A bit that specifies the order that multiple pages are sent to the printer for a single sheet.
	/*
	   Value Meaning
	   0 Pages are printed top-to-bottom first and then left-to-right.
	   1 Pages are printed left-to-right first and then top-to-bottom.
	   */
	unsigned short B:1;//( fPortrait (1 bit): A bit that specifies whether to print using portrait mode or landscape mode. If fNoPls is 1, the value is undefined and MUST be ignored. If fNoOrient is 1, the value is undefined and MUST be ignored.
	/*
	   Value Meaning
	   0 Pages are printed using landscape mode.
	   1 Pages are printed using portrait mode.
	   */
	unsigned short C:1;//( fNoPls (1 bit): A bit that specifies whether the iPaperSize, iScale, iRes, iVRes, iCopies, fNoOrient, and fPortrait data are undefined and ignored. If the value is 1, they are undefined and ignored.
	unsigned short D:1;//( fNoColor (1 bit): A bit that specifies whether the document is printed in black and white.
	unsigned short E:1;//( fDraft (1 bit): A bit that specifies whether the document is printed using draft quality.
	unsigned short F:1;//( fNotes (1 bit): A bit that specifies whether comments are printed.
	unsigned short G:1;//( fNoOrient (1 bit): A bit that specifies whether the paper orientation is set.
	/*
	   Value Meaning
	   0 Paper orientation is specified by the value of fPortrait.
	   1 Pages are printed using portrait mode.
	   */
		unsigned short H:1;//( fUsePage (1 bit): A bit that specifies whether a custom starting page number is used to print. If the value is 1, the custom starting page number specified by the value of iPageStart is used.
	unsigned short I:1;//( unused1 (1 bit): Undefined and MUST be ignored.
	unsigned short J:1;//( fEndNotes (1 bit): A bit that specifies whether the comments are printed at the end of the sheet. If fNotes is 0, the value MUST be ignored.
	/*
	   Value Meaning
	   0 Comments are printed as displayed on the sheet.
	   1 Comments are printed at the end of the sheet.
	   */
		unsigned short K:2;//( iErrors (2 bits): An unsigned integer that specifies how to handle errors in the cell data. MUST be a value from the following table:
	/*
	   Value Meaning
	   0 Print errors as displayed on the sheet.
	   1 Print errors as blank.
	   2 Print errors as dashes ("--").
	   3 Print errors as "#N/A".
	   */
		unsigned short L:4;//( reserved (4 bits): MUST be zero, and MUST be ignored.
	unsigned short iRes;//(2 bytes): An unsigned integer that specifies the print resolution in dots per inch (DPI). If fNoPls is 1, this value is undefined and MUST be ignored.
	unsigned short iVRes;//(2 bytes): An unsigned integer that specifies the vertical print resolution in DPI. If fNoPls is 1, this value is undefined and MUST be ignored.
	double numHdr;//(8 bytes): An Xnum (section 2.5.342) value that specifies the header margin in inches. The value MUST be greater than or equal to 0 and less than 49.
	double numFtr;//(8 bytes): An Xnum value that specifies the footer margin in inches. The value MUST be greater than or equal to 0 and less than 49.
	unsigned short iCopies;//(2 bytes): An unsigned integer that specifies the number of copies to print. If fNoPls is 1, this value is undefined and MUST be ignored.
}Setup;
void Setup_free(Setup * setup)
{
	if(setup)
	{
		free(setup);
	}
}
Setup * Setup_read(ByteArray * bytearray)
{
	printf("Setup (section 2.4.257),%d,",(int)sizeof(Setup));
	Setup * setup = malloc(sizeof(Setup));
	memset(setup,0,sizeof(Setup));
	ByteArray_readBytes(bytearray,16,(char*)setup);
	ByteArray_readBytes(bytearray,8,(char*)&(setup->numHdr));
	ByteArray_readBytes(bytearray,8,(char*)&(setup->numFtr));
	setup->iCopies = ByteArray_readInt16(bytearray);
	printf("numHdr:%lf,",setup->numHdr);
	printf("numFtr:%lf,",setup->numFtr);
	if(setup->iCopies!=1) printf("iCopies:%d,",setup->iCopies);
	return setup;
}
typedef struct Phs
{
	unsigned int ifnt:16;//(2 bytes): A FontIndex structure that specifies the font.
	unsigned int A:2;//( phType (2 bits): An unsigned integer that specifies the type of the phonetic information. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 Use narrow Katakana characters as phonetic string.
	   0x1 Use wide Katakana characters as phonetic string.
	   0x2 Use Hiragana characters as phonetic string.
	   0x3 Use any type of characters as phonetic string.
	   */
	unsigned int B:2;//( alcH (2 bits): An unsigned integer that specifies the alignment of the phonetic string. MUST be a value from the following table:
	/*
	   Value Alignment
	   0x0 General alignment
	   0x1 Left aligned
	   0x2 Center aligned
	   0x3 Distributed alignment
	   */
	unsigned int unused:12;//(12 bits): Undefined and MUST be ignored.
}Phs;

typedef struct LPWideString
{//The LPWideString type specifies a Unicode string which is prefixed by a length.
	unsigned short cchCharacters;//(2 bytes): An unsigned integer that specifies the number of characters.
	unsigned short * rgchData;//(variable): An array of Unicode characters that specifies the characters of the string. The size of this array in bytes MUST equal the following formula: cchCharacters * 2
}LPWideString;
void LPWideString_free(LPWideString * lpwidestring) 
{
	if(lpwidestring)
	{
		if(lpwidestring->rgchData)
			free(lpwidestring->rgchData);
		free(lpwidestring);
	}
}
LPWideString * LPWideString_read(ByteArray * bytearray)
{
	LPWideString * lpwidestring = malloc(sizeof(LPWideString));
	memset(lpwidestring,0,sizeof(LPWideString));
	lpwidestring->cchCharacters = ByteArray_readInt16(bytearray);
	lpwidestring->rgchData = malloc(lpwidestring->cchCharacters*2);
	ByteArray_readBytes(bytearray,lpwidestring->cchCharacters*2,(char*)lpwidestring->rgchData);
	return lpwidestring;
}
typedef struct RPHSSub
{//The RPHSSub structure specifies a phonetic string.
	unsigned short crun;//(2 bytes): An unsigned integer that specifies the number of phonetic text runs. MUST be less than or equal to 32767. If crun is zero, there is one phonetic text run.
	unsigned short cch;//(2 bytes): An unsigned integer that specifies the number of characters in the phonetic string.  MUST be less than or equal to 32767.
	LPWideString * st;//(variable): An LPWideString that specifies the phonetic string. The character count in the string MUST be cch.
}RPHSSub;
void RPHSSub_free(RPHSSub * rphssub)
{
	if(rphssub)
	{
		if(rphssub->st)
			LPWideString_free(rphssub->st);
		free(rphssub);
	}
}
RPHSSub * RPHSSub_read(ByteArray * bytearray)
{
	RPHSSub * rphssub = malloc(sizeof(RPHSSub));
	memset(rphssub,0,sizeof(RPHSSub));
	rphssub->crun = ByteArray_readInt16(bytearray);
	rphssub->cch = ByteArray_readInt16(bytearray);
	rphssub->st = LPWideString_read(bytearray);
	printf("crun:%d,",rphssub->crun);
	printf("cch:%d,",rphssub->cch);
	return rphssub;
}
typedef struct PhRuns
{ //The PhRuns structure specifies a phonetic text run that is displayed above a text run.
	unsigned short ichFirst;//(2 bytes): A signed integer that specifies the zero-based index of the first character of the phonetic text run in the rphssub.st field of the ExtRst structure that contains this PhRuns structure. MUST be greater than or equal to 0.
	unsigned short ichMom;//(2 bytes): A signed integer that specifies the zero-based index of the first character of the text run in the rgb field of the XLUnicodeRichExtendedString that contains the ExtRst that contains this PhRuns that corresponds to the phonetic text run specified in ichFirst. ichMom specifies the location where the text run which phonetic text run specified in ichFirst applies to begins. MUST be greater than or equal to 0.
	unsigned short cchMom;//(2 bytes): A signed integer that specifies the count of characters in the text run specified in ichMom. MUST be greater than or equal to 0.
}PhRuns;

typedef struct ExtRst
{ //The ExtRst structure specifies phonetic string data.
	unsigned short reserved;//(2 bytes): MUST be 1, and MUST be ignored.
	unsigned short cb;//(2 bytes): An unsigned integer that specifies the size, in bytes, of the phonetic string data.
	Phs phs;//(4 bytes): A Phs that specifies the formatting information for the phonetic string.
	RPHSSub * rphssub;//(variable): An RPHSSub that specifies the phonetic string.
	PhRuns * rgphruns;//(variable): An array of PhRuns. This specifies the phonetic text runs. Each PhRuns specifies a phonetic text run within rphssub.st that is displayed above a text run in the rgb field of the XLUnicodeRichExtendedString that contains this structure. The first character of the phonetic text run is the character specified by the ichFirst field of PhRuns. The first character in the rgb field of the XLUnicodeRichExtendedString that contains this structure that the phonetic text run appears earlier is specified by the ichMom field of PhRuns. Each ichMom field of a PhRuns in the array MUST be less than the ichMom field of the subsequent PhRuns in the array.  Each ichFirst field of a PhRuns in the array MUST be less than the ichFirst of the subsequent PhRuns in the array. The sum of the cchMom fields of all PhRuns in the array MUST be less than or equal to the number of characters in rgb field of the XLUnicodeRichExtendedString that contains this structure. The number of elements in this array is rphssub.crun.
}ExtRst;
void ExtRst_free(ExtRst * extrst)
{
	if(extrst)
	{
		if(extrst->rphssub)
		{
			RPHSSub_free(extrst->rphssub);
		}
		if(extrst->rgphruns)
			free(extrst->rgphruns);

		free(extrst);
	}
}
ExtRst * ExtRst_read(ByteArray * bytearray)
{
	ExtRst * exrst = malloc(sizeof(exrst));
	exrst->reserved = ByteArray_readInt16(bytearray);
	exrst->cb = ByteArray_readInt16(bytearray);
	printf("cb:%d,",exrst->cb);
	ByteArray_readBytes(bytearray,4,(char*)&exrst->phs);
	exrst->rphssub = RPHSSub_read(bytearray);
	int len = exrst->rphssub->crun * sizeof(PhRuns);
	exrst->rgphruns = malloc(len);
	ByteArray_readBytes(bytearray,len,(char*)exrst->rgphruns);
	int i = 0;
	while(i<exrst->rphssub->crun)
	{
		PhRuns * rgphruns = (PhRuns*)&(exrst->rgphruns[i]);
		printf("ichFirst:%d,",rgphruns->ichFirst);
		printf("ichMom:%d,",rgphruns->ichFirst);
		printf("cchMom:%d,",rgphruns->ichFirst);
		++i;
	}

	return exrst;
}
typedef struct Format
{//The Format record specifies a number format.
	unsigned short ifmt;//(2 bytes): An IFmt structure that specifies the identifier of the format string specified by stFormat. The value of ifmt.ifmt SHOULD<91> be a value within one of the following ranges.  The value of ifmt.ifmt MUST be a value within one of the following ranges or within 383 to 392.
	XLUnicodeString * stFormat;//(variable): An XLUnicodeString structure that specifies the format string for this number format. The format string indicates how to format the numeric value of the cell. The length of this field MUST be greater than or equal to 1 character and less than or equal to 255 characters. For more information about how format strings are interpreted, see [ECMA-376] Part 4: Markup Language Reference, section 3.8.31.  For a string to be considered a valid format string, it MUST be well-formed according to the following ABNF specification.
}Format;
void Format_free(Format *format)
{
	if(format)
	{
		if(format->stFormat)
			XLUnicodeString_free(format->stFormat);
		free(format);
	}
}
Format * Format_read(ByteArray * bytearray)
{
	Format * format = malloc(sizeof(Format));
	format->ifmt = ByteArray_readInt16(bytearray);
	printf("0x%x,",format->ifmt);
	format->stFormat = XLUnicodeString_read(bytearray);
	return format;
}
typedef struct FormatRun
{//The FormatRun structure specifies formatting information for a text run.
	unsigned int ich;//(2 bytes): An unsigned integer that specifies the zero-based index of the first character of the text that contains the text run. When this record is used in an array, this value MUST be in strictly increasing order.
	unsigned int ifnt;//(2 bytes): A FontIndex structure that specifies the font. If ich is equal to the length of the text, this record is undefined and MUST be ignored.
}FormatRun;

typedef struct XLUnicodeRichExtendedStringFlag
{
	unsigned char A:1;//( fHighByte (1 bit): A bit that specifies whether the characters in rgb are double-byte characters.  MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 All the characters in the string have a high byte of 0x00 and only the low bytes are in rgb.
	   0x1 All the characters in the string are saved as double-byte characters in rgb.
	   */
	unsigned char B:1;//( reserved1 (1 bit): MUST be zero, and MUST be ignored.
	unsigned char C:1;//( fExtSt (1 bit): A bit that specifies whether the string contains phonetic string data.
	unsigned char D:1;//( fRichSt (1 bit): A bit that specifies whether the string is a rich string and the string has at least two character formats applied.
	unsigned char reserved2:4;//(4 bits): MUST be zero, and MUST be ignored.
}XLUnicodeRichExtendedStringFlag;
typedef struct XLUnicodeRichExtendedString
{
	unsigned short cch;//(2 bytes): An unsigned integer that specifies the count of characters in the string.

	unsigned char A:1;//( fHighByte (1 bit): A bit that specifies whether the characters in rgb are double-byte characters.  MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 All the characters in the string have a high byte of 0x00 and only the low bytes are in rgb.
	   0x1 All the characters in the string are saved as double-byte characters in rgb.
	   */
	unsigned char B:1;//( reserved1 (1 bit): MUST be zero, and MUST be ignored.
	unsigned char C:1;//( fExtSt (1 bit): A bit that specifies whether the string contains phonetic string data.
	unsigned char D:1;//( fRichSt (1 bit): A bit that specifies whether the string is a rich string and the string has at least two character formats applied.
	unsigned char reserved2:4;//(4 bits): MUST be zero, and MUST be ignored.

	unsigned short cRun;//(2 bytes): An optional unsigned integer that specifies the number of elements in rgRun. MUST exist if and only if fRichSt is 0x1.
	unsigned int cbExtRst;//(4 bytes): An optional signed integer that specifies the byte count of ExtRst. MUST exist if and only if fExtSt is 0x1. MUST be zero or greater.
	unsigned char *rgb;//(variable): An array of bytes that specifies the characters in the string. If fHighByte is 0x0, the size of the array is cch. If fHighByte is 0x1, the size of the array is cch*2. If fHighByte is 0x1 and rgb is extended with a Continue record the break MUST occur at the double-byte character boundary.
	FormatRun * rgRun;//(variable): An optional array of FormatRun structures that specifies the formatting for each text run. The number of elements in the array is cRun. MUST exist if and only if fRichSt is 0x1.
	ExtRst * ExtRst;//(variable): An optional ExtRst that specifies the phonetic string data. The size of this field is cbExtRst. MUST exist if and only if fExtSt is 0x1.
}XLUnicodeRichExtendedString;
void XLUnicodeRichExtendedString_free(XLUnicodeRichExtendedString * string)
{
	if(string)
	{
		free(string);
	}
}

char * XLUnicodeRichExtendedString_read(ByteArray * bytearray)
{
	char * ret = NULL;
	XLUnicodeRichExtendedString * string = malloc(sizeof(XLUnicodeRichExtendedString));
	memset(string,0,sizeof(XLUnicodeRichExtendedString));
	string->cch = ByteArray_readInt16(bytearray);
	char flag = ByteArray_readByte(bytearray);
	XLUnicodeRichExtendedStringFlag * f = (XLUnicodeRichExtendedStringFlag*)&flag;
	string->A = f->A;
	string->B = f->B;
	string->C = f->C;
	string->D = f->D;
	if(string->D)//fRichSt
	{
		string->cRun = ByteArray_readInt16(bytearray);
		printf("cRun:%d,",string->cRun);
		XLUnicodeRichExtendedString_free(string);
		return 0;
	}
	if(string->C)//fExtSt
	{
		string->cbExtRst = ByteArray_readInt32(bytearray);
		printf("cbExtRst:%d,",string->cbExtRst);
		XLUnicodeRichExtendedString_free(string);
		return 0;
	}
	unsigned int position = bytearray->position;
	if(string->cch){
		printf("cch:%d,",string->cch);
		ret = ByteArray_readUtf16(bytearray,string->cch);
		//ByteArray_print16(bytearray,string->cch);
		position += string->cch*2;
	}
	if(string->D)//fRichSt
	{
		int size = string->cRun*sizeof(FormatRun);
		//ByteArray_print16(bytearray,size);
		ret = ByteArray_readUtf16(bytearray,string->cch);
		/*
		   ByteArray_readBytes(bytearray,size,(char*)string->rgRun);
		   int i = 0;
		   while(i<string->cRun)
		   {
		   FormatRun * ff = &(string->rgRun[i]);
		   printf("cRun: %d,",i);
		   printf("ich:%d,",ff->ich);
		   printf("ifnt:%d\n",ff->ifnt);
		   ++i;
		   }
		   */
		position += size;
	}
	if(string->C)//fExtSt
	{
		int size = string->cbExtRst;
		//ByteArray_print16(bytearray,size);
		ret = ByteArray_readUtf16(bytearray,string->cch);
		/*
		   string->ExtRst = ExtRst_read(bytearray);
		   */
		position += size;
	}
	bytearray->position = position;

	XLUnicodeRichExtendedString_free(string);
	return ret;
}
typedef struct SST
{
	unsigned int cstTotal;//(4 bytes): A signed integer that specifies the total number of references in the workbook to the strings in the shared string table. MUST be greater than or equal to 0.
	unsigned int cstUnique;//(4 bytes): A signed integer that specifies the number of unique strings in the shared string table. MUST be greater than or equal to 0.
	//XLUnicodeRichExtendedString * rgb;//(variable): An array of XLUnicodeRichExtendedString structures. Records in this array are unique.
}SST;
void SST_free(SST * sst)
{
	if(sst)
	{
		free(sst);
	}
}
SST * SST_read(ByteArray * bytearray,XlsFile * file)
{
	printf("SST (section 2.4.265)");
	SST * sst = malloc(sizeof(SST));
	sst->cstTotal = ByteArray_readInt32(bytearray);
	sst->cstUnique = ByteArray_readInt32(bytearray);
	printf("cstTotal:%d,",sst->cstTotal);
	printf("cstUnique:%d,",sst->cstUnique);
	file->aSST = malloc(sst->cstUnique*sizeof(char*));
	file->aSST[0] = XLUnicodeRichExtendedString_read(bytearray);
	printf("%s,",file->aSST[0]);
	int i=1;
	while(bytearray->position < bytearray->length)
	{
		printf("position%d:%d,",i,bytearray->position);
		file->aSST[i] = XLUnicodeRichExtendedString_read(bytearray);
		printf("%s,",file->aSST[i]);
		if((file->aSST[i])==0)break;
		++i;
	}
	printf("position:%d,",bytearray->position);
	return sst;
}

typedef struct StyleXF
{
	unsigned int alc:3;//(3 bits): A HorizAlign that specifies the horizontal alignment.  unsigned int A:1;//( fWrap (1 bit): A bit that specifies whether cell text is wrapped.
	unsigned int alcV:3;//(3 bits): A VertAlign that specifies the vertical alignment.
	unsigned int B:1;//( fJustLast (1 bit): A bit that specifies whether the justified or distributed alignment of the cell is used on the last line of text. (Setting this to 1 is typical for East Asian text but not typical in other contexts). If this field equals 1 then alc MUST equal 7.
	unsigned int trot:1;//(1 byte): An XFPropTextRotation that specifies the text rotation.

	unsigned int cIndent:4;//(4 bits): An unsigned integer that specifies the text indentation level. MUST be less than or equal to 15.
	unsigned int C:1;//( fShrinkToFit (1 bit): A bit that specifies whether a cell is shrink to fit.
	unsigned int D:1;//( reserved1 (1 bit): MUST be zero and MUST be ignored.
	unsigned int E:2;//( iReadOrder (2 bits): A ReadingOrder that specifies the reading order.

	unsigned int unused:8;//(1 byte): Undefined and MUST be ignored.

	unsigned int dgLeft:4;//(4 bits): A BorderStyle that specifies the logical left border formatting.
	unsigned int dgRight:4;//(4 bits): A BorderStyle that specifies the logical right border formatting.

	unsigned int dgTop:4;//(4 bits): A BorderStyle that specifies the top border formatting.
	unsigned int F:4;//( dgBottom (4 bits): A BorderStyle that specifies the bottom border formatting.

	unsigned int icvLeft:7;//(7 bits): An unsigned integer that specifies the color of the logical left border. The value MUST be one of the values specified in the icv field in IcvXF or zero. A value of zero means the left border color is not specified. If this value is zero then dgLeft MUST also be zero.
	unsigned int icvRight:7;//(7 bits): An unsigned integer that specifies the color of the logical right border. The value MUST be one of the values specified in the icv field in IcvXF or zero. A value of zero means the right border color is not specified. If this value is zero then dgRight MUST also be zero.
	unsigned int G:2;//( grbitDiag (2 bits): An unsigned integer that specifies which diagonal borders are present (if any). MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 No diagonal border
	   0x1 Diagonal-down border
	   0x2 Diagonal-up border
	   0x3 Both diagonal-down and diagonal-up
	   */

	unsigned int icvTop:7;//(7 bits): An unsigned integer that specifies the color of the top border. The value MUST be one of the values specified in the icv field in IcvXF or zero. A value of zero means the top border color was not specified. If this value is zero then dgTop MUST also be zero.
	unsigned int icvBottom:7;//(7 bits): An unsigned integer that specifies the color of the bottom border. The value MUST be one of the values specified in the icv field in IcvXF or zero. A value of zero means the bottom border color was not specified. If this value is zero then dgBottom MUST also be zero.
	unsigned int icvDiag:7;//(7 bits): An unsigned integer that specifies the color of the diagonal border. The value MUST be one of the values specified in the icv field in IcvXF or zero. A value of zero means the diagonal border color has not been specified. If this value is zero then dgDiag MUST also be zero.
	unsigned int dgDiag:4;//(4 bits): A BorderStyle that specifies the diagonal border formatting.
	unsigned int H:1;//( reserved2 (1 bit): MUST be zero and MUST be ignored.
	unsigned int fls:6;//(6 bits): A FillPattern that specifies the fill pattern. If this value is 1 which specifies a solid fill pattern only icvFore is rendered.

	unsigned int icvFore:7;//(7 bits): An IcvXF that specifies the foreground color of the fill pattern.
	unsigned int icvBack:7;//(7 bits): An unsigned integer that specifies the background color of the fill pattern. The value MUST be an IcvXF value.
	unsigned int I:2;//( reserved3 (2 bits): MUST be zero and MUST be ignored.

	unsigned int undefined:16;//nothing
}StyleXF;
typedef struct RkNumber
{
	unsigned int A:1;//( fX100 (1 bit): A bit that specifies whether num is the value of the RkNumber or 100 times the value of the RkNumber. MUST be a value from the following table:
	/*
	   Value Meaning
	   0 The value of RkNumber is the value of num.
	   1 The value of RkNumber is the value of num divided by 100.
	   */
	unsigned int B:1;//( fInt (1 bit): A bit that specifies the type of num.
	unsigned int num:30;// (30 bits): A variable type field whose type and meaning is specified by the value of fInt, as defined in the following table: Value of fInt Type of num 0 num is the 30 most significant bits of a 64-bit binary floating-point number as defined in [IEEE754]. The remaining 34-bits of the floating-point number MUST be 0.
}RkNumber;
typedef struct RkRec
{
	unsigned short ixfe;//(2 bytes): An IXFCell that specifies the format of the numeric value.
	RkNumber RK;//(4 bytes): An RkNumber that specifies the numeric value.
}RkRec;
typedef struct RK
{
	unsigned short rw;//(2 bytes): An Rw structure that specifies a row index.
	unsigned short col;//(2 bytes): A Col structure that specifies a column index.
	RkRec rkrec;//(6 bytes): An RkRec structure that specifies the numeric data for a single cell.
}RK;

#ifndef __i386__
double ieee754(u64 v)
{/*{{{*/
	int s, e;
	double r;

	s = v>>52;
	v &= 0x000FFFFFFFFFFFFFull;
	e = s & 0x7FF;
	if(!e)
		goto denorm;
	if(e < 0x7FF) {
		v += 0x0010000000000000ull, e--;
denorm:
		r = ldexp(v, e - 0x3FF - 52 + 1);
	} else if(v) {
		r = NAN; s ^= 0x800;
	} else
		r = INFINITY;
	if(s & 0x800)
		r = -r;
	return r;
}/*}}}*/
#else
double ieee754(u64 v)
{/*{{{*/
	union {
		u64 v;
		double d;
	} u;
	u.v = v;
	return u.d;
}/*}}}*/
#endif
void RK_free(RK * rk)
{
	if(rk)
	{
		free(rk);
	}
}

RK * RK_read(ByteArray * bytearray,XlsFile* file,char * out)
{
	printf("RK(section 2.4.220) ");
	RK * rk = malloc(sizeof(rk));
	rk->rw = ByteArray_readInt16(bytearray);
	rk->col = ByteArray_readInt16(bytearray);
	if(rk->rw)
		printf("row:%d,",rk->rw);
	if(rk->col)
	{
		printf("col:%d,",rk->col);
	}else{
		sprintf(out+strlen(out),"\n");
	}

	rk->rkrec.ixfe = ByteArray_readInt16(bytearray);
	ByteArray_readBytes(bytearray,4,(char*)&(rk->rkrec) + 4);
	if(rk->rkrec.ixfe)
		printf("ixfe:0x%x,",rk->rkrec.ixfe);
	if(rk->rkrec.RK.A)
	{
		printf("A:%d (fX100),",rk->rkrec.RK.A);
		rk->rkrec.RK.num /= 100.0;
	}
	if(rk->rkrec.RK.B)
		printf("B fInt:%d,",rk->rkrec.RK.B);
	if(rk->rkrec.RK.num)
	{
		if(rk->rkrec.RK.B){
			printf("num :%d,",rk->rkrec.RK.num);
			sprintf(out+strlen(out),"%d\t",rk->rkrec.RK.num);
		}else{
			double v = ieee754((u64)((u64)(rk->rkrec.RK.num) << 34));
			printf("numv :%f,",v);
			if(file->dateFrom1900){
				//printf("date %s,",(char*)(asctime(Date_newFrom1970((int)v-25569))));
				sprintf(out+strlen(out),"%s",(char*)(asctime(Date_newFrom1970((int)v-25569))));
				*(out+strlen(out)-1)='\t';
			}else{
				printf(out+strlen(out),"%s",(char*)(asctime(Date_newFrom1970((int)v-24106))));
				*(out+strlen(out)-1)='\t';
			}
		}
	}

	/*
	   bytearray->position = 6;
	   unsigned int RK = ByteArray_readInt32(bytearray);
	   double v=0.0;
	   if (RK & 2) {
	   v = (int)RK>>2;
	   } else {
	   v = ieee754((u64)(RK&~3) << 32);
	   }
	   if (RK & 1) {
	   v /= 100;
	   }
	   printf("%f",v);
	   */
	return rk;
}

typedef struct MulRk
{
	unsigned short rw;//(2 bytes): An Rw structure that specifies the row containing the cells with numeric data.
	unsigned short colFirst;//(2 bytes): A Col structure that specifies the first column in the series of numeric cells within the sheet. The value of colFirst.col MUST be less than or equal to 254.
	RkRec * rgrkrec;//(variable): An array of RkRec structures. Each element in the array specifies an RkRec in the row. The number of entries in the array MUST be equal to the value given by the following formula: Number of entries in rgrkrec = (colLast.col – colFirst.col +1)
	unsigned short colLast;//(2 bytes): A Col structure that specifies the last column in the set of numeric cells within the sheet. This colLast.col value MUST be greater than the colFirst.col value.
}MulRk;
void MulRk_free(MulRk * mulrk)
{
	if(mulrk)
	{
		if(mulrk->rgrkrec)
			free(mulrk->rgrkrec);
		free(mulrk);
	}
}
MulRk * MulRk_read(ByteArray * bytearray)
{
	MulRk * mulrk = malloc(sizeof(MulRk));
	mulrk->rw = ByteArray_readInt16(bytearray);
	mulrk->colFirst = ByteArray_readInt16(bytearray);

	int len = bytearray->length - bytearray->position - 2;
	mulrk->rgrkrec = malloc(sizeof(len));
	ByteArray_readBytes(bytearray,len,(char*)mulrk->rgrkrec);

	mulrk->colLast= ByteArray_readInt16(bytearray);
	if(mulrk->rw)
		printf("rw:%d,",mulrk->rw);
	if(mulrk->colFirst)
		printf("colFirst:%d,",mulrk->colFirst);
	if(mulrk->colLast)
		printf("colLast:%d,",mulrk->colLast);
	int i=0;
	while(i<len/2)
	{
		printf("%d:0x%x,",i,(mulrk->rgrkrec[i].RK.num));
		++i;
	}
	return mulrk;
}
Cell * Cell_read(ByteArray * bytearray,char * out)
{
	Cell * cell = malloc(sizeof(Cell));
	cell->rw = ByteArray_readInt16(bytearray);
	cell->col = ByteArray_readInt16(bytearray);
	cell->ixfe = ByteArray_readInt16(bytearray);
	if(cell->rw)
		printf("row:%d,",cell->rw);
	if(cell->col){
		printf("col:%d,",cell->col);
	}else{
		sprintf(out+strlen(out),"\n");
	}
	if(cell->ixfe)
		printf("ixfe:0x%x,",cell->ixfe);
	return cell;
}
typedef struct Number 
{
	Cell *  cell;
	unsigned char num[8];//(8 bytes): An Xnum (section 2.5.342) value that specifies the cell value.  If this record appears in a SERIESDATA record collection, and this record specifies a cell in the chart data cache that specifies data for an error bar series, then this field is a ChartNumNillable value. If a ChartNumNillable is used, a blank cell is specified by a NilChartNum structure that has a type field with a value of 0x0000, and a cell with a #N/A error is specified by a NilChartNum that has a type field with a value of 0x0100.
}Number;
void Number_free(Number * number)
{
	if(number)
	{
		if(number->cell)
			Cell_free(number->cell);
		free(number);
	}
}
Number * Number_read(ByteArray * bytearray,char * out)
{
	printf("Number (section 2.4.180)");
	Number * number = malloc(sizeof(Number));
	number->cell = Cell_read(bytearray,out);
	ByteArray_readBytes(bytearray,8,(char*)number->num);
	return number;
}
typedef struct LabelSst
{
	Cell* cell;//(6 bytes): A Cell structure that specifies the cell containing the string from the shared string table.
	unsigned int isst;//(4 bytes): An unsigned integer that specifies the zero-based index of an element in the array of XLUnicodeRichExtendedString structure in the rgb field of the SST record in this Workbook Stream ABNF that specifies the string contained in the cell. MUST be greater than or equal to zero and less than the number of elements in the rgb field of the SST record
}LabelSst;
void LabelSst_free(LabelSst * labelsst)
{
	if(labelsst)
	{
		if(labelsst->cell)
		{
			Cell_free(labelsst->cell);
		}
		free(labelsst);
	}
}
LabelSst *LabelSst_read(ByteArray * bytearray,XlsFile * file,char * out)
{
	printf("LabelSst (section 2.4.149)");
	LabelSst * labelsst = malloc(sizeof(LabelSst));
	labelsst->cell = Cell_read(bytearray,out);
	labelsst->isst = ByteArray_readInt32(bytearray);
	if(labelsst->isst)
		printf("isst:0x%x,",labelsst->isst);
	sprintf(out+strlen(out),"%s\t",file->aSST[labelsst->isst]);
	return labelsst;
}
typedef struct ColInfo
{
	unsigned short colFirst;//(2 bytes): A Col2.5.44U structure that specifies the first formatted column.
	unsigned short colLast;//(2 bytes): A Col2.5.44U structure that specifies the last formatted column. The value MUST be greater than or equal to colFirst.
	unsigned short coldx;//(2 bytes): An unsigned integer that specifies the column width in units of 1/256 th of a character width. Character width is defined as the maximum digit width of the numbers 0, 1, 2, ...  9 as rendered in the Normal style’s font.
	unsigned short ixfe;//(2 bytes): An IXFCell structure that specifies the default format for the column cells.
	unsigned short A:1;//( fHidden (1 bit): A bit that specifies whether the column range defined by colFirst and colLast is hidden.
	unsigned short B:1;//( fUserSet (1 bit): A bit that specifies that the column width was either manually set by the user or is different from the default column width as specified by DefColWidth. If the value is 1, the column width was manually set or is different from DefColWidth.
	unsigned short C:1;//( fBestFit (1 bit): A bit that specifies whether the column range defined by colFirst and colLast is set to "best fit." "Best fit" implies that the column width resizes based on the cell contents, and that the column width does not equal the default column width as specified by DefColWidth.
	unsigned short D:1;//( fPhonetic (1 bit): A bit that specifies whether phonetic information is displayed by default for the column range defined by colFirst and colLast.
	unsigned short E:4;//( reserved1 (4 bits): MUST be zero, and MUST be ignored.
	unsigned short F:3;//( iOutLevel (3 bits): An unsigned integer that specifies the outline level of the column range defined by colFirst and colLast.
	unsigned short G:1;//( unused1 (1 bit): Undefined and MUST be ignored.
	unsigned short H:1;//( fCollapsed (1 bit): A bit that specifies whether the column range defined by colFirst and colLast is in a collapsed outline state.
	unsigned short I:3;//( reserved2 (3 bits): MUST be zero, and MUST be ignored.
	unsigned short unused2;//(2 bytes): Undefined and MUST be ignored.
}ColInfo;
void ColInfo_free(ColInfo * colinfo)
{
	if(colinfo)
	{
		free(colinfo);
	}
}
ColInfo * ColInfo_read(ByteArray * bytearray)
{
	printf("ColInfo (section 2.4.53)");
	ColInfo * colinfo = malloc(sizeof(ColInfo));
	ByteArray_readBytes(bytearray,sizeof(ColInfo),(char*)colinfo);
	if(colinfo->colFirst)printf("colFirst:%d,",colinfo->colFirst);
	if(colinfo->colLast)printf("colLast:%d,",colinfo->colLast);
	if(colinfo->coldx)printf("coldx:%d,",colinfo->coldx);
	if(colinfo->ixfe)printf("ixfe:%d,",colinfo->ixfe);
	if(colinfo->A)printf("A:%d,",colinfo->A);
	if(colinfo->B)printf("B:%d,",colinfo->B);
	if(colinfo->C)printf("C:%d,",colinfo->C);
	if(colinfo->D)printf("D:%d,",colinfo->D);
	if(colinfo->E)printf("E:%d,",colinfo->E);
	if(colinfo->F)printf("F:%d,",colinfo->F);
	if(colinfo->G)printf("G:%d,",colinfo->G);
	if(colinfo->H)printf("H:%d,",colinfo->H);
	if(colinfo->I)printf("I:%d,",colinfo->I);
	//if(colinfo->unused2)printf("unused2:%d,",colinfo->unused2);
	return colinfo;
}
typedef struct FilePointer
{
	unsigned int offset;// (4 bytes): Unsigned integer that specifies the offset into a stream or file.
}FilePointer;
typedef struct DBCell
{//The DBCell record specifies a row block, which is a series of up to 32 consecutive rows.  DBCell, combined with the Index record, is used to optimize the lookup of cells in a cell table.
	unsigned int dbRtrw;//(4 bytes): An unsigned integer that specifies the offset in bytes from the starting file position of this record to the file position of the first Row record. If the value is 0, the referenced row block does not contain any rows that contain cells that have data.
	unsigned short * rgdb;//(variable): An array of 2-byte unsigned integers that specify the file offset in bytes to the first record that specifies a CELL in each row that is a part of this row block. For the first array element, the starting position of the file offset is specified relative to the file position of the end of the first Row record in the row block. For all other elements, the file offset is specified relative to the file position of the CELL record specified by the previous element in this array. The number of elements in the array MUST be less than or equal to 32.
}DBCell;
typedef struct HyperlinkString
{
	unsigned int length;//(4 bytes): An unsigned integer that specifies the number of Unicode characters in the string field, including the null-terminating character.
	unsigned short *string;//(variable): A null-terminated array of Unicode characters. The number of characters in the array is specified by the length field.
}HyperlinkString;

typedef struct CompositeMoniker
{
	unsigned int cMonikers;//(4 bytes): An unsigned integer that specifies the count of monikers in monikerArray.
	unsigned int monikerArray;//(variable): An array of HyperlinkMonikers (section 2.3.7.2). Each array element specifies a moniker of arbitrary type.
}CompositeMoniker ;
CompositeMoniker * CompositeMoniker_read(ByteArray * bytearray)
{
	CompositeMoniker * compositemoniker= malloc(sizeof(CompositeMoniker));

	return compositemoniker;
}

typedef struct FileMoniker
{
	unsigned int cAnti;//(2 bytes): An unsigned integer that specifies the number of parent directory indicators at the beginning of the ansiPath field.
	unsigned int ansiLength;//(4 bytes): An unsigned integer that specifies the number of ANSI characters in ansiPath, including the terminating NULL character. This value MUST be less than or equal to 32767.
	unsigned int ansiPath;//(variable): A null-terminated array of ANSI characters that specifies the file path. The number of characters in the array is specified by ansiLength.
	unsigned int endServer;//(2 bytes): An unsigned integer that specifies the number of Unicode characters used to specify the server portion of the path if the path is a UNC path (including the leading "\\"). If the path is not a UNC path, this field MUST equal 0xFFFF.
	unsigned int versionNumber;//(2 bytes): An unsigned integer that specifies the version number of this file moniker serialization implementation. MUST equal 0xDEAD.
	unsigned int reserved1;//(16 bytes): MUST be zero and MUST be ignored.
	unsigned int reserved2;//(4 bytes): MUST be zero and MUST be ignored.
	unsigned int cbUnicodePathSize;//(4 bytes): An unsigned integer that specifies the size, in bytes, of cbUnicodePathBytes, usKeyValue, and unicodePath.  If the file path specified in ansiPath cannot be completely specified by ANSI characters, the value of this field MUST be equal to the size, in bytes, of the path as a Unicode string (without a terminating NULL character) + 6. If the path can be fully specified in ANSI characters then the value of this field MUST be set to zero.  If the value of this field is greater than zero, then the cbUnicodePathBytes, usKeyValue and unicodePath fields MUST exist.  If the value of this field is zero, then the cbUnicodePathBytes, usKeyValue, and unicodePath fields MUST NOT exist.
	unsigned int cbUnicodePathBytes;//(4 bytes): An optional unsigned integer that specifies the size, in bytes, of the unicodePath field. This field exists if and only if cbUnicodePathSize is greater than zero.
	unsigned int usKeyValue;//(2 bytes): An optional unsigned integer that MUST be 3 if present. This field exists if and only if cbUnicodePathSize is greater than zero.
	unsigned int unicodePath;//(variable): An optional array of Unicode characters that specifies the complete file path. This path MUST be the complete Unicode version of the file path specified in ansiPath and MUST include additional Unicode characters that cannot be completely specified in ANSI characters. The number of characters in this array is specified by cbUnicodePathBytes/2. This array MUST not include a terminating NULL character. This field exists if and only if cbUnicodePathSize is greater than zero.

}FileMoniker;

FileMoniker * FileMoniker_read(ByteArray * bytearray)
{
	FileMoniker * filemoniker = malloc(sizeof(FileMoniker));

	return filemoniker;
}

HyperlinkString * HyperlinkString_read(ByteArray * bytearray)
{
	HyperlinkString * hyperlinkstring = malloc(sizeof(HyperlinkString));
	hyperlinkstring->length = ByteArray_readInt32(bytearray);
	hyperlinkstring->string = malloc(hyperlinkstring->length);
	ByteArray_readBytes(bytearray,hyperlinkstring->length,(char *)hyperlinkstring->string);
	printf("%d,",hyperlinkstring->length);
	int position = bytearray->position;
	bytearray->position -= hyperlinkstring->length;
	ByteArray_print16(bytearray,hyperlinkstring->length);
	bytearray->position = position + hyperlinkstring->length*2;
	return hyperlinkstring;
}
typedef struct AntiMoniker
{
	unsigned int count;// (4 bytes): An unsigned integer that specifies the number of anti-monikers that have been composed together to create this instance. When an anti-moniker is composed with another anti- moniker, the resulting composition would have a count field equaling the sum of the two count fields of the composed anti-monikers. This value MUST be less than or equal to 1048576.
}AntiMoniker;
AntiMoniker * AntiMoniker_read(ByteArray * bytearray)
{
	AntiMoniker * antimoniker = malloc(sizeof(AntiMoniker));

	return antimoniker;
}
typedef struct ItemMoniker
{
	unsigned int delimiterLength;//(4 bytes): An unsigned integer that specifies the sum of the count of bytes in the delimiterAnsi and delimiterUnicode fields.
	unsigned int delimiterAnsi;//(variable): A null-terminated array of ANSI characters that specifies a delimiter for this moniker. Delimiters are used to separate monikers that are part of a collection of monikers in a composite moniker. The number of characters in the array is determined by the position of the terminating NULL character.
	unsigned int delimiterUnicode;//(variable): An optional array of Unicode characters that specifies a delimiter for this moniker if the delimiter cannot be completely specified in ANSI characters. This field MUST exist if and only if delimiterLength is greater than the size of delimiterAnsi in bytes. The number of characters in the array is determined by (delimiterLength - (size of delimiterAnsi in bytes)) / 2.
	unsigned int itemLength;//(4 bytes): An unsigned integer that specifies the count of bytes in the itemAnsi and itemUnicode fields.
	unsigned int itemAnsi;//(variable): A null-terminated array of ANSI characters that specifies the string used to identify this item in a collection of items. The number of characters in this array is specified by itemLength.
	unsigned int itemUnicode;//(variable): An optional array of Unicode characters that specifies the string used to identify this item in a collection of items, if the string cannot be completely specified in ANSI characters. This field MUST exist if and only if itemLength is greater than the size of itemAnsi in bytes. The number of characters in the array is determined by (itemLength – (size of itemAnsi field in bytes)) / 2.
}ItemMoniker;

ItemMoniker * ItemMoniker_read(ByteArray *bytearray)
{
	ItemMoniker * itemmoniker= malloc(sizeof(ItemMoniker));

	return itemmoniker;
}

typedef struct URLMoniker
{
	unsigned int length;//(4 bytes): An unsigned integer that specifies the size of this structure in bytes, excluding the size of the length field.  The value of this field MUST be either the byte size of the url field (including the terminating NULL character) or the byte size of the url field plus 24.  If the value of this field is set to the byte size of the url field, then the serialGUID, serialVersion, and uriFlags fields MUST NOT be present.  If the value of this field is set to the byte size of the url field plus 24, then the serialGUID, serialVersion, and uriFlags fields MUST be present.
	unsigned short * url;//(variable): A null-terminated array of Unicode characters that specifies the URL. The number of characters in the array is determined by the position of the terminating NULL character.
	unsigned char serialGUID[16];//(16 bytes): An optional GUID as specified by [MS-DTYP] for this implementation of the URL moniker serialization. This field MUST equal {0xF4815879, 0x1D3B, 0x487F, 0xAF, 0x2C, 0x82, 0x5D, 0xC4, 0x85, 0x27, 0x63} if present.
	unsigned int serialVersion;//(4 bytes): An optional unsigned integer that specifies the version number of this implementation of the URL moniker serialization. This field MUST equal 0 if present.
	unsigned int uriFlags;//(4 bytes): An optional URICreateFlags structure (section 2.3.7.7) that specifies creation flags for an [RFC3986] compliant URI.
} URLMoniker;
URLMoniker * URLMoniker_read(ByteArray * bytearray)
{
	URLMoniker * urlmoniker = malloc(sizeof(URLMoniker));
	memset(urlmoniker,0,sizeof(URLMoniker));
	urlmoniker->length = ByteArray_readInt32(bytearray);
	urlmoniker->url = malloc(urlmoniker->length*2);
	ByteArray_readBytes(bytearray,urlmoniker->length*2,(char *)urlmoniker->url);
	ByteArray_readBytes(bytearray,16,(char*)urlmoniker->serialGUID);
	ByteArray_readBytes(bytearray,4,(char*)&urlmoniker->serialVersion);
	ByteArray_readBytes(bytearray,4,(char*)&urlmoniker->uriFlags);
	return urlmoniker;
}
typedef struct HyperlinkMoniker
{
	unsigned char monikerClsid[16];//(16 bytes): A class identifier (CLSID) that specifies the Component Object Model (COM) component that saved this structure. MUST be a value from the following table:
	/*
	   Value Meaning
	   {0x79EAC9E0, 0xBAF9, 0x11CE, 0x8C, 0x82, 0x00,
	   0xAA, 0x00, 0x4B, 0xA9, 0x0B} Data field contains a URLMoniker (section 2.3.7.6).
	   {0x00000303, 0x0000, 0x0000, 0xC0, 0x00, 0x00,
	   0x00, 0x00, 0x00, 0x00, 0x46} Data field contains a FileMoniker (section 2.3.7.8).
	   {0x00000309, 0x0000, 0x0000, 0xC0, 0x00, 0x00,
	   0x00, 0x00, 0x00, 0x00, 0x46} Data field contains a CompositeMoniker (section
	   2.3.7.3).
	   {0x00000305, 0x0000, 0x0000, 0xC0, 0x00, Data field contains an AntiMoniker (section
	   0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } 2.3.7.4).
	   {0x00000304, 0x0000, 0x0000, 0xC0, 0x00,
	   0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } Data field contains an ItemMoniker (section
	   2.3.7.5).
	   */
	char *data;//(variable): A moniker of the type specified by monikerClsid.
}HyperlinkMoniker;
HyperlinkMoniker * HyperlinkMoniker_read(ByteArray * bytearray)
{
	HyperlinkMoniker * hyperlinkmoniker = malloc(sizeof(HyperlinkMoniker));
	ByteArray_readBytes(bytearray,16,(char *)hyperlinkmoniker->monikerClsid);
	switch((char)*(char*)hyperlinkmoniker->monikerClsid & 0xff)
	{
		case 0xe0:
			hyperlinkmoniker->data = (char *)URLMoniker_read(bytearray);
			break;
		case 0x03:
			hyperlinkmoniker->data = (char *)FileMoniker_read(bytearray);
			break;
		case 0x09:
			hyperlinkmoniker->data = (char *)CompositeMoniker_read(bytearray);
			break;
		case 0x05:
			hyperlinkmoniker->data = (char *)AntiMoniker_read(bytearray);
			break;
		case 0x04:
			hyperlinkmoniker->data = (char *)ItemMoniker_read(bytearray);
			break;
	}

	return hyperlinkmoniker;
}
typedef struct Hyperlink
{
	unsigned int streamVersion;//(4 bytes): An unsigned integer that specifies the version number of the serialization implementation used to save this structure. This value MUST equal 2.

	unsigned int A:1;//( hlstmfHasMoniker (1 bit): A bit that specifies whether this structure contains a moniker. If hlstmfMonikerSavedAsStr equals 1, this value MUST equal 1.
	unsigned int B:1;//( hlstmfIsAbsolute (1 bit): A bit that specifies whether this hyperlink is an absolute path or relative path.
	/*
	   Value Meaning
	   0 This hyperlink is a relative path.
	   1 This hyperlink is an absolute path.
	   */
	unsigned int C:1;//( hlstmfSiteGaveDisplayName (1 bit): A bit that specifies whether the creator of the hyperlink specified a display name.
	unsigned int D:1;//( hlstmfHasLocationStr (1 bit): A bit that specifies whether this structure contains a hyperlink location.
	unsigned int E:1;//( hlstmfHasDisplayName (1 bit): A bit that specifies whether this structure contains a display name.
	unsigned int F:1;//( hlstmfHasGUID (1 bit): A bit that specifies whether this structure contains a GUID as specified by [MS-DTYP].
	unsigned int G:1;//( hlstmfHasCreationTime (1 bit): A bit that specifies whether this structure contains the creation time of the file that contains the hyperlink.
	unsigned int H:1;//( hlstmfHasFrameName (1 bit): A bit that specifies whether this structure contains a target frame name.
	unsigned int I:1;//( hlstmfMonikerSavedAsStr (1 bit): A bit that specifies whether the moniker was saved as a string.
	unsigned int J:1;//( hlstmfAbsFromGetdataRel (1 bit): A bit that specifies whether the hyperlink specified by this structure is an absolute path generated from a relative path.
	unsigned int reserved:22;//(22 bits): MUST be zero and MUST be ignored. 

	HyperlinkString * displayName;//(variable): An optional HyperlinkString (section 2.3.7.9) that specifies the display name for the hyperlink. MUST exist if and only if hlstmfHasDisplayName equals 1.
	HyperlinkString * targetFrameName;//(variable): An optional HyperlinkString (section 2.3.7.9) that specifies the target frame. MUST exist if and only if hlstmfHasFrameName equals 1.
	HyperlinkString * moniker;//(variable): An optional HyperlinkString (section 2.3.7.9) that specifies the hyperlink moniker. MUST exist if and only if hlstmfHasMoniker equals 1 and hlstmfMonikerSavedAsStr equals 1.
	HyperlinkMoniker * oleMoniker;//(variable): An optional HyperlinkMoniker (section 2.3.7.2) that specifies the hyperlink moniker. MUST exist if and only if hlstmfHasMoniker equals 1 and hlstmfMonikerSavedAsStr equals 0.
	HyperlinkString * location;//(variable): An optional HyperlinkString (section 2.3.7.9) that specifies the hyperlink location. MUST exist if and only if hlstmfHasLocationStr equals 1.
	unsigned char guid[16];//(16 bytes): An optional GUID as specified by [MS-DTYP] that identifies this hyperlink. MUST exist if and only if hlstmfHasGUID equals 1.
	unsigned char fileTime[8];//(8 bytes): An optional FileTime structure as specified by [MS-DTYP] that specifies the UTC file creation time. MUST exist if and only if hlstmfHasCreationTime equals 1.
}Hyperlink;
Hyperlink * Hyperlink_read(ByteArray * bytearray)
{
	Hyperlink * hyperlink = malloc(sizeof(Hyperlink));
	memset(hyperlink,0,sizeof(Hyperlink));
	ByteArray_readBytes(bytearray,8,(char*)hyperlink);
	if(hyperlink->A)
		printf("A:%d,",hyperlink->A);
	if(hyperlink->B)
		printf("B:%d,",hyperlink->B);
	if(hyperlink->C)
		printf("C:%d,",hyperlink->C);
	if(hyperlink->D)
		printf("D:%d,",hyperlink->D);
	if(hyperlink->E)
		printf("E:%d,",hyperlink->E);
	if(hyperlink->F)
		printf("F:%d,",hyperlink->F);
	if(hyperlink->G)
		printf("G:%d,",hyperlink->G);
	if(hyperlink->H)
		printf("H:%d,",hyperlink->H);
	if(hyperlink->I)
		printf("I:%d,",hyperlink->I);
	if(hyperlink->J)
		printf("J:%d,",hyperlink->J);

	if(hyperlink->E)
		hyperlink->displayName = HyperlinkString_read(bytearray);
	if(hyperlink->H)
		hyperlink->targetFrameName = HyperlinkString_read(bytearray);
	if(hyperlink->A && hyperlink->I==1)
		hyperlink->moniker= HyperlinkString_read(bytearray);
	if(hyperlink->A && hyperlink->I==0)
		hyperlink->oleMoniker = HyperlinkMoniker_read(bytearray);
	if(hyperlink->D)
		hyperlink->location = HyperlinkString_read(bytearray);
	if(hyperlink->F)
		ByteArray_readBytes(bytearray,16,(char *)hyperlink->guid);
	if(hyperlink->G)
		ByteArray_readBytes(bytearray,8,(char *)hyperlink->fileTime);
	return hyperlink;
}



typedef struct HLink
{
	unsigned char ref8[8];//(8 bytes): A Ref2.5.209U structure that specifies the range of cells containing the hyperlink.
	unsigned char hlinkClsid[16];//(16 bytes): A class identifier (CLSID) that specifies the COM component which saved the Hyperlink Object (as defined by [MS-OSHARED] section 2.3.7.1) in hyperlink.
	Hyperlink * hyperlink;//(variable): A Hyperlink Object (as defined by [MS-OSHARED] section 2.3.7.1) that specifies the hyperlink and hyperlink-related information.
}HLink;
HLink * HLink_read(ByteArray *bytearray)
{
	printf("HLink (section 2.4.140)");
	HLink *  hlink = malloc(sizeof(HLink));
	memset(hlink,0,sizeof(HLink));
	ByteArray_readBytes(bytearray,8,(char*)hlink->ref8);
	ByteArray_readBytes(bytearray,16,(char*)hlink->hlinkClsid);
	unsigned int len = bytearray->length - bytearray->position;
	if(len>0){
		hlink->hyperlink = Hyperlink_read(bytearray);
	}
	printf("position:0x%x,",bytearray->position);
	return hlink;
}

typedef struct Row
{//The Row record specifies a single row on a sheet.
	unsigned short rw;//(2 bytes): An Rw structure that specifies the row index.
	unsigned short colMic;//(2 bytes): An unsigned integer that specifies the zero-based index of the first column that contains a cell populated with data or formatting in the current row. MUST be less than or equal to 255.
	unsigned short colMac;//(2 bytes): An unsigned integer that specifies the one-based index of the last column that contains a cell populated with data or formatting in the current row. MUST be less than or equal to 256. If colMac is equal to colMic, this record specifies a row with no CELL records.
	unsigned short miyRw;//(2 bytes): An unsigned integer that specifies the row height in twips. If fDyZero is 1, the row is hidden and the value of miyRw specifies the original row height. MUST be greater than or equal to 2 and MUST be less than or equal to 8192.
	unsigned short reserved1;//(2 bytes): MUST be zero, and MUST be ignored.
	unsigned short unused1;//(2 bytes): Undefined and MUST be ignored.
	unsigned short A:3;// iOutLevel (3 bits): An unsigned integer that specifies the outline level of the row.
	unsigned short B:1;// reserved2 (1 bit): MUST be zero, and MUST be ignored.
	unsigned short C:1;// fCollapsed (1 bit): A bit that specifies whether the rows that are one level of outlining deeper than the current row are included in the collapsed outline state.
	unsigned short D:1;// fDyZero (1 bit): A bit that specifies whether the row is hidden.
	unsigned short E:1;// fUnsynced (1 bit): A bit that specifies whether the row height was manually set.
	unsigned short F:1;// fGhostDirty (1 bit): A bit that specifies whether the row was formatted.
	unsigned short reserved3:8;//(1 byte): MUST be 1, and MUST be ignored.  

	unsigned short ixfe_val:12;// (12 bits): An unsigned integer that specifies an XF record for the row formatting. See IXFCell for more information. If fGhostDirty is 0, ixfe_val is undefined and MUST be ignored.
	unsigned short G:1;// fExAsc (1 bit): A bit that specifies whether any cell in the row has a thick top border, or any cell in the row directly above the current row has a thick bottom border. Thick borders are specified by the following enumeration values from BorderStyle: THICK and DOUBLE.
	unsigned short H:1;// fExDes (1 bit): A bit that specifies whether any cell in the row has a medium or thick bottom border, or any cell in the row directly below the current row has a medium or thick top border.  Thick borders are previously specified. Medium borders are specified by the following enumeration values from BorderStyle: MEDIUM, MEDIUMDASHED, MEDIUMDASHDOT, MEDIUMDASHDOTDOT, and SLANTDASHDOT.
	unsigned short I:1;// fPhonetic (1 bit): A bit that specifies whether the phonetic guide feature is enabled for any cell in this row.
	unsigned short J:1;// unused2 (1 bit): Undefined and MUST be ignored.
}Row;
void Row_free(Row * row)
{
	if(row)
	{
		free(row);
	}
}
Row * Row_read(ByteArray * bytearray)
{
	printf("Row (section 2.4.221),%d,",(int)sizeof(Row));
	//printf("Row:\t");
	Row * row = malloc(sizeof(Row));
	ByteArray_readBytes(bytearray,sizeof(Row),(char*)row);
	if(row->A)
		printf("A:%d,",row->A);
	if(row->B)
		printf("B:%d,",row->B);
	if(row->C)
		printf("C:%d,",row->C);
	if(row->D)
		printf("D:%d,",row->D);
	if(row->E)
		printf("E:%d,",row->E);
	if(row->F)
		printf("F:%d,",row->F);
	if(row->G)
		printf("G:%d,",row->G);
	if(row->H)
		printf("H:%d,",row->H);
	if(row->I)
		printf("I:%d,",row->I);
	if(row->J)
		printf("J:%d,",row->J);
	if(row->colMic)
		printf("colMic:%d,",row->colMic);
	if(row->colMac)
		printf("colMac:%d,",row->colMac);
	if(row->reserved1)
		printf("reserved1:%d,",row->reserved1);
	if(row->reserved3!=1)
		printf("reserved3:%d,",row->reserved3);
	if(row->unused1)
		printf("unused1:%d,",row->unused1);
	if(row->miyRw)
		printf("miyRw:%d,",row->miyRw);
	if(row->ixfe_val && row->F)
		printf("ixfe_val:%d,",row->ixfe_val);
	return row;
}

void DBCell_free(DBCell * dbcell)
{
	if(dbcell)
	{
		if(dbcell->rgdb)
			free(dbcell->rgdb);
		free(dbcell);
	}
}
DBCell * DBCell_read(ByteArray * bytearray)
{
	printf("DBCell (section 2.4.78),%d,",(int)sizeof(DBCell));
	DBCell * dbcell = malloc(sizeof(DBCell));
	memset(dbcell,0,sizeof(DBCell));
	dbcell->dbRtrw = ByteArray_readInt32(bytearray);
	printf("dbRtrw:%d,",dbcell->dbRtrw);
	int len=0;
	if(dbcell->dbRtrw)
	{
		len = bytearray->length - bytearray->position;
		dbcell->rgdb = malloc(len);
		ByteArray_readBytes(bytearray,len,(char*)dbcell->rgdb);
	}
	int i = 0;
	while(i<len/2)
	{
		printf("%d:0x%x,",i,dbcell->rgdb[i]);
		++i;
	}
	return dbcell;
}
typedef struct Window1
{
	unsigned short xWn;//(2 bytes): A signed integer that specifies the horizontal position, in twips, of the window. The value is relative to the logical left edge of the client area of the window.
	unsigned short yWn;//(2 bytes): A signed integer that specifies the vertical position, in twips, of the window. The value is relative to the top edge of the client area of the window.
	unsigned short dxWn;//(2 bytes): A signed integer that specifies the width, in twips, of the window. MUST be greater than or equal to 1.
	unsigned short dyWn;//(2 bytes): A signed integer that specifies the height, in twips, of the window. MUST be greater than or equal to 1.
	unsigned short A:1;// fHidden (1 bit): A bit that specifies whether the window is in the list of hidden windows.
	unsigned short B:1;// fIconic (1 bit): A bit that specifies whether the window is minimized.
	unsigned short C:1;// fVeryHidden (1 bit): A bit that specifies whether the window has the properties of fHidden and that also specifies that the user cannot see that the window is in the list of hidden windows.
	unsigned short D:1;// fDspHScroll (1 bit): A bit that specifies whether a horizontal scroll bar is displayed.
	unsigned short E:1;// fDspVScroll (1 bit): A bit that specifies whether a vertical scroll bar is displayed.
	unsigned short F:1;// fBotAdornment (1 bit): A bit that specifies whether sheet tabs are displayed.
	unsigned short G:1;// fNoAFDateGroup (1 bit): A bit that specifies whether dates are grouped hierarchically in the AutoFilter menu or listed chronologically in the AutoFilter menu.
	/*
	   Value Meaning
	   0 Dates are grouped by year, month and day in the AutoFilter menu.
	   1 Dates are listed chronologically in the AutoFilter menu.
	   */
	unsigned short reserved:9;//(9 bits): MUST be zero, and MUST be ignored.
	unsigned short itabCur;//(2 bytes): A TabIndex structure that specifies the selected sheet tab.
	unsigned short itabFirst;//(2 bytes): A TabIndex structure that specifies the first displayed sheet tab.
	unsigned short ctabSel;//(2 bytes): An unsigned integer that specifies the number of sheet tabs that are selected.  The value MUST be less than or equal to the number of sheets in the workbook. Each sheet stream in the workbook stream specifies a sheet.
	unsigned short wTabRatio;//(2 bytes): An unsigned integer that specifies the ratio of the width of the sheet tabs to the width of the horizontal scroll bar, multiplied by 1000. MUST be less than or equal to 1000.
}Window1;
typedef struct BookBool
{//The BookBool record specifies some of the properties associated with a workbook.
	unsigned char A:1;// fNoSaveSup (1 bit): A bit that specifies that external link values are saved in the workbook.  MUST be a value from the following table: Value Meaning
	/*
	   0 External link values are saved.
	   1 External link values are not saved.
	   */
	unsigned char B:1;// reserved1 (1 bit): MUST be zero, and MUST be ignored.
	unsigned char C:1;// fHasEnvelope (1 bit): A bit that specifies whether the workbook has an envelope as a result of sending the workbook to a mail recipient. If fEnvelopeVisible is 1 or fEnvelopeInitDone is 1, then this bit MUST be 1.
	unsigned char D:1;// fEnvelopeVisible (1 bit): A bit that specifies whether the envelope is visible.
	unsigned char E:1;// fEnvelopeInitDone (1 bit): A bit that specifies whether the envelope has been initialized.
	unsigned char F:2;// grUpdateLinks (2 bits): An unsigned integer that specifies when the application updates external links in the workbook. The value MUST be one of the following:
	/*
	   Value Meaning
	   0 Prompt user to update.
	   1 Do not update, and do not prompt user.
	   2 Silently update external links. The application can choose to prompt the user because of security concerns<44>.
	   */
	unsigned char G:1;// unused (1 bit): Undefined and MUST be ignored.
	unsigned char H:1;// fHideBorderUnselLists (1 bit): A bit that specifies whether to hide borders of tables that do not contain the active cell<45>.
	unsigned char reserved2:7;// (7 bits): MUST be zero, and MUST be ignored.
}BookBool;
typedef struct MTRSettings
{//The MTRSettings record specifies multithreaded calculation settings.
	FrtHeader frtHeader;//(12 bytes): An FrtHeader structure. The frtHeader.rt field MUST be 0x089A.
	unsigned int fMTREnabled;//(4 bytes): A Boolean (section 2.5.14) that specifies whether the multithreaded calculation is enabled. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x00000000 Multithreaded calculation is disabled.
	   0x00000001 Multithreaded calculation is enabled.
	   */
	unsigned int fUserSetThreadCount;//(4 bytes): A Boolean that specifies whether the thread count was manually specified by the user. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x00000000 The thread count cUserThreadCount was not manually specified by the user.
	   0x00000001 The thread count cUserThreadCount was manually specified by the user.
	   */
	unsigned short cUserThreadCount;//(4 bytes): A signed integer that specifies the count of calculation threads. MUST be greater than or equal to 0x00000001 and MUST be less than or equal to 0x00000400. If fMTREnabled is 0
}MTRSettings;
void MTRSettings_free(MTRSettings * mtrsettings)
{
	if(mtrsettings)
	{
		free(mtrsettings);
	}
}
MTRSettings * MTRSettings_read(ByteArray * bytearray)
{
	printf("MTRSettings (section 2.4.173)");
	MTRSettings * mtrsettings = malloc(sizeof(MTRSettings));
	ByteArray_readBytes(bytearray,sizeof(MTRSettings),(char*)mtrsettings);
	printf("rt:%d,",mtrsettings->frtHeader.rt);
	printf("grbitFrt:%d,",mtrsettings->frtHeader.grbitFrt);
	printf("fMTREnabled:%d,",mtrsettings->fMTREnabled);
	printf("fUserSetThreadCount:%d,",mtrsettings->fUserSetThreadCount);
	printf("cUserThreadCount:%d,",mtrsettings->cUserThreadCount);

	return mtrsettings;
}
typedef struct Country
{
	unsigned short iCountryDef;//(2 bytes): An unsigned integer that specifies the country/region code determined by the locale in effect when the workbook was saved. MUST be greater than or equal to 1 and less than or equal to 981 and MUST be a value from the table in iCountryWinIni.
	unsigned short iCountryWinIni;//(2 bytes): An unsigned integer that specifies the system regional settings country/region code in effect when the workbook was saved. MUST greater than or equal to 1 and less than or equal to 981 and MUST be a value from the table of Country/Region codes in this section.  Country/Region codes are defined as follows:
	/*
	 *
	 */
}Country;
void Country_free(Country * country)
{
	if(country)
	{
		free(country);
	}
}
Country * Country_read(ByteArray * bytearray)
{
	//printf("Country (section 2.4.63)");
	Country * country = malloc(sizeof(Country));
	ByteArray_readBytes(bytearray,4,(char*)country);
	switch(country->iCountryWinIni)
	{
		case 1:
			printf("United States");
			break;
		case 2:
			printf("Canada");
			break;
		case 3:
			printf("Latin America, except Brazil");
			break;
		case 7:
			printf("Russia");
			break;
		case 20:
			printf("Egypt");
			break;
		case 30:
			printf("Greece");
			break;
		case 31:
			printf("Netherlands");
			break;
		case 32:
			printf("Belgium");
			break;
		case 33:
			printf("France");
			break;
		case 34:
			printf("Spain");
			break;
		case 36:
			printf("Hungary");
			break;
		case 39:
			printf("Italy");
			break;
		case 41:
			printf("Switzerland");
			break;
		case 43:
			printf("Austria");
			break;
		case 44:
			printf("United Kingdom");
			break;
		case 45:
			printf("Denmark");
			break;
		case 46:
			printf("Sweden");
			break;
		case 47:
			printf("Norway");
			break;
		case 48:
			printf("Poland");
			break;
		case 49:
			printf("Germany");
			break;
		case 52:
			printf("Mexico");
			break;
		case 55:
			printf("Brazil");
			break;
		case 61:
			printf("Australia");
			break;
		case 64:
			printf("New Zealand");
			break;
		case 66:
			printf("Thailand");
			break;
		case 81:
			printf("Japan");
			break;
		case 82:
			printf("Korea");
			break;
		case 84:
			printf("Viet Nam");
			break;
		case 86:
			printf("People’s Republic of China");
			break;
		case 90:
			printf("Turkey");
			break;
		case 213:
			printf("Algeria");
			break;
		case 216:
			printf("Morocco");
			break;
		case 218:
			printf("Libya");
			break;
		case 351:
			printf("Portugal");
			break;
		case 354:
			printf("Iceland");
			break;
		case 358:
			printf("Finland");
			break;
		case 420:
			printf("Czech Republic");
			break;
		case 886:
			printf("Taiwan");
			break;
		case 961:
			printf("Lebanon");
			break;
		case 962:
			printf("Jordan");
			break;
		case 963:
			printf("Syria");
			break;
		case 964:
			printf("Iraq");
			break;
		case 965:
			printf("Kuwait");
			break;
		case 966:
			printf("Saudi Arabia");
			break;
		case 971:
			printf("United Arab Emirates");
			break;
		case 972:
			printf("Israel");
			break;
		case 974:
			printf("Qatar");
			break;
		case 981:
			printf("Iran");
			break;
		default:
			printf("unknow country!");

	}
	return country;
}
typedef struct CellXF
{
	unsigned int alc:3;//(3 bits): A HorizAlign that specifies the horizontal alignment.
	unsigned int A:1;//( fWrap (1 bit): A bit that specifies whether the cell text is wrapped.
	unsigned int alcV:3;//(3 bits): A VertAlign that specifies the vertical alignment.
	unsigned int B:1;//( fJustLast (1 bit): A bit that specifies whether the justified or distributed alignment of the cell is used on the last line of text (setting this to 1 is typical for East Asian text but not typical in other contexts). If this field equals 1, then alc MUST equal 7.

	unsigned int trot:8;//(1 byte): An XFPropTextRotation that specifies the text rotation.

	unsigned int cIndent:4;//(4 bits): An unsigned integer that specifies the text indentation level. MUST be less than or equal to 15.
	unsigned int C:1;//( fShrinkToFit (1 bit): A bit that specifies whether the cell is shrink to fit.
	unsigned int D:1;//( reserved1 (1 bit): MUST be 0, and MUST be ignored.
	unsigned int E:2;//( iReadOrder (2 bits): A ReadingOrder that specifies the reading order.

	unsigned int F:2;//( reserved2 (2 bits): MUST be 0, and MUST be ignored.
	unsigned int G:1;//( fAtrNum (1 bit): A bit that specifies that if the ifmt field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding field of the containing XF record will not be set to the same value. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The ifmt field of the containing XF record is
	   Value
	   Meaning
	   updated when the corresponding field of the XF
	   record specified by the ixfParent field of the
	   containing XF record is changed.
	   0x1
	   The ifmt field of the containing XF record is not
	   updated when the corresponding field of the XF
	   record specified by the ixfParent field of the
	   containing XF record is changed.
	   */
	unsigned int H:1;//( fAtrFnt (1 bit): A bit that specifies that if the ifnt field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding field of the containing XF record will not be set to the same value. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The ifnt field of the containing XF record is updated
	   when the corresponding field of the XF record
	   specified by the ixfParent field of the containing
	   XF record is changed.
	   0x1 The ifnt field of the containing XF record is not
	   updated when the corresponding field of the XF
	   record specified by the ixfParent field of the
	   containing XF record is changed.
	   */
	unsigned int I:1;//( fAtrAlc (1 bit): A bit that specifies that if the alc field, or the fWrap field, or the alcV field, or the fJustLast field, or the trot field, or the cIndent field, or the fShrinkToFit field or the iReadOrder field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding fields of this structure will not be set to the same values. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The alc, fWrap, alcV, fJustLast, trot, cIndent,
	   fShrinkToFit, iReadOrder fields are updated when
	   the corresponding fields of the XF record specified
	   by the ixfParent field of the containing XF record
	   are changed.
	   0x1 The alc, fWrap, alcV, fJustLast, trot, cIndent,
	   fShrinkToFit, iReadOrder fields are not updated
	   when the corresponding fields of the XF record
	   specified by the ixfParent field of the containing
	   XF record are changed.
	   */
	unsigned int J:1;//( fAtrBdr (1 bit): A bit that specifies that if the dgLeft field, or the dgRight field, or the dgTop field, or the dgBottom field, or the dgDiag field, or the icvLeft field, or the icvRight field, or the grbitDiag field, or the icvTop field, or the icvBottom field, or the icvDiag field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding fields of this structure will not be set to the same values. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The dgLeft, dgRight, dgTop, dgBottom,
	   dgDiag, icvLeft, icvRight, grbitDiag, icvTop,
	   icvBottom, icvDiag fields are updated when the
	   corresponding fields of the XF record specified by
	   the ixfParent field of the containing XF record are
	   changed.
	   Value Meaning
	   0x1 The dgLeft, dgRight, dgTop, dgBottom,
	   dgDiag, icvLeft, icvRight, grbitDiag, icvTop,
	   icvBottom, icvDiag fields are not updated when
	   the corresponding fields of the XF record specified
	   by the ixfParent field of the containing XF record
	   are changed.
	   */
	unsigned int K:1;//( fAtrPat (1 bit): A bit that specifies that if the fls field, the icvFore field, or the icvBack field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding fields of this structure will not be set to the same values. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The fls, icvFore, and icvBack fields are updated
	   when the corresponding fields of the XF record
	   specified by the ixfParent field of the containing
	   XF record are changed.
	   0x1 The fls, icvFore, and icvBack fields are not
	   updated when the corresponding fields of the XF
	   record specified by the ixfParent field of the
	   containing XF record are changed.
	   */
	unsigned int L:1;//( fAtrProt (1 bit): A bit that specifies that if the fLocked field or the fHidden field of the XF record specified by the ixfParent field of the containing XF record is updated, the corresponding fields of the containing XF record will not be set to the same values. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 The fLocked and fHidden fields of the containing
	   XF record are updated when the corresponding
	   fields of the XF record specified by the ixfParent
	   field of the containing XF record are changed.
	   0x1 The fLocked and fHidden fields of the containing
	   XF record are not updated when the corresponding
	   fields of the XF record specified by the ixfParent
	   field of the containing XF record are changed.
	   */

	unsigned int dgLeft:4;//(4 bits): A BorderStyle that specifies the logical left border formatting.
	unsigned int dgRight4:4;//(4 bits): A BorderStyle that specifies the logical right border formatting.

	unsigned int dgTop:4;//(4 bits): A BorderStyle that specifies the top border formatting.
	unsigned int M:4;//( dgBottom (4 bits): A BorderStyle that specifies the bottom border formatting.

	unsigned int icvLeft:7;//(7 bits): An unsigned integer that specifies the color of the logical left border. The value MUST be one of the values specified in IcvXF or 0. A value of 0 means the logical left border color has not been specified. If this value is 0, then dgLeft MUST also be 0.
	unsigned int icvRight:7;//(7 bits): An unsigned integer that specifies the color of the logical right border. The value MUST be one of the values specified in IcvXF or 0. A value of 0 means the logical right border color has not been specified. If this value is 0, then dgRight MUST also be 0.
	unsigned int N:2;//( grbitDiag (2 bits): An unsigned integer that specifies which diagonal borders are present (if any). MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 No diagonal border
	   0x1 Diagonal-down border
	   0x2 Diagonal-up border
	   0x3 Both diagonal-down and diagonal-up
	   */

	unsigned int icvTop:7;//(7 bits): An unsigned integer that specifies the color of the top border. The value MUST be one of the values specified in IcvXF or 0. A value of 0 means the top border color has not been specified. If this value is 0, then dgTop MUST also be 0.
	unsigned int icvBottom:7;//(7 bits): An unsigned integer that specifies the color of the bottom border. The value MUST be one of the values specified in IcvXF or 0. A value of 0 means the bottom border color has not been specified. If this value is 0 then dgBottom MUST also be 0.
	unsigned int icvDiag:7;//(7 bits): An unsigned integer that specifies the color of the diagonal border. The value MUST be one of the values specified in IcvXF or 0. A value of 0 means the diagonal border color has not been specified. If this value is 0 then dgDiag MUST also be 0.
	unsigned int dgDiag:4;//(4 bits): A BorderStyle that specifies the diagonal border formatting.
	unsigned int O:1;//( fHasXFExt (1 bit): A bit that specifies whether an XFExt will extend the information in this XF.
	unsigned int fls:6;//(6 bits): A FillPattern that specifies the fill pattern. If this value is 1, which specifies a solid fill pattern, then only icvFore is rendered.

	unsigned int icvFore:7;//(7 bits): An IcvXF that specifies the foreground color of the fill pattern.
	unsigned int icvBack:7;//(7 bits): An unsigned integer that specifies the background color of the fill pattern. The value SHOULD<152> be an IcvXF value.
	unsigned int P:1;//( fsxButton (1 bit): A bit that specifies whether the XF record is attached to a pivot field drop-down button.
	unsigned int Q:1;//( reserved3 (1 bit): MUST be 0 and MUST be ignored.

	unsigned int undefined:16;//nothing
}CellXF;
typedef struct XF
{//10bytes
	unsigned short ifnt:16;//(2 bytes): A FontIndex structure that specifies a Font record.

	unsigned short ifmt:16;//(2 bytes): An IFmt structure that specifies a number format identifier.

	unsigned short A:1;// fLocked (1 bit): A bit that specifies whether the locked protection property is set to true.
	unsigned short B:1;// fHidden (1 bit): A bit that specifies whether the hidden protection property is set to true.
	unsigned short C:1;// fStyle (1 bit): A bit that specifies whether this record specifies a cell XF or a cell style XF. If the value is 1, this record specifies a cell style XF.
	unsigned short D:1;// f123Prefix (1 bit): A bit that specifies whether prefix characters are present in the cell. The possible prefix characters are single quote (0x27), double quote (0x22), caret (0x5E), and backslash(0x5C).<146> If fStyle equals 1, this field MUST equal 0.
	unsigned short ixfParent:12;//(12 bits): An unsigned integer that specifies the zero-based index of a cell style XF record in the collection of XF records in the Globals Substream that this cell format inherits properties from. Cell style XF records are the subset of XF records with an fStyle field equal to 1. See XFIndex for more information about the organization of XF records in the file.  If fStyle equals 1, this field SHOULD equal 0xFFF, indicating there is no inheritance from a cell style XF. <147>

	void *Data;//(variable): If the value of fStyle equals 0, this field contains a CellXF that specifies additional properties of the cell XF. If the value of fStyle equals 1, this field contains a StyleXF that specifies additional properties of the cell style XF.
}XF;
void XF_free(XF * xf)
{
	if(xf)
	{
		if(xf->Data)
			free(xf->Data);
		free(xf);
	}
}
XF * XF_read(ByteArray * bytearray)
{
	printf("XF (section 2.4.353),%d,",(int)sizeof(XF));
	XF * xf = malloc(sizeof(XF));
	ByteArray_readBytes(bytearray,6,(char*)xf);
	if(xf->ifnt)
		printf("ifnt:%d,",xf->ifnt);
	if(xf->ifmt)
		printf("ifmt:%d,",xf->ifmt);
	if(xf->A)
		printf("A fLocked:%d,",xf->A);
	if(xf->B)
		printf("B fHidden:%d,",xf->B);
	if(xf->C)
		printf("C fStyle:%d,",xf->C);
	if(xf->D)
		printf("D f123Prefix:%d,",xf->D);
	if(xf->ixfParent)
		printf("ixfParent:0x%x,",xf->ixfParent);
	int len = bytearray->length - bytearray->position;
	xf->Data = malloc(len);
	memset(xf->Data,0,len);
	ByteArray_readBytes(bytearray,len,(char*)xf->Data);
	if(xf->C)//Style
	{
		StyleXF * data = (StyleXF*)xf->Data;
		printf("StyleXF,%d",(int)sizeof(*data));
	}else{
		CellXF* data = (CellXF*)xf->Data;
		printf("CellXF,%d",(int)sizeof(*data));
	}
	return xf;
}
typedef struct Style
{
	unsigned short ixfe:12;// (12 bits): An unsigned integer that specifies the zero-based index of the cell style XF in the collection of XF records in the Globals Substream. See XFIndex for more information about the organization of XF records in the file.
	unsigned short A:3;// unused (3 bits): Undefined and MUST be ignored.
	unsigned short B:1;// fBuiltIn (1 bit): A bit that specifies whether the cell style is built-in.
	unsigned short builtInData;//(2 bytes): An optional BuiltInStyle structure that specifies the built-in cell style properties. MUST exist if and only if fBuiltIn is 1. The value of builtInData.istyBuiltIn MUST be less than or equal to 0x09.
	XLUnicodeString * user;//(variable): An optional XLUnicodeString structure that specifies the name of the user-defined cell style. MUST exist if and only if fBuiltIn is 0. The number of characters in this string SHOULD be greater than or equal to 1 and MUST be less than or equal to 255.<127>
}Style;
void Style_free(Style * style)
{
	if(style)
	{
		if(style->user)
			XLUnicodeString_free(style->user);
		free(style);
	}
}
Style * Style_read(ByteArray * bytearray)
{
	printf("Style (section 2.4.269)");
	Style * style = malloc(sizeof(Style));
	ByteArray_readBytes(bytearray,4,(char*)style);
	if(style->A)
		printf("A:%d,",style->A);
	if(style->B)
		printf("B:%d,",style->B);
	if(style->ixfe)
		printf("ixfe:%d,",style->ixfe);
	if(style->B)
		printf("builtInData: 0x%x,",style->builtInData);
	style->user = XLUnicodeString_read(bytearray);
	return style;
}
typedef struct BoundSheet8
{//The BoundSheet8 record specifies basic information about a sheet (1), including the sheet (1) name, hidden state, and type of sheet (1).
	unsigned int lbPlyPos;//(4 bytes): A FilePointer as specified in [MS-OSHARED] section 2.2.1.5 that specifies the stream position of the start of the BOF record for the sheet (1).
	unsigned char A:2;// hsState (2 bits): An unsigned integer that specifies the hidden state of the sheet (1). MUST be a value from the following table:
	/*
	   Value Meaning
	   0x00 Visible
	   0x01 Hidden
	   0x02 Very Hidden; the sheet (1) is hidden and cannot be displayed using the user interface.
	   */
	unsigned char unused:6;//(6 bits): Undefined and MUST be ignored.
	unsigned char dt;//(8 bits): An unsigned integer that specifies the sheet (1) type. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x00 Worksheet or dialog sheet The sheet substream that starts with the BOF record specified in lbPlyPos MUST contain one WsBool record. If the fDialog field in that WsBool is 1 then the sheet is dialog sheet. Otherwise, the sheet is a worksheet.
	   0x01 Macro sheet
	   0x02 Chart sheet
	   0x06 VBA module
	   */
	ShortXLUnicodeString * stName;//(variable): A ShortXLUnicodeString structure that specifies the unique case-insensitive name of the sheet (1). The character count of this string, stName.cch, MUST be greater than or equal to 1 and less than or equal to 31. The string MUST NOT contain the any of the following characters:
	/*
	   0x0000
	   0x0003
	   colon (:)
	   backslash (\)
	   asterisk (*)
	   question mark (?)
	   forward slash (/)
	   opening square bracket ([)
	   closing square bracket (])
	   The string MUST NOT begin or end with the single quote (') character.
	   */

}BoundSheet8;
void BoundSheet8_free(BoundSheet8 * boundsheet8)
{
	if(boundsheet8)
	{
		if(boundsheet8->stName)
			ShortXLUnicodeString_free(boundsheet8->stName);
		free(boundsheet8);
	}
}
BoundSheet8 * BoundSheet8_read(ByteArray * bytearray)
{
	printf("BoundSheet8 (section 2.4.28)");//The BoundSheet8 record specifies basic information about a sheet (1), including the sheet (1) name, hidden state, and type of sheet (1).
	BoundSheet8 * boundsheet8 = malloc(sizeof(BoundSheet8));
	ByteArray_readBytes(bytearray,6,(char*)boundsheet8);
	printf("lbPlyPos:0x%x,",boundsheet8->lbPlyPos);
	switch(boundsheet8->A)
	{
		case 0x00:
			printf("Visible,");
			break;
		case 0x01:
			printf("Hidden,");
			break;
		case 0x02:
			printf("Very Hidden; the sheet (1) is hidden and cannot be displayed using the user interface.");
			break;
	}
	switch(boundsheet8->dt)
	{
		case 0:// 0x00 Worksheet or dialog sheet The sheet substream that starts with the BOF record specified in lbPlyPos MUST contain one WsBool record. If the fDialog field in that WsBool is 1 then the sheet is dialog sheet. Otherwise, the sheet is a worksheet.
			printf("Worksheet or dialog sheet,");
			break;
		case 0x01:
			printf("Macro sheet,");
			break;
		case 0x02:
			printf("Macro sheet,");
			break;
		case 0x06:
			printf("VBA module,");
			break;
	}
	boundsheet8->stName = ShortXLUnicodeString_read(bytearray);
	return boundsheet8;
}

typedef struct Font
{
	unsigned short dyHeight;//(2 bytes): An unsigned integer that specifies the height of the font in twips.  SHOULD<87> be greater than or equal to 20 and less than or equal to 8191. MUST be greater than or equal to 20 and less than or equal to 8191, or 0.

	unsigned short A:1;// unused1 (1 bit): Undefined and MUST be ignored.
	unsigned short B:1;// fItalic (1 bit): A bit that specifies whether the font is italic.
	unsigned short C:1;// unused2 (1 bit): Undefined and MUST be ignored. MUST be 1 when uls is greater than 0x00.
	unsigned short D:1;// fStrikeOut (1 bit): A bit that specifies whether the font has strikethrough formatting applied.
	unsigned short E:1;// fOutline (1 bit): A bit that specifies whether the font has an outline effect applied.
	unsigned short F:1;// fShadow (1 bit): A bit that specifies whether the font has a shadow effect applied.
	unsigned short G:1;// fCondense (1 bit): A bit that specifies whether the font is condensed.
	unsigned short H:1;// fExtend (1 bit): A bit that specifies whether the font is extended.
	unsigned short reserved:8;//(8 bits): MUST be zero, and MUST be ignored.

	unsigned short icv;//(2 bytes): An unsigned integer that specifies the color of the font. The value SHOULD<88> be an IcvFont value. The value MUST be an IcvFont value, or 0.

	unsigned short bls;//(2 bytes): An unsigned integer that specifies the font weight. The value SHOULD<89> be a value from the following table. This value MUST be 0, or greater than or equal to 100 and less than or equal to 1000.
	/*
	   Value Meaning
	   */

	unsigned short sss;//(2 bytes): An unsigned integer that specifies whether superscript, subscript, or normal script is used.
	unsigned short uls:8;//(1 byte): An unsigned integer that specifies the underline style.
	unsigned short bFamily:8;//(1 byte): An unsigned integer that specifies the font family this font belongs to.
	unsigned short bCharSet:8;//(1 byte): An unsigned integer that specifies the character set.
	unsigned short unused3:8;//(1 byte): Undefined and MUST be ignored.
	ShortXLUnicodeString * fontName;//(variable): A ShortXLUnicodeString structure that specifies the name of this font. String length MUST be greater than or equal to 1 and less than or equal to 31. The fontName.fHighByte field MUST equal 1. MUST NOT contain any null characters.
}Font;
void Font_free(Font * font)
{
	if(font)
	{
		if(font->fontName)
			ShortXLUnicodeString_free(font->fontName);
		free(font);
	}
}
Font * Font_read(ByteArray*bytearray)
{
	printf("Font (section 2.4.122),%d,",(int)sizeof(Font));
	Font * font = malloc(sizeof(Font));
	ByteArray_readBytes(bytearray,14,(char*)font);
	if(font->dyHeight)
		printf("dyHeight:%d,",font->dyHeight);
	if(font->A)
		printf("A:%d,",font->A);
	if(font->B)
		printf("B:%d,",font->B);
	if(font->C)
		printf("C:%d,",font->C);
	if(font->D)
		printf("D:%d,",font->D);
	if(font->E)
		printf("E:%d,",font->E);
	if(font->F)
		printf("F:%d,",font->F);
	if(font->G)
		printf("G:%d,",font->G);
	if(font->H)
		printf("H:%d,",font->H);
	switch(font->bls)
	{
		//case 400:printf("Normal font weight");break;
		case 700:printf("Bold font weight");break;
	}
	printf(",");
	switch(font->sss)
	{
		//case 0x0000:printf("Normal script");break;
		case 0x0001:printf("Superscript");break;
		case 0x0002:printf("Subscript");break;
	}
	printf(",");
	switch(font->uls)
	{
		//case 0x00:printf("No underline");break;
		case 0x01:printf("Single underline");break;
		case 0x02:printf("Double underline");break;
		case 0x21:printf("Single accounting");break;
		case 0x22:printf("Double accounting");break;
	}
	printf(",");

	switch(font->bFamily)
	{
		//case 0x00:printf("Not applicable");break;
		case 0x01:printf("Roman");break;
		case 0x02:printf("Swiss");break;
		case 0x03:printf("Modern");break;
		case 0x04:printf("Script");break;
		case 0x05:printf("Decorative");break;
	}
	printf(",");
	switch(font->bCharSet)
	{
		case 0x00:printf("ANSI_CHARSET");break;
		case 0x01:printf("DEFAULT_CHARSET");break;
		case 0x02:printf("SYMBOL_CHARSET");break;
		case 0x4D:printf("MAC_CHARSET");break;
		case 0x80:printf("SHIFTJIS_CHARSET");break;
		case 0x81:printf("HANGEUL_CHARSET");break;
				  //case 0x81:printf("HANGUL_CHARSET");break;
		case 0x82:printf("JOHAB_CHARSET");break;
		case 0x86:printf("GB2312_CHARSET");break;
		case 0x88:printf("CHINESEBIG5_CHARSET");break;
		case 0xA1:printf("GREEK_CHARSET");break;
		case 0xA2:printf("TURKISH_CHARSET");break;
		case 0xA3:printf("VIETNAMESE_CHARSET");break;
		case 0xB1:printf("HEBREW_CHARSET");break;
		case 0xB2:printf("ARABIC_CHARSET");break;
		case 0xBA:printf("BALTIC_CHARSET");break;
		case 0xCC:printf("RUSSIAN_CHARSET");break;
		case 0xDD:printf("THAI_CHARSET");break;
		case 0xEE:printf("EASTEUROPE_CHARSET");break;
		case 0xFF:printf("OEM_CHARSET");break;
	}
	printf(",");
	font->fontName = ShortXLUnicodeString_read(bytearray);

	return font;
}
void BookBool_free(BookBool * b)
{
	if(b)
		free(b);
}
BookBool * BookBool_read(ByteArray * bytearray)
{
	BookBool * bookbool = malloc(sizeof(bytearray));
	ByteArray_readBytes(bytearray,sizeof(BookBool),(char*)bookbool);
	printf("BookBool (section 2.4.22),%d,",(int)sizeof(*bookbool));
	if(bookbool->A)
		printf("A:%d,",bookbool->A);
	if(bookbool->B)
		printf("B:%d,",bookbool->B);
	if(bookbool->C)
		printf("C:%d,",bookbool->C);
	if(bookbool->D)
		printf("D:%d,",bookbool->D);
	if(bookbool->E)
		printf("E:%d,",bookbool->E);
	if(bookbool->F)
		printf("F:%d,",bookbool->F);
	if(bookbool->G)
		printf("G:%d,",bookbool->G);
	if(bookbool->H)
		printf("H:%d,",bookbool->H);
	return bookbool;
}
void Window1_free(Window1 * window)
{
	if(window)
		free(window);
}
Window1 * Window1_read(ByteArray * bytearray)
{
	Window1 * window = malloc(sizeof(Window1));
	ByteArray_readBytes(bytearray,sizeof(Window1),(char*)window);
	printf("x:%d,y:%d,w:%d,h:%d,tableIndex:%d,first:%d,selected:%d,ratio:%d"
			,window->xWn
			,window->yWn
			,window->dxWn
			,window->dyWn
			,window->itabCur
			,window->itabFirst
			,window->ctabSel
			,window->wTabRatio
		  );
	return window;
}

BOF * BOF_read(ByteArray * bytearray)
{
	printf("\nBOF (section 2.4.21)");
	BOF * bof = malloc(sizeof(BOF));
	memset(bof,0,sizeof(BOF));
	if(sizeof(BOF)!=16){
		printf("sizeof BOF:%d\n",(int)sizeof(BOF));
		BOF_free(bof); return NULL;
	}
	bof->vers = ByteArray_readInt16(bytearray);
	bof->dt = ByteArray_readInt16(bytearray);
	bof->rupBuild = ByteArray_readInt16(bytearray);
	bof->rupYear = ByteArray_readInt16(bytearray);
	if(bof->vers!=0x0600)
	{
		printf("\nXXXXXXXXXXXXx not a XlsFile! 0x%x\r\n\r\n",bof->vers);
		BOF_free(bof); return NULL;
	}
	switch(bof->dt)
	{

		case 0x0005:
			printf("workbook substream.\n");
			break;
		case 0x0010:
			printf("dialog sheet substream or worksheet substream.");//  The sheet (1) substream that starts with this BOF record MUST contain one WsBool record. If the fDialog field in that WsBool is 1 then the sheet (1) is dialog sheet otherwise the sheet (1) is a worksheet.
		case 0x0020:
			printf("chart sheet substream.\n");
			break;
		case 0x0040:
			printf("macro sheet substream.\n");
			break;
		default:
			printf("XXXXXXXXXXXXx bof dt error! 0x%x\r\n\r\n",bof->dt);
			BOF_free(bof); return NULL;
			break;
	}
	switch(bof->rupYear)
	{
		case 0x07CC:
			break;
		case 0x07CD:
			break;
		default:
			printf("XXXXXXXXXXXXx bof rupYear error! 0x%x \r\n\r\n",bof->rupYear);
			BOF_free(bof); return NULL;
	}
	ByteArray_readBytes(bytearray,8,(char*)bof+8);
	if(bof->A)
		printf("A:%x\n",bof->A);
	if(bof->B)
		printf("B:%x\n",bof->B);
	if(bof->C)
		printf("C:%x\n",bof->C);
	if(bof->D)
		printf("D:%x\n",bof->D);
	if(bof->E)
		printf("E:%x\n",bof->E);
	if(bof->F)
		printf("F:%x\n",bof->F);
	if(bof->G)
		printf("G:%x\n",bof->G);
	if(bof->H)
		printf("H:%x\n",bof->H);
	if(bof->I)
		printf("I:%x\n",bof->I);
	if(bof->J)
		printf("J:%x\n",bof->J);
	if(bof->K)
		printf("K:%x\n",bof->K);
	if(bof->L)
		printf("L:%x\n",bof->L);
	if(bof->M)
		printf("M:%x\n",bof->M);
	if(bof->N)
		printf("N:%x\n",bof->N);
	if(bof->O)
		printf("O:%x\n",bof->O);
	if(bof->reserved1)
		printf("reserved1:%x\n",bof->reserved1);
	if(bof->reserved2)
		printf("reserved2:%x\n",bof->reserved2);
	if(bof->verLowestBiff!=6)
	{
		printf("XXXXXXXXXXXXx verLowestBiff:%x\n",bof->verLowestBiff);
		BOF_free(bof); return NULL;
	}
	return bof;
}

typedef struct Index
{// The Index record specifies row information and the file locations for all DBCell records corresponding to each row block in the sheet. This record, combined with the DBCell records, is used to optimize the lookup of cells in a cell table.
	unsigned int reserved;// (4 bytes): MUST be zero, and MUST be ignored.
	unsigned int rwMic;// (4 bytes): A RwLongU structure that specifies the first row that has at least one cell with data in current sheet. MUST be 0 if there are no rows that have at least one cell with data.
	unsigned int rwMac;// (4 bytes): An unsigned integer that specifies one plus the zero-based index of the last row that has at least one cell with data in the sheet. MUST be 0 if there are no rows that have at least one cell with data. If not 0, MUST be greater than rwMic.
	unsigned int ibXF;// (4 bytes): A FilePointer as specified in [MS-OSHARED] section 2.2.1.5 that specifies the file position of the DefColWidth record in the current sheet.
	unsigned int *rgibRw;// (variable): An array of FilePointer. Each FilePointer as specified in [MS-OSHARED] section 2.2.1.5 specifies the file position of each referenced DBCell record. If the positions of DBCell records are not correct, there is no optimized method to do cell lookup and this can cause performance issues. The number of elements in the array MUST be equal to the number of row blocks in this sheet.
}Index;
void Index_free(Index * index)
{
	if(index)
	{
		//if(index->rgibRw) free(index->rgibRw);
		free(index);
	}
}
Index * Index_read(ByteArray * bytearray)
{
	printf("Index (section 2.4.144)\n");
	Index * index = malloc(sizeof(Index));
	ByteArray_readBytes(bytearray,16,(char*)index);
	if(index->reserved)
	{
		printf("Index reserved: 0x%x XXXXXXXXXXXXx\n",index->reserved);
		Index_free(index);
		return 0;
	}
	if(index->rwMic)
		printf("rwMic:%d\n",index->rwMic);
	if(index->rwMac)
		printf("rwMac:%d\n",index->rwMac);
	if(index->ibXF)
		printf("ibXF:0x%x\n",index->ibXF);
	index->rgibRw = (unsigned int *)((char*)bytearray->data + bytearray->position);
	int i=0;
	while(bytearray->position<bytearray->length)
	{
		printf("%d:0x%x\n",i++,ByteArray_readInt16(bytearray));
	}
	return index;
}
void ShortXLUnicodeString_free(ShortXLUnicodeString * s)
{
	if(s){
		if(s->rgb)
			free(s->rgb);
		free(s);
	}
}
void XLUnicodeString_free(XLUnicodeString * s)
{
	if(s){
		if(s->rgb)
			free(s->rgb);
		free(s);
	}
}
XLUnicodeString * XLUnicodeString_read(ByteArray* bytearray)
{
	XLUnicodeString * s = malloc(sizeof(XLUnicodeString));
	memset(s,0,sizeof(XLUnicodeString));
	s->cch = ByteArray_readInt16(bytearray);
	s->A = ByteArray_readByte(bytearray);
	if(s->cch)
		printf("cch:%d,",s->cch);
	if(s->A)
		printf("A:%d,",s->A);
	//printf("bytearray->length:%d,",bytearray->length);
	int len = s->cch;
	if(len==0)
	{
		XLUnicodeString_free(s);
		printf("empty!");
		return 0;
	}
	if(s->A){
		len *= 2;
	}
	s->rgb = malloc(len);
	int position = bytearray->position;
	ByteArray_readBytes(bytearray,len,(char*)s->rgb);
	bytearray->position = position;
	ByteArray_print16(bytearray,len/2);
	return s;
}
ShortXLUnicodeString * ShortXLUnicodeString_read(ByteArray* bytearray)
{
	//printf("(%d)",sizeof(ShortXLUnicodeString));
	ShortXLUnicodeString * s = malloc(sizeof(ShortXLUnicodeString));
	memset(s,0,sizeof(ShortXLUnicodeString));
	s->cch = ByteArray_readByte(bytearray);
	s->A = ByteArray_readByte(bytearray); 
	int len = s->cch;
	if(len==0)
	{
		ShortXLUnicodeString_free(s);
		printf("XXXXXXXXXXXXx,len == 0");
		return 0;
	}
	if(s->A){
		len *= 2;
	}
	//printf("cch:%d,A:%d,size:%d,len:%d,",s->cch,s->A,bytearray->length,len);
	s->rgb = malloc(len);
	int position = bytearray->position;
	ByteArray_readBytes(bytearray,len,(char*)s->rgb);
	bytearray->position = position;
	ByteArray_print16(bytearray,len/2);
	return s;
}



typedef struct Record
{
	unsigned short type;
	unsigned short size;
	ByteArray * data;
}Record;

XlsFile * XlsFile_parse(ByteArray * bytearray,char * out)
{
	XlsFile * file = malloc(sizeof(XlsFile));
	memset(file,0,sizeof(XlsFile));

	file->bytearray = bytearray;
	file->header = Ole_read(NULL,file->bytearray);
	if(file->header==NULL)
	{
		sprintf(out+strlen(out),"ole read error!");
		return 0;
	}
	StorageEntry * entry = Ole_getEntryByName(file->header,"Workbook");
	if(entry==NULL){
		sprintf(out+strlen(out),"not a xls file!");
		return 0;
	}
	int position = Ole_getEntryPostion(file->header,entry);
	int start = position;

	int i = 0;
	while(start<position + entry->streamSize)
	{
		bytearray->position = start;
		Record first;
		first.type = ByteArray_readInt16(bytearray);
		first.size = ByteArray_readInt16(bytearray);
		printf("\nrecord[%d]0x%x :%d,%d\t",i,bytearray->position,first.type,first.size);

		char * data = malloc(first.size);
		ByteArray_readBytes(bytearray,first.size,data);
		ByteArray * bytes = ByteArray_new(first.size);
		ByteArray_writeBytes(bytes,data,first.size);
		free(data);
		first.data = bytes;
		bytes->position = 0;

		switch(first.type)
		{
			case 6:
				printf("Formula (section 2.4.127)");
				break;
			case 10:
				printf("EOF (section 2.4.103)\n");
				//return 0;
				break;
			case 12:
				printf("CalcCount (section 2.4.31)");
				break;
			case 13:
				printf("CalcMode (section 2.4.34)");
				break;
			case 14:
				printf("CalcPrecision (section 2.4.35)");
				if(ByteArray_readInt16(bytes))
				{
					//printf("the precision as displayed mode is not selected.");
				}else{
					printf("the precision as displayed mode is selected.");
				}
				//fFullPrec (2 bytes): A Boolean (section 2.5.14) that specifies whether the precision as displayed mode is selected.
				break;
			case 15:
				printf("CalcRefMode (section 2.4.36)");
				break;
			case 16:
				printf("CalcDelta (section 2.4.32)");
				break;
			case 17:
				printf("CalcIter (section 2.4.33)");
				break;
			case 18:
				switch(ByteArray_readInt16(bytes))//The Protect record specifies the protection state for the sheet or workbook. If this record exists in the Globals Substream, then the protection state specified in this record applies to the workbook. If this record exists in a worksheet substream, chart sheet substream, macro sheet substream, or dialog sheet substream, then the protection state specified in this record applies to only that sheet. This record MUST exist for the workbook. For a sheet, the sheet is protected if and only if this record exists.
				{
					//fLock (2 bytes): A Boolean (section 2.5.14) that specifies whether the sheet or workbook is protected. For a sheet, fLock MUST be 0x0001, and it means the sheet is protected. For the workbook, it MUST be a value from the following table:
					case 0:
						//printf("The workbook is not protected.");
						break;
					case 1:
						printf("The workbook is protected.");
						break;
					default:
						printf("WinProtect(section 2.4.207) XXXXXXXXXXXXx");
						return 0;
				}

				break;
			case 19:
				if(ByteArray_readInt16(bytes))
					printf("Password (section 2.4.191).");//The Password record specifies the password verifier for the sheet or workbook. If this record exists in the Globals Substream, then it is a password for the workbook. If this record exists in a worksheet substream, chart sheet substream, macro sheet substream, or dialog sheet substream, then it is a password for only that sheet. This record MUST exist for the workbook. A sheet has a password if and only if this record exists.
				//wPassword (2 bytes): An unsigned integer that specifies the password verifier<100>. See Password Verifier Algorithm for more information. If the password is for a sheet, MUST NOT equal 0x0000. If wPassword is 0x0000 it means the workbook has no password.

				break;
			case 20:
				printf("Header (section 2.4.136)");
				break;
			case 21:
				printf("Footer (section 2.4.124)");
				break;
			case 23:
				printf("ExternSheet (section 2.4.106)");
				break;
			case 24:
				printf("Lbl (section 2.4.150)");
				break;
			case 25:
				switch(ByteArray_readInt16(bytes))//fLockWn (2 bytes): A Boolean (section 2.5.14) that specifies whether the windows can be resized or moved and whether the window state can be changed.
				{
					case 0:
						//printf("The workbook windows can be resized or moved and the window state can be changed.");
						break;
					case 1:
						printf("The workbook windows cannot be resized or moved and the window state cannot be changed.");
						break;
					default:
						printf("WinProtect (section 2.4.347) XXXXXXXXXXXXx");
						return 0;
				}
				break;
			case 26:
				printf("VerticalPageBreaks (section 2.4.343)");
				break;
			case 27:
				printf("HorizontalPageBreaks (section 2.4.142)");
				break;
			case 28:
				printf("Note (section 2.4.179)");
				break;
			case 29:
				printf("Selection (section 2.4.248)");
				break;
			case 34:
				switch(ByteArray_readInt16(bytes))
				{
					case 0:
						printf("January 1, 1900 = 1.");
						file->dateFrom1900 = 1;
						break;
					case 1:
						printf("January 1, 1904 = 0.");
						break;
					default:
						printf("Date1904 (section 2.4.77) XXXXXXXXXXXXx");
						return 0;
						break;
				}

				break;
			case 35:
				printf("ExternName (section 2.4.105)");
				break;
			case 38:
				printf("LeftMargin (section 2.4.151)");
				break;
			case 39:
				printf("RightMargin (section 2.4.219)");
				break;
			case 40:
				printf("TopMargin (section 2.4.328)");
				break;
			case 41:
				printf("BottomMargin (section 2.4.27)");
				break;
			case 42:
				printf("PrintRowCol (section 2.4.203)");
				break;
			case 43:
				printf("PrintGrid (section 2.4.202)");
				break;
			case 47:
				printf("FilePass (section 2.4.117)");
				break;
			case 49:
				Font_free(Font_read(bytes));
				break;
			case 51:
				printf("PrintSize (section 2.4.204)");
				break;
			case 60:
				printf("Continue (section 2.4.58)");
				break;
			case 61:
				printf("Window1 (section 2.4.345)");//The Window1 record specifies attributes of a window used to display a sheet (called "the window" within this record definition). For each Window2.4.345 record in the Globals Substream there MUST be an associated Window2.4.346 record in each chart sheet, worksheet, macro sheet, and dialog sheet substream that exists in the workbook. The Window2.4.346 record in a given substream associated with a given Window2.4.345 record is the Window2.4.346 record whose ordinal position in the collection of Window2.4.346 records in the containing substream is equal to the ordinal position of the given Window2.4.345 record in the collection of Window2.4.345 records in the Globals Substream.  Each Window2.4.346 record specifies extended properties of the associated Window2.4.345 record.
				Window1_read(bytes);
				break;
			case 64:
				//if(ByteArray_readInt16(bytes))
				printf("Backup (section 2.4.14)");
				break;
			case 65:
				printf("Pane (section 2.4.189)");
				break;
			case 66:
				printf("CodePage (section 2.4.52)");
				switch(ByteArray_readInt16(bytes))
				{
					case 1200:
						printf("Unicode");
						break;
				}
				break;
			case 77:
				printf("Pls (section 2.4.199)");
				break;
			case 80:
				printf("DCon (section 2.4.82)");
				break;
			case 81:
				printf("DConRef (section 2.4.86)");
				break;
			case 82:
				printf("DConName (section 2.4.85)");
				break;
			case 85:
				printf("DefColWidth (section 2.4.89)");
				break;
			case 89:
				printf("XCT (section 2.4.352)");
				break;
			case 90:
				printf("CRN (section 2.4.65)");
				break;
			case 91:
				printf("FileSharing (section 2.4.118)");
				break;
			case 92:
				printf("WriteAccess (section 2.4.349)");
				if(first.size!=112)
				{
					printf("XXXXXXXXXXXXx");
				}
				XLUnicodeString_free(XLUnicodeString_read(bytes));
				break;
			case 93:
				printf("Obj (section 2.4.181)");
				break;
			case 94:
				printf("Uncalced (section 2.4.331)");
				break;
			case 95:
				printf("CalcSaveRecalc (section 2.4.37)");
				break;
			case 96:
				printf("Template (section 2.4.323)");
				break;
			case 97:
				printf("Intl (section 2.4.147)");
				break;
			case 99:
				printf("ObjProtect (section 2.4.183)");
				break;
			case 125:
				ColInfo_free(ColInfo_read(bytes));
				break;
			case 128:
				printf("Guts (section 2.4.134)");
				break;
			case 129:
				printf("WsBool (section 2.4.351)");
				break;
			case 130:
				printf("GridSet (section 2.4.132)");
				break;
			case 131:
				printf("HCenter (section 2.4.135)");
				break;
			case 132:
				printf("VCenter (section 2.4.342)");
				break;
			case 133:
				BoundSheet8_free(BoundSheet8_read(bytes));
				break;
			case 134:
				printf("WriteProtect (section 2.4.350)");
				break;
			case 140:
				Country_free(Country_read(bytes));
				break;
			case 141:
				switch(ByteArray_readInt16(bytes))
				{
					case 0:
						printf("SHOWALL 0x0000 ActiveX objects, OLE objects, and drawing objects are displayed in the window that contains the workbook.");
						break;
					case 1:
						printf("HideObj (section 2.4.139)");
						printf("SHOWPLACEHOLDER 0x0001 Placeholders are displayed in place of ActiveX objects, OLE objects, and drawing objects in the window that contains the workbook.");
						break;
					case 2:
						printf("HideObj (section 2.4.139)");
						printf("HIDEALL 0x0002 ActiveX objects, OLE objects, and drawing objects are not displayed in the window that contains the workbook.");
						break;
				}
				break;
			case 144:
				printf("Sort (section 2.4.263)");
				break;
			case 146:
				printf("Palette (section 2.4.188)");
				break;
			case 151:
				printf("Sync (section 2.4.318)");
				break;
			case 152:
				printf("LPr (section 2.4.158)");
				break;
			case 153:
				printf("DxGCol (section 2.4.98)");
				break;
			case 154:
				printf("FnGroupName (section 2.4.120)");
				break;
			case 155:
				printf("FilterMode (section 2.4.119)");
				break;
			case 156:
				printf("BuiltInFnGroupCount (section 2.4.30)");
				break;
			case 157:
				printf("AutoFilterInfo (section 2.4.8)");
				break;
			case 158:
				printf("AutoFilter (section 2.4.6)");
				break;
			case 160:
				printf("Scl (section 2.4.247)");
				break;
			case 161:
				Setup_free(Setup_read(bytes));
				break;
			case 174:
				printf("ScenMan (section 2.4.246)");
				break;
			case 175:
				printf("SCENARIO (section 2.4.244)");
				break;
			case 176:
				printf("SxView (section 2.4.313)");
				break;
			case 177:
				printf("Sxvd (section 2.4.309)");
				break;
			case 178:
				printf("SXVI (section 2.4.312)");
				break;
			case 180:
				printf("SxIvd (section 2.4.292)");
				break;
			case 181:
				printf("SXLI (section 2.4.293)");
				break;
			case 182:
				printf("SXPI (section 2.4.298)");
				break;
			case 184:
				printf("DocRoute (section 2.4.91)");
				break;
			case 185:
				printf("RecipName (section 2.4.216)");
				break;
			case 189:
				printf("MulRk (section 2.4.175)");
				MulRk_free(MulRk_read(bytes));
				break;
			case 190:
				printf("MulBlank (section 2.4.174)");
				MulBlank_free(MulBlank_read(bytes,out));
				break;
			case 193:
				printf("Mms (section 2.4.169)");
				if(ByteArray_readInt16(bytes))
				{
					printf("XXXXXXXXXXXXx");
				}
				break;
			case 197:
				printf("SXDI (section 2.4.278)");
				break;
			case 198:
				printf("SXDB (section 2.4.275)");
				break;
			case 199:
				printf("SXFDB (section 2.4.283)");
				break;
			case 200:
				printf("SXDBB (section 2.4.276)");
				break;
			case 201:
				printf("SXNum (section 2.4.296)");
				break;
			case 202:
				printf("SxBool (section 2.4.274)");
				break;
			case 203:
				printf("SxErr (section 2.4.281)");
				break;
			case 204:
				printf("SXInt (section 2.4.289)");
				break;
			case 205:
				printf("SXString (section 2.4.304)");
				break;
			case 206:
				printf("SXDtr (section 2.4.279)");
				break;
			case 207:
				printf("SxNil (section 2.4.295)");
				break;
			case 208:
				printf("SXTbl (section 2.4.305)");
				break;
			case 209:
				printf("SXTBRGIITM (section 2.4.307)");
				break;
			case 210:
				printf("SxTbpg (section 2.4.306)");
				break;
			case 211:
				printf("ObProj (section 2.4.185)");
				break;
			case 213:
				printf("SXStreamID (section 2.4.303)");
				break;
			case 215:
				DBCell_free(DBCell_read(bytes));
				//return 0;
				break;
			case 216:
				printf("SXRng (section 2.4.300)");
				break;
			case 217:
				printf("SxIsxoper (section 2.4.290)");
				break;
			case 218:
				BookBool_free(BookBool_read(bytes));
				break;
			case 220:
				printf("DbOrParamQry (section 2.4.79)");
				break;
			case 221:
				printf("ScenarioProtect (section 2.4.245)");
				break;
			case 222:
				printf("OleObjectSize (section 2.4.187)");
				break;
			case 224:
				XF_free(XF_read(bytes));
				break;
			case 225:
				printf("InterfaceHdr (section 2.4.146)");
				/*
				   int codepage;
				   codepage = ByteArray_readInt16(bytes);
				   printf("codepage==0x%x,0x04b0,%d\n",codepage,codepage);
				   */
				break;
			case 226:
				printf("InterfaceEnd (section 2.4.145)");
				break;
			case 227:
				printf("SXVS (section 2.4.317)");
				break;
			case 229:
				printf("MergeCells (section 2.4.168)");
				break;
			case 233:
				printf("BkHim (section 2.4.19)");
				break;
			case 235:
				printf("MsoDrawingGroup (section 2.4.171)");
				break;
			case 236:
				printf("MsoDrawing (section 2.4.170)");
				break;
			case 237:
				printf("MsoDrawingSelection (section 2.4.172)");
				break;
			case 239:
				printf("PhoneticInfo (section 2.4.192)");
				break;
			case 240:
				printf("SxRule (section 2.4.301)");
				break;
			case 241:
				printf("SXEx (section 2.4.282)");
				break;
			case 242:
				printf("SxFilt (section 2.4.285)");
				break;
			case 244:
				printf("SxDXF (section 2.4.280)");
				break;
			case 245:
				printf("SxItm (section 2.4.291)");
				break;
			case 246:
				printf("SxName (section 2.4.294)");
				break;
			case 247:
				printf("SxSelect (section 2.4.302)");
				break;
			case 248:
				printf("SXPair (section 2.4.297)");
				break;
			case 249:
				printf("SxFmla (section 2.4.286)");
				break;
			case 251:
				printf("SxFormat (section 2.4.287)");
				break;
			case 252:
				SST_free(SST_read(bytes,file));
				//return 0;
				break;
			case 253:
				LabelSst_free(LabelSst_read(bytes,file,out));
				break;
			case 255:
				printf("ExtSST (section 2.4.107)");
				ExtSST_free(ExtSST_read(bytes));
				break;
			case 256:
				printf("SXVDEx (section 2.4.310)");
				break;
			case 259:
				printf("SXFormula (section 2.4.288)");
				break;
			case 290:
				printf("SXDBEx (section 2.4.277)");
				break;
			case 311:
				printf("RRDInsDel (section 2.4.228)");
				break;
			case 312:
				printf("RRDHead (section 2.4.226)");
				break;
			case 315:
				printf("RRDChgCell (section 2.4.223)");
				break;
			case 317:
				printf("RRTabId (section 2.4.241)");
				while(bytes->position<bytes->length)
				{
					printf("Table Id :%d,",ByteArray_readInt16(bytes));
				}
				break;
			case 318:
				printf("RRDRenSheet (section 2.4.234)");
				break;
			case 319:
				printf("RRSort (section 2.4.240)");
				break;
			case 320:
				printf("RRDMove (section 2.4.231)");
				break;
			case 330:
				printf("RRFormat (section 2.4.238)");
				break;
			case 331:
				printf("RRAutoFmt (section 2.4.222)");
				break;
			case 333:
				printf("RRInsertSh (section 2.4.239)");
				break;
			case 334:
				printf("RRDMoveBegin (section 2.4.232)");
				break;
			case 335:
				printf("RRDMoveEnd (section 2.4.233)");
				break;
			case 336:
				printf("RRDInsDelBegin (section 2.4.229)");
				break;
			case 337:
				printf("RRDInsDelEnd (section 2.4.230)");
				break;
			case 338:
				printf("RRDConflict (section 2.4.224)");
				break;
			case 339:
				printf("RRDDefName (section 2.4.225)");
				break;
			case 340:
				printf("RRDRstEtxp (section 2.4.235)");
				break;
			case 351:
				printf("LRng (section 2.4.159)");
				break;
			case 352:
				if(ByteArray_readInt16(bytes))
					printf("UsesELFs (section 2.4.337)");

				break;
			case 353:
				if(ByteArray_readInt16(bytes))
				{
					printf("DSF (section 2.4.94) XXXXXXXXXXXXx");
					return 0;
				}
				break;
			case 401:
				printf("CUsr (section 2.4.72)");
				break;
			case 402:
				printf("CbUsr (section 2.4.40)");
				break;
			case 403:
				printf("UsrInfo (section 2.4.340)");
				break;
			case 404:
				printf("UsrExcl (section 2.4.339)");
				break;
			case 405:
				printf("FileLock (section 2.4.116)");
				break;
			case 406:
				printf("RRDInfo (section 2.4.227)");
				break;
			case 407:
				printf("BCUsrs (section 2.4.16)");
				break;
			case 408:
				printf("UsrChk (section 2.4.338)");
				break;
			case 425:
				printf("UserBView (section 2.4.333)");
				break;
				/*
				   case 426:
				   printf("UserSViewBegin (section 2.4.334)");
				   break;
				   */
			case 426:
				printf("UserSViewBegin_Chart (section 2.4.335)");
				break;
			case 427:
				printf("UserSViewEnd (section 2.4.336)");
				break;
			case 428:
				printf("RRDUserView (section 2.4.237)");
				break;
			case 429:
				printf("Qsi (section 2.4.208)");
				break;
			case 430:
				printf("SupBook (section 2.4.271)");
				break;
			case 431:
				//if(ByteArray_readInt16(bytes))
				printf("Prot4Rev (section 2.4.205)");
				break;
			case 432:
				printf("CondFmt (section 2.4.56)");
				break;
			case 433:
				printf("CF (section 2.4.42)");
				break;
			case 434:
				printf("DVal (section 2.4.96)");
				break;
			case 437:
				printf("DConBin (section 2.4.83)");
				break;
			case 438:
				printf("TxO (section 2.4.329)");
				break;
			case 439:
				printf("RefreshAll (section 2.4.217)");
				if(ByteArray_readInt16(bytes))
					printf("Force refresh of external data ranges, PivotTables and XML maps on workbook load.");
				break;
			case 440:
				HLink_read(bytes);
				break;
			case 441:
				printf("Lel (section 2.4.154)");
				break;
			case 442:
				printf("CodeName (section 2.4.51)");
				break;
			case 443:
				printf("SXFDBType (section 2.4.284)");
				break;
			case 444:
				//if(ByteArray_readInt16(bytes))
				printf("Prot4RevPass (section 2.4.206)");//protPwdRev (2 bytes): An unsigned integer that specifies the password verifier that is required to change the value of the fRevLock field of the Prot2.4.205Rev record that immediately precedes this record<104>. The algorithm to generate the password verifier is documented in the password verifier algorithm. If the value is 0, there is no password. MUST be 0 if the fRevLock field of Prot2.4.205Rev is 0x0000.
				break;
			case 445:
				printf("ObNoMacros (section 2.4.184)");
				break;
			case 446:
				printf("Dv (section 2.4.95)");
				break;
			case 448:
				printf("Excel9File (section 2.4.104)");
				break;
			case 449:
				printf("RecalcId (section 2.4.215)");
				break;
			case 450:
				printf("EntExU2 (section 2.4.102)");
				break;
			case 512:
				printf("Dimensions (section 2.4.90)");
				Dimensions_free(Dimensions_read(bytes));
				//return 0;
				break;
			case 513:
				printf("Blank (section 2.4.20)");
				break;
			case 515:
				Number_free(Number_read(bytes,out));
				break;
			case 516:
				printf("Label (section 2.4.148)");
				break;
			case 517:
				printf("BoolErr (section 2.4.24)");
				break;
			case 519:
				printf("String (section 2.4.268)");
				break;
			case 520:
				Row_free(Row_read(bytes));
				//sprintf(out+strlen(out),"\n");
				break;
			case 523:
				Index_free(Index_read(bytes));
				break;
			case 545:
				printf("Array (section 2.4.4)");
				break;
			case 549:
				printf("DefaultRowHeight (section 2.4.87)");
				break;
			case 566:
				printf("Table (section 2.4.319)");
				break;
			case 574:
				printf("Window2 (section 2.4.346)");
				break;
			case 638:
				RK_free(RK_read(bytes,file,out));
				break;
			case 659:
				Style_free(Style_read(bytes));
				break;
			case 1048:
				printf("BigName (section 2.4.18)");
				break;
			case 1054:
				printf("Format (section 2.4.126)");
				Format_free(Format_read(bytes));
				break;
			case 1084:
				printf("ContinueBigName (section 2.4.59)");
				break;
			case 1212:
				printf("ShrFmla (section 2.4.260)");
				break;
			case 2048:
				printf("HLinkTooltip (section 2.4.141)");
				break;
			case 2049:
				printf("WebPub (section 2.4.344)");
				break;
			case 2050:
				printf("QsiSXTag (section 2.4.211)");
				break;
			case 2051:
				printf("DBQueryExt (section 2.4.81)");
				break;
			case 2052:
				printf("ExtString (section 2.4.108)");
				break;
			case 2053:
				printf("TxtQry (section 2.4.330)");
				break;
			case 2054:
				printf("Qsir (section 2.4.210)");
				break;
			case 2055:
				printf("Qsif (section 2.4.209)");
				break;
			case 2056:
				printf("RRDTQSIF (section 2.4.236)");
				break;
			case 2057:
				BOF_free(BOF_read(bytes));
				sprintf(out+strlen(out),"\n");
				break;
			case 2058:
				printf("OleDbConn (section 2.4.186)");
				break;
			case 2059:
				printf("WOpt (section 2.4.348)");
				break;
			case 2060:
				printf("SXViewEx (section 2.4.314)");
				break;
			case 2061:
				printf("SXTH (section 2.4.308)");
				break;
			case 2062:
				printf("SXPIEx (section 2.4.299)");
				break;
			case 2063:
				printf("SXVDTEx (section 2.4.311)");
				break;
			case 2064:
				printf("SXViewEx9 (section 2.4.315)");
				break;
			case 2066:
				printf("ContinueFrt (section 2.4.60)");
				break;
			case 2067:
				printf("RealTimeData (section 2.4.214)");
				break;
			case 2128:
				printf("ChartFrtInfo (section 2.4.49)");
				break;
			case 2129:
				printf("FrtWrapper (section 2.4.130)");
				break;
			case 2130:
				printf("StartBlock (section 2.4.266)");
				break;
			case 2131:
				printf("EndBlock (section 2.4.100)");
				break;
			case 2132:
				printf("StartObject (section 2.4.267)");
				break;
			case 2133:
				printf("EndObject (section 2.4.101)");
				break;
			case 2134:
				printf("CatLab (section 2.4.38)");
				break;
			case 2135:
				printf("YMult (section 2.4.356)");
				break;
			case 2136:
				printf("SXViewLink (section 2.4.316)");
				break;
			case 2137:
				printf("PivotChartBits (section 2.4.196)");
				break;
			case 2138:
				printf("FrtFontList (section 2.4.129)");
				break;
			case 2146:
				printf("SheetExt (section 2.4.259)");
				break;
			case 2147:
				printf("BookExt (section 2.4.23)");
				break;
			case 2148:
				printf("SXAddl (section 2.4.273.2)");
				break;
			case 2149:
				printf("CrErr (section 2.4.64)");
				break;
			case 2150:
				printf("HFPicture (section 2.4.138)");
				break;
			case 2151:
				printf("FeatHdr (section 2.4.112)");
				break;
			case 2152:
				printf("Feat (section 2.4.111)");
				break;
			case 2154:
				printf("DataLabExt (section 2.4.75)");
				break;
			case 2155:
				printf("DataLabExtContents (section 2.4.76)");
				break;
			case 2156:
				printf("CellWatch (section 2.4.41)");
				break;
			case 2161:
				printf("FeatHdr11 (section 2.4.113)");
				break;
			case 2162:
				printf("Feature11 (section 2.4.114)");
				break;
			case 2164:
				printf("DropDownObjIds (section 2.4.93)");
				break;
			case 2165:
				printf("ContinueFrt11 (section 2.4.61)");
				break;
			case 2166:
				printf("DConn (section 2.4.84)");
				break;
			case 2167:
				printf("List12 (section 2.4.157)");
				break;
			case 2168:
				printf("Feature12 (section 2.4.115)");
				break;
			case 2169:
				printf("CondFmt12 (section 2.4.57)");
				break;
			case 2170:
				printf("CF12 (section 2.4.43)");
				break;
			case 2171:
				printf("CFEx (section 2.4.44)");
				break;
			case 2172:
				printf("XFCRC (section 2.4.354)");
				break;
			case 2173:
				printf("XFExt (section 2.4.355)");
				break;
			case 2174:
				printf("AutoFilter12 (section 2.4.7)");
				break;
			case 2175:
				printf("ContinueFrt12 (section 2.4.62)");
				break;
			case 2180:
				printf("MDTInfo (section 2.4.162)");
				break;
			case 2181:
				printf("MDXStr (section 2.4.166)");
				break;
			case 2182:
				printf("MDXTuple (section 2.4.167)");
				break;
			case 2183:
				printf("MDXSet (section 2.4.165)");
				break;
			case 2184:
				printf("MDXProp (section 2.4.164)");
				break;
			case 2185:
				printf("MDXKPI (section 2.4.163)");
				break;
			case 2186:
				printf("MDB (section 2.4.161)");
				break;
			case 2187:
				printf("PLV (section 2.4.200)");
				break;
			case 2188:
				printf("Compat12 (section 2.4.54)");
				break;
			case 2189:
				printf("DXF (section 2.4.97)");
				break;
			case 2190:
				printf("TableStyles (section 2.4.322)");
				break;
			case 2191:
				printf("TableStyle (section 2.4.320)");
				break;
			case 2192:
				printf("TableStyleElement (section 2.4.321)");
				break;
			case 2194:
				printf("StyleExt (section 2.4.270)");
				break;
			case 2195:
				printf("NamePublish (section 2.4.178)");
				break;
			case 2196:
				printf("NameCmt (section 2.4.176)");
				break;
			case 2197:
				printf("SortData (section 2.4.264)");
				break;
			case 2198:
				printf("Theme (section 2.4.326)");
				break;
			case 2199:
				printf("GUIDTypeLib (section 2.4.133)");
				break;
			case 2200:
				printf("FnGrp12 (section 2.4.121)");
				break;
			case 2201:
				printf("NameFnGrp12 (section 2.4.177)");
				break;
			case 2202:
				MTRSettings_free(MTRSettings_read(bytes));
				break;
			case 2203:
				printf("CompressPictures (section 2.4.55)");
				break;
			case 2204:
				printf("HeaderFooter (section 2.4.137)");
				HeaderFooter_free(HeaderFooter_read(bytes));
				break;
			case 2205:
				printf("CrtLayout12 (section 2.4.66)");
				break;
			case 2206:
				printf("CrtMlFrt (section 2.4.70)");
				break;
			case 2207:
				printf("CrtMlFrtContinue (section 2.4.71)");
				break;
			case 2211:
				printf("ForceFullCalculation (section 2.4.125)");
				break;
			case 2212:
				printf("ShapePropsStream (section 2.4.258)");
				break;
			case 2213:
				printf("TextPropsStream (section 2.4.325)");
				break;
			case 2214:
				printf("RichTextStream (section 2.4.218)");
				break;
			case 2215:
				printf("CrtLayout12A (section 2.4.67)");
				break;
			case 4097:
				printf("Units (section 2.4.332)");
				break;
			case 4098:
				printf("Chart (section 2.4.45)");
				break;
			case 4099:
				printf("Series (section 2.4.252)");
				break;
			case 4102:
				printf("DataFormat (section 2.4.74)");
				break;
			case 4103:
				printf("LineFormat (section 2.4.156)");
				break;
			case 4105:
				printf("MarkerFormat (section 2.4.160)");
				break;
			case 4106:
				printf("AreaFormat (section 2.4.3)");
				break;
			case 4107:
				printf("PieFormat (section 2.4.195)");
				break;
			case 4108:
				printf("AttachedLabel (section 2.4.5)");
				break;
			case 4109:
				printf("SeriesText (section 2.4.254)");
				break;
			case 4116:
				printf("ChartFormat (section 2.4.48)");
				break;
			case 4117:
				printf("Legend (section 2.4.152)");
				break;
			case 4118:
				printf("SeriesList (section 2.4.253)");
				break;
			case 4119:
				printf("Bar (section 2.4.15)");
				break;
			case 4120:
				printf("Line (section 2.4.155)");
				break;
			case 4121:
				printf("Pie (section 2.4.194)");
				break;
			case 4122:
				printf("Area (section 2.4.2)");
				break;
			case 4123:
				printf("Scatter (section 2.4.243)");
				break;
			case 4124:
				printf("CrtLine (section 2.4.68)");
				break;
			case 4125:
				printf("Axis (section 2.4.11)");
				break;
			case 4126:
				printf("Tick (section 2.4.327)");
				break;
			case 4127:
				printf("ValueRange (section 2.4.341)");
				break;
			case 4128:
				printf("CatSerRange (section 2.4.39)");
				break;
			case 4129:
				printf("AxisLine (section 2.4.12)");
				break;
			case 4130:
				printf("CrtLink (section 2.4.69)");
				break;
			case 4132:
				printf("DefaultText (section 2.4.88)");
				break;
			case 4133:
				printf("Text (section 2.4.324)");
				break;
			case 4134:
				printf("FontX (section 2.4.123)");
				break;
			case 4135:
				printf("ObjectLink (section 2.4.182)");
				break;
			case 4146:
				printf("Frame (section 2.4.128)");
				break;
			case 4147:
				printf("Begin (section 2.4.17)");
				break;
			case 4148:
				printf("End (section 2.4.99)");
				break;
			case 4149:
				printf("PlotArea (section 2.4.197)");
				break;
			case 4154:
				printf("Chart3d (section 2.4.46)");
				break;
			case 4156:
				printf("PicF (section 2.4.193)");
				break;
			case 4157:
				printf("DropBar (section 2.4.92)");
				break;
			case 4158:
				printf("Radar (section 2.4.212)");
				break;
			case 4159:
				printf("Surf (section 2.4.272)");
				break;
			case 4160:
				printf("RadarArea (section 2.4.213)");
				break;
			case 4161:
				printf("AxisParent (section 2.4.13)");
				break;
			case 4163:
				printf("LegendException section 2.4.153()");
				break;
			case 4164:
				printf("ShtProps (section 2.4.261)");
				break;
			case 4165:
				printf("SerToCrt (section 2.4.256)");
				break;
			case 4166:
				printf("AxesUsed (section 2.4.10)");
				break;
			case 4168:
				printf("SBaseRef (section 2.4.242)");
				break;
			case 4170:
				printf("SerParent (section 2.4.255)");
				break;
			case 4171:
				printf("SerAuxTrend (section 2.4.250)");
				break;
			case 4174:
				printf("IFmtRecord (section 2.4.143)");
				break;
			case 4175:
				printf("Pos (section 2.4.201)");
				break;
			case 4176:
				printf("AlRuns (section 2.4.1)");
				break;
			case 4177:
				printf("BRAI (section 2.4.29)");
				break;
			case 4187:
				printf("SerAuxErrBar (section 2.4.249)");
				break;
			case 4188:
				printf("ClrtClient (section 2.4.50)");
				break;
			case 4189:
				printf("SerFmt (section 2.4.251)");
				break;
			case 4191:
				printf("Chart3DBarShape (section 2.4.47)");
				break;
			case 4192:
				printf("Fbi (section 2.4.109)");
				break;
			case 4193:
				printf("BopPop (section 2.4.25)");
				break;
			case 4194:
				printf("AxcExt (section 2.4.9)");
				break;
			case 4195:
				printf("Dat (section 2.4.73)");
				break;
			case 4196:
				printf("PlotGrowth (section 2.4.198)");
				break;
			case 4197:
				printf("SIIndex (section 2.4.262)");
				break;
			case 4198:
				printf("GelFrame (section 2.4.131)");
				break;
			case 4199:
				printf("BopPopCustom (section 2.4.26)");
				break;
			case 4200:
				printf("Fbi2 (section 2.4.110)");
				break;
		}
		printf("\t");
		//printf("\n");

		start += 4 + first.size;

		++i;
		//if(i>28) break;
		//if(i>58) break;
		//if(i>88) break;
		//if(i>134) break;
		//if(i>180) break;
		//if(i>153) break;
		//if(i>107) break;
		//if(i>200) break;
		//if(i>142) break;
	}
	return file;
}
void XlsFile_free(XlsFile * file)
{
	if(file)
	{
		if(file->header)
			Ole_free(file->header);
		if(file->aSST)
			free(file->aSST);

		free(file);
	}
}
#ifdef debug_xls
int main()
{
	char * filename = "xls.xls";
	FILE * _file = fopen(filename,"rb");
	int fileLen = fseek(_file,0,SEEK_END);
	fileLen = ftell(_file);
	rewind(_file);
	printf("%s,%d\n",filename,fileLen);

	ByteArray * bytearray = ByteArray_new(fileLen);
	fread(bytearray->data,1,fileLen,_file);
	fclose(_file);

	char * out = malloc(fileLen);
	memset(out,0,fileLen);
	XlsFile * file = XlsFile_parse(bytearray,out);
	printf("%s\n",out);
	XlsFile_free(file);
	return 0;
}
#endif


#ifdef as_api
#include "AS3.h"

//Method exposed to ActionScript
//Takes a String and echos it
static AS3_Val echo(void* self, AS3_Val args)
{
	AS3_Val byteArray;
	AS3_ArrayValue( args, "AS3ValType", &byteArray);
	AS3_Val length = AS3_GetS(byteArray, "length");  

	sztrace("length getted!");
	int len = AS3_IntValue(length);
	//if(len>0) return length;

	char *data=NULL;
	data=malloc(len);
	memset(data,0,len);
	int fileLen = AS3_ByteArray_readBytes(data,byteArray, len);
	char *out= NULL;
	out = malloc(fileLen);
	memset(out,0,fileLen);



	ByteArray * bytearray = ByteArray_new(fileLen);
	bytearray->data = data;

	XlsFile_free(XlsFile_parse(bytearray,out));
	ByteArray_free(bytearray);

	return AS3_String(out);
}

int main()
{
	//define the methods exposed to ActionScript
	//typed as an ActionScript Function instance
	AS3_Val echoMethod = AS3_Function( NULL, echo );

	// construct an object that holds references to the functions
	AS3_Val result = AS3_Object( "echo: AS3ValType", echoMethod );

	// Release
	AS3_Release( echoMethod );

	// notify that we initialized -- THIS DOES NOT RETURN!
	AS3_LibInit( result );

	// should never get here!
	return 0;
}
#endif

