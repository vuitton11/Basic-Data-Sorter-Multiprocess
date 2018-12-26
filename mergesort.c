#define SIZE_MAX 10000
#define NUM_ROW 90000

/*Global vars*/
char headerRow[SIZE_MAX]; //Strores headers 
int totalRows; //Keeps track of number of rows
int totalCols; //Keeps track of number of columns
int tH; //Keeps index of the target col that is to be sorted
//int RowCounter; //Keeps track of how many rows there are
//int firstCounter = 0; //Used to keep track of when to print a comma for the PIDs


/*A struct that contains fields for the row and data 
 *that is going to be sorted in the CSV file
 */
typedef struct data{
	char rowHolder[SIZE_MAX]; //store row
	char* sData; //data that is sorting 
} data;


/*Takes in data* array, first index position, last index position, middle index position, and data type(int)
 *Checks data type then converts to such type(default is a string) then compares and combines the array.
 *Places stored array back into inputted array
 */
void mergeSort(data* array, int lIndex, int hIndex, int mIndex, int dataType){
    int i, j, lptr = lIndex, rptr = (mIndex + 1);
    data* newArray = malloc(totalRows * sizeof(data));
    for(i = lIndex; (lptr <= mIndex) && (rptr <= hIndex); i++){
        //printf("array[lptr].sData = %s\n", array[lptr].sData);
        //printf("array[rptr].sData = %s\n", array[rptr].sData);
        switch(dataType){
            case 1: //dealing with strings
                if(strcmp(array[lptr].sData, array[rptr].sData) <= 0){ //Uses strcmp to compare which string is less then
                    newArray[i] = array[lptr];
                    lptr++;
                }else{
                    newArray[i] = array[rptr];
                    rptr++;
                }
                break;
            case 2: //dealing with ints
                /*if(strcmp(array[lptr].sData, "") == 0){
                    strcpy(array[lptr].sData, "0");
                }
                if(strcmp(array[rptr].sData, "") == 0){
                    strcpy(array[rptr].sData, "0");
                }*/
                if((atoi(array[lptr].sData)) <= (atoi(array[rptr].sData))){ //converts the string data into ints, then compares them using boolean expression
                    newArray[i] = array[lptr];
                    lptr++;
                }else{ // sData > sData
                    newArray[i] = array[rptr];
                    rptr++;
                }
                break;
            case 3: //dealing with doubles
                if((atof(array[lptr].sData)) <= (atof(array[rptr].sData))){ //converts the string into floats, then compares them using boolean experssion
                    newArray[i] = array[lptr];
                    lptr++;
                }else{ // sData > sData
                    newArray[i] = array[rptr];
                    rptr++;
                }
                break;
        }
    }
    //Places the remaining index of the array into the new array
    for(j = lptr; j <= mIndex; j++){ //PLaces the remaining index of the array into the new array
        newArray[i] = array[j];
        i++;
    }

    //Places the remaining index of the array into the new array
    for(j = rptr; j <= hIndex; j++){ 
        newArray[i] = array[j];
        i++;
    }

    //Now stores the sorted array into the original array so its easier to work with void then return the array
    for(i = lIndex; i <= hIndex; i++){ 
        array[i] = newArray[i];
    }
}


/*Data type 1 = string 
 *Data type 2 = int
 *Data type 3 = float
 *Sets up the array for merge sort
 *Finds middle index and then splits the array
 */
void startMergeSort(data* array, int lIndex, int hIndex, int dataType){
   //printf("In function startMergeSort\n");
   if (lIndex < hIndex) {
      int mIndex = ((hIndex + lIndex) / 2); //Finds the middle index of the array
      startMergeSort(array, lIndex, mIndex, dataType);
      startMergeSort(array, mIndex+1, hIndex, dataType);
      mergeSort(array, lIndex, hIndex, mIndex, dataType); //starts actually comparing the data and sorts them
   }
}