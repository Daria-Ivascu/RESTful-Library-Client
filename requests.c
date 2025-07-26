#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helper.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Adds cookies in the request
    if (cookies != NULL && cookies_count > 0) {
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
       }
       compute_message(message, line);
    }
    // Adds final new line
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_get_request_with_token(char *host, char *url, char *query_params,
                                      char *cookies, int cookies_count, char *jwt_token) 
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adds the authorization header with JWT
    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
    }

    // Adds cookie
    if (cookies != NULL && cookies_count > 0) {
        strcpy(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Adds final new line
    compute_message(message, "");

    free(line);
    return message;
}


char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int body_data_fields_count, char *cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adds necessary headers (Content-Type and Content-Length are mandatory)
    // in order to write Content-Length you must first compute the message size
    // Content-Type header
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    
    // Content-Length header
    int content_length = strlen(&body_data[0]);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    // Adds cookies
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");
       for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
       }
       compute_message(message, line);
    }
    // Adds new line at end of header
    compute_message(message, "");

    // Adds the actual payload data
    compute_message(message, &body_data[0]);

    free(line);
    return message;
}

char *compute_post_request_with_token(char *host, char *url, char *content_type, char *body_data,
                                       int body_data_fields_count, char *cookies, int cookies_count, char *jwt_token) 
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adds necessary headers (Content-Type and Content-Length are mandatory)
    // in order to write Content-Length you must first compute the message size
    // Content-Type header
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // Content-Length header
    int content_length = strlen(&body_data[0]);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    // Adaugă header-ul Authorization cu token-ul JWT
    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
    }

    // Adds the authorization header with JWT
    if (cookies != NULL) {
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Adds new line at end of header
    compute_message(message, "");

    // Adds the actual payload data
    compute_message(message, &body_data[0]);

    free(line);
    return message;
}


char *compute_delete_request(char *host, char *url, char *query_params,
                                    char *cookies, int cookies_count){
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    // Adds cookies
    if (cookies != NULL && cookies_count > 0) {
       strcpy(line, "Cookie: ");
       for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
       }
       compute_message(message, line);
    }

    // Adds final new line
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_delete_request_with_token(char *host, char *url, char *query_params,
                                         char *cookies, int cookies_count, char *jwt_token) 
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adds the authorization header with JWT
    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
    }

    // Adds cookies
    if (cookies != NULL && cookies_count > 0) {
        strcpy(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Adds final new line 
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_put_request_with_token(char *host, char *url, char *content_type, char *body_data,
                                      int body_data_fields_count, char *cookies, int cookies_count, char *jwt_token) 
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Writes the method name, URL, request params (if any) and protocol type
    sprintf(line, "PUT %s HTTP/1.1", url);
    compute_message(message, line);

    // Adds the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Adds necessary headers (Content-Type and Content-Length are mandatory)
    // Content-Type header
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // Content-Length header
    int content_length = strlen(body_data);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    // Add Authorization header with JWT token
    if (jwt_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt_token);
        compute_message(message, line);
    }

    // Adds cookies (if provided)
    if (cookies != NULL && cookies_count > 0) {
        strcpy(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, &cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Adds final new line
    compute_message(message, "");

    // Adds the actual payload data (body_data)
    compute_message(message, body_data);

    free(line);
    return message;
}
