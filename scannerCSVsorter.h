#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include "mergesort.c"

/*Takes a parameter of a string which is our header row
 *Goes through the header to check if we have more than 28 header
 *If so returns 1 as invaild
 *else returns 0 -> valid
 */
int validHeaderCheck(char* headers){
	char* ptr = strtok(headers, ","); //Breaks up the strings using the commas as a delimiter
	int counter = 0;
	while(ptr != NULL){
		counter++;
		ptr = strtok(NULL, ","); //Next part of the string is broken up
	}
	if(counter > 28){
		return 1;
	}else{
		return 0;
	}
}

/*Takes in char array of headers and target header and a FILE ptr
 *Compares input headers to expected headers
 *Reads from stdin and takes in the first row which is just the header columns
 *Calcuates total number of cols and keeps track where of the position of target column header
 *Returns NULL if headers are not a match. 
 *If a match returns the file pointer
 */
FILE* headerCheck(char* array[], char *col, FILE* ptr1){
	FILE* fp = ptr1;
	fscanf(fp, "%s\n", headerRow);
	int i, j = 0;
	char* temp = strdup(headerRow);
	char* temp2 = strdup(headerRow);
	int valid = validHeaderCheck(temp2);
	if(valid == 1){
		free(temp);
		free(temp2);
		return NULL;
	} 
	char* ptr = strtok(temp, ","); //Breaks up the strings using the commas as a delimiter
	while(ptr != NULL){
		for(i = 0; i < 28; i++){ //Iterates through the headers to see if this column is valid
			if(strcmp(array[i], ptr) == 0 && strcmp(ptr, col) == 0){ //If target col is found
				//printf("Found target header\n");
				tH = j;
				//printf("Target column index%d\n", tH);
				j++;
				break;
			}
			if(strcmp(array[i], ptr) == 0){ //If col is found
				j++;
				break;
			}
			if((strcmp(array[i], ptr) != 0) && i == 27){ //If the header wasnt found in the valid list
				free(temp);
				//printf("Invalid header\n");
				return NULL;
			}
		}
		ptr = strtok(NULL, ","); //Next part of the string is broken up
	}
	free(temp); //Frees the memory of the buffer string
	free(temp2);
	//printf("Header names: \n%s\n", headerRow);
	totalCols = j;
	//printf("Total headers = %d\n", j);
	return fp;
}

/*Takes in a string
 *Checks for extra whitespaces and gets rid of them along with leading or ending whitespaces
 *Returns string
 */
char* fixString(char* buffer){
	//printf("Inside fixString\n");
	int i, j;
	int length = strlen(buffer);
	if(length == 0){return buffer;} //Empty string
	//printf("%d\n", length);
	
	//Gets rid of extra spaces within the beginning and middle of the string
  	for(i=j=0; i < length; ++i){
    	if(isspace(buffer[i]) == 0 || ((isspace(buffer[i-1]) == 0) && i > 0)){
      		//printf("%c\n", buffer[i]);
      		buffer[j] = buffer[i];
      		//printf("%c\n", buffer[j]);
      		j++;
    	}
  	}
  	buffer[j] = '\0'; //adds null terminator to the ebd of the string 

  	//printf("pass part 1\n");

	 /*Whitespace - 4 different cases:
	 *1: whitespace in front of the string -> return stripped of space
	 *2: whitespace in the end of the string -> ^^^
	 *3: whitespaces in both ends of the string -> ^^^
	 *else return string
	 */

	//case 1
	if(buffer[0] == ' ' && buffer[length-1] != ' '){
		buffer++;
		return buffer;
	}

	//case 2
	else if(buffer[0] != ' ' && buffer[length-1] == ' '){
		buffer[length-1] = 0;
		return buffer;
	}

	//case 3
	else if(buffer[0] == ' ' && buffer[length-1] == ' '){
		buffer++;
		buffer[length-1] = 0;
		return buffer;
	}

	else{return buffer;} //no whitespaces
}

/*Takes in a fp
 *fgets each row then places row into a field in struct
 *uses 2 methods to find correct col:
 *1. no quote check -> no double quotes
 *2. quote checke -> ignores commas in double quotes
 *places the column data into a field in struct called .sData
 *returns data* array
 */
data* readCSV(FILE* fp){
	char buffer[SIZE_MAX]; //Buffer
	char delim[2] = ","; //Delimiter
	char* dummyString, *tempS; //temp strings used for manipulating strings
	char comma = ',', quote = '\"';
	data* row = malloc(NUM_ROW * sizeof(struct data)); //allocate memory for struct -> not sure of size so i used a really big number
	int i = 0, commaCounter, index, quoteComma;
	//RowCounter = 0; 
	while(fgets(buffer, SIZE_MAX, fp) != NULL){ //Reads stdin row by row
		//printf("\n\n");
		//RowCounter++;
		strcpy(row[i].rowHolder, buffer); //Places the entire row in this field
		//printf("Row - %d\n", i+1);
		//printf("row[%d].rowHolder = %s", i, row[i].rowHolder);
		
		/*Checks for the target col data by checking the*/

		//Checks to see if there is any movie title with double quotes 
		tempS = strdup(buffer); //copies str into a new string so the original string isnt being manipulated
		if(strchr(tempS, quote) == NULL){ //no commas to worry in movie title -> no quote checker here :)
			commaCounter = 0;
			while(commaCounter < tH+1){ //Iterates through the row. Only checks commmas before the tagert header, we arent worried about commas after
				dummyString = strsep(&tempS, delim); //strip string
				if(commaCounter == tH){ //Target header is found
					row[i].sData = malloc((strlen(dummyString)+1) *sizeof(char)); //malloc enough space for string to avoid seg fault
					strcpy(row[i].sData, fixString(dummyString)); //places data of the target column into struct
					//printf("row[%d].sData = %s", i, row[i].sData);
					break;	
				}
				commaCounter++;
			}
		}else{ //quote checker is in place
			commaCounter = 0;
			index = 0;
			quoteComma = 0;
			while(index < strlen(tempS)){
				if(tempS[index] == comma){
					if(quoteComma == 1){ //If quote is found
						//printf("found commma in quotes\n");
						tempS[index] = ' '; //replaces comma in quotes with space, fixstring will get rid of the space
					}
				}
				else if(tempS[index] == quote){ //quote found
					//printf("Found quote\n");
					tempS[index] = ' '; //replaces double quote with a space.
					quoteComma++;
				}
				index++;
			}
			//repeats no quote checker step
			commaCounter = 0;
			//printf("String = %s\n", tempS);
			//printf("tH = %d\n", tH);
			while(commaCounter < tH+1){ 
				dummyString = strsep(&tempS, delim);
				//printf("dummyString = %s\n", dummyString);
				//printf("commaCounter = %d\n", commaCounter);
				if(commaCounter == tH){
					row[i].sData = malloc((strlen(dummyString)+1) *sizeof(char)); //malloc enough space for string to avoid seg fault
					strcpy(row[i].sData, fixString(dummyString));
					//printf("row[%d].sData = %s", i, row[i].sData);
					break;	
				}
				commaCounter++;
			}
		}
		i++; //row increase
	}
	totalRows = i; //Keeps track of all rows so we know how much space to allocate exactly
	return row;
}

/*Takes in data* array, and target column
 *Determines what kind of type of data to sort by determining what type the target header is
 */
void deterType(data* array, char *targetHeader){
	if(strcmp(targetHeader, "color") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1); 
	}
	else if(strcmp(targetHeader, "director_name") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1); 
	}
	else if(strcmp(targetHeader, "num_critic_for_reviews") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "duration") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "director_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "actor_3_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "actor_2_name") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "actor_1_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "gross") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "genres") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "actor_1_name") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "movie_title") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "num_voted_users") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "cast_total_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "actor_3_name") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "facenumber_in_poster") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "plot_keywords") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "movie_imdb_link") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "num_user_for_reviews") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "language") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "country") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "content_rating") == 0){ //string
		startMergeSort(array, 0, totalRows-1, 1);
	}
	else if(strcmp(targetHeader, "budget") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "title_year") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "actor_2_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
	else if(strcmp(targetHeader, "imdb_score") == 0){ //float
		startMergeSort(array, 0, totalRows - 1, 3);
	}
	else if(strcmp(targetHeader, "aspect_ratio") == 0){ //float
		startMergeSort(array, 0, totalRows - 1, 3);
	}
	else if(strcmp(targetHeader, "movie_facebook_likes") == 0){ //int
		startMergeSort(array, 0, totalRows - 1, 2);
	}
}

/* Takes in the data and file pointer
 * Writes the info into the new file 
 *Prints out the header and then the sorted array with the original values
 */
void printArray(data* node, FILE* fp){
	int i;
	fprintf(fp,"%s\n", headerRow);
	for(i = 0; i < totalRows; i++){
		//printf("node[%d].sData = \"%s\"\n", i, node[i].sData); //Debugging -> prints out sData
		fprintf(fp, "%s", node[i].rowHolder);
		//if(i == totalRows-1){printf("\n");}
		//printf("------------------------------------------------------\n");
	}
	fclose(fp);		
}

/*Takes in a string 
 *Checks the file extension(last 4 characters) to see if it is a CSV file
 *returns 0 if true
 *else returns 1*/
int validCSVfile(char* fileName){
	int length = strlen(fileName);
	const char* fileExtension = &fileName[length - 4]; //Grabs the last 4 chars of the file name which should the .extension
	//printf("File extension = %s\n", fileExtension);
	if(strcmp(fileExtension, ".csv") == 0){
		//printf("Match\n");
		return 0;
	}
	return 1;
}

/*Takes in a string 
 *Checks if the csv File is sorted but searching for the substring "-sorted-"
 *returns 0 if it is a sorted file
 *else returns 1*/
int isSortedFile(char* fileName){
	//printf("fileName in function = %s\n", fileName);
	char* sorted = "-sorted-";
	int x = strlen(fileName);
	int y = strlen(sorted);
	//printf("x = %d && y = %d\n", x, y);
	int i,j;
	int total = x-y;
	//printf("total = %d\n", total);
	//nested for-loop is going through the array looking for a substring "-sorted-" to avoid sorting it again
	for(i = 0; i <= total; i++){
		for(j = 0; j < y; j++){
			if(sorted[j] != fileName[i+j]){
				break;
			}
		}
		if(j == y){
			return 0;
		}
	}
	return 1;
}

/*Takes in the filename, fieldName, output directory
 *Combines the string together to form the sorted file path
 *returns that string
 */
char* newNameFileCreator(char* fileName, char* fieldName, char* outDir){
	char* sorted = "-sorted-";
	char* extension = ".csv";
	int total = strlen(fileName) + strlen(sorted) + strlen(fieldName) + strlen(outDir) + 1; //extension is included in the fileName
	char* sortedFile = (char*) malloc(total * sizeof(char));
	char* temp = fileName;
	temp[strlen(temp)- 4] = 0; //Gets rid of the .csv attached to the end of the string
	if(outDir[0] != '/'){
		strcpy(sortedFile, temp);
		//printf("String1 = %s\n", sortedFile);
		strcat(sortedFile, sorted);
		//printf("String1 = %s\n", sortedFile);
		strcat(sortedFile, fieldName);
		//printf("String1 = %s\n", sortedFile);
		strcat(sortedFile, extension);
		//printf("String1 = %s\n", sortedFile);
	}else{
		strcpy(sortedFile, outDir);
		//printf("String2 = %s\n", sortedFile);
		strcat(sortedFile, "/");
		//printf("String2 = %s\n", sortedFile);
		strcat(sortedFile, temp);
		//printf("String2 = %s\n", sortedFile);
		strcat(sortedFile, sorted);
		//printf("String2 = %s\n", sortedFile);
		strcat(sortedFile, fieldName);
		//printf("String2 = %s\n", sortedFile);
		strcat(sortedFile, extension);
		//printf("String2 = %s\n", sortedFile);

	}
	//printf("Sorted filename is = %s\n", sortedFile);
	return sortedFile;
}

/*Takes in the target header, working directory and output directory and valid headers
 Recursive, finds a dircetory and recursives throu each directory, else it finds a file
 *Returns the number of processes made*/
int mulitProc(char* targetColumn, char* workDir, char* outDir, char* headers[]){
	DIR* direct= opendir(workDir);
	struct dirent * searchDir;
	pid_t cPID1, cPID2, pid;
	int stat, numOfProc = 0;
	char* cur = ".";
	char* pre = "..";
	char * tempHolder = (char*) malloc(40* sizeof(char));
	FILE* ptr;

	if(direct != NULL){
		while((searchDir = readdir(direct)) != NULL){
			if((strcmp(searchDir->d_name, cur) != 0) && (strcmp(searchDir->d_name, pre) != 0) && (searchDir->d_type == DT_DIR)){ //A directory is found
				//printf("This is it chief\n");
				cPID1 = fork(); //Directory is found so we fork
				/*Fork Cases:
				1) returns < 0 -> child process was unsuccesful
				2) returns == 0 -> child process successful
				3) returns > 0 -> returned to parent*/

				if(cPID1 < 0){ //Case 1
					//printf("Failed to fork\n");
					fprintf(stderr, "%s\n", "Falied to fork child process");
					exit(0);
				}
				else if(cPID1 == 0){ //Case 2
					numOfProc++;
					//printf("%s\n", searchDir->d_name);
					strcpy(tempHolder, searchDir->d_name);
					int status = chdir(workDir);
					if( status == 0){ //Changes to the working directory was successful
						numOfProc += mulitProc(targetColumn, tempHolder, outDir, headers);
					}else{
						fprintf(stderr, "%s\n", "Changing directory failed");
					}
					printf("%d,", getpid());
					/*if(numOfProc == 1){
						printf("%d,",getpid());
					}else{
						printf("%d", getpid());
					}*/
					//firstCounter = 1;
					fflush(stdout);
					_exit(1);
				}
				//free(tempHolder); double free error
			}else if(((strcmp(searchDir->d_name, cur) == 0) || (strcmp(searchDir->d_name, pre) == 0)) && (searchDir->d_type == DT_DIR)){
				//printf("This isnt it chief\n");
				continue;
			}else{ //Dealing with files
				//printf("Looking for file\n");
				//printf("File name = %s\n", searchDir->d_name);
				int status = chdir(workDir);
				if( status == 0){ //Changes to the working directory was successful
					cPID2 = fork();
					if(cPID2 < 0){
						//printf(Failed to fork\n);
						fprintf(stderr, "%s\n", "Failed to fork child process");
						exit(0);
					}
					else if(cPID2 == 0){
						numOfProc++;
						printf("%d,", getpid());
						/*if(numOfProc == 1){
							printf("%d,",getpid());
						}else{
							printf("%d", getpid());
						}*/

						//firstCounter = 1;
						/*Checks
					 	*1) Is it a csv file? checks last 4 chars is equal to ".csv"
					 	*2) Is it already sorted? contains "-sorted-"
						*3) Is a valid csv? contains proper headers and is more than a 1 row long(only includes headers)?
						 */

						//Case 1)
						int isCSV = validCSVfile(searchDir->d_name);
						if(isCSV != 0){
							//printf("case1\n");
							//printf("%d, ", getpid());
							//fflush(stdout);
							//exit(0);
							fflush(stdout);
							_exit(1);
						}
						
						//Case 2)
						int isSorted = isSortedFile(searchDir->d_name);
						//printf("case 2 output = %d\n", isSorted);
						if(isSorted == 0){
							//printf("case2\n");
							//fflush(stdout);
							//exit(0);
							fflush(stdout);
							_exit(1);						
						}

						//Case 3)
						//printf("case3\n");
						ptr = fopen(searchDir->d_name, "r");


						FILE* fp = headerCheck(headers, targetColumn, ptr);

							//FILE* fp = stdin; //Debugging
							
						if(fp == NULL){
							//exit(0);
							fflush(stdout);
							_exit(1);
						}
							
							//printf("Done\n");

						data* array = readCSV(fp); //Read csv file and inputs the data into the struct 
						fclose(fp);

						//New thing -> checks total rows
						if(totalRows == 0){ //only row in the csv file is the header
							free(array);
							fflush(stdout);
							_exit(1);
						}

						data* newArr = malloc((totalRows) * sizeof(data)); //get an array of exact size
						newArr = array; //transfer contents of old array into new array
							
							//printf("Done \n");

						deterType(newArr, targetColumn);

						char* sortedFileName = newNameFileCreator(searchDir->d_name, targetColumn, outDir);
						FILE* newPtr = fopen(sortedFileName, "w+"); //We are going to write data	
						printArray(newArr, newPtr);
							
							//printf("pass7\n");
							
							///free(array); //free original array which free all arrays
							
							//printf("pass8\n");

						fflush(stdout);
						_exit(1);
					}
				}else{
					fprintf(stderr, "%s\n", "Changing directory failed");
				}
					//printf("%d,", getpid());
				//fflush(stdout);
				//_exit(1);
			}
		}
		//pid_t pid;  // we wait for all the children to finsih 
		while ((pid = waitpid(-1, &stat, 0)) && pid != -1) {
			//printf("%d\n", pid);
			numOfProc += WEXITSTATUS(stat);
			
			if (errno == ECHILD) {// all is done
				break;
			}
		}
	}

	//Free memory used
	free(tempHolder);
	//Close directory
	closedir(direct);
	return numOfProc;
}

/*Frees all malloced space used in the main fuction*/
void freeEnding(char* stuff1, char* stuff2, char* stuff3){
	free(stuff1);
	free(stuff2);
	free(stuff3);
}