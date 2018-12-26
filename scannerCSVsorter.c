#include "scannerCSVsorter.h"

int main(int argc, char* argv[]){

	int numOfProc = 0;

	//Allocating space
	char * workDir = (char *) malloc(50 * sizeof(char));
	char* outputDir = (char *) malloc(50 * sizeof(char));
	char* thisDir = (char*) malloc(100 * sizeof(char));

	strcpy(workDir, ".");
	strcpy(outputDir, ".");

	/*We are given these headers to work with -> these are all valid headers
	 *Used to compare with input headers
	 */
	char* headers[] = {"color", "director_name", "num_critic_for_reviews", "duration",
	"director_facebook_likes", "actor_3_facebook_likes", "actor_2_name",
	"actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title",
	"num_voted_users", "cast_total_facebook_likes", "actor_3_name", "facenumber_in_poster",
	"plot_keywords", "movie_imdb_link", "num_user_for_reviews", "language", "country",
	"content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score",
	"aspect_ratio", "movie_facebook_likes"};

	char* cHeader; //sorting target header

	char tempVar[300];
	//Obtained the current directory we are in
	if(getcwd((tempVar), sizeof(tempVar)) != NULL){
		strcpy(thisDir, tempVar);
	}

	/*Error Checking*/
	//if(errorCheck(argc, argv, headers) == 1){exit(0); } //Used for old code
	if(argc <= 2){ //Not enough arguments
		printf("Error! Not enough arguments\n");
		exit(0);
	}
	else if(argc == 3){	//Only one agrument -> if not a -c then its an error
		if(strcmp(argv[1], "-c") != 0){
			printf("column parameter is missing\n");
			exit(0);
		}else{
			cHeader = argv[2];
		}
	}else if(argc == 4 || argc == 6){ //If given -o or -c or -d doesnt have a feild after it
		printf("Error! Flag is missing flied\n");
		exit(0);
	}
	else{ //Checks if for new flags given -> 2 or 3 flags -> -o and/or -d. However -c is a must 
		int flag = 0; //Keeps track of what flags we've seen by adding the values. c = 1, d = 2, o = 3. 
		int outCount = 0; //Keeps track if we are given an output directory = 1, 0 if none is given
		char* outDir; //used as a temp var
		if(argc > 4){ // 2 flags are present
			if(((strcmp(argv[1], "-c") != 0) && (strcmp(argv[3], "-c")!= 0)) && (argc <= 5)){ //-c isnt present
			 	printf("Error! column parameter is missing\n");
			 	exit(0);
			 }
			if(strcmp(argv[1], "-c") == 0){ //First flag is -c
				flag = flag + 1; //seen -c flag
				cHeader = argv[2];
			}
			if(strcmp(argv[1], "-d") == 0){ //First flag is -d
				flag = flag + 2; //seen -d flag
				strcpy(workDir, argv[2]);
			}
			if(strcmp(argv[1], "-o") == 0){ //First flag is -o
				flag = flag + 3; //seen -o flag
				outDir = argv[2];
				outCount = 1; //-o flag is present

			}
			if(strcmp(argv[3], "-c") == 0){ //Second flag is -c
				flag = flag + 1; //seen -c flag
				cHeader = argv[4];
			}
			if(strcmp(argv[3], "-d") == 0){ //Second flag is -d
				flag = flag + 2;//seen -d flag
				strcpy(workDir, argv[4]);
			}
			if(strcmp(argv[3], "-o") == 0){ //Second flag is -o
				flag = flag + 3;//seen -o flag
				outDir = argv[4];
				outCount = 1; //present
			}
		}
		if(argc > 5){ //we have a third flag present
			switch((6 - flag)){ //the total of each flag add - the sum of the 2 present = the flag we havent seen
				case 1:	//Looking for -c
					if(strcmp(argv[5], "-c") != 0){
						printf("Error!\n");
						exit(0);
					}else{
						cHeader = argv[6];
					}
					break;
				case 2:	//Looking for -d
					if(strcmp(argv[5], "-d") != 0){
						printf("Error!\n");
						exit(0);
					}else{
						strcpy(workDir, argv[6]);
					}					
					break;
				case 3: //Looking for -o
					if(strcmp(argv[5], "-o") != 0){
						printf("Error!\n");
						exit(0);
					}else{
						outDir = argv[6];
						outCount = 1;
					}					
					break;
			}
		}
		if(outCount == 1){ //If given an output directory
			if(outDir[0] == '/'){ //dealing with absolute Path
				strcpy(outputDir, outDir);
			}else{
				strcat(thisDir, "/");
				strcat(thisDir, outDir);
				strcpy(outputDir, thisDir);
			}
		}
	}

	//Checking if output directory exist
	DIR* dirdir = opendir(outputDir);
	if(dirdir){ //Directory exist
		//printf("output direct %s exist\n", outputDir);
		closedir(dirdir); 
	}else{ //Directory does not exist
		printf("Error! Directory does not exist\n");
		exit(0);
	}

	//Checking header
	int i;
	for(i = 0; i < 28; i++){ //checks to see if the target header is valid by comparing it to all valid headers
		if(strcmp(cHeader, headers[i]) == 0){ //Found valid header
			//printf("Found\n");
			break;
		}
		else if((strcmp(argv[2], headers[i]) != 0) && i == 27){
			printf("Error! Not a valid header\n");
			exit(0);
		}
	}

	//printf("%s\n", cHeader);
	//printf("%s\n", workDir);
	//printf("%s\n", outputDir);
	/*Error Checking Done*/

	/*Format:
	Initial PID: XXXXX
	PIDS of all child processes: AAA,BBB,CCC,DDD,EEE,FFF, etc
	Total number of processes: ZZZZZ*/

	printf("Initial PID: %d\n", getpid());
	printf("PIDS of all child processes: ");
	fflush(stdout);
	int end = (mulitProc(cHeader, workDir, outputDir, headers) + 1);
	freeEnding(thisDir, outputDir, workDir);
	printf("\n");
	printf("Total number of Processes: %d\n", end);


	/*Forking*/


	/*Old stuff -> Asst0*/
	///FILE* fp = headerCheck(headers, argv[2]);

	//FILE* fp = stdin; //Debugging
	
	///if(fp == NULL){exit(0);}
	
	//printf("Done\n");

	///data* array = readCSV(fp); //Read csv file and inputs the data into the struct 

	///data* newArr = malloc((totalRows) * sizeof(data)); //get an array of exact size
	///newArr = array; //transfer contents of old array into new array
	
	//printf("Done \n");

	///deterType(newArr, argv[2]);
	
	//printf("pass6\n");
	
	///printArray(newArr);
	
	//printf("pass7\n");
	
	///free(array); //free original array which free all arrays
	//printf("pass8\n");
	return 0;
}