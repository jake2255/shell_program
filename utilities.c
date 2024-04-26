#include "utilities.h"
#include "builtin.h"
#include "programs.h"

// displays welcome message when shell is started
void welcomeMessage(){
    printf("\t                             __          __ __ \n");
    printf("\t .-----.-----.--.--.--.-----|  |--.-----|  |  |\n");
    printf("\t |     |  -__|  |  |  |__ --|     |  -__|  |  |\n");
    printf("\t |__|__|_____|________|_____|__|__|_____|__|__|\n");
    printf("\n");         
}

// displays the command line prompt
void cmdPrompt(){
    printf("~$ ");
}

// parse command line buffer, call the specified command functions
void cmdLineReader(char cmdBuffer[]){

    // remove newline from the end of cmdBuffer
    int length = strlen(cmdBuffer);
    if(length > 0 && cmdBuffer[length - 1] == '\n')
        cmdBuffer[--length] = '\0';

    // tokenize cmdBuffer string into individual strings, using semicolon as delimiter
    char* parsedCmdBuffer[BUFFER_MAX];
    memset(parsedCmdBuffer, 0, sizeof(parsedCmdBuffer));
    char* token = strtok(cmdBuffer, ";");
    int count = 0;

    // store tokens into parsedCmdBuffer array
    while(token != NULL){
        parsedCmdBuffer[count] = token;
        token = strtok(NULL, ";");  // get next token
        count++;
    }

    // variables used to execute exit command last
    int lastCommand = 0;
    int exitCalled = 0;

    // tokenize every parsedCmdBuffer string, using whitespace as delimiter
    char* currentCmd[BUFFER_MAX];   // current command to parse and call
    for(int i = 0; i < count; i++){
        memset(currentCmd, 0, sizeof(currentCmd));
        char* token2 = strtok(parsedCmdBuffer[i], " ");
        int count2 = 0;

        // store tokens into currentCmd array
        while(token2 != NULL){
            currentCmd[count2] = token2;
            token2 = strtok(NULL, " "); // get next token
            count2++;
        }

        // checks and implements I/O direction
        redirectionCheck(currentCmd, count2);

        // check if current command iteration is last
        if((i + 1) == count)
            lastCommand = 1;

        // calls function based on currentCmd 0th element
        if(currentCmd[0] == NULL){
            // empty line, do nothing
        }   
        else if(strcmp(currentCmd[0], "cd") == 0){
            if(count2 == 1 || (count2 == 2 && strcmp(currentCmd[1],"~"))){
                const char *home_dir = getenv("HOME");
                if(home_dir == NULL)
                {
                    fprintf(stderr, "cd: no home directory exists");
                }
                else 
                {
                    if(chdir(home_dir) != 0)
                    {
                        perror("cd");
                    }
                }
            }
            if(count2 == 2)
            {
                if(chdir(currentCmd[1])!=0)
                {
                    perror("cd");
                }
            }
            else if(count2 > 2)
            {
              fprintf(stderr, "cd: too many arguments\n");
            }
        }
        else if(strcmp(currentCmd[0], "exit") == 0){
            // if exit command is not last command, will be skipped until end
            exitCalled = 1;
            if(lastCommand && exitCalled){
                handleExitCmd();
                exitCalled = 0;
            }
        }
        else if(strcmp(currentCmd[0], "path") == 0){
            // todo: call path function
            {
                if(count2 == 1)
                {
                    displayCurrentPath();
                }
                else if(strcmp(currentCmd[1],"+") == 0)
                {
                    appPath(currentCmd[2]);
                }
                else if(strcmp(currentCmd[1],"-") == 0)
                {
                    remPath(currentCmd[2]);
                }
                else 
                {
                    fprintf(stderr, "Invalid 'path'\n");
                }
            }
        }
        else if(strcmp(currentCmd[0], "myhistory") == 0){
            // todo: implement myhistory
            printf("myhistory not implemented\n");
        }
        else if(strcmp(currentCmd[0], "\t") == 0){
            // empty line, do nothing
        }
        else{
            // if pipelineCheck returns 1 execute standard cmd function, returns 0 and run pipe cmd function
            int standardCmd = pipelineCheck(currentCmd, count2);
            if(standardCmd == 1)
                executeStdCmds(currentCmd);
        }
        }
        // call exit if previously skipped
        if(lastCommand && exitCalled)
            handleExitCmd();
    }
