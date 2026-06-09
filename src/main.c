#include "raylib.h"
#include "assert.h"


typedef enum {
    Scene_Menu,
    Scene_Game
} Scene;

typedef struct {
    char *text;
    int font_size;
    int _x;
    int y;
    Color color;
} Centered_Text;

static void centered_text_init(Centered_Text *ct, char *text, int font_size, int y, Color color, int screen_width) {
    ct->text = text;
    ct->font_size = font_size;
    ct->y = y;
    ct->color = color;

    const int w = MeasureText(text, font_size);
    ct->_x = (screen_width - w) / 2;
}

static void centered_text_render(Centered_Text *ct) {
    DrawText(ct->text, ct->_x, ct->y, ct->font_size, ct->color);
}

int main(void)
{
    const int screen_width = 800;
    const int screen_height = 800;

    Centered_Text menu_title;
    Scene scene = Scene_Menu;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);
    centered_text_init(&menu_title, "Tic Tac Toe", 40, screen_width / 4, WHITE, screen_width);

    while (!WindowShouldClose())
    {
        switch(scene) {
            case Scene_Menu: {
                if(IsKeyPressed(KEY_ENTER)) {
                    scene = Scene_Game;
                }

                break;
            }

            case Scene_Game: {
                if(IsKeyPressed(KEY_ENTER)) {
                    scene = Scene_Menu;
                }

                break;
            }

            default: {
                assert(false);
            }
        }
        BeginDrawing();
        ClearBackground(BLACK);

        switch(scene) {
             case Scene_Menu: {
                centered_text_render(&menu_title);
                break;
            }

            case Scene_Game: {
                DrawText("Game Screen", 100, 100, 50, WHITE);


                break;
            }

            default: {
                assert(false);
            } 
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}