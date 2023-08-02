#include "morse_core.h"

const double default_frequency = 800.0;
const int default_rate_wpm = 12; 

int morse_table[MAX_CHARS][MAX_CHAR_SEQ + 1] = {
	{'A', DOT, DASH, ENDCODE },
	{'B', DASH, DOT, DOT, DOT, ENDCODE },
	{'C', DASH, DOT, DASH, DOT, ENDCODE },
	{'D', DASH, DOT, DOT, ENDCODE },
	{'E', DOT, ENDCODE },
	{'F', DOT, DOT, DASH, DOT, ENDCODE },
	{'G', DASH, DASH, DOT, ENDCODE },
	{'H', DOT, DOT, DOT, DOT, ENDCODE },
	{'I', DOT, DOT, ENDCODE },
	{'J', DOT, DASH, DASH, DASH, ENDCODE },
	{'K', DASH, DOT, DASH, ENDCODE },
	{'L', DOT, DASH, DOT, DOT, ENDCODE },
	{'M', DASH, DASH, ENDCODE },
	{'N', DASH, DOT, ENDCODE },
	{'O', DASH, DASH, DASH, ENDCODE },
	{'P', DOT, DASH, DASH, DOT, ENDCODE },
	{'Q', DASH, DASH, DOT, DASH, ENDCODE },
	{'R', DOT, DASH, DOT, ENDCODE },
	{'S', DOT, DOT, DOT, ENDCODE },
	{'T', DASH, ENDCODE },
	{'U', DOT, DOT, DASH, ENDCODE },
	{'V', DOT, DOT, DOT, DASH, ENDCODE },
	{'W', DOT, DASH, DASH, ENDCODE },
	{'X', DASH, DOT, DOT, DASH, ENDCODE },
	{'Y', DASH, DOT, DASH, DASH, ENDCODE },
	{'Z', DASH, DASH, DOT, DOT, ENDCODE },
	{'0', DASH, DASH, DASH, DASH, DASH, ENDCODE },
	{'1', DOT, DASH, DASH, DASH, DASH, ENDCODE },
	{'2', DOT, DOT, DASH, DASH, DASH, ENDCODE },
	{'3', DOT, DOT, DOT, DASH, DASH, ENDCODE },
	{'4', DOT, DOT, DOT, DOT, DASH, ENDCODE },
	{'5', DOT, DOT, DOT, DOT, DOT, ENDCODE },
	{'6', DASH, DOT, DOT, DOT, DOT, ENDCODE },
	{'7', DASH, DASH, DOT, DOT, DOT, ENDCODE },
	{'8', DASH, DASH, DASH, DOT, DOT, ENDCODE },
	{'9', DASH, DASH, DASH, DASH, DOT, ENDCODE },
	{'.', DOT, DASH, DOT, DASH, DOT, DASH, ENDCODE },
	{',', DASH, DASH, DOT, DOT, DASH, DASH, ENDCODE },
	{'?', DOT, DOT, DASH, DASH, DOT, DOT, ENDCODE },
	{'\'', DOT, DASH, DASH, DASH, DASH, DOT, ENDCODE },
	{'!', DASH, DOT, DASH, DOT, DASH, DASH, ENDCODE },
	{'/', DASH, DOT, DOT, DASH, DOT, ENDCODE },
	{'(', DASH, DOT, DASH, DASH, DOT, ENDCODE },
	{')', DASH, DOT, DASH, DASH, DOT, DASH, ENDCODE }, 
	{'&', DOT, DASH, DOT, DOT, DOT, ENDCODE },
	{':', DASH, DASH, DASH, DOT, DOT, DOT, ENDCODE },
	{';', DASH, DOT, DASH, DOT, DASH, DOT, ENDCODE },
	{'=', DASH, DOT, DOT, DOT, DASH, ENDCODE },
	{'+', DOT, DASH, DOT, DASH, DOT, ENDCODE },
	{'-', DASH, DOT, DOT, DOT, DOT, DASH, ENDCODE },
	{'_', DOT, DOT, DASH, DASH, DOT, DASH, ENDCODE },
	{'\"', DOT, DASH, DOT, DOT, DASH, DOT, ENDCODE },
	{'$', DOT, DOT, DOT, DASH, DOT, DOT, DASH, ENDCODE },
	{'@', DOT, DASH, DASH, DOT, DASH, DOT, ENDCODE },
	{' ', WORD_SPC, ENDCODE }
};

struct Parameters params;

void initialize_params(int argc, char *argv[]){
    
    if (argc > 2) {
		fprintf(stderr, "Too many arguments.\n\n");
		exit(EXIT_FAILURE);
	}
	else if (argc == 2) {
		strcpy(params.input_file_name, argv[1]);
	}
	else {
		fprintf(stderr, "No input file specified.\n\n");
		exit(EXIT_FAILURE);
	}

	/* Set default values */
	params.frequency = default_frequency;
	params.rate_wpm = default_rate_wpm;
	params.unit_duration = WAVFILE_SAMPLES_PER_SECOND / default_rate_wpm;
	params.lower_upper_ascii_difference = 'a' - 'A';
}


void process_input(){
    /* Open input file */
	params.input_file = fopen(params.input_file_name, "r");
	if (params.input_file == NULL) {
		printf("\nCould not read file %s, let me create one instead.\n\n", params.input_file_name);
		params.input_file = fopen(params.input_file_name, "w+");
		if (params.input_file == NULL) {
			fprintf(stderr, "Could not create file %s.\n", params.input_file_name);
			exit(EXIT_FAILURE);
		}

		char command[7 + MAX_FILENAME];
		strcpy(command, "cat >> ");
		printf("What message would you like to send?\n\t(press [Ctrl] + [d] at the end of the message)\n");
		
		if (system(strcat(command, params.input_file_name)) < 0) {
			fprintf(stderr, "Could not write to file %s.\n", params.input_file_name);
			exit(EXIT_FAILURE);
        }
    }

}

void process_output(){
    /* Open output file, using input file name with .wav extension */
	int i = strlen(params.output_file_name);
	if (params.output_file_name[i - 4] == '.') {
		strcpy(&params.output_file_name[i - 3], "wav");
	}
	else {
		fprintf(stderr,
				"Bad format for file name %s, please provide a text file (.txt extension).\n", params.input_file_name);
		exit(EXIT_FAILURE);
	}

	params.output_file = wavfile_open(params.output_file_name);
	if (params.output_file == NULL) {
		fprintf(stderr, "Could not write to file %s.\n", params.output_file_name);
		exit(EXIT_FAILURE);
	}
    params.output_text_file = fopen("output_text_file.txt", "w+");
}

void create_wav_file(){
    /* Read in characters, look up series of dots and dashes in sign
		table, output appropriate format for each dot, dash, or space. */
	while ((params.ascii_char = fgetc(params.input_file)) != EOF) {
		/* Ensure valid input */
		if (params.ascii_char > MAX_ASCII) {
			break;
		}
		/* Ignore newlines, no processing needed */
		else if (params.ascii_char == '\n') {
			continue;
		}
		/* Convert lowercase to uppercase */
		else if (params.ascii_char >= 'a' && params.ascii_char <= 'z') {
			params.ascii_char -= params.lower_upper_ascii_difference; 
		}
    
		/* Get morse output patterns for each component character from lookup table */
		params.morse_table_index = params.ascii_table[params.ascii_char];
		params.signal_code = &morse_table[params.morse_table_index][1];
		write_morse_char(params.output_file, params.waveform, params.frequency, 
			params.unit_duration, params.signal_code, params.output_text_file);
	}
}

void close_files(){
    /* Close input and output files */
    fclose(params.input_file);
	wavfile_close(params.output_file);
	fclose(params.output_text_file);
}

void play_output(){
    char command[7 + MAX_FILENAME];
	strcpy(command, "paplay ");
	printf("\nThis is your message in morse code:\n");
	params.output_text_file = fopen("output_text_file.txt", "r");
	int ch;
    while ((ch = fgetc(params.output_text_file)) != EOF) {
        putchar(ch); // Print the character to the console (or any other output stream)
    }
	printf("\nPlaying the audio file...\n");
	if (system(strcat(command, params.output_file_name)) < 0) {
		fprintf(stderr, "Could not play audio file %s.\n", params.output_file_name);
		exit(EXIT_FAILURE);
	}
    fclose(params.output_text_file);
}
