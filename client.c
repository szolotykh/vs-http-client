#include "client.h"

#define HEAD_BUFFER_SIZE 10240

int main(int argc, char *argv[]){

	int clientSock; // Client socket
	int portNum; // Server port
	struct sockaddr_in serverAddr; // Server addres structure
	struct hostent *server;

	struct timeval start_time, end_time; // Start and end time of RTT

	char* devCh;
	char* hostName;
	char* object;

	int n = 0; // read, write bites counter
	int receivedLen = 0;	
	int stopInd = 0; // Stop index
	char* stopChars = "\r\n\r\n";
	char cBuffer; // One byte buffer
	char headBuffer[HEAD_BUFFER_SIZE]; //Header buffer

	int rtt = 0;
	int argShift = 0;
	printf("===== HTTP Client =====\n");
	if (argc < 3) {
		printf("Use %s hostname port\n", argv[0]);
		printf("Use %s -p hostname port\n", argv[0]);
		exit(0);
	}
	if (argc == 4){
		if(strcmp(argv[1], "-p") == 0){
			rtt = 1;
			argShift = 1;
		}else{
			printf("Use %s hostname port\n", argv[0]);
			printf("Use %s -p hostname port\n", argv[0]);
			exit(0);
		}
	}
	portNum = atoi(argv[2+argShift]); // Convert port number to int

	devCh = strstr(argv[1+argShift],"/");
	if(devCh != NULL){
		int devCharInd = devCh - argv[1+argShift];
		hostName = (char*)malloc(devCharInd + 1);
		memcpy(hostName, argv[1+argShift], devCharInd);
		hostName[devCharInd]='\0';

		object = (char*)malloc(strlen(argv[1+argShift])-devCharInd + 1);
		memcpy(object, devCh,strlen(argv[1+argShift])-devCharInd);
		object[strlen(argv[1+argShift])-devCharInd]='\0';	
	}else{
		hostName = (char*)malloc(strlen(argv[1+argShift])+1);
		memcpy(hostName, argv[1+argShift], strlen(argv[1+argShift]));
		hostName[strlen(argv[1+argShift])] = '\0';
	
		object = (char*)malloc(2);
		memcpy(object, "/", 1);
		object[1] = '\0';		
	}
	printf("Host name: %s\n", hostName);
	printf("Object: %s\n", object);


    	clientSock = socket(AF_INET, SOCK_STREAM, 0);
    	if(clientSock < 0){
        	printf("Error opening socket");
		exit(0);
	}
	printf("Socked created\n");
	server = gethostbyname(hostName);
	if(server == NULL) {
        	printf("Error, no such host\n");
        	exit(0);
	}
	printf("Address obtained\n");
	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
         (char *)&serverAddr.sin_addr.s_addr,
         server->h_length);
    	serverAddr.sin_port = htons(portNum);

	if (connect(clientSock,(struct sockaddr *) &serverAddr,sizeof(serverAddr)) < 0){
        	printf("Error connecting");
		exit(0);
	}
	printf("Connected\n");

	//Sent request to server
	struct request* cReq = createRequest(
		"GET",
		object,
		"HTTP/1.0");
	addHeaderToRequest(cReq,"Connection: close");

	char* reqStr = requestToSrting(cReq);

	// Sent client request
	int msgLen = strlen(reqStr);
	n = 0;
	int i;
	if(rtt == 1){
		gettimeofday(&start_time, NULL); //Start time
	}
	for(i = 0; i < msgLen; i+=n){
		n = write(clientSock,reqStr+i, msgLen-i);
		if(n < 0){
			printf("ERROR writing to socket");
			exit(1);
		}
	}
	printf("Client request:\n%s\n",reqStr); // Print client request
	// free client request	
	free(reqStr);
	freeRequest(cReq);

	

	// Read header of server response
	while(1){
		if(receivedLen == HEAD_BUFFER_SIZE)
			break; // handle this
		n = read(clientSock, &cBuffer, 1);
     		if(n < 0){
			printf("ERROR reading from socket");
			exit(1);
		}
		headBuffer[receivedLen] = cBuffer;
		receivedLen++;
		if(stopChars[stopInd] == cBuffer){
			stopInd++;
			if(stopInd == 4)
				break;
		}else{
			stopInd = 0;
		}
	}
	if(rtt == 1){
		gettimeofday(&end_time, NULL);// End Time
	}

	headBuffer[receivedLen] = '\0';
	printf("Server response:\n %s\n", headBuffer);// Preant server response header
	struct response* serverResp = createResponse(headBuffer); // Create server response

	// Reading body body
	for(i = 0; i < serverResp->length; i+=n){
		n = read(clientSock,serverResp->body+i, serverResp->length-i);
		if(n < 0){
			printf("ERROR reading to socket");
			exit(1);
		}
	}
	printf("%s\n\n", serverResp->body); // Print body
	// Calcilate and print RTT
	if(rtt == 1){
		printf("RTT: %ld microseconds\n\n",
		((end_time.tv_sec * 1000000 + end_time.tv_usec)
		  - (start_time.tv_sec * 1000000 + start_time.tv_usec)));
	}
	freeResponse(serverResp); // Free server response
	close(clientSock); // Close socket
	return 1;
}
