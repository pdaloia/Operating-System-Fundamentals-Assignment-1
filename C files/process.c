/*

# Family Name: D'Aloia

# Given Name: Philip

# Section: E

# Student Number: 213672431

# CSE Login: pdaloia

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//define struct for the statistics of a word
typedef struct {

	char word[101];
	int freq;

} wordStats;

//define struct for the statistics of a file
typedef struct {

	char word[101];
	int distinct;
	int fileName;

} fileStats;

//comparing function for two words (struct of type wordStats)
int word_information_compare(const void *firstInfo, const void *secondInfo){

	int compute;

	int firstFreq = ((wordStats *)firstInfo)->freq;
	int secondFreq = ((wordStats *)secondInfo)->freq;

	char firstWord[101];
	char secondWord[101];

	strcpy(firstWord, ((wordStats *)firstInfo)->word);
	strcpy(secondWord, ((wordStats *)secondInfo)->word);

	compute = secondFreq - firstFreq;

	if(compute != 0)
		return compute;
	else {
		return strcmp(secondWord, firstWord);
	}

} //end of word_information_compare

//comparing function for two files (struct of type fileStats)
int file_information_compare(const void *firstInfo, const void *secondInfo){

	int compute;

	int firstDistinct = ((fileStats *)firstInfo)->distinct;
	int secondDistinct = ((fileStats *)secondInfo)->distinct;

	compute = secondDistinct - firstDistinct;

	return compute;

} //end of file_information_compare

//function to find the statistics of each file sent through the parameters
fileStats find_statistics(int argc, char *argv[], int currentFile){

	//create a file pointer
	FILE *filePointer;
	wordStats wordArray[1000]; //array for words to be stored from the file
	char currentWord[101]; //string for the current word being looked at in the file

	int i;
	int numberOfArrayElements = 0; //counter for number of words in the array

	filePointer = fopen(argv[currentFile], "r");

	fscanf(filePointer, "%s", currentWord); //scan the file

	//create and store the inital word of the text file
	wordStats initialWord;
	initialWord.freq = 1;
	strcpy(initialWord.word, currentWord);
	wordArray[0] = initialWord;
	numberOfArrayElements = 1;

	fileStats fileToAnalyze; //create struct for file statistics

	//create a loop to scan through the whole text file word by word
	while(fscanf(filePointer, "%s", currentWord) != EOF){

		int flag = 0; //flag to see if a word is already in our word array

		for(i = 0; i < sizeof(wordArray) / sizeof(wordStats); i++){

			int wordFound = strcmp(wordArray[i].word, currentWord);

			if(wordFound == 0){
				wordArray[i].freq++;
				flag = 1;
				break;
			} //end of if statement

		} //end of for loop

		if(flag == 0){
			wordStats wordToAdd;
			wordToAdd.freq = 1;
			strcpy(wordToAdd.word, currentWord);
			wordArray[numberOfArrayElements] = wordToAdd;
			numberOfArrayElements++;
		}//end of if statement

	} //end of while loop

	//sort the word array 
	qsort(wordArray, numberOfArrayElements, sizeof(wordStats), word_information_compare);

	char medianWord[101];
	int medianFrequency;

	//calculate the median word and its frequency
	if(numberOfArrayElements % 2 == 0){
		strcpy(medianWord, wordArray[(numberOfArrayElements / 2) - 1].word);
		medianFrequency = wordArray[(numberOfArrayElements / 2) - 1].freq;
	}
	else{
		strcpy(medianWord, wordArray[(numberOfArrayElements / 2)].word);
		medianFrequency = wordArray[(numberOfArrayElements / 2)].freq;
	}

	//create the statistics of the file and the median word
	strcpy(fileToAnalyze.word, medianWord);
	fileToAnalyze.distinct = numberOfArrayElements;
	fileToAnalyze.fileName = currentFile;

	//reset the word array
	memset(wordArray, 0, sizeof(wordArray));

	//close the file pointer
	fclose(filePointer);

	//return the file statistics
	return fileToAnalyze;	

} //end of find_median_word_and_freq

int main (int argc, char *argv[]) {

	//create the array of files and their statistics
	fileStats fileArray[argc - 1];

	int i;
	//create array of process IDs
	pid_t pid[argc - 1];
	//create array of pipes
	int fd[argc-1][2];

	//create argc-1 pipes
	for(i = 0; i < argc - 1; i++) {

		if(pipe(fd[i]) < 0){
			fprintf(stderr, "Pipe Failed!\n"); //output error meesage if pipe failed
		}

	}

	//create and simultaneuosly run argc-1 child processes
	for(i = 0; i < argc - 1; i++) {

		pid[i] = fork(); //fork the main process creating a child for a text file

		if(pid[i] < 0){
			fprintf(stderr, "Fork Failed!\n"); //output error message if child not created successfully
			exit(-1);
		}
		else if(pid[i] == 0){
			//child process will send file to be analyzed
			fileStats fileToSend;
			fileToSend = find_statistics(argc, argv, i + 1);
			//child will write file statistics to its pipe and close the writing side of its pipe
			write(fd[i][1], &fileToSend, sizeof(fileStats));
			close(fd[i][1]);
			//child exits once analyzed and written file statistics
			exit(0);
		}

	}

	//parent process waits for children to finish their processes
	wait(NULL);
	//read pipes from array
	for(i = 0; i < argc - 1; i++){

		//parent process reads from all pipes and closes them all
		read(fd[i][0], &fileArray[i], sizeof(fileStats));
		close(fd[i][0]);

	}

	//sort the file array
	qsort(fileArray, argc - 1, sizeof(fileStats), file_information_compare);

	//print out the file array
	for(i = 0; i < sizeof(fileArray) / sizeof(fileStats); i++){

		printf("%s %d %s\n", argv[fileArray[i].fileName], fileArray[i].distinct, fileArray[i].word);

	}

	return 0;

} //end of main