#ifndef xls_h
#define xls_h

#include "bytearray.h"
#include "ole.h"

typedef struct BOF
{//The BOF record specifies the beginning of the individual substreams as specified by the workbook section. It also specifies history information for the substreams.
	unsigned short vers;//vers (2 bytes): An unsigned integer that specifies the BIFF version of the file. The value MUST be 0x0600.
	unsigned short dt;// (2 bytes): An unsigned integer that specifies the document type of the substream of records following this record. For more information about the layout of the sub-streams in the workbook stream see File Structure. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0005 Specifies the workbook substream.
	   0x0010 Specifies the dialog sheet substream or the worksheet substream.  The sheet (1) substream that starts with this BOF record MUST contain one WsBool record. If the fDialog field in that WsBool is 1 then the sheet (1) is dialog sheet otherwise the sheet (1) is a worksheet.
	   0x0020 Specifies the chart sheet substream.
	   0x0040 Specifies the macro sheet substream.
	   */
	unsigned short rupBuild;// (2 bytes): An unsigned integer that specifies the build identifier.
	unsigned short rupYear;// (2 bytes): An unsigned integer that specifies the year when this BIFF version was first created. The value MUST be 0x07CC<27> or 0x07CD.

	unsigned int A:1;// - fWin (1 bit): A bit that specifies whether this file was last edited on a Windows platform. The value MUST be 1.
	unsigned int B:1;// - fRisc (1 bit): A bit that specifies whether the file was last edited on a RISC platform. The value MUST be 0.
	unsigned int C:1;// - fBeta (1 bit): A bit that specifies whether this file was last edited by a beta version of the application. The value MUST be 0.
	unsigned int D:1;// - fWinAny (1 bit): A bit that specifies whether this file has ever been edited on a Windows platform. The value SHOULD<28> be 1.
	unsigned int E:1;// - fMacAny (1 bit): A bit that specifies whether this file has ever been edited on a Macintosh platform. The value MUST be 0.
	unsigned int F:1;//- fBetaAny (1 bit): A bit that specifies whether this file has ever been edited by a beta version of the application. The value MUST be 0.
	unsigned int G:2;//- unused1 (2 bits): Undefined and MUST be ignored.

	unsigned int H:1;//- fRiscAny (1 bit): A bit that specifies whether this file has ever been edited on a RISC platform.  The value MUST be 0.
	unsigned int I:1;//- fOOM (1 bit): A bit that specifies whether this file had an out-of-memory failure.
	unsigned int J:1;//- fGlJmp (1 bit): A bit that specifies whether this file had an out-of-memory failure during rendering.
	unsigned int K:2;//- unused2 (2 bits): Undefined, and MUST be ignored.
	unsigned int L:1;//- fFontLimit (1 bit): A bit that specified that whether this file hit the 255 font limit<29>.
	unsigned int M:4;//- verXLHigh (4 bits): An unsigned integer that specifies the highest version of the application that once saved this file. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 Specifies the highest version of the application that has ever saved this file. <30>
	   0x1 Specifies the highest version of the application that has ever saved this file. <31>
	   0x2 Specifies the highest version of the application that has ever saved this file. <32>
	   0x3 Specifies the highest version of the application that has ever saved this file. <33>
	   0x4 Specifies the highest version of the application that has ever saved this file. <34>
	   0x6 Specifies the highest version of the application that has ever saved this file. <35>
	   0x7 Specifies the highest version of the application that has ever saved this file. <36>
	   */
	unsigned int N:1;//- unused3 (1 bit): Undefined, and MUST be ignored.
	unsigned int reserved1:13;//(13 bits): MUST be zero, and MUST be ignored.

	unsigned int verLowestBiff:8;//(8 bits): An unsigned integer that specifies the BIFF version saved. The value MUST be 6.
	unsigned int O:4;//- verLastXLSaved (4 bits): An unsigned integer that specifies the application that saved this file most recently. The value MUST be the value of field verXLHigh or less. MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 Specifies the highest version of the application that has ever saved this file. <37>
	   0x1 Specifies the highest version of the application that has ever saved this file.<38>
	   0x2 Specifies the highest version of the application that has ever saved this file.<39>
	   0x3 Specifies the highest version of the application that has ever saved this file. <40>
	   0x4 Specifies the highest version of the application that has ever saved this file.<41>
	   0x6 Specifies the highest version of the application that has ever saved this file.<42>
	   0x7 Specifies the highest version of the application that has ever saved this file.<43>
	   */
	unsigned int reserved2:20;//(20 bits): MUST be zero, and MUST be ignored.
}BOF;

typedef struct XLUnicodeString
{//The XLUnicodeString structure specifies a Unicode string.
	unsigned short cch;// (2 bytes): An unsigned integer that specifies the count of characters in the string.
	unsigned char A:1;// - fHighByte (1 bit): A bit that specifies whether the characters in rgb are double-byte characters.  MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 All the characters in the string have a high byte of 0x00 and only the low bytes are in
	   rgb.
	   0x1 All the characters in the string are saved as double-byte characters in rgb.
	   */
	unsigned char reserved:7;// (7 bits): MUST be zero, and MUST be ignored.
	char *rgb;// (variable): An array of bytes that specifies the characters. If fHighByte is 0x0, the size of the array MUST be equal to cch. If fHighByte is 0x1, the size of the array MUST be equal to cch*2.
}XLUnicodeString;

typedef struct ShortXLUnicodeString//The ShortXLUnicodeString structure specifies a Unicode string.
{
	unsigned char cch;//(1 bytes): An unsigned integer that specifies the count of characters in the string.
	unsigned char A:1;// fHighByte (1 bit): A bit that specifies whether the characters in rgb are double-byte characters.  MUST be a value from the following table:
	/*
	   Value Meaning
	   0x0 All the characters in the string have a high byte of 0x00 and only the low bytes are in rgb.
	   0x1 All the characters in the string are saved as double-byte characters in rgb.
	   */
	unsigned char reserved:7;//(7 bits): MUST be zero, and MUST be ignored.
	unsigned char *rgb;//(variable): An array of bytes that specifies the characters. If fHighByte is 0x0, the size of the array MUST be equal to the value of cch. If fHighByte is 0x1, the size of the array MUST be equal to the value of cch*2.
}ShortXLUnicodeString;
typedef struct XlsFile{
	ByteArray *bytearray;
	OleHeader *header;

	char ** aSST;
	int dateFrom1900;
}XlsFile;



XLUnicodeString * XLUnicodeString_read(ByteArray* bytearray);
void XLUnicodeString_free(XLUnicodeString * s);
ShortXLUnicodeString * ShortXLUnicodeString_read(ByteArray* bytearray);
void ShortXLUnicodeString_free(ShortXLUnicodeString * s);
#endif

