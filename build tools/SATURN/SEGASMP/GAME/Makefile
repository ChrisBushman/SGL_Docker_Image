#
#■この makefile の利用手引き
# 1. このファイルをサンプルのディレクトリにコピーしてください。
# 2. ディレクトリの設定を調整してください。
#    マクロ GCC, SEGALIB, SEGASMP を定義してください。
# 3. コンフィギュレーションファイル sample.cfg を作成してください。
#    マクロ TARGET と OBJS を定義してください。
# 4. make を実行してサンプルプログラムを作成してください。
#

#
#■このファイルで使われているマクロの説明
#  CC           Ｃコンパイラを指定します。（実行可能な形で記述してください）
#  CFLAGS       Ｃコンパイラのオプションを指定します。
#  AS           アセンブラを指定します。（実行可能な形で記述してください）
#  ASFLAGS      アセンブラのオプションを指定します。
#  LDFLAGS      リンカのオプションを指定します。
#  SATURN       サターン用のホームディレクトリを指定します。
#  GCC          ＧＮＵのあるディレクトリを指定します。
#  SEGASMP      ＳＢＬのサンプルのあるディレクトリを指定します。
#  SEGALIB      ＳＢＬのライブラリのあるディレクトリを指定します。
#  LOCATE_FILE  メモリのロケーションを記述したファイルです。
#  ENTRY_POINT  サンプルプログラムの実行開始アドレスを指定します。
#  LIBS         リンクするライブラリを指定します。
#                (segasmp/lib/libs.lst にて定義します)
#
#  （コンフィギュレーションファイル sample.cfg の中で定義します）
#  TARGET       サンプルプログラムの名前を指定します。
#               （ファイルのプライマリネームになります）
#  SYS_OBJS     リンクするオブジェクトファイルを指定します。
#  OBJS         リンクするオブジェクトファイルを指定します。
#

CC = gcc
AS = snasmsh2
CV = objcopy
RM = deltree

# option
CFLAGS  = -g -O2 -Wall -m2 -DMODEL_S -I$(CYGNUS)/include -I$(SEGALIB)/include -I$(SEGASMP)/per/smpclib
ASFLAGS = /l
LDFLAGS = -T $(LOCATE_FILE) -e $(ENTRY_POINT) -nostartfiles
CVFLAGS = -O binary
RMFLAGS = -Y

# directory
SATURN  = ../..
CYGNUS  = ../../../CYGNUS
GCC     = $(CYGNUS)/bin
SEGASMP = $(SATURN)/segasmp
SEGALIB = $(SATURN)/segalib

LOCATE_FILE = $(SEGASMP)/lib/saturn.lnk
ENTRY_POINT = START
CONFIG_FILE = sample.cfg
LIBS_FILE = $(SEGASMP)/lib/libs.lst

include	$(CONFIG_FILE)
include	$(LIBS_FILE)

.SUFFIXES:
.SUFFIXES: .cof .o .src .c

all: $(TARGET).cof $(TARGET).bin

$(TARGET).cof: $(SYS_OBJS) $(OBJS)
	$(CC) $(LDFLAGS) $(_LDFLAGS) -o $(TARGET).cof -Xlinker -Map -Xlinker $(TARGET).map $(SYS_OBJS) $(OBJS) $(LIBS)

$(TARGET).bin: $(TARGET).cof
	$(CV) $(CVFLAGS) $(TARGET).cof $(TARGET).bin

%.o: %.c
	$(CC) -c $(CFLAGS) $(_CFLAGS) -o $@ $<

%.o: %.src
	$(AS) $(ASFLAGS) $(_ASFLAGS) $<,$@

clean CLEAN:
	$(RM) $(RMFLAGS) $(OBJS) $(TARGET).*

