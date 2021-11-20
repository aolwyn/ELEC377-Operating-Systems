//+
// File:	shell.c
//
// Purpose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//
//		if the command is not recognized an error is printed.
//-

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

// function declaration
int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
void doCommand(char * args[], int nargs);
char * skipChar(char * charPtr, char skip);

//+
// Function:    main
//
// Purpose:
//		    This function facilitates the way the program runs
//		    and the order in which functions are called.
//
// Parameters:
//			no parameters
//
// Returns:
//			An integer, not returned in this code
//-

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    char *args[MAXARGS];
    int nargs;

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        // Remove newline at end of buffer
        commandBuffer[strcspn(commandBuffer, "\n")] = 0;

        // Split command line into words.
        nargs = splitCommandLine(commandBuffer, args, MAXARGS);

        // Debugging for step 2
        //printf("%d\n", nargs);
        //int i;
        //for (i = 0; i < nargs; i++){
        //  printf("%d: %s\n",i,args[i]);
        //}

        // Execute the command
        // Remember to check if there is a command (i.e. value of nargs)
        if(nargs > 0){
            doCommand(args, nargs);
        }

        // print prompt
        printf("%%> ");
        fflush(stdout);
    }
}

////////////////////////////// String Handling ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	This function skips over a given char in a string
//		    For security, the function will not skip null chars.
//
// Parameters:
//      charPtr	    Pointer to string
//      skip	    character to skip
//
// Returns:	Pointer to first character after skipped chars.
//		Identity function if the string doesn't start with skip,
//		or skip is the null character
//-

char * skipChar(char * charPtr, char skip) {
    if (skip == '\0') {
        return charPtr;
    }

    while (charPtr) {
        if (*charPtr == skip) {
            charPtr++;
        } else {
            return charPtr;
        }

    }
}

//+
// Function:	splitCommandLine
//
// Purpose:	This splits a string into an array of strings.
//		The array is passed as an argument. The string
//		is modified by replacing some of the space characters
//		with null characters to terminate each of the strings.
//
// Parameters:
//	commandBuffer	The string to split
//	args		An array of char pointers
//	maxargs		Size of array args (max number of arguments)
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
    char* frontOfW; //pointer to front of a word
    char* backOfW; //pointer to the end/back of a word
    int nargs = 0; //number of words stored, to be returned

    frontOfW = skipChar(commandBuffer, ' '); //skip the first spaces
    backOfW = strchr(frontOfW, ' '); //find end of word

    while(backOfW){
        *backOfW = '\0'; //change first space after word to \0
        args[nargs] = frontOfW; //save the pointer to the word to the array
        nargs++;

        if(nargs == maxargs) {
            fprintf(stderr, "Exceeded maximum number of words\n");
            return nargs;
        }
        frontOfW = skipChar(++backOfW, ' '); //increment pointer by one then use skipChar to find next word
        backOfW = strchr(frontOfW, ' ');
    } //no more spaces in the commandBuffer

    if(*frontOfW != '\0'){ //if there is another word, save it
        args[nargs] = frontOfW;
    }

    return nargs;
}


////////////////////////////// Command Handling ///////////////////////////////////

// typedef for the function handler pointer
typedef void (*functionPtr)(char*[], int);

// cmdStruct type:
// Associates a command name with a command handling function
typedef struct {
    char* commandName;
    functionPtr function;
} command;

// prototypes for command handling functions
void exitFunc(char* [], int);
void pwd(char* [], int);
void ls(char* [], int);
void cd(char* [], int);

// Array that provides the list commands and functions; terminated by {NULL, NULL}
command commandDispTable[] = {
        {"pwd", pwd},{"cd", cd},{"ls", ls},{"exit", exitFunc},{NULL, NULL}
};

//+
// Function:	doCommand
//
// Purpose:	This command calls a command handling function from
//		the commands array based on the first argument passed
//		in the args array.
//
// Parameters:
//	    args	command and parameters, an array of pointers to strings
//	    nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

void doCommand(char * args[], int nargs){
    command* tempCommand = commandDispTable; //point to the front the the array
    while(tempCommand->commandName){ //loop when name is not NULL
        if(strcmp(tempCommand->commandName, args[0])==0){ //if the name and args[0] are the same
            return tempCommand->function(args, nargs);
        }
        tempCommand++; //point to next element of array
    }
    fprintf(stderr, "%s: Command was not found\n", args[0]); //if a return did not occur, there was no matching function
}

//////////////////////////////////////////////////
//            command Handling Functions        //
//////////////////////////////////////////////////
// all command handling functions have the same
// parameter types and return values.
//////////////////////////////////////////////////

//+
// Function:	exitFunc
//
// Purpose:	this command exits the current shell
//	 	    Since it is called by doCommand, it is known
//		    that arg is at least one element long
//
// Parameters:
//	        args	command and parameters, an array of pointers to strings
//	        nargs	number of entries in the args array
//      	parameters are not used
//
// Returns	nothing (void)
//-

void exitFunc(char * args[], int nargs){
    exit(0);
}

//+
// Function:	pwd
//
// Purpose:	this command performs prints the working directory
//          and displays the full path to the current directory
//	 	    Since it is called by doCommand, it is known
//		    that arg is at least one element long
//
// Parameters:
//	    args	command and parameters, an array of pointers to strings
//	    nargs	number of entries in the args array
//      parameters are not used in this function
//
// Returns	nothing (void)
//-

void pwd(char * args[], int nargs){
    char * cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
}

//+
// Function:	cd
//
// Purpose:	this command changes the current working directory to
//          a specified new directory.
//		    If no directory is specified, the new directory will
//		    be the home directory.
//	 	    Since it is called by doCommand, it is known
//		    that arg is at least one element long
//
// Parameters:
//	        args	command and parameters, an array of pointers to strings
//	        nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

void cd(char * args[], int nargs){
    char* directory;
    if(nargs == 2){
        directory = args[1];
    }else{
        struct passwd *pw = getpwuid(getuid());
        if(pw){
            directory = pw->pw_dir;
        }else{
            fprintf(stderr, "cd: Error getting home directory\n");
        }
    }
    if(chdir(directory)!= 0){
        fprintf(stderr, "cd: %s: No such directory\n");
    }
}

typedef int (*lsFilter)(const struct dirent*);

// filter function prototypes
int filterHidden (const struct dirent* d) {
    return !(d->d_name[0] == '.');
}

//+
// Function:	ls
//
// Purpose:	this command lists the files in the current
//          working directory.
//		    If there is no argument, the filter function
//		    will act as the third parameter.
//          If the argument is "-a", then NULL is the third parameter.
//	 	    Since it is called by doCommand, it is known
//		    that arg is at least one element long
//
// Parameters:
//	    args	command and parameters, an array of pointers to strings
//	    nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

void ls (char *args[], int nargs) {
    lsFilter filter = filterHidden; // default filter
    struct dirent** namelist;  // used to keep a list of all entities

    // passed an option
    if (nargs == 2) {
        if (strcmp(args[1], "-a") == 0)
            filter = NULL; // disable filter

        else { // recieved an unknown option
            fprintf(stderr, "ls: Invalid option -- '%s'\n", ++args[1]);
            return;
        }
    }
    int numEnts = scandir(".", &namelist, filter, NULL); // scan current directory
    int i;
    for (i=0;i<numEnts;++i) // print out all entities
        printf("%s\n", namelist[i]->d_name);
}
