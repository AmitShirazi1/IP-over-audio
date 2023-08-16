#include "networking.h"


int recv_file(char* buf, int s){
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == EOF)
            return 1;
    }
    return 0;
}


int start_udp_server() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    socklen_t len;

    len = sizeof(cliaddr); 
	char* filename = "received.wav";
	ssize_t rec;
    size_t written;
	char *confirm = "File received by server";
	
	
	int exitFlag=1;
	while(exitFlag){
	// Creating a file.
	FILE* fp = fopen(filename, "wb"); //Opens the file for writing in binary mode.
	    while (rec = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len) > 0) {
		if ((buffer[0]=='e') && (buffer[1]=='x') && (buffer[2]=='i') && (buffer[3]=='t')) {
			exitFlag = 0;
			break;
		}
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
	//onvert_Wav_to_morse(); //??
		
	
	}
		

    return 0;
}
