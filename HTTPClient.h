#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

// Request structure
struct request{
	char *method;
	char *object;
	char *protocol;
	char *headers;
};
// Create request object function
// GET /index.html HTTP 1.0
struct request* createRequest(char *method, char *object, char *protocol);

// Add header to request object
void addHeaderToRequest(struct request* req, char *header);

// Convert request to string
char *requestToSrting(struct request* req);

// Delete request object
void freeRequest(struct request* req);


// Response structure
struct response{
	char* protocol;
	char* code;
	char* msg;
	char* headers;
	int length;
	char* body;
};

// Create response object
struct response* createResponse(char *str);

// Free respons object
void freeResponse(struct response* resp);

#endif
