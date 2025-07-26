#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#include "helper.h"
#include "requests.h"
#include "parson.h"

// Reads data written in terminal
// type: 0 - char, 1 - int, 2 - double
void read_info(char *print, void *info, int type) {
    // Prints the messages in the desired format
    printf("%s=", print);

    if (type == 0) {
        // Reads the data (float)
        fgets((char *)info, 100, stdin);

        // Deletes the new line character
        int len = strlen(info);
        if (((char *)info)[len - 1] == '\n') {
            ((char *)info)[len - 1] = '\0';
        }
    } else if (type == 1) {
        // Reads the data (integer)
        char buffer[100];
        fgets(buffer, 100, stdin);

        // Convert the string input to integer
        if (buffer[0] == '\n') {
            *(int*)info = 0;
        } else {
            *(int*)info = atoi(buffer);
        }

    } else if (type == 2) {
        // Reads the data (double)
        char buffer[100];
        fgets(buffer, 100, stdin);

        // Convert the string input to double
        if (buffer[0] == '\n') {
            *(double*)info = 0.0;
        } else {
            *(double*)info = atof(buffer);
        }
    }
}

// Logs the admin to the server
void login_admin(char *admin_cookie, int *is_admin_logged_in) {
    // Checks if the admin is already logged in
    if (*is_admin_logged_in) {
        printf("ERROR: Adminul este deja conectat!\n");
        return;
    }

    struct user_info admin;

    // Reads the input from the admin
    read_info("username", admin.username, 0);
    read_info("password", admin.password, 0);
    
    // Creates a JSON object with admin's credentials
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", admin.username);
    json_object_set_string(root_object, "password", admin.password);

    serialized_string = json_serialize_to_string_pretty(root_value);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request(HOST, "/api/v1/tema/admin/login", 
                        "application/json", serialized_string, 1, NULL, 0);
    
    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCESS: Admin logat cu succes!\n");

        // Marks the admin's login
        *is_admin_logged_in = 1;

        // Extracts the cookie from the valid response
        if (strstr(response, "Set-Cookie:")) {
            char *cookies = strstr(response, "Set-Cookie:") + strlen("Set-Cookie: ");
            char *cookie_end = strchr(cookies, ';');
            if (cookie_end) {
                *cookie_end = '\0';
            }

            strcpy(admin_cookie, cookies);
        }
    } else if (response_status == 400) {
        printf("ERROR: Cererea este invalida!\n");
    } else if (response_status == 403) {
        printf("ERROR: Credentiale nu se potrivesc!\n");
    } else if (response_status == 409) {
        printf("ERROR: Admin deja logat!\n");
    }
    
    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Adds a new client to the server
void add_user(char *admin_cookie, int *is_admin_logged_in) {
    // Checks if the admin is already logged in
    if (!*is_admin_logged_in) {
        printf("ERROR: Trebuie sa ai rol de admin pentru aceasta actiune!\n");
        return;
    }

    struct user_info user;

    // Reads the input from the admin
    read_info("username", user.username, 0);
    read_info("password", user.password, 0);

    // Creates a JSON object with the new user's credentials
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", user.username);
    json_object_set_string(root_object, "password", user.password);

    serialized_string = json_serialize_to_string_pretty(root_value);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request(HOST, "/api/v1/tema/admin/users",
                            "application/json", serialized_string, 1, admin_cookie, 1);

    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 201) {
        printf("SUCCES: Utilizator a fost adaugat cu succes!\n");
    } else if (response_status == 409) {
        printf("ERROR: Utilizatorul exista deja! \n");
    } else if (response_status == 400)  {
        printf("ERROR: Cererea este invalida!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Extracts all the users from the server
void get_users(char *admin_cookie, int *is_admin_logged_in) {
    // Checks if the admin is logged in
    if (!*is_admin_logged_in) {
        printf("ERROR: Trebuie sa ai rol de admin pentru aceasta actiune!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request(HOST, "/api/v1/tema/admin/users",
                                        NULL, admin_cookie, 1);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Lista utilizatorilor:\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts the info about each user and prints it in the desired format
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);
        JSON_Array *users = json_object_get_array(response_obj, "users");
        size_t users_nr = json_array_get_count(users);

        for (size_t i = 0; i < users_nr; i++) {
            JSON_Object *current_user = json_array_get_object(users, i);
            int user_id = (int)json_object_get_number(current_user, "id");
            const char *user_username = json_object_get_string(current_user, "username");
            const char *user_password = json_object_get_string(current_user, "password");
            printf("#%d %s:%s\n", user_id, user_username, user_password);
        }

        // Frees the resource
        json_value_free(response_val);
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Deletes a user from the server
void delete_user(char *admin_cookie, int *is_admin_logged_in) {
    // Checks if the admin is logged in
    if (!*is_admin_logged_in) {
        printf("ERROR: Trebuie sa ai rol de admin pentru aceasta actiune!\n");
        return;
    }

    // Extracts the username given by the admin
    char given_username[100];
    read_info("username", given_username, 0);

    // Keeps the path to the given username
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/admin/users/%s", given_username);

    // Opens the coneection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the DELETE request
    char *delete_request = compute_delete_request(HOST, final_url, NULL, admin_cookie, 1);
    
    // Sends the request to the server
    send_to_server(sock_fd, delete_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Utilizatorul a fost sters cu succes!\n");
    } else if (response_status == 404) {
        printf("ERROR: Username-ul introdus este invalid!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(delete_request);
    free(response);
}

// Logouts the admin
void logout_admin(char *admin_cookie, int *is_admin_logged_in) {
    // Checks if the admin is already logged in
    if (!*is_admin_logged_in) {
        printf("ERROR: Adminul nu este conectat!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request(HOST, "/api/v1/tema/admin/logout",
                                        NULL, admin_cookie, 1);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Adminul a fost delogat cu succes!\n");
        *is_admin_logged_in = 0;
        admin_cookie[0] = '\0';
    } else if (response_status == 401) {
        printf("ERROR: Nu esti autentificat!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Logins the user
void login_user(char *user_cookie, int *is_user_logged_in) {
    // Checks if the user is already connected
    if (*is_user_logged_in) {
        printf("ERROR: Utilizatorul este deja conectat!\n");
        return;
    }

    struct user_info user;
    char admin_username[100];

    // Read the input from the user
    read_info("admin_username", admin_username, 0);
    read_info("username", user.username, 0);
    read_info("password", user.password, 0);

    // Creates a JSON object with the user's credentials
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    json_object_set_string(root_object, "admin_username", admin_username);
    json_object_set_string(root_object, "username", user.username);
    json_object_set_string(root_object, "password", user.password);

    serialized_string = json_serialize_to_string_pretty(root_value);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request(HOST, "/api/v1/tema/user/login",
                                "application/json", serialized_string, 1, NULL, 0);
    
    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status code
    if (response_status == 200) {
        printf("SUCCESS: Autentificare reusita!\n");

        // Marks the user's login
        *is_user_logged_in = 1;

        // Extracts the cookie from the valid response
        if (strstr(response, "Set-Cookie:")) {
            char *cookies = strstr(response, "Set-Cookie:") + strlen("Set-Cookie: ");
            char *cookie_end = strchr(cookies, ';');
            if (cookie_end) {
                *cookie_end = '\0';
            }

            strcpy(user_cookie, cookies);
        }
    } else if (response_status == 400) {
        printf("ERROR: Cererea este invalida!\n");
    } else if (response_status == 403) {
        printf("ERROR: Credentiale nu se potrivesc!\n");
    } else if (response_status == 409) {
        printf("ERROR: Admin deja logat!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Logouts the user
void logout_user(char *user_cookie, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Userul nu este conectat!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request(HOST, "/api/v1/tema/user/logout", 
                                        NULL, user_cookie, 1);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Ai fost delogat cu succes!\n");
        *is_user_logged_in = 0;
        user_cookie[0] = '\0';
    } else if (response_status == 401) {
        printf("ERROR: Nu esti autentificat!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Gets access to the movie collection (for users)
void get_access(char *cookie, char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request(HOST, "/api/v1/tema/library/access",
                                            NULL, cookie, 1);
    
    // Sends the request to tge server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status code
    if (response_status == 200) {
        printf("SUCCES: Token JWT primit cu succes!\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // EXtracts the token's info
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);

        const char *received_token = json_object_get_string(response_obj, "token");
        strcpy(jwt_token, received_token);

        // Frees the resource
        json_value_free(response_val);
    } 

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Adds a movie to the movie collection
void add_movie(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is already logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    struct movie_info movie;

    // Reads the input from the user
    read_info("title", movie.title, 0);
    read_info("year", &movie.year, 1);
    read_info("description", movie.description, 0);
    read_info("rating", &movie.rating, 2);

    // Checks if the rating is valid
    if (movie.rating < 0 || movie.rating > 10.0) {
        printf("ERROR: Ratingul trebuie sa fie intre 0 si 10!\n");
        return;
    }

    // Creates the JSON object with the new movie's information
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", movie.title);
    json_object_set_number(root_object, "year", movie.year);
    json_object_set_string(root_object, "description", movie.description);
    json_object_set_number(root_object, "rating", movie.rating);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request_with_token(HOST, "/api/v1/tema/library/movies",
                            "application/json", serialized_string, 1, NULL, 0, jwt_token);

    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 201) {
        printf("SUCCES: Filmul a fost adaugat cu succes!\n");
    } else if (response_status == 400) {
        printf("ERROR: Datele introduse nu sunt valide/nu sunt complete!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Extracts all the movied from the server and assigns each movie an id
void get_movies(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request_with_token(HOST, "/api/v1/tema/library/movies",
                                            NULL, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

     // Checks the possible status codes
     if (response_status == 200) {
        printf("SUCCES: Lista filmelor:\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts the info about each movie and prints it in the desired format
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);
        JSON_Array *movies = json_object_get_array(response_obj, "movies");
        size_t movies_nr = json_array_get_count(movies);

        for (size_t i = 0; i < movies_nr; i++) {
            JSON_Object *current_movie = json_array_get_object(movies, i);
            int movie_id = (int)json_object_get_number(current_movie, "id");
            const char *title = json_object_get_string(current_movie, "title");
            printf("#%d %s\n", movie_id, title);
        }

        // Frees the resource
        json_value_free(response_val);
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Extracts the information about a movie
void get_movie(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Extracts the desired movie id
    int movie_id;
    read_info("id", &movie_id, 1);

    // Keeps the path to the given movie
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/movies/%d", movie_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request_with_token(HOST, final_url,
                                            NULL, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Detalii film:\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts the info about each movie and prints it in the desired format
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);
        
        // Extracts the movie's info from the JSON response
        const char *title = json_object_get_string(response_obj, "title");
        int year = (int)json_object_get_number(response_obj, "year");
        const char *description = json_object_get_string(response_obj, "description");
        const char *rating_string = json_object_get_string(response_obj, "rating");

        // Transforms the rating into a real number
        double rating = atof(rating_string);
        
        // Prints the data in the desired format
        printf("title: %s\n", title);
        printf("year: %d\n", year);
        printf("description: %s\n", description);
        printf("rating: %.1f\n", rating);

        // Frees the resource
        json_value_free(response_val);
    } else if (response_status == 404) {
        printf("ERROR: ID-ul cerut nu se regaseste in server!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Deletes a movie from the library
void delete_movie(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Extracts the movie id given by the user
    int given_id;
    read_info("id", &given_id, 1);

    // Keeps the path to the given movie
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/movies/%d", given_id);

    // Opens the coneection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the DELETE request
    char *delete_request = compute_delete_request_with_token(HOST, final_url, 
                                    NULL, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, delete_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Filmul a fost sters cu succes!\n");
    } else if (response_status == 404) {
        printf("ERROR: ID invalid. Filmul nu este in library!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(delete_request);
    free(response);
}

// Updates the information about a movie
void update_movie(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    struct movie_info new_movie;
    int given_id;

    // Reads the input from the user
    read_info("id", &given_id, 1);
    read_info("title", new_movie.title, 0);
    read_info("year", &new_movie.year, 1);
    read_info("description", new_movie.description, 0);
    read_info("rating", &new_movie.rating, 2);

    // Checks if the rating is valid
    if (new_movie.rating < 0 || new_movie.rating > 10.0) {
        printf("ERROR: Ratingul trebuie sa fie intre 0 si 10!\n");
        return;
    }

    // Creates the JSON object with the new movie's information
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", new_movie.title);
    json_object_set_number(root_object, "year", new_movie.year);
    json_object_set_string(root_object, "description", new_movie.description);
    json_object_set_number(root_object, "rating", new_movie.rating);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    // Keeps the path to the given movie
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/movies/%d", given_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the PUT request
    char *put_request = compute_put_request_with_token(HOST, final_url, 
                            "application/json", serialized_string, 1, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, put_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Filmul a fost actualizat cu succes!\n");
    } else if (response_status == 404) {
        printf("ERROR: ID invalid. Filmul nu este in library!\n");
    } else if (response_status == 400) {
        printf("ERROR: Datele introduse sunt invalide/incomplete!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(put_request);
    free(response);
}

// Adds a collection to the server
void add_collection(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Reads the input from the user
    char collection_title[100];
    read_info("title", collection_title, 0);

    int num_movies;
    read_info("num_movies", &num_movies, 1);

    // Creates the JSON object with the new collection's information
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", collection_title);
    json_object_set_number(root_object, "num_movies", num_movies);
    
    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request_with_token(HOST, "/api/v1/tema/library/collections",
                                "application/json", serialized_string, 1, NULL, 0, jwt_token);

    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 201) {
        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts created collection's id
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);

        int collection_id = (int)json_object_get_number(response_obj, "id");
        int check = 1;
        for (int i = 0; i < num_movies; i++) {
            // Keeps the path to the given username
            char final_url[256];
            sprintf(final_url, "/api/v1/tema/library/collections/%d/movies", collection_id);

            // Reads the movie_id from the user
            int movie_id;
            char print_prompt[50];
            sprintf(print_prompt, "movie_id[%d]", i);
            read_info(print_prompt, &movie_id, 1);

            // Creates the JSON object with the movie's information
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_number(root_object, "id", movie_id);

            char *serialized_string = NULL;
            serialized_string = json_serialize_to_string_pretty(root_value); 
            
            // Creates the POST request
            char *movie_post_request = compute_post_request_with_token(HOST, final_url, "application/json", 
                                                            serialized_string, 1, NULL, 0, jwt_token);

            // Sends the request to the server
            send_to_server(sock_fd, movie_post_request);

            // Extracts the response from the server
            char *movie_response = receive_from_server(sock_fd);

            // Creates a copy of the server's response so that no needed information
            // is lost during 'strtok' use
            char movie_copy_response[BUFLEN];
            strcpy(movie_copy_response, movie_response);

            // Extracts the info from the first line of the server's response
            char *movie_response_info = strtok(movie_copy_response, "\r\n");

            // Extracts the status code of the action
            int movie_response_status = 0;
            sscanf(movie_response_info, "HTTP/1.1 %d", &movie_response_status);

            // Checks if the movie was created successfully
            if (movie_response_status == 404) {
                check = 0;
            }
        }

        if (check == 1) {
            printf("SUCCES: Colectia a fost creata cu succes!\n");
        } else if (check == 0) {
            printf("ERROR: Filmele nu au putut fi adaugate in colectie!\n");
        }

        // Frees up the resource
        json_value_free(response_val);
    } else if (response_status == 404) {
        printf("ERROR: Datele introduse sunt invalide/incomplete!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Adds a movie to a collection
void add_movie_to_collection(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Reads the given ids for the collection and the movie
    int collection_id, movie_id;
    read_info("collection_id", &collection_id, 1);
    read_info("movie_id", &movie_id, 1);

    // Creates the JSON object with the movie's information
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_number(root_object, "id", movie_id);

    char *serialized_string = NULL;
    serialized_string = json_serialize_to_string_pretty(root_value);

    // Keeps the path to the given collection_id
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/collections/%d/movies", collection_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the POST request
    char *post_request = compute_post_request_with_token(HOST, final_url, "application/json", 
                                serialized_string, 1, NULL, 0, jwt_token);

    // Sends the request to the server
    send_to_server(sock_fd, post_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 201) {
        printf("SUCCES: Filmul a fost adaugat in colectie!\n");
    } else if (response_status == 403) {
        printf("ERROR: Nu esti owner al colectiei!\n");
    } else if (response_status == 404) {
        printf("ERROR: Datele introduse sunt invalie/incomplete!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(post_request);
    free(response);
}

// Gets the titles of all the user's collections
void get_collections(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request_with_token(HOST, "/api/v1/tema/library/collections",
                                        NULL, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Lista colectiilor:\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts the info about each collection and prints it in the desired format
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);
        JSON_Array *collections = json_object_get_array(response_obj, "collections");
        size_t collections_nr = json_array_get_count(collections);

        for (size_t i = 0; i < collections_nr; i++) {
            JSON_Object *current_collection = json_array_get_object(collections, i);
            int collection_id = (int)json_object_get_number(current_collection, "id");
            const char *collection_title = json_object_get_string(current_collection, "title");
            printf("#%d %s\n", collection_id, collection_title);
        }

        // Frees the resource
        json_value_free(response_val);
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Gets the details of a specific collection
void get_collection(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Extracts the collection id given by the user
    int collection_id;
    read_info("id", &collection_id, 1);

    // Keeps the path to the given username
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/collections/%d", collection_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the GET request
    char *get_request = compute_get_request_with_token(HOST, final_url, NULL, 
                                                        NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, get_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Detalii colectie:\n");

        // Extracts the data from the JSON answer
        char *response_body = strstr(response, "\r\n\r\n");
        if (response_body) {
            response_body = response_body + 4;
        }

        // Extracts the info about the collection and prints it in the desired format
        JSON_Value *response_val = json_parse_string(response_body);
        JSON_Object *response_obj = json_value_get_object(response_val);
        
        const char *collection_title = json_object_get_string(response_obj, "title");
        const char *collection_owner = json_object_get_string(response_obj, "owner");
        JSON_Array *movies = json_object_get_array(response_obj, "movies");
        
        // Prints the data in the desired format
        printf("title: %s\n", collection_title);
        printf("owner: %s\n", collection_owner);
        
        size_t movies_nr = json_array_get_count(movies);
        for (size_t i = 0; i < movies_nr; i++) {
            JSON_Object *movie_obj = json_array_get_object(movies, i);
            int movie_id = (int)json_object_get_number(movie_obj, "id");
            const char *movie_title = json_object_get_string(movie_obj, "title");
            printf("#%d: %s\n", movie_id, movie_title);
        }

        // Frees the resource
        json_value_free(response_val);
    } else if (response_status == 403) {
        printf("ERROR: ID-ul colectiei nu este valid!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(get_request);
    free(response);
}

// Deletes a collection
void delete_collection(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Extracts the collection id given by the user
    int collection_id;
    read_info("id", &collection_id, 1);

    // Keeps the path to the given collection
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/collections/%d", collection_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the DELETE request
    char *delete_request = compute_delete_request_with_token(HOST, final_url, 
                                    NULL, NULL, 0, jwt_token);
    
    // Sends the request to the server
    send_to_server(sock_fd, delete_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 200) {
        printf("SUCCES: Colectia a fost stearsa cu succes!\n");
    } else if (response_status == 404) {
        printf("ERROR: ID-ul colectiei nu este valid!\n");
    } else if (response_status == 403) {
        printf("ERROR: Nu esti owner al colectiei!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(delete_request);
    free(response);
}

// Deletes a movie from a collection
void delete_movie_from_collection(char *jwt_token, int *is_user_logged_in) {
    // Checks if the user is logged in
    if (!*is_user_logged_in) {
        printf("ERROR: Utilizatorul trebuie sa fie conectat pentru aceasta actiune!\n");
        return;
    }

    // Checks if the user has access to the library
    if (jwt_token == NULL || strlen(jwt_token) == 0) {
        printf("ERROR: Nu ai acces la library!\n");
        return;
    }

    // Reads the given ids for the collection and the movie
    int collection_id, movie_id;
    read_info("collection_id", &collection_id, 1);
    read_info("movie_id", &movie_id, 1);

    // Keeps the path to the given collection_id
    char final_url[256];
    sprintf(final_url, "/api/v1/tema/library/collections/%d/movies/%d", collection_id, movie_id);

    // Opens the connection to the server
    int sock_fd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // Creates the DELETE request
    char *delete_request = compute_delete_request_with_token(HOST, final_url, NULL, 
                                    NULL, 0, jwt_token);

    // Sends the request to the server
    send_to_server(sock_fd, delete_request);

    // Extracts the response from the server
    char *response = receive_from_server(sock_fd);

    // Creates a copy of the server's response so that no needed information
    // is lost during 'strtok' use
    char copy_response[BUFLEN];
    strcpy(copy_response, response);

    // Extracts the info from the first line of the server's response
    char *response_info = strtok(copy_response, "\r\n");

    // Extracts the status code of the action
    int response_status = 0;
    sscanf(response_info, "HTTP/1.1 %d", &response_status);

    // Checks the possible status codes
    if (response_status == 201) {
        printf("SUCCES: Filmul a fost sters din colectie!\n");
    } else if (response_status == 403) {
        printf("ERROR: Nu esti owner al colectiei!\n");
    } else if (response_status == 400) {
        printf("ERROR: ID-ul colectiei nu este valid sau filmul nu a fost gasit in colectie!\n");
    }

    // Frees up the used resources and closes the socket
    close(sock_fd);
    free(delete_request);
    free(response);
}

int main(int argc, char *argv[]) {
    // Admin's cookie
    char *session_cookie_admin = malloc(BUFLEN * sizeof(char));
    memset(session_cookie_admin, 0, BUFLEN);
    int is_admin_logged_in = 0;

    // User's cookie
    char *session_cookie_user = malloc(BUFLEN * sizeof(char));
    memset(session_cookie_user, 0, BUFLEN);
    int is_user_logged_in = 0;

    // User's token
    char *jwt_token = malloc(BUFLEN * sizeof(char));
    memset(jwt_token, 0, BUFLEN);

    while (1) {
        // Allocates space for the command inserted by the client
        char *command = malloc(100 * sizeof(char));

        // Extracts the command
        fgets(command, 100, stdin);

        int len = strlen(command);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        // Checks what the inserted command is
        if (strcmp(command, "login_admin") == 0) {
            login_admin(session_cookie_admin, &is_admin_logged_in);
        } else if (strcmp(command, "add_user") == 0) {
            add_user(session_cookie_admin, &is_admin_logged_in);
        } else if (strcmp(command, "get_users") == 0) {
            get_users(session_cookie_admin, &is_admin_logged_in);
        } else if (strcmp(command, "delete_user") == 0) {
            delete_user(session_cookie_admin, &is_admin_logged_in);
        } else if (strcmp(command, "logout_admin") == 0) {
            logout_admin(session_cookie_admin, &is_admin_logged_in);
        } else if (strcmp(command, "login") == 0) {
            login_user(session_cookie_user, &is_user_logged_in);
        } else if (strcmp(command, "logout") == 0) {
            logout_user(session_cookie_user, &is_user_logged_in);
        } else if (strcmp(command, "add_movie") == 0) {
            add_movie(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "get_access") == 0) {
            get_access(session_cookie_user, jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "get_movies") == 0) {
            get_movies(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "get_movie") == 0) {
            get_movie(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "delete_movie") == 0) {
            delete_movie(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "update_movie") == 0) {
            update_movie(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "add_collection") == 0) {
            add_collection(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "add_movie_to_collection") == 0) {
            add_movie_to_collection(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "get_collections") == 0) {
            get_collections(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "get_collection") == 0) {
            get_collection(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "delete_collection") == 0) {
            delete_collection(jwt_token, &is_user_logged_in);
        } else if (strcmp(command, "delete_movie_from_collection") == 0) {
            delete_movie_from_collection(jwt_token, &is_user_logged_in);
        } else if (strcmp (command, "exit") == 0) {
            break;
        } else {
            printf("Comanda nerecunoscuta, probabil introdusa incorect! Mai incearca!\n");
        }

        // Frees the space used by the command
        free(command);
    }

    return 0;
}
