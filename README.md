# Tema 4 - PCOM

Pentru aceasta tema, am avut de implementat un client pentru un REST API. Eu am implementat
tema in C++, asa ca, pentru parsarea JSON, am folosit biblioteca nlohmann.

Am definit in "requests.h" toate caile de acces pentru requesturile HTTP, cat si IP-ul 
serverului la care se va conecta clientul si portul pe care trebuie sa se conecteze.

Pentru fiecare comanda data clientului, el deschide cate un socket TCP, trimite request-ul
HTTP si intoarce raspunsul, dupa care conexiunea va fi inchisa.
De asemenea, pentru fiecare comanda am facut cate o functie separata.

Pentru requesturile POST si GET, exista niste functii definite care completeaza o parte din 
campurile necesare.

De asemenea, exista niste bool-uri care tin cont daca utilizatorul este logat sau are
acces la biblioteca, astfel incat sa nu se mai trimita un mesaj aiurea catre server.

## Register

In functia registerFunct(), iau datele de la utilizator si le trimit catre server, construiesc un json cu ele, si creez mesajul cu headerele care va fi trimis catre server. Dupa ce primesc raspunsul de la server, verific codul HTTP. In functie de cod, afisez un mesaj corespunzator.
Daca e o eroare, parsez jsonul si afisez mesajul de eroare.
Functia nu returneaza nimic si nu tine cont de faptul ca utilizatorul este logat sau nu.
De asemenea, dupa inregistrare, utilizatorul nu va fi logat automat. 

## Login

In functia loginFunct(), pasii sunt aceiasi ca la functia de register. Diferenta este ca,
mesajul va fi transmis catre alta cale de acces. Din nou, verific codul HTTP si afisez
mesajul corespunzator. Daca e o eroare, parsez jsonul si afisez mesajul de eroare. Functia
intoarce cookie-ul primit, alaturi de cheie. Pe acestea le voi pune intr-un obiect de tip
Cookie, care are ca atribute cookie-ul si cheia. Am facut un vector de Cookie-uri, pentru
cazul in care va fi nevoie, desi pentru tema aceasta nu este necesar. De asemenea, setez
bool-ul de logare pe true, pentru ca utilizatorul este logat.

## Enter Library

In functia getLibraryAccess(), creez mesajul cu headerele necesare, adaugand acolo si cookie-ul
si cheia, alaturi de headerul de authorization. Dupa ce primesc raspunsul de la server, verific codul HTTP. In functie de cod, afisez corespunzator. In caz de succes, primesc de la
server un json cu tokenul JWT. Parsez jsonul si intorc tokenul. Acesta va fi salvat in main
si va fi folosit pentru requesturile de GET si POST. De asemenea, setez bool-ul de acces la
biblioteca pe true, pentru ca utilizatorul are acces la biblioteca.

## Get Books

In functia getBooks(), pasii sunt aceiasi ca la functia de getLibraryAccess(). Diferenta este ca, aici voi adauga in mesajul cu headerele si tokenul JWT. Dupa ce primesc raspunsul de la server, verific codul HTTP. In functie de cod, afisez corespunzator. In caz de succes, primesc de la server un json cu toate cartile din biblioteca. Parsez jsonul si afisez cartile in ordinea in care le-am primit. Daca nu exista carti in biblioteca, afisez un mesaj corespunzator.

## Get Book

In functia getBook(), pasii sunt aceiasi ca la functia de getBooks(). Diferenta este ca, aici voi avea
nevoie de id-ul cartii pe care utilizatorul vrea sa o vada, si il voi adauga in calea de acces. Parsez jsonul primit si afisez detaliile cartii. Daca nu exista cartea, afisez un mesaj corespunzator.

## Add Book

In functia addBook(), utilizatorul trebuie sa introduca datele cartii pe care vrea sa o adauge. Se va face o verificare a campurilor, sa nu fie empty sau nr de pagini sa fie un string si nu un numar. Daca e asa, se afiseaza un mesaj de eroare. Creez jsonul, il trimit catre server si astept raspunsul. Daca e o eroare, parsez jsonul si afisez mesajul de eroare. Daca nu, afisez un mesaj corespunzator.

## Delete Book

In functia deleteBook(), pasii sunt aceiasi ca la functia de addBook(). Diferenta este ca, aici voi avea
nevoie doar de id-ul cartii pe care utilizatorul vrea sa o stearga, si il voi adauga in calea de acces. Daca nu exista cartea, afisez un mesaj corespunzator. Aici nu am mai facut o functie pentru creearea
headerelor, ci am facut-o eu direct. Dupa, indiferent de rezultat, afisez un mesaj corespunzator.

## Logout

In functia logout(), creez headerul si il trimit catre server, dovedind ca sunt logat. Acesta va fi sters de la server. Dupa, setez bool-ul de logare pe false, pentru ca utilizatorul nu mai este logat.

## Exit

Cand clientul primeste comanda "exit", se va apela functia de delogare, si se va inchide clientul, eliberand memoria alocata.

## Alte detalii

Am incercat, ca la fiecare pas, sa dezaloc memoria, sa afisez mesajele cat mai estetic si corect,
sa inchid socketii si sa inchid clientul corect. De asemenea, am incercat sa fac codul cat mai
usor de citit.

In enunt nu scrie nimic de faptul ca un utilizator logat n-ar trebui sa poata sa se inregistreze
sau sa se logheze din nou, de aceea nu am implementat asta.
