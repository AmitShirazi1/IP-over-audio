#include "morse_code/morse_core.h"
#include "networking/networking.h"

#define DEFAULT_PORT 8080
#define DEFAULT_ADDRESS "127.0.0.1"

bool sender = true;
bool receiver = false;
int port = DEFAULT_PORT;
char* address = DEFAULT_ADDRESS;
char* protocol = "TCP";

void printHelp(char* prg, bool printHeader)
{
	   
    printf(" Options:\n"
	   "  -s / --send      	  send the message\n"
	   "  -r / --receive      receive the message\n"
	   "  -p / --port    	  specify port(default:8080)\n"
	   "  -a / --address      specify destination address(default: 127.0.0.1)\n"
	   "  -pcl  		  specify protocol: UDP or TCP (default:TCP)\n"
       "  -h / --help      	  print this help\n");

}

void parseArgs(int argc, char* argv[])
{
    int arg=0;
    while(arg+1<argc) 
    {
	    arg++;
	    if ((strcmp(argv[arg],"-h")==0) ||
	        strcmp(argv[arg],"--help")==0)
        {
            printHelp(argv[0], true);
            exit(0);
        }

	    if ((strcmp(argv[arg],"-s")==0) ||
	        (strcmp(argv[arg],"--send")==0)) {

	        sender = true;
            receiver = false;
            continue;
        }
    
	    if ((strcmp(argv[arg],"-r")==0) ||
	        (strcmp(argv[arg],"--receive")==0)) {
	        sender = false;
            receiver = true;
            continue;
        }
    
	    if ((strcmp(argv[arg],"-p")==0) ||
	        (strcmp(argv[arg],"--port")==0) && (arg+1<argc)) {
	        arg++;     
	    	port = atoi(argv[arg]);
	        continue;
	    }
    
	    if ((strcmp(argv[arg],"-a")==0) ||
	        (strcmp(argv[arg],"--address")==0) && (arg+1<argc)) {
	        arg++;
	    	address = argv[arg];
	        continue;
	    }
	    
        if ((strcmp(argv[arg],"-pcl")==0) && (arg+1<argc)) {
            arg++;
            protocol = argv[arg];
            printf("Using %s protocol\n", protocol);
	        continue;           
        }
    }
}

char *get_wav_file()
{
    printf("Enter a text file name to send: ");
    char* file_name = (char*)malloc(100);
    scanf("%s", file_name);
    initialize_params(file_name);
    process_input();
    strcpy(params.output_file_name, params.input_file_name);
    char *wav_filename = process_output();
    /* Make lookup table to access morse codes through ASCII values */
	/* First make empty ASCII entries point to space character,
		which is last value of morse_table */
	for (int i = 0; i < MAX_CHARS; i++) {
		params.ascii_char = morse_table[i][0]; //For example, if i=1 than the variable 'ascii_char' contains 'B'.
		params.ascii_table[params.ascii_char] = i; // The 66-th ('B' in ASCII) element of the 'ascii_table' array contains 1,
	}
    create_wav_file();
    // Cleanup
    close_files();
    // /* Play audio file */
    play_output();
    free(file_name);
    return wav_filename;
}

char* decode_wav_file(){
    read_Wav_file();
    return convert_Wav_to_morse();
}

#define UDP_CLIENT 0
#define UDP_SERVER 1
#define TCP_CLIENT 2
#define TCP_SERVER 3

int main(int argc, char *argv[])
{

    /****************************************************************/
    // To-do: take user input on whether to send/receive, 
    // protocol, port, address
    /****************************************************************/
    parseArgs(argc, argv);
    
    /****************************************************************/
    // To-do: initialize server/client
    /****************************************************************/
    int temp;
    if(sender){
        if(strcmp(protocol, "UDP") == 0){
            temp = UDP_CLIENT;
            udp_start_client(address);
        }
        else
        {
            temp = TCP_CLIENT;
            tcp_start_client(address);
        }
    }
        
    else
    {
        if(strcmp(protocol, "UDP") == 0)
        {
            temp = UDP_SERVER;
            udp_start_server();
        }
        else
        {
            temp = TCP_SERVER;
            tcp_start_server();
        }
    }
        

    /****************************************************************/
    // To-do: bind server/client
    /****************************************************************/
    bool keep_sending = true;
    while(1)
    {
        /****************************************************************/
        // To-do: encode/decode the message
        /****************************************************************/
         
        /****************************************************************/
        // To-do: send/receive the message
        /****************************************************************/
        if (temp == UDP_CLIENT)
        {
            udp_send(get_wav_file());
        }
            
        else if (temp == UDP_SERVER)
        {
            udp_recv_request();
            // Decode WAV to text
            udp_send_response();
            printf("----------WAV File info----------\n");
            printf("%s\n", decode_wav_file());
            printf("------------------------------\n");
        }
            
        else if (temp == TCP_CLIENT)
        {
            tcp_send(get_wav_file());
        }
            
        else
        {
            tcp_handle_request();
            printf("----------WAV File info----------\n");
            printf("%s\n", decode_wav_file());
            printf("------------------------------\n");
        }

        /****************************************************************/
        // To-do: Ask the user if he wants to send/receive another message
        // if yes, continue, else break
        /****************************************************************/
        printf("Do you want to send/receive another message? (y/n): ");
        char c;
        scanf(" %c", &c);
        if(c == 'n')
        {
            break;
        }

    }
    

}
