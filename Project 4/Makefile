SNIPER_BASE_DIR ?= home/orestis/Downloads/sniper-6.1
SNIPER_INCLUDE_DIR = $(SNIPER_BASE_DIR)/include

CC = gcc

TARGET = locks 

LFLAG ?= -DMUTEX ## or -DTAS_CAS or -DTAS_TS or -DTTAS_CAS or DTTAS_TS
IMPLFLAG ?= -DSNIPER ## or -DREAL 

CFLAGS ?= -Wall -O0 -lpthread $(IMPLFLAG) $(LFLAG) #-DDEBUG
CFLAGS += -I$(SNIPER_INCLUDE_DIR)
CFLAGS += -Wno-unused-variable

$(TARGET): locks_scalability.c
	$(CC) $^ $(HOOKS_LDFLAGS) $(CFLAGS) -o $@

clean :
	rm -f $(TARGET)
