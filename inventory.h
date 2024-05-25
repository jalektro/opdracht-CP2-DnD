/*
to run and compile the code use these commands.
gcc inventory.c main.c -o inventory.exe
Inventory.exe -w 180.75 -m 4gp 42sp 69cp greatsword.json small-knife.json torch.json 2 waterskin.json wooden-staff.json yew-wand.json -c camp.log

extra note: the clear screen is a windows command for CMD. --> CLS
*/
#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdio.h>

#define MAX_ITEMS 40
#define MAX_COIN_TYPES 5

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

void freeItem(struct Item *item);
struct Item *allocateItem();


int isValidCoin(const char* unit);
void clearScreen();
void printInterface();
void interfaceClient(struct Inventory *inventory, struct Item **currentItem) ;
void writeCampLog(const char *campFile, struct Item *item) ;
void removeCampLog(const char *campFile, const char *itemName) ;

#endif  