# CMPT 434
# ASSIGNMENT 1
# KODY MANASTYRSKI
# KOM607
# 11223681
#

CC = gcc
CPPFLAGS = -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L -c

EXE = client server proxy
DOTO = client.o server.o proxy.o common.o

all: common.o server client proxy 

common.o: common.c
	$(CC) $(CPPFLAGS) common.c -o common.o

client: client.o common.o
	$(CC) client.o common.o -o client

client.o: client.c
	$(CC) $(CPPFLAGS) client.c -o client.o

proxy: proxy.o
	$(CC) proxy.o common.o -o proxy

proxy.o: proxy.c
	$(CC) $(CPPFLAGS) proxy.c -o proxy.o

server: server.o common.o
	$(CC) server.o common.o -o server

server.o: server.c
	$(CC) $(CPPFLAGS) server.c -o server.o

.PHONY: clean

clean: 
	rm -f $(EXE) $(DOTO)
