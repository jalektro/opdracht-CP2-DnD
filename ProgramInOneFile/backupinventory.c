#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 40
#define MAX_COIN_TYPES 5

double totalWeight = 0;

enum placeItem{
BAGPACK,
CAMP
};
struct Cost {
    char unit[3];
    int quantity;
};

struct Item {
    char name[50];
	int itemCount;
    double weight;
	struct Cost cost;
	enum placeItem place;
	struct Item *prev;
	struct Item *next;
};

struct Inventory {
    double maxWeight;
	char *campFile;
    struct Cost coins[MAX_COIN_TYPES];
	struct Item *items;
};


int handlingFile(const char *fileName, struct Inventory *inventory, int itemCount);
char *jsonVal(FILE *filePointer, const char *keyWord);
char *jsonNum(FILE *filePointer, const char *keyWord);
void addItem(struct Inventory *inventory, struct Item *newItem);
void displayInventory(struct Inventory *inventory , struct Item *current) ;
void freeInventory(struct Inventory *inventory);
int isValidCoin(const char* unit);
void clearScreen();
void printInterface();
void interfaceClient(struct Inventory *inventory, struct Item **currentItem) ;
void writeCampLog(const char *campFile, struct Item *item) ;
void removeCampLog(const char *campFile, const char *itemName) ;

//Inventory.exe -w 180.75 -m 4gp 42sp 69cp greatsword.json explorers-pack.json small-knife.json 2 waterskin.json leather-armor.json -c camp.log
int main(int argc, char* argv[])
{
  struct Inventory inventory= {0};
  int i;
  
	if (argc <= 2) {
	printf("Usage: %s equipment-files [number-of-items] [-w max-weight] [-m money] [-c camp-file]\n", argv[0]);
	return 1;
	}

	for(i=1; i< argc; i++)
	{ 		
		if (strcmp(argv[i],"-w") == 0)					//handle max_weight
		{
			inventory.maxWeight= atof(argv[++i]);
			printf("The player can carry max : %.2f \n", inventory.maxWeight);
		} else if (strcmp(argv[i],"-m") == 0)			//handle coins
		{
			printf("The player has this many coins in his pocket: ");
			for( int j =0 ; j< MAX_COIN_TYPES && i+1 <argc; j++)  
			{	
			int qty;
			char unit[3];
				if (sscanf(argv[i+1], "%d%2s", &qty, unit)==2 && isValidCoin(unit )) {  //also chekc iff valid coin and not quantity of item.
					inventory.coins[j].quantity = qty;
					strcpy(inventory.coins[j].unit,unit);
			
				printf("%d%s\t", inventory.coins[j].quantity, inventory.coins[j].unit);
				i++;
				} else {
				break;
				}
			}
			printf("\n"); //add new line after coins. 

			
		} else if (strcmp(argv[i],"-c") == 0)			//handle camp file
		{
	
			inventory.campFile=argv[++i];

			FILE *campFilePtr = fopen( inventory.campFile, "a");		//open campfile for appending
				if (inventory.campFile == NULL){
				printf("Unable to open file: %s\n", inventory.campFile);
                continue;
				}
				//printf("campfile %s opened and closed\n",inventory.campFile);
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
	printf("Press a key to continue.");
	getchar();
	
	displayInventory(&inventory, inventory.items);
	freeInventory(&inventory);
	
	return 0;
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
	item->place = BAGPACK; //item is standard in the bagpack and can changed later on.
	item->itemCount = itemCount;  // put aantal in the struct 

    // Parse "name"
    token = jsonVal(filePointer, "name");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'name' key in JSON.\n");
        return 1;
    }   
    strcpy(item->name, token); // put in the keyword inside of the struct
 	
    // Parse "quantity"
    token = jsonNum(filePointer, "quantity");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'quantity' key in JSON.\n");
        return 1;
	}
	item->cost.quantity = atoi(token);
	
    // Parse "unit"
    token = jsonVal(filePointer, "unit");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'unit' key in JSON.\n");
        return 1;
    }
    strcpy(item->cost.unit, token);
 
    // Parse "weight"
    token = jsonNum(filePointer, "weight");
    if (token == NULL) {
        fprintf(stderr, "Error: Could not find 'weight' key in JSON.\n");
        return 1;
    }
    item->weight = atof(token);
   
	item->next = NULL;
	item->prev = NULL;
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

void clearScreen(){
	system("cls");
}

void displayInventory(struct Inventory *inventory, struct Item *current) {  
    printf("Inventory:\n");
    
	   if (current == NULL) {
        printf("Inventory is empty.\n");
        return;
    }
	struct Item *start = current;
	do{
		clearScreen();
		printf("The player can carry max : %.2f \n", inventory->maxWeight);
		printf("The player has this many coins in his pocket: ");
		for(int j = 0; j<MAX_COIN_TYPES; j++){
			if(inventory->coins[j].quantity!=0){
			printf("%d%s\t", inventory->coins[j].quantity, inventory->coins[j].unit);
			} else {
				break;
			}
		}		
        printf("\n\nItem: %s\nWeight: %.2f\nCost: %d %s\nQuantity : %d\nPlace: %s\n\n",
		current->name, current->weight, current->cost.quantity, current->cost.unit , current->itemCount, current->place == BAGPACK ? "BAGPACK": "CAMP");
		if (current->place == BAGPACK){
		totalWeight = (current->weight*current->itemCount) + totalWeight;		
		printf("Total Weight in backpack: %.2f\n", totalWeight);
		printf("Encumbered: %s\n", totalWeight > inventory->maxWeight ? "Yes" : "No");
		} else {
			printf("Total Weight in backpack: %.2f\n", totalWeight);
		printf("Encumbered: %s\n", totalWeight > inventory->maxWeight ? "Yes" : "No");
		}
		
		printInterface();
		interfaceClient(inventory, &current);
		
       /* totalWeight = (current->weight*current->itemCount) + totalWeight;		
		printf("Total Weight in backpack: %.2f\n", totalWeight);
		printf("Encumbered: %s\n", totalWeight > inventory->maxWeight ? "Yes" : "No");
        current = current->next;	*/	
    } while (current != start);  
}

void freeInventory(struct Inventory *inventory) {
      if (inventory->items == NULL) {
        return;
    }

    struct Item *current = inventory->items;
      do {
        struct Item *next = current->next;
        free(current);
        current = next;
    } while (current != inventory->items);

    inventory->items = NULL;
	exit(0);
}

void addItem(struct Inventory *inventory, struct Item *newItem) {
    if (inventory->items == NULL) {
        inventory->items = newItem;
		newItem->next = newItem;
		newItem->prev = newItem;
    } else {
        struct Item *last = inventory->items->prev;
      last->next = newItem;
	  newItem->prev = last;
	  newItem->next = inventory->items;
	  inventory->items->prev = newItem;
    }
}


void printInterface(){
	
printf("What do you want to do with this item?\nYou can choose between these options: \n");
printf("1. Move item from player to camp.\n");
printf("2. Move item from camp to player.\n");
printf("3. Go to the next item.\n");
printf("4. Go to the previous item.\n");
printf("5. quit the inventory.\n");
}

void interfaceClient(struct Inventory *inventory, struct Item **currentItem) {
    int choice;
	FILE *campFilePtr;
	
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                if ((*currentItem)->place == BAGPACK) {
                    (*currentItem)->place = CAMP;
                    printf("Item moved to camp.\n");
					writeCampLog(inventory->campFile, *currentItem);				
					displayInventory(inventory, *currentItem);
					
                } else {
                    printf("Item is already in camp.\n");
					displayInventory(inventory, *currentItem);
                }
                break;
            case 2:
                if ((*currentItem)->place == CAMP) {
                    (*currentItem)->place = BAGPACK;
                    printf("Item moved to bagpack.\n");
					removeCampLog(inventory->campFile, (*currentItem)->name);
					
		
                } else {
                    printf("Item is already in the bagpack.\n");
					displayInventory(inventory, *currentItem);
                }
                break;
            case 3:
				*currentItem = (*currentItem)->next;
				displayInventory(inventory, *currentItem);
                return; 
            case 4:
				*currentItem = (*currentItem)->prev;
				displayInventory(inventory, *currentItem);
                return;
            case 5:
			freeInventory(inventory);
               return;
            default:
                printf("Invalid choice. Please try again.\n");
        }	
    
}

void writeCampLog(const char *campFile, struct Item *item) {
    FILE *filePointer = fopen(campFile, "a");
    if (filePointer == NULL) {
        perror("Failed to open camp log file for writing");
        return;
    }

    fprintf(filePointer, "Name: %s, Weight: %.2f, Cost: %d %s, Quantity: %d\n",
            item->name, item->weight, item->cost.quantity, item->cost.unit, item->itemCount);
    fclose(filePointer);
}

void removeCampLog(const char *campFile, const char *itemName) {
    FILE *filePointer = fopen(campFile, "r");
    FILE *tempFilePointer = fopen("temp.log", "a");
    if (filePointer == NULL || tempFilePointer == NULL) {
        perror("Failed to open camp log file for reading/writing");
        if (filePointer != NULL) fclose(filePointer);
        if (tempFilePointer != NULL) fclose(tempFilePointer);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), filePointer) != NULL) {
        if (strstr(buffer, itemName) == NULL) {
            fputs(buffer, tempFilePointer);
        }
    }

    fclose(filePointer);
    fclose(tempFilePointer);

    remove(campFile);
    rename("temp.log", campFile);
}