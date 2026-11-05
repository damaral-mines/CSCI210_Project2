#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

struct NODE* splitPath(char* pathName, char* baseName, char* dirName) {
    if (pathName == NULL || strlen(pathName) == 0) {
        if (baseName) strcpy(baseName, "");
        if (dirName) strcpy(dirName, "");
        return NULL;
    }

    char tempPath[256];
    strncpy(tempPath, pathName, sizeof(tempPath) - 1);
    tempPath[sizeof(tempPath) - 1] = '\0';

    int len = strlen(tempPath);
    while (len > 1 && tempPath[len - 1] == '/') {
        tempPath[len - 1] = '\0';
        len--;
    }

    char *lastSlash = strrchr(tempPath, '/');

    if (lastSlash == NULL) {
        strcpy(dirName, "");
        strcpy(baseName, tempPath);
    } else if (lastSlash == tempPath) {
        strcpy(dirName, "/");
        strcpy(baseName, lastSlash + 1);
    } else {
        strcpy(baseName, lastSlash + 1);
        *lastSlash = '\0';
        strcpy(dirName, tempPath);
    }

    struct NODE *startNode = (pathName[0] == '/') ? root : cwd;

    if (strlen(dirName) == 0) {
        return cwd;
    }
    if (strcmp(dirName, "/") == 0) {
        return root;
    }

    char tempDir[256];
    strncpy(tempDir, dirName, sizeof(tempDir) - 1);
    tempDir[sizeof(tempDir) - 1] = '\0';

    struct NODE *curr = startNode;
    char *token = strtok(tempDir, "/");

    while (token != NULL) {
        struct NODE *child = curr->childPtr;
        struct NODE *found = NULL;

        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                found = child;
                break;
            }
            child = child->siblingPtr;
        }

        if (found == NULL) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }

        if (found->fileType != 'D') {
            printf("ERROR: %s is not a directory\n", token);
            return NULL;
        }

        curr = found;
        token = strtok(NULL, "/");
    }

    return curr;
}

void mkdir(char pathName[]) {
    if (pathName == NULL || strlen(pathName) == 0 || strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[256] = "";
    char dirName[256] = "";

    struct NODE *parent = splitPath(pathName, baseName, dirName);

    if (parent == NULL) {
        return;
    }

    if (strlen(baseName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    struct NODE *child = parent->childPtr;
    while (child != NULL) {
        if (strcmp(child->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        child = child->siblingPtr;
    }

    struct NODE *newDir = (struct NODE *)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        printf("MKDIR ERROR: memory allocation failed\n");
        return;
    }

    // Initialize node fields
    strcpy(newDir->name, baseName);
    newDir->fileType = 'D';
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;
    newDir->parentPtr = parent;

    if (parent->childPtr == NULL) {
        parent->childPtr = newDir;
    } else {
        struct NODE *curr = parent->childPtr;
        while (curr->siblingPtr != NULL) {
            curr = curr->siblingPtr;
        }
        curr->siblingPtr = newDir;
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}
