# RESTful Library Client

## General Description
I implemented an HTTP client that can communicate with a REST API by making POST, GET, DELETE, and PUT requests. Each request includes the necessary data to interact with the server in order to request the specific information related to a movie library. Only the relevant information from the server's JSON responses are extracted, based on the command entered in the terminal by the client, and also displayed in terminal.

To make the data related to users and movies easier to parse, I created two structures that are used when building requests to the server.

## Parson library
I used the Parson library because it provides a simple and efficient interface for serializing and deserializing JSON data.
It is used for creating, deleting, and updating movies/collections, as well as for extracting information from JSON responses received from the server to display to the user.

## Session Cookies and Conectivity Checks
The application uses two session cookies: one for the admin and one for the user. This ensures that both the admin and the user remain logged in until they choose to log out (or until the session expires) and are authorized to perform actions specific to their roles.

Once a client logs in (either as an admin or a user), the server responds with a session cookie, which is automatically included in every subsequent HTTP request. This cookie is used to validate the client session and determine the client's identity and permissions.

To track the connectivity status of both admin and user roles, I implemented two variables (one for each role). These variables are set to 1 when the connection is active and 0 otherwise. They are used throughout the code to check if the user has the required permissions to perform certain actions.

## JWT Token
For operations related to the movie library and collections, I used a JWT token to verify user authentication and authorization. This ensures that only authenticated users with appropriate permissions can access or modify the library's data.

## Application Features

### Admin Role
An admin can perform the following actions:
- **login_admin** – Extracts the admin's credentials and sends a POST request to the server. On success, the server returns a session cookie.
- **logout_admin** – Sends a GET request to the server to log out, if currently logged in as admin.
- **add_users** – Sends a POST request to create a new user, using the username and password provided by the admin.
- **get_users** – Sends a GET request to retrieve a list of all users created under the current admin.
- **delete_user** – Sends a DELETE request to remove a specific user, using their username.

### User Role
A user can perform the following actions:
- **login** – Extracts the user's credentials and sends a POST request. On success, the server returns a session cookie.
- **logout** – Sends a GET request to log out, if currently logged in.
- **get_access** – Sends a GET request to obtain a JWT token granting access to the movie library.
- **add_movie** – Sends a POST request to add a new movie, with fields such as title, year, description and rating.
- **get_movie** – Sends a GET request for a specific movie ID, returning its details (if the user is connected and has access).
- **get_movies** – Sends a GET request to retrieve a list of all available movies.
- **delete_movie** – Sends a DELETE request to remove a movie, based on its requested ID.
- **update_movie** – Sends a PUT request with updated information for a specific movie, identified by its ID provided by the user.
- **add_collection** – Sends a POST request to create a new movie collection, with a given title and a list of movie IDs.
- **add_movie_to_collection** – Sends a POST request to add a movie to a collection (if the user is the owner).
- **get_collections** – Sends a GET request to retrieve all collections.
- **get_collection** – Sends a GET request for a specific collection ID to retrieve its details.
- **delete_collection** – Sends a DELETE request to remove a collection (if the user is the owner).
- **delete_movie_from_collection** – Sends a DELETE request to remove a movie from a collection (if the user is the owner).