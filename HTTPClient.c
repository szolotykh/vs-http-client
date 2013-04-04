#include "HTTPClient.h"

// GET /index.html HTTP 1.0
struct request* createRequest(char *method, char *object, char *protocol){
	struct request *req = (struct request*)malloc(sizeof(struct request));
	req->method = (char*)malloc(strlen(method)+1);
	strcpy(req->method, method);
	
	req->object = (char*)malloc(strlen(object)+1);
	strcpy(req->object, object);

	req->protocol = (char*)malloc(strlen(protocol)+1);
	strcpy(req->protocol, protocol);

	req->headers = (char*)malloc(1);
	strcpy(req->headers, "");

	return req;
}
void addHeaderToRequest(struct request* req, char *header){
	char* str = (char*)malloc(strlen(req->headers) + strlen(header)+1+2);
	strcpy(str, req->headers);
	strcat(str, header);
	strcat(str, "\r\n");
	free( req->headers );
	req->headers = str;
}
char *requestToSrting(struct request* req){
	int size = strlen(req->method)+1+strlen(req->object)+1+strlen(req->protocol)+2+
		strlen(req->headers)+2;
	char* str = (char*)malloc( size );
	strcpy(str, req->method);
	strcat(str, " ");
	strcat(str, req->object);
	strcat(str, " ");
	strcat(str, req->protocol);
	strcat(str, "\r\n");

	if( strlen(req->headers) != 0 )
		strcat(str, req->headers);
	strcat(str, "\r\n"); // Empty line
	return str;

}
void freeRequest(struct request* req){
	free(req->method);
	free(req->object);
	free(req->protocol);
	free(req->headers);
	free(req);
}


// Create response from server
// HTTP 1.0 200 OK
// Headers
// BODY
struct response* createResponse(char *str){
	char* ch_end;
	char* ch_start = str;
	int len = strlen(str);
	int length;
	struct response* resp = (struct response*)malloc(sizeof(struct response));

	// Protocol
	ch_end = (char*)memchr(ch_start,' ',len);
	length = ch_end - ch_start;
	resp->protocol = (char*)malloc(length+1);
	memcpy(resp->protocol, str, length);
	resp->protocol[length] = '\0';
	ch_start = ch_end+1;

	// code
	ch_end = ch_start + 3;
	length = 3;
	resp->code = (char*)malloc(length+1);
	memcpy(resp->code, ch_start, length);
	resp->code[length] = '\0';
	ch_start = ch_end+1;

	// Msg
	ch_end = (char*)memchr(ch_start,'\r',len-(ch_start-str));
	length = ch_end - ch_start;
	resp->msg = (char*)malloc(length+1);
	memcpy(resp->msg, ch_start, length);
	resp->msg[length] = '\0';
	ch_start = ch_end+2;

	//Headers
	length = len-(ch_start-str)-2;
	resp->headers = (char*)malloc(length+1);
	memcpy(resp->headers, ch_start, length);
	resp->headers[length] = '\0';

	// Length
	resp->length = atoi(strstr(resp->headers,"Content-Length: ")+16);

	// Body
	resp->body = (char*)malloc(resp->length+1);
	resp->body[resp->length] = '\0';
	
	return resp;	
}

//Free response
void freeResponse(struct response* resp){
	free(resp->protocol);
	free(resp->code);
	free(resp->msg);
	free(resp->body);
	free(resp);
}
