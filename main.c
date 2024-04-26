/*
    Main Function
*/

#include "utilities.h"
#include "builtin.h"
#include "programs.h"

int main(int argc, char* argv[]){

    welcomeMessage();           // prints welcome text
    signalHandling();             // shell process ignores any signal    
    char cmdBuffer[BUFFER_MAX]; // commands & args that user types in command line

    // Batch Mode
    if(argc == 2){
        storeOriginalPath();
        FILE *batchFile = fopen(argv[1],"r");
        if(batchFile == NULL)
        {
            printf("Error: Unable to open batch file.\n");
            return 1;
        }
        while (fgets(cmdBuffer, BUFFER_MAX, batchFile) != NULL)
        {
            printf(">%s", cmdBuffer);
            cmdLineReader(cmdBuffer);
        }
    }   
    // when file commands completed, shell to run in interactive mode

    // Interactive Mode
    while(1){
        storeOriginalPath();
        cmdPrompt();
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        fgets(cmdBuffer, BUFFER_MAX, stdin);
        cmdLineReader(cmdBuffer);  
    }

    return 0;
}
