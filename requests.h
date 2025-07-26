#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char *cookies, int cookies_count);

// computes the GET request with token
char *compute_get_request_with_token(char *host, char *url, char *query_params,
                            char *cookies, int cookies_count, char *jwt_token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
							int body_data_fields_count, char* cookies, int cookies_count);

// computes the POST request with JWT token
char *compute_post_request_with_token(char *host, char *url, char *content_type, char *body_data,
                                int body_data_fields_count, char *cookies, int cookies_count, char *jwt_token);

// computes and returns a DELETE request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_delete_request(char *host, char *url, char *query_params,
							char *cookies, int cookies_count);

// computes and returns a DELETE request string with JWT token
char *compute_delete_request_with_token(char *host, char *url, char *query_params,
                                         char *cookies, int cookies_count, char *jwt_token) ;

// computes and returns a PUT request string with JWT token (query_params
// and cookies can be set to NULL if not needed)                        
char *compute_put_request_with_token(char *host, char *url, char *content_type, char *body_data,
                                      int body_data_fields_count, char *cookies, int cookies_count, char *jwt_token);

#endif
