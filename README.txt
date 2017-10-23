The following function in my code will read one line from file stream.
char * inputFile(FILE *fd);

a). add command
	1. Read one line from command file (using code from HW8’s skeleton code) 
	2. If the entry in the command is already in the database or the date is not valid, ignore it.
	3. Repeat until finish all the commands
	
b. edit command
	1. Read all the entries from the database, and make a singled linked list.
	2. If the edit command is valid (date), edit the node which matches the id in the command
	3. Write the list back to database file.

c. remove command
	1. Read all the entries from the database, and make a single linked list. (The node has one element, the entry).
	2. Find the node that needs removal, remove the node from the linked list.
	3. Write the linked list back to the database.

d. lookup command

	1. Read the entries from database, if the entry matches the feature and data, write it into outPut file.
	2. If the command has invalid date, ignore it.
	3. If the command has wildcard, call patternMatch function to check if the entry matches.

e. display command
	1. Read all the entries from database, make an array.
	2. Sort the array by feature and order.
	3. Write the first MAX entries into the output file.

