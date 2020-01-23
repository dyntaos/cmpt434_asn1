###################################################
##            CMPT 434 - Assignment 1            ##
##          University of Saskatchewan           ##
##                     2020                      ##
##-----------------------------------------------##
##                  Kale Yuzik                   ##
##                kay851@usask.ca                ##
##      NSID: kay851     Student #11071571       ##
###################################################



CC = gcc
CFLAGS =
CPPFLAGS = -Wall -Wextra -pedantic -g
LDFLAGS =

ARCH = $(shell uname -s)$(shell uname -m)

BUILD = ./build
BIN = $(BUILD)/bin/$(ARCH)
OBJ = $(BUILD)/obj/$(ARCH)
LIB = $(BUILD)/lib/$(ARCH)

.PHONY: all mkdirs clean

all: mkdirs $(BIN)/kv_bintree_test $(BIN)/tcp_client $(BIN)/tcp_server

mkdirs:
	mkdir -p $(BIN) $(OBJ) $(LIB)

clean:
	rm -rf ./build ./kv_bintree_test ./tcp_client

$(OBJ)/kv_bintree.o: kv_bintree.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(OBJ)/kv_bintree_test.o: kv_bintree_test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(BIN)/kv_bintree_test: $(OBJ)/kv_bintree.o $(OBJ)/kv_bintree_test.o
	$(CC) -o $@ $^
	ln -fs $@ ./kv_bintree_test


$(OBJ)/tcp_client.o: tcp_client.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(OBJ)/read_command.o: read_command.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(OBJ)/kv_packet.o: kv_packet.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(OBJ)/kv_server_packet.o: kv_server_packet.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(BIN)/tcp_client: $(OBJ)/read_command.o $(OBJ)/tcp_client.o $(OBJ)/kv_packet.o
	$(CC) -o $@ $^ -lreadline
	ln -fs $@ ./tcp_client

$(OBJ)/tcp_server.o: tcp_server.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(BIN)/tcp_server: $(OBJ)/tcp_server.o $(OBJ)/kv_packet.o $(OBJ)/kv_server_packet.o $(OBJ)/kv_bintree.o
	$(CC) -o $@ $^
	ln -fs $@ ./tcp_server