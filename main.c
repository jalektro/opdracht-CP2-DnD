#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"

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