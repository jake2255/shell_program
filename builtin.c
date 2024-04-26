#include "builtin.h"
#include "utilities.h"
#include "builtin.h"

// built-in function to exit shell when called
void handleExitCmd(){
    printf("Exiting shell...\n");
    restoreOriginalPath();
    exit(EXIT_SUCCESS);
}

//  variable stores the original PATH value
char originalPath[BUFFER_MAX];

//  displays the current pathnames in the PATH variable
void displayCurrentPath()
 {
    char *path = getenv("PATH");
    if (path != NULL) {
        printf("Current path: %s\n", path);
    } else {
        printf("PATH is undefined.\n");
    }
}

// appends a pathname to the PATH variable
void appPath(const char *pathname) 
{
    char *path = getenv("PATH");
    if (path != NULL) {
        char new_path[BUFFER_MAX];
        snprintf(new_path, sizeof(new_path), "%s:%s", path, pathname);
        setenv("PATH", new_path, 1);
    } else {
        setenv("PATH", pathname, 1);
    }
}

// removes a pathname from the PATH variable
void remPath(const char *pathname) {
    char *path = getenv("PATH");
    if (path != NULL) {
        char *new_path = strtok(path, ":");
        char modified_path[BUFFER_MAX] = "";
        while (new_path != NULL) {
            if (strcmp(new_path, pathname) != 0) {
                strcat(modified_path, new_path);
                strcat(modified_path, ":");
            }
            new_path = strtok(NULL, ":");
        }
        // removes the trailing colon if present
        if (strlen(modified_path) > 0 && modified_path[strlen(modified_path) - 1] == ':') {
            modified_path[strlen(modified_path) - 1] = '\0';
        }
        setenv("PATH", modified_path, 1);
    }
}

//stores the original PATH value
void storeOriginalPath() {
    char *path = getenv("PATH");
    if (path != NULL) {
        strcpy(originalPath, path);
    }
}

// restores the original PATH value
void restoreOriginalPath() {
    setenv("PATH", originalPath, 1);
}
