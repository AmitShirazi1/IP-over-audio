#include "networking.h"

int sockfd;
char buffer[BUFFER_SIZE];
struct sockaddr_in servaddr, cliaddr;
socklen_t len;


int udp_start_server() {
    
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return ERROR;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return ERROR;
    }
		

    return 0;
}

void udp_handle_request()
{
    len = sizeof(cliaddr); 
	char* filename = "received.wav";
	ssize_t rec;
    size_t written;
	char *confirm = "File received by server";
	
	// Creating a file.
	FILE* fp = fopen(filename, "wb"); //Opens the file for writing in binary mode.
	while (rec = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len) > 0) {
	    // if (buffer[0]=='e' && buffer[1] == 'x' && buffer[2]=='i' && buffer[3]=='t') {
	    // 	break;
	    // }

	    // Writing the received bytes to the file.
	    written = fwrite(buffer, 1, rec, fp);
	    if (written != rec) {
	        perror("Error while writing to file");
	        fclose(fp);
	        return 1;
	    }

	    bzero(buffer, BUFFER_SIZE);
	    //printf("received %d bytes\n",n);
	}
    
	    
	// send response to the client
	sendto(sockfd, confirm, strlen(confirm), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
	fclose(fp);
	printf("File received!\n");

    // TO DO: Process received wave file
	//read_Wav_file();  //??
	//convert_Wav_to_morse(); //??


	
		
}
