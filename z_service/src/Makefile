# RaspberryPi-ProjZ Server                #
#                                         #
#    499750058 廖重淯                     #
#                                         #

CC = gcc
CFLAGS  = -Wall -O2
LDLAGS  = -lpthread

TARGET = z_server

.PHONY: clean

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)