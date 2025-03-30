#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WORDS 50
#define MAX_RECENT 5
#define RECENT_FILE "recente.txt"

typedef enum Scene { MENU, GAME, OPTIONS, EXIT } Scene;

Scene currentScene = MENU;

Rectangle playButton, optionsButton, exitButton;

void InitButtons(int screenWidth, int screenHeight) {
    int buttonWidth = 200;
    int buttonHeight = 60;
    int centerX = screenWidth / 2 - buttonWidth / 2;
    int startY = screenHeight / 2 - 100;

    playButton = (Rectangle){ centerX, startY, buttonWidth, buttonHeight };
    optionsButton = (Rectangle){ centerX, startY + 80, buttonWidth, buttonHeight };
    exitButton = (Rectangle){ centerX, startY + 160, buttonWidth, buttonHeight };
}

void DrawMenu(int screenWidth, int screenHeight) {
    ClearBackground((Color){30, 30, 30, 255});

    DrawText("HANGMAN", screenWidth / 2 - MeasureText("HANGMAN", 50) / 2, screenHeight / 4, 50, GREEN);

    DrawRectangleRec(playButton, LIGHTGRAY);
    DrawText("Play", playButton.x + 70, playButton.y + 15, 30, BLACK);

    DrawRectangleRec(optionsButton, LIGHTGRAY);
    DrawText("Options", optionsButton.x + 45, optionsButton.y + 15, 30, BLACK);

    DrawRectangleRec(exitButton, LIGHTGRAY);
    DrawText("Exit", exitButton.x + 70, exitButton.y + 15, 30, BLACK);
}

void incarcaRecente(char recente[][20], int* count) {
    FILE* f = fopen(RECENT_FILE, "r");
    *count = 0;
    if (f) {
        while (*count < MAX_RECENT && fscanf(f, "%s", recente[*count]) == 1) {
            (*count)++;
        }
        fclose(f);
    }
}

int esteRecent(char* cuvant, char recente[][20], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(cuvant, recente[i]) == 0)
            return 1;
    }
    return 0;
}

void salveazaRecent(char* cuvant, char recente[][20], int count) {
    FILE* f = fopen(RECENT_FILE, "w");
    if (f) {
        fprintf(f, "%s\n", cuvant);
        for (int i = 0; i < count && i < MAX_RECENT - 1; i++) {
            fprintf(f, "%s\n", recente[i]);
        }
        fclose(f);
    }
}

void GenerareCuvant(char str[][20], char cuvant[20], char cuvantafisat[20]) {
    char recente[MAX_RECENT][20];
    int count;
    incarcaRecente(recente, &count);

    int ok = 0;
    while (!ok) {
        int rdm = rand() % MAX_WORDS;
        if (!esteRecent(str[rdm], recente, count)) {
            strcpy(cuvant, str[rdm]);
            salveazaRecent(cuvant, recente, count);
            ok = 1;
        }
    }
    int n = strlen(cuvant);
    for(int i = 0; i < n; i++){
        if(cuvant[i] == cuvant[0])
            cuvantafisat[i] = cuvant[i];
        else if(cuvant[i] == cuvant[n - 1])
            cuvantafisat[i] = cuvant[i];
        else
            cuvantafisat[i] = '_';
    }
    cuvantafisat[n] = '\0';
}

void DeseneazaFundal(int screenWidth, int screenHeight) {
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 200, 70, 255});
    DrawRectangleLinesEx((Rectangle){screenWidth * 0.05f, screenHeight * 0.05f, screenWidth * 0.6f, screenHeight * 0.9f}, 4, DARKGRAY);
}

void DeseneazaSpanzuratoarea(int greseli, int cx, int cy, int scale) {
    DrawLine(cx, cy, cx, cy - scale * 3, BLACK);
    DrawLine(cx, cy - scale * 3, cx + scale * 2, cy - scale * 3, BLACK);
    DrawLine(cx + scale * 2, cy - scale * 3, cx + scale * 2, cy - scale * 2, BLACK);

    if (greseli > 0) DrawCircle(cx + scale * 2, cy - scale * 2 + 10, scale / 2, BLACK);
    if (greseli > 1) DrawLine(cx + scale * 2, cy - scale * 2 + scale, cx + scale * 2, cy - scale * 2 + scale * 2, BLACK);
    if (greseli > 2) DrawLine(cx + scale * 2, cy - scale * 2 + scale + 10, cx + scale, cy - scale * 2 + scale + 20, BLACK);
    if (greseli > 3) DrawLine(cx + scale * 2, cy - scale * 2 + scale + 10, cx + scale * 3, cy - scale * 2 + scale + 20, BLACK);
    if (greseli > 4) DrawLine(cx + scale * 2, cy - scale * 2 + scale * 2, cx + scale, cy - scale * 2 + scale * 3, BLACK);
    if (greseli > 5) DrawLine(cx + scale * 2, cy - scale * 2 + scale * 2, cx + scale * 3, cy - scale * 2 + scale * 3, BLACK);
}

void StartJocRaylib(char cuvant[], char cuvantafisat[], char str[][20]) {
    
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);

    int fontSize = screenHeight / 18;
    int lineSpacing = fontSize + 10;
    int x = screenWidth * 0.08f;

    do {
        int greseli = 0;
        int n = strlen(cuvant);
        int v[27] = {0};
        char mesaj[100] = "";
        bool jocTerminat = false;

        while (!WindowShouldClose()) {
            BeginDrawing();
            DeseneazaFundal(screenWidth, screenHeight);

            int y = screenHeight * 0.1f;

            DrawText("Joc Hangman", x, y, fontSize + 10, DARKBLUE);
            y += lineSpacing + 10;

            DrawText(TextFormat("Cuvant: %s", cuvantafisat), x, y, fontSize, BLACK);
            y += lineSpacing;

            DrawText(TextFormat("Greseli: %d / 6", greseli), x, y, fontSize, RED);
            y += lineSpacing;

            DrawText("Apasa litere de la tastatura", x, y, fontSize - 4, GRAY);
            y += lineSpacing;

            if (jocTerminat) {
                DrawText(mesaj, x, y, fontSize, DARKGREEN);
                y += lineSpacing;
                DrawText("Apasa R pentru a juca din nou sau ESC pentru a iesi", x, y, fontSize - 4, GRAY);
                EndDrawing();

                if (IsKeyPressed(KEY_R)) {
                    while (GetCharPressed() > 0) {}
                    GenerareCuvant(str, cuvant, cuvantafisat);
                    break; // reintră în bucla principală
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    CloseWindow();
                    return;
                }

                continue;
            }

            int scale = screenHeight / 20;
            int cx = screenWidth * 0.8f;
            int cy = screenHeight * 0.75f;
            DeseneazaSpanzuratoarea(greseli, cx, cy, scale);

            DrawText("Litere incercate:", x, y, fontSize - 2, DARKGRAY);
            y += lineSpacing;
            int lx = x;
            for (int i = 0; i < 26; i++) {
                if (v[i]) {
                    DrawText(TextFormat("%c ", 'A' + i), lx, y, fontSize - 2, MAROON);
                    lx += fontSize - 5;
                }
            }


            int key = GetCharPressed();
            if (key >= 'a' && key <= 'z') key -= 32;

            if (key >= 'A' && key <= 'Z') {
                if (v[key - 'A'] == 1) {
                    strcpy(mesaj, "Litera deja incercata.");
                } else {
                    v[key - 'A'] = 1;
                    int gasit = 0;
                    for (int i = 0; i < n; i++) {
                        if (cuvant[i] == key && cuvantafisat[i] == '_') {
                            cuvantafisat[i] = key;
                            gasit = 1;
                        }
                    }
                    if (!gasit) {
                        greseli++;
                        strcpy(mesaj, "Litera gresita!");
                    } else {
                        strcpy(mesaj, "Corect!");
                    }

                    if (strcmp(cuvant, cuvantafisat) == 0) {
                        sprintf(mesaj, "Ai castigat! Cuvantul era: %s", cuvant);
                        jocTerminat = true;
                    } else if (greseli == 6) {
                        sprintf(mesaj, "Ai pierdut! Cuvantul era: %s", cuvant);
                        jocTerminat = true;
                    }
                }
            }

            EndDrawing();
        }
    } while (!WindowShouldClose());

    CloseWindow();
}


void RunGame(char str[][20]) {
    char cuvant[20], cuvantafisat[20];
    GenerareCuvant(str, cuvant, cuvantafisat);
    StartJocRaylib(cuvant, cuvantafisat, str);
}


int main() {
    srand(time(NULL));
    
    char str[50][20] = {
        "CASUTA", "MASINA", "FRUNZA", "CIRESE", "BRADUT", "LAPTOP", "SCOALA", "PENITA", "CLADIRE", "SIRENE",
        "FEREASTRA", "PAPUCII", "SALATA", "PISCINA", "FANTOMA", "LIMONADA", "ZMEURICA", "JURNALUL", "FLOARE",
        "PLANTE", "VIOARA", "CASTEL", "POIANA", "MUNTELE", "BUCURIE", "PAINEA", "SOARELE", "CALCUL", "TRAMVAI",
        "BALERIN", "TRICOU", "HARTIE", "LUMINA", "CORNET", "GHIOZDA", "CREION", "FOTOLIU", "MELCUL", "ARICIUL",
        "LUPTARE", "PAHARUL", "TAVANUL", "DULAPUL", "BANANA", "FRIGARUI", "TELEVIZOR", "FERMARE", "RACHETA",
        "PIATRA", "ZIDARIA"
    };
    
    InitWindow(800, 600, "Hangman Menu");
    SetExitKey(0); 
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();
    SetTargetFPS(60);

    InitButtons(screenWidth, screenHeight);


    while (!WindowShouldClose()) {
        BeginDrawing();

        switch (currentScene) {
            case MENU:
                DrawMenu(screenWidth, screenHeight);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mouse = GetMousePosition();
                    if (CheckCollisionPointRec(mouse, playButton)) {
                        RunGame(str);  
                    } else if (CheckCollisionPointRec(mouse, optionsButton)) {
                        currentScene = OPTIONS;
                    } else if (CheckCollisionPointRec(mouse, exitButton)) {
                        currentScene = EXIT;
                    }
                }
                break;

            case OPTIONS:
                ClearBackground(RAYWHITE);
                DrawText("OPTIONS SCREEN - Apasa ESC pentru a reveni", screenWidth / 8, screenHeight / 2, 20, DARKGRAY);
                if (IsKeyPressed(KEY_ESCAPE)) currentScene = MENU;
                break;

            case EXIT:
                CloseWindow();
                exit(0);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
