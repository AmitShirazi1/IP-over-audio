#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wav_to_morse.h"

const char *morseTable[][2] = {
    {"A", ".-"},     {"B", "-..."},   {"C", "-.-."},
    {"D", "-.."},    {"E", "."},      {"F", "..-."},
    {"G", "--."},    {"H", "...."},    {"I", ".."},
    {"J", ".---"},   {"K", "-.-"},     {"L", ".-.."},
    {"M", "--"},     {"N", "-."},      {"O", "---"},
    {"P", ".--."},   {"Q", "--.-"},    {"R", ".-."},
    {"S", "..."},    {"T", "-"},       {"U", "..-"},
    {"V", "...-"},   {"W", ".--"},     {"X", "-..-"},
    {"Y", "-.--"},   {"Z", "--.."},
    {"0", "-----"},  {"1", ".----"},   {"2", "..---"},
    {"3", "...--"},  {"4", "....-"},   {"5", "....."},
    {"6", "-...."},  {"7", "--..."},   {"8", "---.."},
    {"9", "----."},
    {".", ".-.-.-"}, {",", "--..--"},  {"?", "..--.."},
    {"!", "-.-.--"}, {" ", " "}, {"'",".----."},
    {"", NULL}  // Null-terminated entry to indicate the end of the array
};


char* decode(char* s){
	for (int i=0; morseTable[i][1] != NULL; i++){
		if (strcmp(s, morseTable[i][1]) ==0) return morseTable[i][0];
	}
return "";
}


char* wav_to_morse(ConversionParameters param, WavHeader head)
{
    int new_data[param.new_data_count];
    short x;
    // converts samples to 0, 1
    for (int j = 0; j<param.new_data_count; j++) {
        int sum=0;
        for (int i = 0; i < param.samples_group; i++) {
            fread(&x, head.bytes_by_capture, 1, param.file);
            if (x > 0) sum += x;
            else sum -= x;
        }
        if (sum>param.samples_group*50) new_data[j] = 1;
        else new_data[j] = 0;
        //printf(" %d", new_data[j]);
    }

    // detecting minimal sequence of 0 (for letter pause) and 1 (for dot)
    // maximum sequence of 1 (for dash)
    int min[2] = {0 ,0};
    int max[2]= {0 ,0};

    x = new_data[0];
    int count = 0;
    for (int j = 0; j<param.new_data_count; j++) {
        if (new_data[j] == x) count++;
        else {
            if (count>max[x]) max[x] = count;
            if (count<min[x] || min[x] ==0) min[x] = count;
            count = 1;
            x = new_data[j];
        }
    }
    int pause_letter = 2*min[0]-1;
    int pause_word = max[0] - min[0];
    int dash = (max[1]+min[1])/2;

    x = new_data[0];
    count = 0;
	char word[6] = "";
	char* sentence = (char*)malloc((1024) * sizeof(char));
    memset(sentence, 0, sizeof(char));

    for (int j = 0; j<param.new_data_count; j++) {
        if (new_data[j] == x)
        {
            count++;
        }
        else {
            if (x==1){
                if (count>dash){
                    // printf("-");
					strcat(word, "-");
                }
                else{
                    // printf(".");
					strcat(word, ".");
                }
            }
            else{
                if (count>pause_letter){
                    //new morse word (text letter)
                    // printf(" ");
                    // printf("Word %s \n", word);
					strcat(sentence, decode(word));
					word[0] = '\0';
                }
                if (count>pause_word){
                    // printf(" \n");
                    // new sentence (text word)
					strcat(sentence, " ");
                }
            }
            count = 1;
            x =  new_data[j];
        }
    }
	//printf("The wave file says: %s \n", sentence);
    return sentence;
	free(sentence);
}


