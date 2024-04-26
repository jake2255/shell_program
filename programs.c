#include "programs.h"
#include "utilities.h"
#include "builtin.h"
#include <fcntl.h>
#include <unistd.h>

// implements input redirection
void handleInputRedirection(char *fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

// implements output redirection
void handleOutputRedirection(char *fileName) {
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(fd);
}


void redirectionCheck(char* currentCmd[], int count2) {
    // Check if there are enough arguments to check for redirection
    if (count2 < 3) {
        return; // Not enough arguments for redirection
    }

    pid_t pid = fork(); // Fork a child process to handle redirection
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process handles redirection
        // Check for input and output redirection operators and handle them
        char *inputFile = NULL;
        char *outputFile = NULL;
        for (int i = 0; i < count2 - 1; i++) {
            if (strcmp(currentCmd[i], "<") == 0) {
                // input redirection found
                inputFile = currentCmd[i + 1];
                // Remove redirection operator and filename from the command
                for (int k = i; k < count2 - 2; k++) {
                    currentCmd[k] = currentCmd[k + 2];
                }
                count2 -= 2;
                break; // Exit loop after handling input redirection
            } else if (strcmp(currentCmd[i], ">") == 0) {
                // output redirection detected
                outputFile = currentCmd[i + 1];
                // Remove the redirection operator and filename from the command
                for (int k = i; k < count2 - 2; k++) {
                    currentCmd[k] = currentCmd[k + 2];
                }
                count2 -= 2;
                break; // Exit loop after handling output redirection
            }
        }
        // Handle input and output redirection
        if (inputFile != NULL) {
            handleInputRedirection(inputFile);
        }
        if (outputFile != NULL) {
            handleOutputRedirection(outputFile);
        }
        // Execute the command after redirection
        executeStdCmds(currentCmd);
        // Exit the child process after executing the command
        exit(EXIT_SUCCESS);
    } else {
        // Parent process waits for the child to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (!WIFEXITED(status)) {
            fprintf(stderr, "Child process exited abnormally\n");
            exit(EXIT_FAILURE);
        }
    }
}









// executes non pipeline standard commands
void executeStdCmds(char* args[]){
    pid_t pid = fork();

    if(pid == 0){
        // child proccess
        if(setpgid(0,0) == -1)  // set proccess group id to process id
            perror("setpgid failed\n");   

        if(execvp(args[0], args) == -1) // executes commmand called
            fprintf(stderr, "%s: command not found\n", args[0]);

        exit(EXIT_FAILURE);
    }
    else if(pid > 0){
        // parent process
        if(setpgid(pid,pid) == -1)  // set proccess group id to process id
            perror("setpgid failed\n");    

        // if(tcsetpgrp(STDIN_FILENO, pid) == -1)   // not working
        //     perror("tcsetpgrp failed\n");

        if(waitpid(pid, NULL, 0) == -1) // waits on child process
            perror("waitpid failed\n");
    }
    else{
        perror("forking failed\n");
    }
}

// checks for pipeline operator and calls execute function
int pipelineCheck(char* currentCmd[], int count2){
    // store pipe cmds in individual arrays
    char* pipeCmd1[count2];
    char* pipeCmd2[count2];
    char* pipeCmd3[count2];
    int pipes = 0;
    int k = 0;

    for(int j = 0; j < count2; j++){
        // pipelining found
        if(strcmp(currentCmd[j], "|") == 0) {
            pipes++;
            k = 0;
            continue;
        }

        // store cmd + args in array
        if(pipes == 0)
            pipeCmd1[k] = currentCmd[j];
        else if(pipes == 1)
            pipeCmd2[k] = currentCmd[j];
        else if(pipes == 2)
            pipeCmd3[k] = currentCmd[j];
        else
            fprintf(stderr, "invalid amount of pipes\n");
        k++;
    }

    // if pipes found run executePipeCmds, else run executeStdCmds
    if(pipes == 1){
        executeOnePipeCmds(pipeCmd1, pipeCmd2);
        return -1;
    }
    else if(pipes == 2){
        executeTwoPipeCmds(pipeCmd1, pipeCmd2, pipeCmd3);
        return -1;
    }
    else{
        return 1;
    }
}

// executes one pipeline command
void executeOnePipeCmds(char* pipeCmd1[], char* pipeCmd2[]){
    int fd1[2];   // file descriptor
    pid_t pid1 = fork();  // fork

    // create & check pipes
    if(pipe(fd1) == -1){
        perror("pipe failed\n");
        exit(EXIT_FAILURE);
    }

    // create proccesses and connect with pipes
    if(pid1 == -1){
        perror("fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){
        // Child process 1
        close(fd1[READ]);

        // redirect stdout to the write end of first pipe
        if(dup2(fd1[WRITE], STDOUT_FILENO) == -1)
            perror("dup2 failed\n");

        close(fd1[WRITE]);

        // execute the first command
        if(execvp(pipeCmd1[0], pipeCmd1) == -1)
            fprintf(stderr, "%s: command not found\n", pipeCmd1[0]);
    } 
    else{
        pid_t pid2 = fork();
        if(pid2 == -1){
            perror("pipe failed\n");
            exit(EXIT_FAILURE);
        } 
        else if(pid2 == 0){
            // Child process 2
            close(fd1[WRITE]);

            // redirect stdin from the read end of the first pipe
            if(dup2(fd1[READ], STDIN_FILENO) == -1)
                perror("dup2 failed\n");

            close(fd1[READ]);

            // execute the second command
            if(execvp(pipeCmd2[0], pipeCmd2) == -1)
                fprintf(stderr, "%s: command not found\n", pipeCmd2[0]);
        } 
        else{
            // Parent process
            close(fd1[READ]);
            close(fd1[WRITE]);

            // wait for all child processes
            if(waitpid(pid1, NULL, 0) == -1) 
                perror("waitpid failed\n");
            if(waitpid(pid2, NULL, 0) == -1)
                perror("waitpid failed\n");
        }
    }
}

// executes two pipeline command
void executeTwoPipeCmds(char* pipeCmd1[], char* pipeCmd2[], char* pipeCmd3[]){
    int fd1[2], fd2[2];   // file descriptors
    pid_t pid1 = fork();  // fork

    // create & check pipes
    if(pipe(fd1) == -1 || pipe(fd2) == -1){
        perror("pipe failed\n");
        exit(EXIT_FAILURE);
    }

    // create proccesses and connect with pipes
    if(pid1 == -1){
        perror("fork failed\n");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){
        // Child process 1
        close(fd1[READ]);
        close(fd2[READ]);
        close(fd2[WRITE]);

        // redirect stdout to the write end of first pipe
        if(dup2(fd1[WRITE], STDOUT_FILENO) == -1)
            perror("dup2 failed\n");

        close(fd1[WRITE]);

        // execute the first command
        if(execvp(pipeCmd1[0], pipeCmd1) == -1)
            fprintf(stderr, "%s: command not found\n", pipeCmd1[0]);
    } 
    else{
        pid_t pid2 = fork();
        if(pid2 == -1){
            perror("pipe failed\n");
            exit(EXIT_FAILURE);
        } 
        else if(pid2 == 0){
            // Child process 2
            close(fd1[WRITE]);
            close(fd2[READ]);

            // redirect stdin from the read end of the first pipe
            if(dup2(fd1[READ], STDIN_FILENO) == -1)
                perror("dup2 failed\n");
            // redirect stdout to the write end of the second pipe
            if(dup2(fd2[WRITE], STDOUT_FILENO) == -1)
                perror("dup2 failed\n");

            close(fd1[READ]);
            close(fd2[WRITE]);

            // execute the second command
            if(execvp(pipeCmd2[0], pipeCmd2) == -1)
                fprintf(stderr, "%s: command not found\n", pipeCmd2[0]);
        } 
        else{
            pid_t pid3 = fork();
            if(pid3 == -1){
                perror("pipe failed\n");
                exit(EXIT_FAILURE);
            } 
            else if(pid3 == 0){
                // Child process 3
                close(fd1[READ]);
                close(fd1[WRITE]);
                close(fd2[WRITE]);

                // Redirect stdin from the read end of the second pipe
                if(dup2(fd2[0], STDIN_FILENO) == -1)
                    perror("dup2 failed\n");

                close(fd2[READ]);

                // execute the third command
                if(execvp(pipeCmd3[0], pipeCmd3) == -1)
                    fprintf(stderr, "%s: command not found\n", pipeCmd3[0]);
            } 
            else{
                // Parent process
                close(fd1[READ]);
                close(fd1[WRITE]);
                close(fd2[READ]);
                close(fd2[WRITE]);

                // wait for all child processes
                if(waitpid(pid1, NULL, 0) == -1) 
                    perror("waitpid failed\n");
                if(waitpid(pid2, NULL, 0) == -1)
                    perror("waitpid failed\n");
                if(waitpid(pid3, NULL, 0) == -1)
                    perror("waitpid failed\n");
            }
        }
    }
}

// default signal actions
void signalHandling(){
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}
