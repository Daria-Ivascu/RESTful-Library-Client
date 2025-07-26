# Tema 3 - Client web. Comunicatie cu REST API
_Ivașcu Andreea-Daria_
_322CC_

## Cerinta generala a temei
    Pornind de la informatiile si functiile date in laboratorul 9, am 
implementat un client HTTP care poate comunica cu un REST API, realizand
request-uri de tipul POST, GET, DELETE si PUT. In request-uri, am adaugat
informatiile necesare pentru a comunica cu serverul pentru a cere anumite
informatii despre o biblioteca de filme. Din raspunsurile primite de la server,
sunt extrase doar datele necesare comenzii introduse de client in terminal,
urmand ca mai apoi acestea sa fie afisate.

    -> Pentru a parsa mai clar datele despre un user sau un film, am creat 2
structuri pe care le-am folosit in crearea cererilor pentru server.

## Biblioteca parson
    Am folosit biblioteca parson, deoarece ofera o interfata destul de simpla 
si eficienta pentru partea de serializare a si deserializare a datelor din JSON.
    Am folosit-o pentru crearea, stergerea, actualizarea filmelor/colectiilor
de filme, dar si pentru extragerea informatiilor din raspunsurile de tip JSON
de la server pentru a le afisa utilizatorului.

## Cookie-urile de sesiune si verificarea conectivitatii
    In cadrul aplicatiei, am folosit 2 cookie-uri de sesiune: unul pentru
admin si unul pentru user. Astfel, atat adminul cat si userul raman conectati
pana la delogare (sau pana la expirarea conexiunii) si sunt autorizati sa
faca actiunile destinate rolurilor lor.

    Odata ce un client se logheaza (fie admin, fie user), ca raspuns al cererii
de conectare cu succes, serverul va trimite un cookie de sesiune, care va fi
trimis dupa automat la fiecare cerere HTTP care va urma. Astfel, cookie-ul de
sesiune este folosit pentru a valida sesiunea clientului, serverul stiind cine
efectueaza cererile si care sunt permisiunile acestuia.    

    Pentru a verifica conectivitatea atat a adminului, cat si a clientului, am
realizat o variabila de verificare (o variabila pentru admin si una pentru user)
care va fi marcata cu 1 atunci cand conexiunea este activa si cu 0 altfel. Apoi,
am folosit aceasta variabila pe tot parcursul codului, pentru a verifica
permisiunile clientului pentru anumite actiuni.

## JWT Token
    Pentru operatiile cu biblioteca de filme si colectii, pentru a verifica
autentificarea si autorizarea utilizatorilor, am folosit JWT token. Astfel,
prin acesta doar utilizatorii autentificati si cu permisiunile necesare pot
accesa sau modifica informatiile din biblioteca.

## Functionalitatile aplicatiei
    Pentru rolul de admin, acesta poate sa faca urmatoarele actiuni:
    - login_admin - se extrag credentialele acestuia si se trimite o cerere
        POST catre server, care intoarce spre admin un cookie de login
    - logout_admin - daca utilizatorul este conectat ca admin, se va trimite
        o cerere GET catre server
    - add_users - daca utilizatorul este conectat ca admin, se va trimite o 
        cerere POST catre server cu informatiile introduse de admin (noul
        username si parola pentru cont)
    - get_users - daca utilizatorul este conectat ca admin, se va trimite o
        cerere GET catre server care va intoarce ca raspuns JSON toti
        utilizatorii creati pe server sub adminul curent
    - delete_user - daca utilizatorul este conectat ca admin, acesta va trimite
        o cerere DELETE catre server cu username-ul userului care va fi sters
        de pe server
    
    Pentru rolul de user, acesta poate sa faca urmatoarele actiuni:
    - login - se extrag credentialele userului si se trimite o cerere POST catre
        server, care intoarce spre user un cookie de login
    - logout - daca utilizatorul este conectat, se va trimite o cerere
        GET catre server
    - get_access - daca utilizatorul este conectat, se va trimite o 
        cerere GET catre server, iar daca cererea a fost primita cu succes, se
        va returna un JWT token care confirma accesul utilizatorului la biblioteca
    - add_movie - daca utilizatorul este conectat, se va trimite o
        cerere POST catre server cu informatiile necesare filmului (title, year,
        description, rating)
    - get_movie - daca utilizatorul este conectat si are acces la biblioteca, 
        se solicita un id de film si se trimite o cerere GET catre server pentru 
        a extrage detalii despre filmul respectiv
    - get_movies - daca utilizatorul este conectat si are acces la biblioteca,
        se trimite o cerere GET catre server pentru a obtine lista filmelor
        disponibile
    - delete_movie - daca utilizatorul este conectat si are acces la biblioteca,
        se solicita id-ul unui film si se trimite o cerere DELETE catre server
        pentru a sterge filmul respectiv
    - update_movie - daca utilizatorul este conectat si are acces la biblioteca,
        se solicita id-ul si noile detalii ale filmului, apoi se trimite o cerere
        PUT catre server pentru a actualiza informatiile filmului
    - add_collection - daca utilizatorul este conectat si are acces la biblioteca,
        se solicita titlul colectiei si lista de filme, apoi se trimite o cerere
        POST catre server pentru a crea o noua colectie de filme
    - add_movie_to_collection - daca utilizatorul este conectat, are acces la
        biblioteca si este owner al colectiei, se solicita id-ul colectiei si
        id-ul filmului, apoi se trimite o cerere POST catre server pentru a adauga
        filmul in colectia respectiva
    - get_collections - daca utilizatorul este conectat si are acces la biblioteca,
        se trimite o cerere GET catre server pentru a obtine lista colectiilor
        disponibile
    - get_collection - daca utilizatorul este conectat si are acces la biblioteca,
        se solicita id-ul unei colectii si se trimite o cerere GET catre server
        pentru a obtine detalii despre colectia respectiva
    - delete_collection - daca utilizatorul este conectat, are acces la biblioteca
        si este owner al colectiei, se solicita id-ul colectiei si se trimite o
        cerere DELETE catre server pentru a sterge colectia respectiva
    - delete_movie_from_collection - daca utilizatorul este conectat, are acces
        la biblioteca si este owner al colectiei, se solicita id-ul colectiei si
        id-ul filmului, apoi se trimite o cerere DELETE catre server pentru a
        sterge filmul din colectie



