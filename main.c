#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct item{
	char *name;	
	struct coin_type{
		int amount;
		char type[3];
		}coin;
	double weight;
	struct item *nextItem;
}itemList[20]; //max 20 items


/*struct inventory{
	struct item;
	struct */

int main( int argc, char* argv[]) 
{
	
	printf("compiler works\n");
	
	char buffer[1024] ;
	//char *parsing ; //helping for strsep function
	
	FILE *filePointer = fopen("wich file to open argv", "r");  //check in the list argv wich file i need to open.
	if (filePointer == NULL) // check if file is succesful opened.
	{
		perror("File openening failed");
		return 1;
	}
	
	while (!feof(filePointer)) // Keep reading file till EndOfFile is reached
    {
        if (fgets(buffer, sizeof(buffer), filePointer) == 
            NULL) // Read one line (stops on newline or eof), will return NULL on
            // eof or fail
        {
            fclose( filePointer );
            break; // Stop reading
        }
		
		
		char *token = strtok(buffer, "\"");
//printf("token: %s\n", token);
        while (token) // If token exists
        {
            token = strtok(NULL, "\""); // Find next token
//  printf("token: %s\n", token);
            if (token != NULL) {
                if (strcmp(token, "name") == 0) {
					 token = strtok(NULL, "\"");
					 if (token != NULL){
						  token = strtok(NULL, "\"");
					 }
					 if (token != NULL){
						itemList->name =  (char*)malloc(strlen(token)+1);
						if(itemList->name == NULL){
							printf("Memory allocation failed.\n");
							return 1;
						}
				printf("token = %s\n", token);
				
						strcpy(itemList->name, token);
				printf("name = %s\n",itemList->name);
						}
				}
            }
        }
    }
    
	
	return 0;
}