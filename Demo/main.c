/* Find duplicate emails of large set of data.  The following code will identify any duplicate email addresses 
 * from a given set of emails (up to 150000) and return all unique email addresses in the order in which they 
 * are first listed.  If any duplicate is found, the code will only keep the email that is first received.
 * 
 * The algorithm is as follows:  From the same directory, the algorithm will read in a file call "emails.txt".
 * As the emails are being read in, the code will format each email address into a structure consisting of the 
 * email and the position of the email.  Next, using merge sort, the code will sort the data in alphabetical order 
 * and discard any duplicate emails as they are found.  Finally, the code will sort the data again using merge sort, 
 * this time, using the position first assigned to them when they are read.  The output of this sort as well as
 * the processing time (excluding file reading and writing time since we are mostly concerned about the performance 
 * of the duplicate removal) are saved in the file "emailsOut.txt"
 * 
 * Assumptions on input:
 * 1. There is exaclty one email on each row of the input file.
 * 2. There are no other information in the input file except emails.
 * 3. There are no other characters or special symbols (newline, whitespaces) after the last row of email.
 * 4. Each individual email cannot exceed 50 characters in total length.
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAXSIZE 150000 //This is the assumed maximum entries of emails. This is can be changed if dataset is higher.
#define EMAILLENGTH 50 //This is the assumed length of each email in entirety.  This can be changed if assumption is incorrect.

//Data structure for each entry of email.  Order indicates the position(index) of the email in the input file.
struct data
{
	int order;
	char email[EMAILLENGTH];
};

//Array used to sort and manipulate the DATA. Each entry points to a Data structure of email.  Length is the number of entries in the array.
struct array		
{
	struct data **item;
	int length;
};

typedef struct array ARR;
typedef struct data DATA;

ARR* msort(ARR* arr, char sortBy[5]);
ARR* merge(ARR* left, ARR* right, char sortBy[5]);
ARR* partList(ARR* arr, int start, int end);

int main()
{	
	ARR* emailList;
	FILE* inputFile;
	FILE* outFile;
	DATA *newData;
	int i = 0;
	char buff[EMAILLENGTH];
	emailList = malloc(sizeof(ARR));
	emailList->item = malloc(sizeof(DATA)*MAXSIZE);
		
	if(!emailList)  		//error function for not enough memory when using malloc
	{
		printf("ERROR: malloc failed");
		exit(EXIT_FAILURE);
	}
	emailList->length = 0;
		
	//read in file and store in DATA structure
	inputFile = fopen ("emails.txt","r");	
	if (inputFile!=NULL)
	{
		while(fgets(buff,sizeof(buff),inputFile)!=NULL)
		{
			newData = malloc(sizeof(DATA));
			if(buff[strlen(buff)-1]=='\n')  //trim off newline at end of each row
			{
				buff[strlen(buff)-1]='\0';
			}
			if(!newData) 					//error function for not enought memory when using malloc
			{
				printf("malloc failed");
			}
			strcpy(newData->email, buff);
			newData->order = emailList->length;
			emailList->item[emailList->length] = newData;
			emailList->length++;
			
		}
	}
	else
	{
		printf("ERROR: Cannot open file.");
		exit(EXIT_FAILURE);
	}
	fclose (inputFile);
	
	//the actual sorting and duplication deletion
	clock_t t;
	t = clock();
	ARR* sortedList = msort(emailList,"email");
	sortedList = msort(sortedList,"order");
	t = clock()-t;
	
	//writing results out to output file
	outFile = fopen ("emailsOut.txt","w");
	fprintf(outFile,"Done in %f seconds. Sorted %d values. Found %d duplicates\n\n",((float)t)/CLOCKS_PER_SEC,emailList->length,emailList->length-sortedList->length);
	printf("Done in %f seconds. Sorted %d values. Found %d duplicates\n\n",((float)t)/CLOCKS_PER_SEC,emailList->length,emailList->length-sortedList->length);
	while(i < sortedList->length)
	{
		fprintf(outFile,"%s\n",sortedList->item[i]->email);
		i++;
	}
	fclose (outFile);
	
	free(emailList->item);
	free(emailList);
	free(sortedList->item);
	free(sortedList);
	free(newData);
	return 0;
}

/*function for merge sort.  Given the input arr and sortBy, sort the 
 * list of arrays alphabetically or by their original position*/	
ARR* msort( ARR* arr, char sortBy[5] ) 
{
	if (arr->length <= 1)
	{
		return arr;
	}
	int m = round((arr->length-1)/2);
	ARR* left = partList( arr, 0, m );
	ARR* right = partList( arr, m+1, arr->length-1 );
	left = msort(left, sortBy);
	right = msort(right, sortBy);
	ARR* sorted = merge(left,right,sortBy);
	
	free(left->item);
	free(left);
	free(right->item);
	free(right);
	return sorted;
} 

/*partitions a given array by copying everything 
 * between array's start index and end index to a new array*/
ARR* partList(ARR* arr, int start, int end)
{
	ARR *newList;
	newList = malloc(sizeof(ARR));
	newList->item = malloc(sizeof(DATA)*end-start+1);
	if(!newList)    //error function for not enought memory when using malloc
	{
		printf("malloc failed");
		exit(EXIT_FAILURE);
	}
	newList->length = 0;
	while(newList->length <= end-start)
	{
		newList->item[newList->length] = arr->item[newList->length+start];
		newList->length++;
	}
	return newList;
}

/*mege multiple partitions. This function takes two arrays and merge them into a new sorted array 
 * according to alphabetically or by original position.  This funcion also skips over any duplicate entries.*/
ARR* merge( ARR* left, ARR* right, char sortBy[5] )
{
	ARR* temp;
	temp  = malloc(sizeof(ARR));
	temp->item = malloc(sizeof(DATA)*(left->length+right->length));
	if(!temp)			//error function for not enought memory when using malloc
	{
		printf("malloc failed");
	}
	temp->length = 0;
	int leftIndex = 0; 
	int rightIndex = 0; 
		
	while ((leftIndex < left->length)&&(rightIndex < right->length))
	{
		//value in left partition is smaller
		if ( (strcmp(sortBy,"email")==0 && strcmp(left->item[leftIndex]->email, right->item[rightIndex]->email) < 0 )
			||(strcmp(sortBy,"order")==0 && left->item[leftIndex]->order <= right->item[rightIndex]->order))
		{
			temp->item[temp->length] = left->item[leftIndex];
			leftIndex++;
		} 
		//value in right partition is smaller
		else if ((strcmp(sortBy,"email")==0 && strcmp(left->item[leftIndex]->email, right->item[rightIndex]->email) > 0)
			|| (strcmp(sortBy,"order")==0 && left->item[leftIndex]->order > right->item[rightIndex]->order))
		{
			temp->item[temp->length] = right->item[rightIndex];
			rightIndex++;
		} 
		//both values are the same. take the one in left partition
		else
		{
			temp->item[temp->length] = left->item[leftIndex];
			leftIndex++;
			rightIndex++;
		}
		temp->length++;
	} 
	//no more values in right partition. Move everything in left to new array.
	while (leftIndex<left->length)
	{
		temp->item[temp->length] = left->item[leftIndex];
		leftIndex++;
		temp->length++;
	} 
	//no more values in left partition. Move everything in right to new array.
	while (rightIndex<right->length)
	{
		temp->item[temp->length] = right->item[rightIndex];
		rightIndex++;
		temp->length++;
	} 
	return temp;
}


