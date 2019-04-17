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
#include <pthread.h>

typedef struct{

	char threadFileName[101];
	int fileNumber;

} fileInfoForThread;

typedef struct {

	char word[101];
	int freq;

} wordStats;

typedef struct {

	char word[101];
	int distinct;
	int fileName;

} fileStats;

fileStats *fileArray;

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

}

int file_information_compare(const void *firstInfo, const void *secondInfo){

	int compute;

	int firstDistinct = ((fileStats *)firstInfo)->distinct;
	int secondDistinct = ((fileStats *)secondInfo)->distinct;

	compute = secondDistinct - firstDistinct;

	return compute;

}

void *find_file_stats(void *passedThreadFileInfo){

	fileInfoForThread* threadFile = (fileInfoForThread*)passedThreadFileInfo;

	FILE *filePointer;
	wordStats wordArray[1000];
	char currentWord[101];

	char currentFileName[101];
	strcpy(currentFileName, threadFile->threadFileName);
	//printf("%s\n", currentFileName);

	int i;
	int numberOfArrayElements = 0;

	//printf("%s\n", currentFileName);
	filePointer = fopen(currentFileName, "r");

	fscanf(filePointer, "%s", currentWord);
	//printf("%s\n", currentWord);

	wordStats initialWord;
	initialWord.freq = 1;
	strcpy(initialWord.word, currentWord);
	//printf("%s", initialWord.word);

	wordArray[0] = initialWord;
	numberOfArrayElements = 1;

	fileStats fileToAnalyze;

	while(fscanf(filePointer, "%s", currentWord) != EOF){

		int flag = 0;

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
		}

	} //end of while loop

	qsort(wordArray, numberOfArrayElements, sizeof(wordStats), word_information_compare);

	char medianWord[101];
	int medianFrequency;

	if(numberOfArrayElements % 2 == 0){
		strcpy(medianWord, wordArray[(numberOfArrayElements / 2) - 1].word);
		medianFrequency = wordArray[(numberOfArrayElements / 2) - 1].freq;
	}
	else{
		strcpy(medianWord, wordArray[(numberOfArrayElements / 2)].word);
		medianFrequency = wordArray[(numberOfArrayElements / 2)].freq;
	}

	strcpy(fileToAnalyze.word, medianWord);
	fileToAnalyze.distinct = numberOfArrayElements;
	fileToAnalyze.fileName = threadFile->fileNumber;

	memset(wordArray, 0, sizeof(wordArray));

	fclose(filePointer);

	fileArray[threadFile->fileNumber - 1] = fileToAnalyze;
	//printf("%s %d %d\n",fileArray[threadFile->fileNumber].word, fileArray[threadFile->fileNumber].distinct, fileArray[threadFile->fileNumber].fileName);

	pthread_exit(0);

} //end of find_median_word_and_freq

int main (int argc, char *argv[]) {

	fileArray = malloc(argc * sizeof(fileStats));

	pthread_t tid[argc - 1];

	int i;

	fileInfoForThread arrayThreadArgs[argc - 1];

	for(i = 0; i < argc - 1; i++){

		arrayThreadArgs[i].fileNumber = i + 1;
		strcpy(arrayThreadArgs[i].threadFileName, argv[i + 1]);

	}

	for(i = 0; i < argc - 1; i++){

		pthread_create(&tid[i], NULL, find_file_stats, &arrayThreadArgs[i]);

	}

	for(i = 0; i < argc - 1; i++){

		pthread_join(tid[i], NULL);

	}

	/*printf("%s %d %d\n", fileArray[0].word, fileArray[0].fileName, fileArray[0].distinct);
	printf("%s %d %d\n", fileArray[1].word, fileArray[1].fileName, fileArray[1].distinct);
	printf("%s %d %d\n", fileArray[2].word, fileArray[2].fileName, fileArray[2].distinct);
	printf("%s %d %d\n", fileArray[3].word, fileArray[3].fileName, fileArray[3].distinct);*/

	qsort(fileArray, argc - 1, sizeof(fileStats), file_information_compare);

	for(i = 0; i < argc - 1; i++){

		printf("%s %d %s\n", argv[fileArray[i].fileName], fileArray[i].distinct, fileArray[i].word);

	}

	return 0;

} //end of main