#include <stdio.h>
#include <stdlib.h>

int initMatrix(int ***matrix, int anzahlSpalten, int anzahlZeilen) {
    *matrix = malloc(anzahlSpalten * sizeof(int *));
    if (*matrix == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < anzahlSpalten; i++) {
        (*matrix)[i] = malloc(anzahlZeilen * sizeof(int));
        if ((*matrix)[i] == NULL) {
            printf("Kein Speicher mehr fuer Zeile %d\n", i);
            return EXIT_FAILURE;
        }
    }

    for (int x = 0; x < anzahlSpalten; x++) {
        for (int y = 0; y < anzahlZeilen; y++) {
            (*matrix)[x][y] = 0;
        }
    }
    return EXIT_SUCCESS;
}

void print(int **matrix, int anzahlSpalten, int anzahlZeilen) {
    printf("\nAktuelles Spielfeld:\n\n");

    for (int y = 0; y < anzahlZeilen; y++) {
        for (int x = 0; x < anzahlSpalten; x++) {
            printf("%d ", matrix[x][y]);
        }
        printf("\n");
    }
}

int addToRow(int **matrix, int anzahlZeilen, int *player) {
    int i = 0;
    while ((*matrix)[anzahlZeilen - i - 1] != 0) {
        ++i;
        if (i >= anzahlZeilen) {
            return 0; //Spalte ist voll
        }
    }
    (*matrix)[anzahlZeilen - i - 1] = *player;
    *player = (*player == 1) ? 2 : 1;
    return 1; //Erfolgreich
}

int interpreteUserInput(int ***matrix, int anzahlSpalten, int anzahlZeilen, int *player) {
    int temp;
    do {
        printf("Am Zug: Spieler %d. Waehle Aktion:\n", *player);
        printf("Verlassen [-1] | Lege einen Stein in Spalte [n]\n");
        scanf("%d", &temp);

        if (temp == -1) {//exit
            return 0;
        } else if (temp >= 1 && temp <= anzahlSpalten) {//Korrekte Spaltenangabe
            int result = addToRow(*matrix + (temp - 1), anzahlZeilen, player);
            if (result) { //Wenn result == 1, dann einfügen erfolgreich!
                return result; //Erfolgreich hinzugefügt
            }
            printf("Spalte ist voll... Versuchen sie es nochmal!\n");
            temp = 0;
        } else {//inkorrekte Spaltenangabe
            printf("Falscher Input... Versuchen sie es nochmal!\n");
        }

    } while (!(temp >= 1 && temp <= anzahlSpalten));
    return 1;
}

int detectWin(int ***matrix, int anzahlSpalten, int anzahlZeilen) {
    //Überprüft, ob ein Gewinner feststeht und gibt diesen ggf aus
    int counter;

    //Vertical
    for (int x = 0; x < anzahlSpalten; x++) {
        counter = 0;
        for (int y = anzahlZeilen - 1; y >= 0; y--) { //fangen von unten an
            if ((*matrix)[x][y] == 0) {
                break;
            }
            if (counter == 0) {
                counter++;
            } else {
                //Wenn dieselbe Zahl darunter ist, wird der counter erhöht, ansonsten counter = 0
                ((*matrix)[x][y] == (*matrix)[x][y + counter]) ? counter++ : (counter = 0);
            }

            if (counter == 4) {
                return (*matrix)[x][y]; //Gewinner erkannt
            }
        }

    }
    //Horizontal
    for (int y = 0; y < anzahlZeilen; y++) {
        counter = 0;
        for (int x = 0; x < anzahlSpalten; x++) {
            if ((*matrix)[x][y] == 0) {
                counter = 0;
                continue;
            }
            if (counter == 0) {
                counter++;
            } else {
                printf("[%d][%d]\n", x,y);
                printf("[%d][%d]\n", x - counter,y);
                //Wenn dieselbe Zahl davor ist, wird der counter erhöht, ansonsten counter = 0
                ((*matrix)[x][y] == (*matrix)[x-counter][y]) ? counter++ : (counter = 0);
            }

            if (counter == 4) {
                return (*matrix)[x][y]; //Gewinner erkannt
            }
        }
    }
    //Diagonal

    return 0; // Kein Gewinner erkannt
}


int main() {
    int anzahlSpalten = 0;
    int anzahlZeilen = 0;
    int **matrix;
    int player = 1; // Spieler eins beginnt

    printf("Anzahl der Spalten eingeben: ");
    scanf("%d", &anzahlSpalten);

    printf("Anzahl der Zeilen eingeben: ");
    scanf("%d", &anzahlZeilen);

    printf("Anzahl der Spalten: %d""\n", anzahlSpalten);
    printf("Anzahl der Zeilen:  %d\n", anzahlZeilen);

    if (initMatrix(&matrix, anzahlSpalten, anzahlZeilen) == EXIT_FAILURE) {
        printf("Matrixinitialisierung fehlgeschlagen\n");
        return EXIT_FAILURE;
    }

    printf("Matrix initialisiert\n");


    int running = 1;
    while (running) {
        print(matrix, anzahlSpalten, anzahlZeilen);
        running = interpreteUserInput(&matrix, anzahlSpalten, anzahlZeilen, &player);

        int winner = detectWin(&matrix, anzahlSpalten, anzahlZeilen);
        if (winner) { // if (winner != 0)
            print(matrix, anzahlSpalten, anzahlZeilen);
            printf("Spieler %d hat gewonnen!", winner);
            running = 0;
        }
    }
    return 0;
}