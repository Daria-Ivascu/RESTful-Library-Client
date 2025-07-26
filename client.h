#ifndef _CLIENT_H_
#define _CLIENT_H

#define HOST "63.32.125.183"
#define PORT 8081

// Struct to keep user's credentials (for both normal user and admin)
struct user_info {
    char username[100];
    char password[100];
};

// Struct to keep movie's information
struct movie_info {
    char title[100];
    int year;
    char description[100];
    double rating;
};

// Commands available only for admin
void login_admin(char *cookie, int *is_logged_in);
void add_user(char *cookie, int *is_logged_in);
void get_users(char *cookie, int *is_logged_in);
void delete_user(char *cookie, int *is_logged_in);
void logout_admin(char *cookie, int *is_logged_in);

// Normal user login and logout
void login_user(char *user_cookie, int *is_user_logged_in);
void logout_user(char *user_cookie, int *is_user_logged_in);

// Command to access the library
void get_access(char *cookie, char *jwt_token, int *is_user_logged_in);

// Actions on movies
void add_movie(char *jwt_token, int *is_user_logged_in);
void get_movies(char *jwt_token, int *is_user_logged_in);
void get_movie(char *jwt_token, int *is_user_logged_in);
void delete_movie(char *jwt_token, int *is_user_logged_in);
void update_movie(char *jwt_token, int *is_user_logged_in);

// Actions on collections
void add_collection(char *jwt_token, int *is_user_logged_in);
void add_movie_to_collection(char *jwt_token, int *is_user_logged_in);
void get_collections(char *jwt_token, int *is_user_logged_in);
void get_collection(char *jwt_token, int *is_user_logged_in);
void delete_collection(char *jwt_token, int *is_user_logged_in);
void delete_movie_from_collection(char *jwt_token, int *is_user_logged_in);

#endif