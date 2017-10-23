all: miniDB

miniDB: miniDB.c
	gcc -Wall -g -Werror -o miniDB miniDB.c

