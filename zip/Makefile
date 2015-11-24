# source /home/libiao/alchemy-ubuntu-v0.5a/alchemy-setup && alc-on
ALCHEMY_HOME:=/home/libiao/alchemy-ubuntu-v0.5a
FLEX_HOME:=/home/libiao/flex_sdk_4.5
FLASHPLAYER:=$(FLEX_HOME)/bin/flashplayerdebugger
MXMLC:=$(FLEX_HOME)/bin/mxmlc
SWF:=test.swf
AS3_PATH:=.
AS3:=$(AS3_PATH)/test.as
SWC:=z.swc
OBJS:=zip.o bytearray.o inflate.o crc32.o uncompr.o adler32.o inffast.o inftrees.o zutil.o
CFLAGS:=-D as_api -O3 -Wall
CC:=$(ALCHEMY_HOME)/achacks/gcc

all: $(SWF)
	$(FLASHPLAYER) $(SWF)
$(SWF): $(SWC) $(AS3)
	$(MXMLC) -source-path=$(AS3_PATH) -library-path+=$(SWC) $(AS3) -o $(SWF)
$(SWC): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -swc -o $(SWC)
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.swc *.o *.swf a.out
