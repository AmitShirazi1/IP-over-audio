#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "morse_decoder_j.h"

#define BUFFER_SIZE 1024
typedef struct {
    char   id[4];            // should always contain "RIFF"
    int    totallength;      // total file length minus 8
    char   wavefmt[8];       // should be "WAVEfmt "
    int    format;           // 16 for PCM format
    short  pcm;              // 1 for PCM format
    short  channels;         // channels
    int    sampleRate;        // sampling frequency
    int    bytes_per_second;
    short  bytes_by_capture;
    short  bits_per_sample;
    char   data[4];          // should always contain "data"
    int    bytes_in_data;
} WavHeader;


#define THRESHOLD 0.5 // Adjust this threshold according to your audio
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
    {"!", "-.-.--"}, {" ", " "},  // Space character
    {"", NULL}  // Null-terminated entry to indicate the end of the table
};


char* decode(char* s){
	for (int i=0; morseTable[i][1] != NULL; i++){
		if (strcmp(s, morseTable[i][1]) ==0) return morseTable[i][0];
	}
return "";
}

int main() {
    int num_samples, sample_rate;
    // Read audio file
    const char* filename = "example_file.wav";

    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }

    // Read WAV file header
    WavHeader header;
    fread(&header, sizeof(WavHeader), 1, file);
    printf("Length %d \n",header.bytes_in_data);
    printf("sampleRate %d \n",header.sampleRate);
    printf("bytes_per_second %d \n",header.bytes_per_second);
    printf("bytes_by_capture %d \n",header.bytes_by_capture);
    printf("bits_per_sample %d \n",header.bits_per_sample);
    int samples_group = header.sampleRate/50;
    int new_data_count = (header.bytes_in_data/header.bytes_by_capture)/samples_group;
    int new_data[new_data_count];
    short x;

    // converts samples to 0, 1
    for (int j = 0; j<new_data_count; j++) {
        int sum=0;
        for (int i = 0; i < samples_group; i++) {
            fread(&x, header.bytes_by_capture, 1, file);
            if (x > 0) sum += x;
            else sum -= x;
        }
        if (sum>samples_group*50) new_data[j] = 1;
        else new_data[j] = 0;
        //printf(" %d", new_data[j]);
    }

    // detecting minimal sequence of 0 (for letter pause) and 1 (for dot)
    // maximum sequence of 1 (for dash)
    int min[2];
    int max[2];

    x = new_data[0];
    int count = 0;
    for (int j = 0; j<new_data_count; j++) {
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
	
    for (int j = 0; j<new_data_count; j++) {
        if (new_data[j] == x) count++;
        else {
            if (x==1){
                if (count>dash){
                    //printf("-");
					strcat(word, "-");
                }
                else{
                    //printf(".");
					strcat(word, ".");
                }
            }
            else{
                if (count>pause_letter){
                    //new word
					//trans[0] = decode(word);
					strcat(sentence, decode(word));
					word[0] = '\0';
					
					
                }
                if (count>pause_word){
                    // new sentence
					strcat(sentence, " ");
                }
            }
            count = 1;
            x =  new_data[j];
        }
    }
	printf("The wave file says: %s \n", sentence);
	free(sentence);



    // Decode Morse code into text

    // Free allocated memory
    //free(pcm_data);

    return 0;
}





	
