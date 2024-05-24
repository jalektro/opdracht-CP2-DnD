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
	int itemCount;
    double weight;
	struct Cost cost;
	struct Item *next;
};

struct Inventory {
    struct Item *head;
	double maxWeight;
    struct Cost coins[MAX_COIN_TYPES];
};

int allocateAndCheck(void **ptr, size_t size, const char *errorMessage);
int handlingFile(const char *fileName, struct Inventory *inventory, int itemCount);
char *jsonVal(FILE *filePointer, const char *keyWord);
char *jsonNum(FILE *filePointer, const char *keyWord);
void addItem(struct Inventory *inventory, struct Item *newItem);
void displayInventory(struct Inventory *inventory) ;
void freeInventory(struct Inventory *inventory);
int isValidCoin(const char* unit);

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
			printf("Coins : \n");
			for( int j =0 ; j< MAX_COIN_TYPES && i+1 <argc; j++)  
			{	
			int qty;
			char unit[3];
				if (sscanf(argv[i+1], "%d%2s", &qty, unit)==2 && isValidCoin(unit )) {  //also chekc iff valid coin and not quantity of item.
					inventory.coins[j].quantity = qty;
					strcpy(inventory.coins[j].unit,unit);
			
				printf("%d%s\n", inventory.coins[j].quantity, inventory.coins[j].unit);
				i++;
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
		
       
            if (i + 1 < argc && argv[i][0] >= '0' && argv[i][0] <= '9') {                    
                handlingFile(argv[i+1], &inventory,atoi(argv[i]));
				i++;
            } else {
				handlingFile(argv[i], &inventory,1);					
			}           
		}	
	}
	displayInventory(&inventory);
	freeInventory(&inventory);
	
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


// Function to open the file and call to function to fill data in this file.
int handlingFile(const char *fileName, struct Inventory *inventory, int itemCount) {
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
	
	item->itemCount = itemCount;  // put aantal in the struct 

    // Parse "name"
    token = jsonVal(filePointer, "name");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'name' key in JSON.\n");
        return 1;
    }
   
    strcpy(item->name, token); // put in the keyword inside of the struct
   // printf("name = %s\n", item->name);
	
    // Parse "quantity"
    token = jsonNum(filePointer, "quantity");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'quantity' key in JSON.\n");
        return 1;
	}
	item->cost.quantity = atoi(token);
	
   // printf("quantity = %d\n", item->cost.quantity);
	
    // Parse "unit"
    token = jsonVal(filePointer, "unit");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'unit' key in JSON.\n");
        return 1;
    }
    strcpy(item->cost.unit, token);
   // printf("unit = %s\n", item->cost.unit);

    // Parse "weight"
    token = jsonNum(filePointer, "weight");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'weight' key in JSON.\n");
        return 1;
    }
    item->weight = atof(token);
    //printf("weight = %.2f\n", item->weight);

	item->next = NULL;

    addItem(inventory, item);

    fclose(filePointer);
    return 0;
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

//function to verify that the coin is valid or it's a itemCount
int isValidCoin(const char *unit){
return strcmp(unit, "cp") == 0 || strcmp(unit, "sp") == 0 ||
       strcmp(unit, "ep") == 0 || strcmp(unit, "gp") == 0 ||
       strcmp(unit, "pp") == 0;
}

void displayInventory(struct Inventory *inventory) {
    struct Item *current = inventory->head;
    double totalWeight = 0;
    printf("Inventory:\n");
    while (current != NULL) {
        printf("Item: %s, Weight: %.2f, Cost: %d %s Quantity : %d\n", current->name, current->weight, current->cost.quantity, current->cost.unit , current->itemCount);
        totalWeight = (current->weight*current->itemCount) + totalWeight;
        current = current->next;
    }
    printf("Total Weight: %.2f\n", totalWeight);
    printf("Encumbered: %s\n", totalWeight > inventory->maxWeight ? "Yes" : "No");
}

void freeInventory(struct Inventory *inventory) {
    struct Item *current = inventory->head;
    while (current != NULL) {
        struct Item *next = current->next;
        free(current);
        current = next;
    }
    inventory->head = NULL;
}

void addItem(struct Inventory *inventory, struct Item *newItem) {
    if (inventory->head == NULL) {
        inventory->head = newItem;
    } else {
        struct Item *current = inventory->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newItem;
    }
}
