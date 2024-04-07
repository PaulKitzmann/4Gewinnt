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

int addToRow(int **matrix, int anzahlZeilen, int * player) {
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

int interpreteUserInput(int ***matrix, int anzahlSpalten, int anzahlZeilen, int * player) {
    int temp;
    do {
        printf("Am Zug: Spieler %d. Waehle Aktion:\n", *player);
        printf("Verlassen [-1] | Lege einen Stein in Spalte [n]\n");
        scanf("%d", &temp);

        if (temp == -1) {//exit
            return 0;
        }
        else if (temp >= 1 && temp <= anzahlSpalten) {//Korrekte Spaltenangabe
            int result = addToRow(*matrix + (temp - 1), anzahlZeilen, player);
            if (result) { //Wenn result == 1, dann einfügen erfolgreich!
                return result; //Erfolgreich hinzugefügt
            }
            printf("Spalte ist voll... Versuchen sie es nochmal!\n");
            temp = 0;
        }
        else {//inkorrekte Spaltenangabe
            printf("Falscher Input... Versuchen sie es nochmal!\n");
        }

    } while (!(temp >= 1 && temp <= anzahlSpalten));
    return 1;
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
    }
    return 0;
}