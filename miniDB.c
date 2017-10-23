#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniDB.h"
#include <ctype.h>

// Returns 1 line from file for every function call
// Second call will return second line, etc.
// The code of inputFile function is from the skeleton code from HW8
char * inputFile(FILE *fd) {
    char * buffer = (char *) malloc(512 * sizeof(char));
    char * bufferptr = buffer;
    *bufferptr = fgetc(fd);
    while (*bufferptr != EOF && *bufferptr != '\0' && *bufferptr != '\n') {
        bufferptr++;
        *bufferptr = fgetc(fd);
    }
    *bufferptr = '\0';
    return buffer;
}

void sllistInit(SLLIST * sl) {
	sl->head = NULL;
}

int commandParser(char *cmd){
	if (cmd == NULL) return 0;
	char command[8]; 
	sscanf(cmd,"%[^,]", command);

	if (!strcmp(command, "ADD")) {
		return 1;
	} else if (!strcmp(command, "EDIT")) {
		return 2;	
	} else if (!strcmp(command, "REMOVE")) {
		return 3;	
	} else if (!strcmp(command, "LOOKUP")) {
		return 4;	
	} else if (!strcmp(command, "DISPLAY")) {
		return 5;
	}
	
	return 0;
}

void add(char* filename, char *cmd){
	
	char ID[20];
	char date[20];
	sscanf(cmd, "%*[^,], %*[^,], %[^,], %*[^,], %[^,]", date,ID);
	if (!validDate(date)) {
		return;
	}
	
	char entry[512];
	while (*cmd) {
		if (*cmd == ',') {
			cmd += 2;
			break;
		}
		cmd++;
	}
	strcpy(entry,cmd);

	FILE *db = fopen(filename, "r");
	char *buffer = inputFile(db);

	if(strcmp(buffer, "") == 0) {
		fclose(db);
		db = fopen(filename, "w");
		fprintf(db, "%s", entry);
		fprintf(db, "%s", "\n");
		fclose(db);
		free(buffer);
		return;
	}

	while (strcmp(buffer, "")) {
		if(hasID(buffer, ID)){
			fclose(db);
			free(buffer);
			return;		
		}
		free(buffer); 			
		buffer = inputFile(db);	
	}				
	free(buffer);
	fclose(db);
	db = fopen(filename, "a");
	fprintf(db, "%s", entry);
	fprintf(db, "%s", "\n");
	fclose(db);
}

int hasID(char *entry, char* IDFromCmd){
	char IDInDB[15];
	int commaCounter = 0;
	int counterID = 0;
	while (*entry) {
		if(*entry == ',') {
			commaCounter++;
			entry += 2;
			continue;
		}
		if (commaCounter == 3 ) {
			IDInDB[counterID] = *entry;
			counterID++;
		}
		entry++;
	}
	
	IDInDB[counterID] = '\0';	

	if (!strcmp(IDInDB, IDFromCmd)) {
		
		return 1;
	} 

	return 0;

}

void edit(char* dbfilename, char* cmd){
	SLLIST sl;
	sllistInit(&sl);
	FILE* db = fopen(dbfilename, "r");
	makeLLT(db, &sl);
	fclose(db);
	
	char IDInCmd[15];
	char feature[10];
	char data[400];
	sscanf(cmd, "%*[^,], %[^,], %[^,], %[^,]", IDInCmd, feature, data);

	if (!strcmp(feature, "DATE") && !validDate(data)) {			
		return;
	}

	DBEntry* node = sl.head;
		
	while (node != NULL){
			
		if (hasID(node->ENTRY, IDInCmd)){
				
			char title[100];
			char date[20];
			char director[200];	
			
			sscanf(node->ENTRY, "%[^,], %[^,], %[^,]", title, date, director);
				
			if (!strcmp(feature, "TITLE")) {
				strcpy(title, data);
				
			}else if (!strcmp(feature, "DATE")) {
				strcpy(date, data);
				
			}else if (!strcmp(feature, "DIRECTOR")) {
				strcpy(director, data);
			}
				
			char *newEntry = (char*) malloc(sizeof(char) * MAXLENGTH);
			newEntry[0] = '\0';
			strcat(newEntry, title);
			strcat(newEntry, ", ");
			strcat(newEntry, date);
			strcat(newEntry, ", ");
			strcat(newEntry, director);
			strcat(newEntry, ", ");
			strcat(newEntry, IDInCmd);
			node->ENTRY = strdup(newEntry);
			free(newEntry);

			break;
				
		}
		node = node->next;
	}
	writeLLT(dbfilename, &sl); // write it back to database
	freeLLT(&sl);	
}

void removeEty(char* dbfilename, char *cmd){
	
	FILE *db = fopen(dbfilename, "r+");
	if (db == NULL) {
		return;	
	}
	//parse the ID in the remove command
	char IDInCmd[15];
	sscanf(cmd, "%*[^,], %[^,]", IDInCmd);

	//read all the entries from database
	//make a linked list
	SLLIST dbList;
	sllistInit(&dbList);
	makeLLT(db, &dbList);
	fclose(db);

	DBEntry * entryPtr = dbList.head;
	DBEntry * prev = NULL;
	
	while (entryPtr){
			
		if (hasID(entryPtr->ENTRY, IDInCmd)){ 	//find the entries needs removal
			//remove it 
			if (prev == NULL) {
				dbList.head = dbList.head->next;
			} else {
				prev->next = entryPtr->next;
			}
			free(entryPtr->ENTRY);
			free(entryPtr);
			break;
		}
		prev = entryPtr;
		entryPtr = entryPtr->next;
	}
	
	//write the entries back to the database
	writeLLT(dbfilename, &dbList);
	freeLLT(&dbList);
}

int makeLLT(FILE* db, SLLIST* sl) {
	int nodeCount = 0;
	char *buffer = inputFile(db);
	
	DBEntry *curr, *prev;
	
	while (strcmp(buffer, "")) {
		curr = sl->head;
		if ( sl->head == NULL) {
			curr = (DBEntry*) malloc(sizeof(DBEntry));
			curr->ENTRY = strdup(buffer);
			curr->next = NULL;	
			sl->head = curr;
			nodeCount++;
		
		} else {
	
			while(curr != NULL) {
				prev = curr;
				curr = curr->next;
			}
			DBEntry* new = (DBEntry*) malloc(sizeof(DBEntry));
			new->ENTRY = strdup(buffer);
			prev->next = new;	
			new->next = NULL;
			nodeCount++;
			
		}
		free(buffer); 			
		buffer = inputFile(db);	
	}				
	free(buffer);

	return nodeCount;
}

void printList(SLLIST *sl) {
	DBEntry* node = sl->head;
	while (node){
		printf("%s\n",node->ENTRY);
		node = node->next;
	}
}

void writeLLT(char *dbfilename, SLLIST* sl) {

	FILE *db = fopen(dbfilename, "w");
	if(sl == NULL) return;
	DBEntry* temp = sl->head;
	if (temp == NULL) {
		fprintf(db, "%s", "");	
	} else {
		while (temp) {
			fprintf(db, "%s", temp->ENTRY);
			fprintf(db, "%s", "\n");
			temp = temp->next;
		}
	}	
	fclose(db);
}

void freeLLT(SLLIST* sl){
	DBEntry * ent = sl->head;
	while (ent) {
		if (ent->ENTRY) {
			free(ent->ENTRY);
		}
		DBEntry * temp = ent;
		ent = ent->next;
		free(temp);		
	}
}

int hasWildcard(char* data){
	
	while(*data){
		if (*data == '*') {
			return 1;
		}
		data++;
	}

	return 0;
}

int patternMatch(char *str, char* pat){
    if (str == NULL || pat == NULL) return 0;
    int lens = (int)strlen(str);
    int lenp = (int)strlen(pat);
	        
    int i = 0, j = 0;
    while (i < lens && j < lenp) {
			        
        if(str[i] == pat[j]) {
            i++;
            j++;
            continue;
        }
			            
        if (pat[j] != '*') return 0;
        j++;
													            
        if (pat[j] == '\0') return 1;
        for (; str[i] != '\0' && pat[j] != '*' && pat[j] != str[i]; i++)
            ;
																		        
    }
    
    if (pat[j] != '\0') return 0;
	if (i < lens) {
		if (str[lens - 1] != pat[lenp - 1]) {
			return 0;
		}
	}																				        
    return 1;
}



int hasFeature(char* entry, char* cmd) {
	char title[100];
	char date[20];
	char director[200];
	char ID[15];
	sscanf(entry, "%[^,], %[^,], %[^,], %[^,]", title, date, director, ID);
	char feature[20];
	char data[100];
	sscanf(cmd, "%*[^,], %[^,], %[^,]", feature, data);

	if (!strcmp(feature, "TITLE")) {
		// wildcard
		if (hasWildcard(data) && patternMatch(title, data)) {
			return 1;
		}
	
		if (!strcmp(title, data)) return 1;
	}

	if (!strcmp(feature, "DIRECTOR")) {
		if (hasWildcard(data) && patternMatch(director, data)) {
			return 2;
		}
		if (!strcmp(director, data)) return 2;
	}
	if (!strcmp(feature, "DATE")) {
		if (hasWildcard(data) && patternMatch(date, data)) {
			return 3;
		}
		if (!strcmp(date, data)) {
			return 3;
		}
	}
	if (!strcmp(feature, "ID") && !strcmp(ID, data)) {
		return 4;
	}				
	return 0;
}



void lookup(char* dbfilename, char *cmd, FILE * output){
	// wirte the command into output file
	//load the database
	//read all the entries 
	//if the entry's feature fit the data, write entry to output file
	char feature[20];
	sscanf(cmd, "%*[^,], %[^,]", feature);
	
	if (!strcmp(feature, "DATE")) { // if lookup date, check if date is valid
		char date[100];
		sscanf(cmd, "%*[^,], %*[^,], %[^,]", date);
		if (!validDate(date)&& !hasWildcard(date)){
			return;		
		}
	}	

	fprintf(output, "%s", cmd);
	fprintf(output, "%s", "\n");
	FILE *db = fopen(dbfilename, "r");
	if (db != NULL) {
		char *buffer = inputFile(db);
		while (strcmp(buffer, "")){
			if (hasFeature(buffer, cmd)) {
				fprintf(output, "%s", buffer);
				fprintf(output, "%s", "\n");	
			}
			free(buffer); 			
			buffer = inputFile(db);	
		}				
		free(buffer);
	}
	fclose(db);
}

void display(char* dbfilename, char *cmd, FILE* output){
	char feature[10];
	char orderChar[5];
	char maxChar[10];
	sscanf(cmd, "%*[^,], %[^,], %[^,], %[^,]", feature, orderChar, maxChar);
	int order = atoi(orderChar);
	int MAX = atoi(maxChar);
	if (MAX == 0) {
		fprintf(output, "%s", cmd);
		fprintf(output, "%s", "\n");
		return;
	} 
	//read the database file, make a list
	SLLIST dbList;
	sllistInit(&dbList);
	FILE* db = fopen(dbfilename, "r");
	int nodeNum = makeLLT(db, &dbList);
	DBEntry **dbArr = (DBEntry**)malloc(sizeof(DBEntry*)*nodeNum);
	DBEntry* node = dbList.head;
	int i = 0;
	while(node) {
		dbArr[i] = (DBEntry*)malloc(sizeof(DBEntry));
		dbArr[i]->ENTRY = strdup(node->ENTRY);
		i++;
		node = node->next;
	}
	sortList(dbArr, nodeNum, order, feature);
	//print the command into the file
	fprintf(output, "%s", cmd);
	fprintf(output, "%s", "\n");
	// print out the MAX  nodes into output file
	int j;
	for (j = 0; j < MAX && j < nodeNum; j++) {
		fprintf(output, "%s", dbArr[j]->ENTRY);
		fprintf(output, "%s", "\n");
	}
}

void getFeature(char* entry, char * feature, char* data) {

	if (*feature == 'T') { // title
		sscanf(entry, "%[^,]", data);
	}

	if (*feature == 'I') { // id
		sscanf(entry, "%*[^,], %*[^,], %*[^,], %[^,]", data);
	}

	if (!strcmp(feature, "DATE")) {
		sscanf(entry, "%*[^,], %[^,]", data);
	}

	if (!strcmp(feature, "DIRECTOR")) {
		sscanf(entry, "%*[^,], %*[^,], %[^,]", data);
	}
}

int isleap(int year) {
    
    if (year % 400 == 0) {
        return 1;
    } else if (year % 100 == 0) {
        return 0;
    
    } else if (year % 4 == 0) {
        return 1;
    }
    return 0;
}

int isNumber(char *str) {
	
	while(*str){
		if (!isdigit(*str)){
			return 0;
		}
		str++;
	}

	return 1;
}

int validDate(char *date) {
    char month[4];
    char day[4];
    char year[6];
    sscanf(date, "%[0-9]/%[0-9]/%[0-9]", month, day, year);
  
    int mon_int, day_int, year_int;
	int mon_len, day_len, year_len;

	mon_len = strlen(month);
	day_len = strlen(day);
	year_len = strlen(year);
	
	if (mon_len != 2 || day_len != 2 || year_len != 4) {
		return 0;
	}

	if (!isNumber(month) || !isNumber(day) || !isNumber(year)) {
		return 0;
	}

    mon_int = atoi(month);
    day_int = atoi(day);
    year_int = atoi(year);
    
    if (mon_int < 1 || day_int < 1 || mon_int > 12 || day_int > 31 || year_int >9999 || year_int < 1000 ) {
        return 0;
    
    }
    
    switch(mon_int) {
        case 2:
        case 4:
        case 6:
        case 9:
        case 11:
            if (day_int > 30) {
                return 0;
            }
            break;
    
    }
    
    if (!isleap(year_int)) {
        if (mon_int == 2 && day_int > 28) {
            return 0;
        }
    }

	return 1;
}


void sortList (DBEntry **display, int n, int order, char *feature) {

	if (order == 0) { // ascending
		if (!strcmp(feature, "TITLE")) {
			// sort the entire list by title in ascending order
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char titlej[200];
					char titlej1[200];
					sscanf(display[j]->ENTRY, "%[^,]", titlej);
					sscanf(display[j+1]->ENTRY, "%[^,]", titlej1);
					if (strcmp(titlej,titlej1) > 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(titlej,titlej1) == 0) {
						char IDj[20];
						char IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) > 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}			
		}

		if (!strcmp(feature, "DATE")) {
			// sort the list by date in ascending order
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char datej[50];
					char datej1[50];
					sscanf(display[j]->ENTRY, "%*[^,], %[^,]", datej);
					sscanf(display[j+1]->ENTRY, "%*[^,], %[^,]", datej1);
					char monthj[4], monthj1[4];
					char dayj[4], dayj1[4];
					char yearj[6], yearj1[6];
					sscanf(datej, "%[0-9]/%[0-9]/%[0-9]", monthj, dayj, yearj);
					sscanf(datej1, "%[0-9]/%[0-9]/%[0-9]", monthj1, dayj1, yearj1);
					datej[0] = '\0';
					datej1[0] = '\0';
					strcat(datej, yearj);
					strcat(datej, monthj);
					strcat(datej, dayj);
					strcat(datej1, yearj1);
					strcat(datej1, monthj1);
					strcat(datej1, dayj1);
					
					if (strcmp(datej,datej1) > 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(datej,datej1) == 0) {
						char  IDj[20];
						char  IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) > 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}
		}

		if (!strcmp(feature, "DIRECTOR")) {
			// sort the list by director in ascending order
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char directorj[100];
					char directorj1[100];
					sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %[^,]", directorj);
					sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %[^,]", directorj1);

					if (strcmp(directorj, directorj1) > 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(directorj, directorj1) == 0) {
						char IDj[20];
						char IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) > 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}
		}

		if (!strcmp(feature, "ID")) {
			// sort the list by ID in ascending order
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char IDj[20];
					char IDj1[20];
					sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
					sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
					
					if (strcmp(IDj,IDj1) > 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
				}
			}
		}
		
	} else { // descending

		if (!strcmp(feature, "TITLE")) {
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char titlej[200];
					char titlej1[200];
					sscanf(display[j]->ENTRY, "%[^,]", titlej);
					sscanf(display[j+1]->ENTRY, "%[^,]", titlej1);
					if (strcmp(titlej, titlej1) < 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(titlej, titlej1) == 0) {
						char IDj[20];
						char IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) < 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}			
		}

		if (!strcmp(feature, "DATE")) {
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char datej[50];
					char datej1[50];
					sscanf(display[j]->ENTRY, "%*[^,], %[^,]", datej);
					sscanf(display[j+1]->ENTRY, "%*[^,], %[^,]", datej1);
					char monthj[4], monthj1[4];
					char dayj[4], dayj1[4];
					char yearj[6], yearj1[6];
					sscanf(datej, "%[0-9]/%[0-9]/%[0-9]", monthj, dayj, yearj);
					sscanf(datej1, "%[0-9]/%[0-9]/%[0-9]", monthj1, dayj1, yearj1);
					datej[0] = '\0';
					datej1[0] = '\0';
					strcat(datej, yearj);
					strcat(datej, monthj);
					strcat(datej, dayj);
					strcat(datej1, yearj1);
					strcat(datej1, monthj1);
					strcat(datej1, dayj1);
					
					if (strcmp(datej,datej1) < 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(datej,datej1) == 0) {
						char IDj[20];
						char IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) < 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}		
		}

		if (!strcmp(feature, "DIRECTOR")) {
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char directorj[100];
					char directorj1[100];
					sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %[^,]", directorj);
					sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %[^,]", directorj1);
					
					if (strcmp(directorj,directorj1) < 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
					if (strcmp(directorj,directorj1) == 0) {
						char  IDj[20];
						char  IDj1[20];
						sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
						sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);
						if(strcmp(IDj, IDj1) < 0) {
							DBEntry* temp = display[j];
							display[j] = display[j+1];
							display[j+1] = temp;
						}
					}
				}
			}
		}

		if (!strcmp(feature, "ID")) {
			int i, j;
			for (i = 0; i < n - 1; i++) {
				for (j = 0; j < n - i - 1; j++) {
					char IDj[20];
					char IDj1[20];
					sscanf(display[j]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj);
					sscanf(display[j+1]->ENTRY, "%*[^,], %*[^,], %*[^,], %[^,]", IDj1);	
					
					if (strcmp(IDj,IDj1) < 0) {
						DBEntry* temp = display[j];
						display[j] = display[j+1];
						display[j+1] = temp;
					}
				}
			}
		}
	}
 }

int main(int argc, char* argv[]) {
	if (argc != 4) {
		return 0;	
	}
	char dbfilename[100];
	strcpy(dbfilename,argv[1]);
	char commandfilename[100];
	strcpy(commandfilename,argv[2]);
	char outputfilename[100];
	strcpy(outputfilename, argv[3]);
	FILE* commands = fopen(commandfilename, "r");
	char *cmd = inputFile(commands);
	
	FILE* optFile = fopen(outputfilename, "w");
	
	while (strcmp(cmd, "")) {
		int cmdType = commandParser(cmd);
		switch(cmdType) {
			case 1: // add 
				add(dbfilename, cmd);
				break;
			case 2: // edit 
				edit(dbfilename,cmd);
				break;
			case 3: // remove 
				removeEty(dbfilename, cmd);
				break;
			case 4: // lookup
				lookup(dbfilename, cmd ,optFile);
				break;
			case 5: // display
				display(dbfilename, cmd, optFile);
				break;
		}
		free(cmd);
		cmd = inputFile(commands);
	}
	
	free(cmd);
	fclose(optFile);
	fclose(commands);	
	
	return 0;
}

