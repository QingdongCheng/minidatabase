#include <stdio.h>
#define MAXLENGTH 512


typedef struct dbEntry {
	char *ENTRY;
	struct dbEntry * next;

}DBEntry;

typedef struct SingleLinkedList {
	DBEntry * head;
}SLLIST;

int hasID(char *entry, char* IDFromCmd);

void printList(SLLIST *sl);

void sllistInit(SLLIST *sl);

int makeLLT(FILE *db, SLLIST* sl);

char * inputFile(FILE *fd);

int commandParser(char*cmd);

void commandHandler(char* dbfilename, char *cmdfilename, char *optfilename);

void add(char* filename, char *cmd);

void edit(char* dbfile, char *cmd);

void removeEty(char* filename, char *cmd);

void lookup(char* dbfilename, char *cmd, FILE * outputFile);

void display(char* dbfilename, char *cmd, FILE* outputFile);

void writeLLT(char* filename, SLLIST* sl);

void freeLLT(SLLIST* sl);

void sllistInit(SLLIST* sl);

void sortList (DBEntry **displayResult, int n, int order, char *feature);

int validDate(char *date);

int isleap(int year);

int hasWildcard(char* data);

int patternMatch(char *str, char* pattern);



