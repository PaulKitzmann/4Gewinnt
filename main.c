#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


void setTextColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

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
        for (int x = -1; x < anzahlSpalten; x++) {
            if (x == -1) {
                printf("|");
            } else {
                switch (matrix[x][y]) {
                    case 0:
                        printf(" |");
                        break;
                    case 1:
                        printf("1|");
                        break;
                    case 2:
                        printf("2|");
                        break;
                        //Einfärben von "Gewinner-Steinen"
                    case -1:
                        setTextColor(FOREGROUND_RED);
                        printf("1");
                        setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        printf("|");
                        break;
                    case -2:
                        setTextColor(FOREGROUND_RED);
                        printf("2");
                        setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        printf("|");
                        break;
                    default:
                        break;
                }
            }
        }
        printf("\n");
    }
    printf("\n");
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
                if ((*matrix)[x][y] == (*matrix)[x][y + counter]) {
                    counter++;
                } else if ((*matrix)[x][y + counter] != 0) {
                    counter = 1;
                } else {
                    counter = 0;
                }
            }

            if (counter == 4) {
                int winner = (*matrix)[x][y];
                //Einfärben der "Gewinner-Steine"
                for (int i = 0; i < 4; i++) {
                    (*matrix)[x][y + i] = -winner;
                }
                return winner; //Gewinner erkannt
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
                if ((*matrix)[x][y] == (*matrix)[x - counter][y]) {
                    counter++;
                } else if ((*matrix)[x - counter][y] != 0) {
                    counter = 1;
                } else {
                    counter = 0;
                }
            }

            if (counter == 4) {
                int winner = (*matrix)[x][y];
                //Einfärben der "Gewinner-Steine"
                for (int i = 0; i < 4; i++) {
                    (*matrix)[x - i][y] = -winner;
                }
                return winner; //Gewinner erkannt
            }
        }
    }

    //Diagonalen
    int x;
    int y;
    //Diagonale unten links nach oben rechts
    x = 0;
    y = 0;
    while (!(x == anzahlSpalten - 1 && y == anzahlZeilen - 1)) { // solange wir nicht im rechten unteren Eck sind
        counter = 0;
        for (int x1 = 0, y1 = 0; x - x1 >= 0 && y + y1 < anzahlZeilen; x1++, y1++) { // diagonal zurück bis auf kante

            if ((*matrix)[x - x1][y + y1] == 0) {
                counter = 0;
                continue;
            }
            if (counter == 0) {
                counter++;
            } else {
                if ((*matrix)[x - x1][y + y1] == (*matrix)[x - x1 + counter][y + y1 - counter]) {
                    counter++;
                } else if ((*matrix)[x - x1 + counter][y + y1 - counter] != 0) {
                    counter = 1;
                } else {
                    counter = 0;
                }
            }

            if (counter == 4) {
                int winner = (*matrix)[x - x1][y + y1];
                //Einfärben der "Gewinner-Steine"
                for (int i = 0; i < 4; i++) {
                    (*matrix)[x - x1 + i][y + y1 - i] = -winner;
                }
                return winner; //Gewinner erkannt
            }
        }
        (x < anzahlSpalten - 1) ? x++ : y++;
    }

    //Diagonale unten rechts nach oben links
    x = anzahlSpalten - 1;
    y = 0;
    while (!(x == 0 && y == anzahlZeilen - 1)) { // solange wir nicht im rechten unteren Eck sind
        counter = 0;
        for (int x1 = 0, y1 = 0;
             x + x1 < anzahlSpalten && y + y1 < anzahlZeilen; x1++, y1++) { // diagonal zurück bis auf kante

            if ((*matrix)[x + x1][y + y1] == 0) {
                counter = 0;
                continue;
            }
            if (counter == 0) {
                counter++;
            } else {
                if ((*matrix)[x + x1][y + y1] == (*matrix)[x + x1 - counter][y + y1 - counter]) {
                    counter++;
                } else if ((*matrix)[x + x1 - counter][y + y1 - counter] != 0) {
                    counter = 1;
                } else {
                    counter = 0;
                }
            }

            if (counter == 4) {
                int winner = (*matrix)[x + x1][y + y1];
                for (int i = 0; i < 4; i++) {
                    (*matrix)[x + x1 - i][y + y1 - i] = -winner;
                }
                return winner; //Gewinner erkannt
            }
        }
        (x > 0) ? x-- : y++;
    }

    return 0; // Kein Gewinner erkannt
}


int cloneMatrix(int ***origin, int ***destiny, int anzahlSpalten, int anzahlZeilen) {
    if (initMatrix(destiny, anzahlSpalten, anzahlZeilen) == EXIT_FAILURE) {
        printf("Matrixinitialisierung fehlgeschlagen\n");
        return EXIT_FAILURE;
    }
    for (int x = 0; x < anzahlSpalten; x++) {
        for (int y = 0; y < anzahlZeilen; y++) {
            (*destiny)[x][y] = (*origin)[x][y];
        }
    }
    return EXIT_SUCCESS;
}

int miniMax(int evals[], int player, int anzahlSpalten) {
    //start with the worst choice
    int bestChoice = (player == 1) ? 2 : 1;
    int noMoves = 1;
    for (int i = 0; i < anzahlSpalten; i++) {
        if (evals[i] != -1) {
            noMoves = 0;
            if (evals[i] == player) {//player has chance to win
                return player;//optimal play
            }
            if (evals[i] == 0) {
                bestChoice = 0;
            }
        }
    }
    return noMoves ? 0 : bestChoice;
}

void initEvals(int *array, int anzahlspalten) {
    for (int i = 0; i < anzahlspalten; i++) {
        array[i] = -1;
    }
}

int evalMoves(int ***matrix, int anzahlSpalten, int anzahlZeilen, int *p) {
    int *player = malloc(sizeof(int *));
    int winner = abs(detectWin(matrix, anzahlSpalten, anzahlZeilen));
    if (winner) {
        return winner;
    }

    //clone Matrix
    int **temp;
    int ***evalMatrix;

    //Eval Moves pro Spalte
    int evals[anzahlSpalten];
    initEvals(evals, anzahlSpalten);
    for (int x = 0; x < anzahlSpalten; x++) {
        //clone
        cloneMatrix(matrix, &temp, anzahlSpalten, anzahlZeilen);
        evalMatrix = &temp;
        *player = *p;
        if (!(*evalMatrix)[x][0]) {//Prüfe ob Spalte voll
            addToRow((*evalMatrix) + x, anzahlZeilen, player);
            evals[x] = evalMoves(evalMatrix, anzahlSpalten, anzahlZeilen, player);
        }
    }
    return miniMax(evals, *p, anzahlSpalten);
}

int getEvals(int ***matrix, int anzahlSpalten, int anzahlZeilen, int *p) {
    int *player = malloc(sizeof(int *));

    int **temp;
    int ***evalMatrix;

    //Eval Moves pro Spalte
    for (int x = 0; x < anzahlSpalten; x++) {
        //clone
        cloneMatrix(matrix, &temp, anzahlSpalten, anzahlZeilen);
        evalMatrix = &temp;
        *player = *p;

        if (!(*evalMatrix)[x][0]) {//Prüfe ob Spalte voll
            addToRow((*evalMatrix) + x, anzahlZeilen, player);
            int eval = evalMoves(evalMatrix, anzahlSpalten, anzahlZeilen, player);
            printf("Eval Zeile %d: %d\n", x + 1, eval);
        }
    }
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
        getEvals(&matrix, anzahlSpalten, anzahlZeilen, &player);
        int winner = detectWin(&matrix, anzahlSpalten, anzahlZeilen);
        if (winner) { // if (winner != 0)
            print(matrix, anzahlSpalten, anzahlZeilen);
            printf("Spieler %d hat gewonnen!", winner);
            break;
        }
    }
    return 0;
}
//aphabeta pruning
//evtl. reeinforcement learning 