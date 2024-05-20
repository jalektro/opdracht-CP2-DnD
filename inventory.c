#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 40
#define MAX_COIN_TYPES 5

struct Cost {
    char unit[3];
    int quantity;
};

struct Item {
    char name[50];
    double weight;
	struct Cost cost;
};

struct Inventory {
    struct Item items[MAX_ITEMS];
	double maxWeight;
    struct Cost coins[MAX_COIN_TYPES];
};

int allocateAndCheck(void **ptr, size_t size, const char *errorMessage);
char *jsonVal(FILE *filePointer, const char *keyWord);
char *jsonNum(FILE *filePointer, const char *keyWord);
int handlingFile(const char *fileName, struct Inventory *inventory);

//Inventory.exe -w 180.75 -m 4gp 42sp 69cp greatsword.json explorers-pack.json small-knife.json 2 waterskin.json leather-armor.json -c camp.log
int main(int argc, char* argv[])
{
  struct Inventory inventory= {0};
  int i;
  char campFile[50]= "";

  
	if (argc <= 2) {
	printf("Usage: %s equipment-files [number-of-items] [-w max-weight] [-m money] [-c camp-file]\n", argv[0]);
	return 1;
	}

	for(i=1; i< argc; i++)
	{
		if (strcmp(argv[i],"-w") == 0)					//handle max_weight
		{
			inventory.maxWeight= atof(argv[++i]);
			printf("maxweight : %.2f \n", inventory.maxWeight);
		} else if (strcmp(argv[i],"-m") == 0)			//handle coins
		{
			for( int j =0 ; j< MAX_COIN_TYPES && i+1 <argc; j++)
			{	
				if (argv[i+1][0] >= '0' && argv[i+1][0] <= '9') {
				sscanf(argv[++i], "%d%2s", &inventory.coins[j].quantity, &inventory.coins[j].unit);  //(cp, sp, ep, gp, pp
				printf("coins\n%d%s\n", inventory.coins[j].quantity, inventory.coins[j].unit);
				} else {
				break;
				}
			}

			
		} else if (strcmp(argv[i],"-c") == 0)			//handle camp file
		{
			strcpy(campFile,argv[++i]);
			FILE *campFilePtr = fopen( campFile, "a");		//open campfile for appending
				if (campFile == NULL){
				printf("Unable to open file: %s\n", campFile);
                continue;
				}
				printf("campfile opened and closed\n");
					fclose(campFilePtr);
		} else 	{			// handle all other files
		
         int quantityItems = 1;
            if (i + 1 < argc && argv[i + 1][0] >= '0' && argv[i + 1][0] <= '9') {
                quantityItems = atoi(argv[++i]);
            }
            for (int j = 0; j < quantityItems; j++) {
                handlingFile(argv[i], &inventory);
            }
        }	
           
	}	
	
	return 0;
}


// Function to open the file and call to function to fill data in this file.
int handlingFile(const char *fileName, struct Inventory *inventory) {
    FILE *filePointer = fopen(fileName, "r");
    if (filePointer == NULL) {
		printf("jsonfile name : %s strlen = %d\n", fileName, strlen(fileName));
        perror("json File opening failed ");
        return 1;
    }

    char *token;
	struct Item *item = malloc(sizeof(struct Item));
    if (item == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    // Parse "name"
    token = jsonVal(filePointer, "name");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'name' key in JSON.\n");
        return 1;
    }
    if (allocateAndCheck((void **)&item->name, strlen(token) + 1, "Memory allocation failed.") != 0) {
        return 1;
    }
    strcpy(item->name, token); // put in the keyword inside of the struct
    printf("name = %s\n", item->name);
	
    // Parse "quantity"
    token = jsonNum(filePointer, "quantity");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'quantity' key in JSON.\n");
        return 1;
	}
	item->cost.quantity = atoi(token);
	
    printf("quantity = %d\n", item->cost.quantity);
	
    // Parse "unit"
    token = jsonVal(filePointer, "unit");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'unit' key in JSON.\n");
        return 1;
    }
    strcpy(item->cost.unit, token);
    printf("unit = %s\n", item->cost.unit);

    // Parse "weight"
    token = jsonNum(filePointer, "weight");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'weight' key in JSON.\n");
        return 1;
    }
    item->weight = atof(token);
    printf("weight = %.2f\n", item->weight);

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
char * jsonNum(FILE *filePointer, const char *keyWord) {
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