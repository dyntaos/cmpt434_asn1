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
CPPLAGS = -Wall -Wextra -pedantic -g
LDFLAGS =

ARCH = $(shell uname -s)$(shell uname -m)

BUILD = ./build
BIN = $(BUILD)/bin/$(ARCH)
OBJ = $(BUILD)/obj/$(ARCH)
LIB = $(BUILD)/lib/$(ARCH)

.PHONY: all mkdirs clean

all: mkdirs $(BIN)/kv_bintree_test

mkdirs:
	mkdir -p $(BIN) $(OBJ) $(LIB)

clean:
	rm -rf ./build ./kv_bintree_test

$(OBJ)/kv_bintree.o: kv_bintree.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(OBJ)/kv_bintree_test.o: kv_bintree_test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -c -o $@ $<

$(BIN)/kv_bintree_test: $(OBJ)/kv_bintree.o $(OBJ)/kv_bintree_test.o
	$(CC) -o $@ $^
	ln -fs $@ ./kv_bintree_test