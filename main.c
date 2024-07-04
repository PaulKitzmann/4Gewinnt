#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

int* getEvals(double maxDauer, int ***matrix, int anzahlSpalten, int anzahlZeilen, int *p, int ergebnisAnzeigen);

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

int addToColumn(int **spalte, int anzahlZeilen, int *player) {
    int i = 0;
    while ((*spalte)[anzahlZeilen - i - 1] != 0) {
        ++i;
        if (i >= anzahlZeilen) {
            return 0; //Spalte ist voll
        }
    }
    (*spalte)[anzahlZeilen - i - 1] = *player;
    *player = (*player == 1) ? 2 : 1;
    return 1; //Erfolgreich
}

int interpreteUserInput(int ***matrix, int anzahlSpalten, int anzahlZeilen, int *player) {
    int temp;
    do {
        printf("Am Zug: Spieler %d. Waehle Aktion:\n", *player);
        printf("Verlassen [-1] | Lege einen Stein in Spalte [n] | Zuege evaluieren [0]\n");
        scanf("%d", &temp);

        if (temp == -1) {//exit
            return 0;
        } else if (temp >= 1 && temp <= anzahlSpalten) {//Korrekte Spaltenangabe
            int result = addToColumn(*matrix + (temp - 1), anzahlZeilen, player);
            if (result) { //Wenn result == 1, dann einfügen erfolgreich!
                return result; //Erfolgreich hinzugefügt
            }
            printf("Spalte ist voll... Versuchen sie es nochmal!\n");
            temp = 0;
        } else if (temp == 0) {
            double zeit = 5.0;
            while (1) {
                printf("Aktuelle Suchdauer pro Zug pro Tiefe: %f Sekunden\n", zeit);
                printf("Evaluation starten [0] | Suchdauer aendern: [Zeit in Sekunden] | Abbrechen [-1]\n");
                double input;
                scanf("%lf", &input);
                if (input == 0) {
                    getEvals(zeit / anzahlSpalten, matrix, anzahlSpalten, anzahlZeilen, player, 1);
                    return 1;
                }
                if (input == -1) {
                    break;
                }
                if (input > 0) {
                    zeit = input;
                }
                if (input < -1) {
                    printf("Falscher Input\n");
                }
            }
            return -1;
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

int freeMatrix(int ***matrix, int anzahlSpalten) {
    for (int x = 0; x < anzahlSpalten; x++) {
        free((*matrix)[x]);
    }
    free(*matrix);
    *matrix = NULL;
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

int evalMiniMax(int tiefe, int ***matrix, int anzahlSpalten, int anzahlZeilen, int *p) {
    int *player = malloc(sizeof(int *));
    int winner = abs(detectWin(matrix, anzahlSpalten, anzahlZeilen));
    if (winner != 0 || tiefe == 0) {
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
            addToColumn((*evalMatrix) + x, anzahlZeilen, player);
            evals[x] = evalMiniMax(tiefe - 1, evalMatrix, anzahlSpalten, anzahlZeilen, player);
        }
        //alpha-beta-pruning
        if (evals[x] == *p) {
            break;
        }
        freeMatrix(evalMatrix, anzahlSpalten);
    }
    free(player);
    return miniMax(evals, *p, anzahlSpalten);
}

int countFreieFelder(int ***matrix, int anzahlSpalten, int anzahlZeilen) {
    int counter = 0;
    for (int x = 0; x < anzahlSpalten; x++) {
        for (int y = 0; y < anzahlZeilen; y++) {
            if ((*matrix)[x][y] == 0) {
                counter++;
            }
        }
    }
    return counter;
}

int* getEvals(double maxDauer, int ***matrix, int anzahlSpalten, int anzahlZeilen, int *p, int ergebnisAnzeigen) {
    int *player = malloc(sizeof(int *));
    int **temp;
    int ***evalMatrix;
    int maxTiefe = countFreieFelder(matrix, anzahlSpalten, anzahlZeilen);
    //für Engine
    int *evals = (int *)malloc(anzahlSpalten * sizeof(int));
    initEvals(evals, anzahlSpalten);
    //Eval Moves pro Spalte
    for (int x = 0; x < anzahlSpalten; x++) {
        //clone
        cloneMatrix(matrix, &temp, anzahlSpalten, anzahlZeilen);
        evalMatrix = &temp;
        *player = *p;

        if (!(*evalMatrix)[x][0]) {//Prüfe ob Spalte voll
            addToColumn((*evalMatrix) + x, anzahlZeilen, player);
            long timeStart = clock();
            long timeStartTiefe = clock();
            int eval = 0;
            int tiefe;
            for (tiefe = 0;
                 (double) (clock() - timeStartTiefe) / CLOCKS_PER_SEC <= maxDauer && tiefe <= maxTiefe; tiefe++) {
                timeStartTiefe = clock();
                eval = evalMiniMax(tiefe, evalMatrix, anzahlSpalten, anzahlZeilen, player);
                //Zwischenergebnisse
                //printf("Tiefe: %d, Zeitinterval = %f\n", tiefe, (double) (clock() - timeStartTiefe) / CLOCKS_PER_SEC);
            }
            if(ergebnisAnzeigen) {
                printf("Eval Zeile %d: %d, %f s - Tiefe %d:\n", x + 1, eval,
                       (double) (clock() - timeStart) / CLOCKS_PER_SEC, tiefe);
            }
            //Ergebnis speichern
            evals[x] = eval;
        }
    }
    return evals;
}

int engineMakeMove(int * evals, int ***matrix, int anzahlSpalten, int anzahlZeilen, int *player){
    int isLoosing = 1;
    for(int i = 0; i<anzahlSpalten; i++){
        if(evals[i]  == -1){
            continue;
        }
        if(evals[i] == 2){
            addToColumn((*matrix) + i, anzahlZeilen, player);
            return 0;
        }
        if(evals[i] == 0){
            isLoosing = 0;
        }
    }
    int move = 0;
    if(isLoosing){
        while(!addToColumn((*matrix) + move, anzahlZeilen, player)){
            move = rand() % anzahlSpalten;
        }
    }
    else{
        while(1){
            move = rand() % anzahlSpalten;
            if(evals[move]==0){
                addToColumn((*matrix) + move, anzahlZeilen, player);
                break;
            }
        }
    }
    printf("Engine hat in Spalte %d gezogen\n", move + 12);
    return 0;
}

int initGame(int *** matrix, int* anzahlSpalten, int* anzahlZeilen){
    while (*anzahlSpalten <= 0) {
        printf("Anzahl der Spalten eingeben: ");
        scanf("%d", anzahlSpalten);
    }

    while (*anzahlZeilen <= 0){
        printf("Anzahl der Zeilen eingeben: ");
        scanf("%d", anzahlZeilen);
    }

    printf("Anzahl der Spalten: %d""\n", *anzahlSpalten);
    printf("Anzahl der Zeilen:  %d\n", *anzahlZeilen);

    if (initMatrix(matrix, *anzahlSpalten, *anzahlZeilen) == EXIT_FAILURE) {
        printf("Matrixinitialisierung fehlgeschlagen\n");
        return EXIT_FAILURE;
    }

    printf("Matrix initialisiert\n");
}

int playervsplayer() {
    int anzahlSpalten = 0;
    int anzahlZeilen = 0;
    int **matrix;
    int player = 1; // Spieler eins beginnt
    initGame(&matrix, &anzahlSpalten, &anzahlZeilen);

    int running = 1;
    while (running) {
        print(matrix, anzahlSpalten, anzahlZeilen);
        running = interpreteUserInput(&matrix, anzahlSpalten, anzahlZeilen, &player);
        int winner = detectWin(&matrix, anzahlSpalten, anzahlZeilen);
        if (winner) { // if (winner != 0)
            print(matrix, anzahlSpalten, anzahlZeilen);
            printf("Spieler %d hat gewonnen!", winner);
            break;
        }
        if (countFreieFelder(&matrix, anzahlSpalten, anzahlZeilen) == 0){
            printf("Unentschieden!\n");
            break;
        }
    }
    return 0;
}

int playervsengine() {
    int anzahlSpalten = 0;
    int anzahlZeilen = 0;
    int **matrix;
    int player = 0;
    while (player == 0){
        printf("Du beginnst [1] | Die Engine beginnt [2]\n");
        scanf("%d", &player);
        (player == 1 || player == 2)? : (player = 0);
    }
    double bedenkzeit = 5;
    while(1){
        printf("Aktuelle Bedenkzeit fuer die Engine: %lf s\n", bedenkzeit);
        printf("Spiel starten [0] | Bedenkzeit aendern: [Zeit in Sekunden]\n");
        double input;
        scanf("%lf", &input);
        if(input==0){
            break;
        }
        if(input>0){
            bedenkzeit = input;
            continue;
        }
        printf("Falscher Input\n");

    }
    initGame(&matrix, &anzahlSpalten, &anzahlZeilen);

    int running = 1;
    while (running) {
        //Dein Zug
        print(matrix, anzahlSpalten, anzahlZeilen);
        do {
            running = interpreteUserInput(&matrix, anzahlSpalten, anzahlZeilen, &player);
        }while(running==-1);

        int winner = detectWin(&matrix, anzahlSpalten, anzahlZeilen);
        if (winner == 1) {
            print(matrix, anzahlSpalten, anzahlZeilen);
            printf("Du hast gewonnen!");
            break;
        }
        if (countFreieFelder(&matrix, anzahlSpalten, anzahlZeilen) == 0){
            printf("Unentschieden!\n");
            break;
        }

        //Zug der Engine
        int* evals = getEvals(bedenkzeit / anzahlSpalten / anzahlSpalten, &matrix, anzahlSpalten, anzahlZeilen, &player, 0);
        engineMakeMove(evals, &matrix, anzahlSpalten, anzahlZeilen, &player);
        if (winner == 2) {
            print(matrix, anzahlSpalten, anzahlZeilen);
            printf("Du hast verloren!");
            break;
        }
        if (countFreieFelder(&matrix, anzahlSpalten, anzahlZeilen) == 0){
            printf("Unentschieden!\n");
            break;
        }
    }
    return 0;
}

void printIntro() {
    puts("  _  _      _____               _             _   ");
    puts(" | || |    / ____|             (_)           | |  ");
    puts(" | || |_  | |  __  _____      ___ _ __  _ __ | |_ ");
    puts(" |__   _| | | |_ |/ _ \\ \\ /\\ / / | '_ \\| '_ \\| __|");
    puts("    | |   | |__| |  __/\\ V  V /| | | | | | | | |_ ");
    puts("    |_|    \\_____|\\___| \\_/\\_/ |_|_| |_|_| |_|\\__|");
    puts("                                                 ");
    puts("                                                 ");
    puts("      Ein einfaches 4-Gewinnt-Spiel in der Konsole");
    puts("                                                 ");
}

int main() {
    printIntro();
    while (1) {
        int input;
        printf("Anwendung beenden [0] | PvP-Spiel starten [1] | PvE-Spiel starten [2]\n");
        scanf("%d", &input);
        if (input == 0) {
            break;
        } else if (input == 1) {
            playervsplayer();
        } else if (input == 2) {
            playervsengine();
        } else {
            printf("Falscher Input \n");
        }
    }
    return 0;
}