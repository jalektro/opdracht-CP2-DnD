#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct item {
    char *name;
    struct cost {
        int quantity;
        char unit[4]; // Increased to accommodate null terminator
    } coin;
    float weight;
    struct item *nextItem;
} itemList[20]; // max 20 items

int allocateAndCheck(void **ptr, size_t size, const char *errorMessage);
char *jsonVal(FILE *filePointer, const char *keyWord);
char *jsonNum(FILE *filePointer, const char *keyWord);
int handlingFile();


int main(int argc, char *argv[]) {
    printf("compiler works\n");
    handlingFile();
    return 0;
}

// Function to open the file and call to function to fill data in this file.
int handlingFile() {
    FILE *filePointer = fopen("yew-wand.json", "r");
    if (filePointer == NULL) {
        perror("File opening failed");
        return 1;
    }

    char *token;

    // Parse "name"
    token = jsonVal(filePointer, "name");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'name' key in JSON.\n");
        return 1;
    }
    if (allocateAndCheck((void **)&itemList->name, strlen(token) + 1, "Memory allocation failed.") != 0) {
        return 1;
    }
    strcpy(itemList->name, token); // put in the keyword inside of the struct
    printf("name = %s\n", itemList->name);
	
    // Parse "quantity"
    token = jsonNum(filePointer, "quantity");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'quantity' key in JSON.\n");
        return 1;
	}
	itemList->coin.quantity = atoi(token);
	
    printf("quantity = %d\n", itemList->coin.quantity);
	
    // Parse "unit"
    token = jsonVal(filePointer, "unit");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'unit' key in JSON.\n");
        return 1;
    }
    strcpy(itemList->coin.unit, token);
    printf("unit = %s\n", itemList->coin.unit);

    // Parse "weight"
    token = jsonNum(filePointer, "weight");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'weight' key in JSON.\n");
        return 1;
    }
    itemList->weight = atof(token);
    printf("weight = %.2f\n", itemList->weight);

    fclose(filePointer);
    return 0;
}

// Function to control error memory
int allocateAndCheck(void **ptr, size_t size, const char *errorMessage) {
    *ptr = malloc(size);
    if (*ptr == NULL) {
        fprintf(stderr, "%s\n", errorMessage);
        return 1; // indicating failure
    }
    return 0; // indicating success
}

// Function to parse JSON and extract string value
char *jsonVal(FILE *filePointer, const char *keyWord) {
    char buffer[1024];
    char *token;

    while (fgets(buffer, sizeof(buffer), filePointer) != NULL) {
        token = strtok(buffer, "\"");
        while (token) {
            token = strtok(NULL, "\"");
            if (token != NULL && strcmp(token, keyWord) == 0) {
                token = strtok(NULL, "\"");
                if (token != NULL) {
                    token = strtok(NULL, "\"");
                    if (token != NULL) {
                        return token;
                    }
                }
            }
        }
    }
    return NULL; // Key not found
}

// Function to parse JSON and extract numeric value
char *jsonNum(FILE *filePointer, const char *keyWord) {
    char buffer[1024];
    char *token;

    while (fgets(buffer, sizeof(buffer), filePointer) != NULL) {
        token = strtok(buffer, "\"");
        while (token) {
            token = strtok(NULL, "\"");
//printf("token = %s\n",token);
            if (token != NULL && strcmp(token, keyWord) == 0) {
                token = strtok(NULL, ":");
                if (token != NULL) {
                    size_t len = strcspn(token, ",");
                    token[len] = '\0';					
                    return token;
                }
            }
        }
    }
    return NULL; // Key not found
}