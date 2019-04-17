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

typedef struct {

	char word[101];
	int freq;

} wordStats;

typedef struct {

	char word[101];
	int distinct;
	int fileName;

} fileStats;

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

fileStats find_median_word_and_freq(int argc, char *argv[], int currentFile){

	FILE *filePointer;
	wordStats wordArray[1000];
	char currentWord[101];
	int i;
	int numberOfArrayElements = 0;

	filePointer = fopen(argv[currentFile], "r");

	fscanf(filePointer, "%s", currentWord);
	//printf("%s\n", currentWord);

	wordStats initialWord;
	initialWord.freq = 1;
	strcpy(initialWord.word, currentWord);
	//printf("%s", initialWord.word);

	wordArray[0] = initialWord;
	numberOfArrayElements = 1;

	fileStats fileToAnalyze;

	//printf("Word 1 is: %s and the frequency so far is: %d\n", wordArray[0].word, wordArray[0].freq);

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
	fileToAnalyze.fileName = currentFile;

	memset(wordArray, 0, sizeof(wordArray));

	fclose(filePointer);

	return fileToAnalyze;

} //end of find_median_word_and_freq

int main (int argc, char *argv[]) {

	fileStats fileArray[argc - 1];

	int i;

	for(i = 0; i < argc - 1; i++) {

		fileArray[i] = find_median_word_and_freq(argc, argv, i + 1);

	}

	qsort(fileArray, argc - 1, sizeof(fileStats), file_information_compare);

	for(i = 0; i < sizeof(fileArray) / sizeof(fileStats); i++){

		printf("%s %d %s\n", argv[fileArray[i].fileName], fileArray[i].distinct, fileArray[i].word);

	}

	return 0;

} //end of main