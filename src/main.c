#include "stdio.h"
#include "raylib.h"
#include "assert.h"

#define NUM_CELLS 9

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

typedef enum {
    Cell_Empty = 0,
    Cell_O,
    Cell_X
} Cell;

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
    Centered_Text menu_start_button_text;
    Rectangle menu_start_button;
    Color menu_start_button_color = WHITE;
    Scene scene = Scene_Menu;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);

    
    
    menu_start_button.width = 200;
    menu_start_button.height = 100;
    menu_start_button.x = (screen_width - menu_start_button.width) / 2;
    menu_start_button.y = screen_height / 2;
    centered_text_init(&menu_title, "Tic Tac Toe", 40, screen_height / 4, WHITE, screen_width);
    centered_text_init(&menu_start_button_text, "Play", 40, menu_start_button.y + (menu_start_button.height / 4), BLACK, menu_start_button.width);
    menu_start_button_text._x += menu_start_button.x;


    Cell grid[NUM_CELLS] = {0};
    Cell player_turn;
    const int square_size = screen_width / 3;

    RenderTexture2D texture_o = LoadRenderTexture(square_size, square_size);
    
    BeginTextureMode(texture_o);
    DrawCircle(square_size / 2, square_size / 2, square_size / 2.4, WHITE);
    DrawCircle(square_size / 2, square_size / 2, square_size / 2.8, BLACK);
    EndTextureMode();

    RenderTexture2D texture_x = LoadRenderTexture(square_size, square_size);
    
    BeginTextureMode(texture_x);
    
    Vector2 start = {.x = 0, .y = 0};
    Vector2 end = {.x = square_size, .y = square_size};
    DrawLineEx(start, end, 12, WHITE);

    start.x = square_size;
    end.x = 0;
    DrawLineEx(start, end, 12, WHITE);

    EndTextureMode();


    while (!WindowShouldClose())
    {
        switch(scene) {
            case Scene_Menu: {
                Vector2 mouse_pos = GetMousePosition();

                if(CheckCollisionPointRec(mouse_pos, menu_start_button)) {
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        scene = Scene_Game;

                        for (int i = 0; i < NUM_CELLS; i++) {
                            grid[i] = Cell_Empty;
                        }

                        grid[0] = Cell_X;
                        grid[3] = Cell_O;
                        player_turn = Cell_X;
                    } else {
                        menu_start_button_color = GREEN;
                    }
                } else {
                    menu_start_button_color = WHITE;
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
                DrawRectangleRec(menu_start_button, menu_start_button_color);
                centered_text_render(&menu_title);
                centered_text_render(&menu_start_button_text);


                break;
            }

            case Scene_Game: {

                

                DrawLine(screen_width / 3, 0, screen_width / 3, screen_height, WHITE);
                DrawLine(2 * screen_width / 3, 0, 2 * screen_width / 3, screen_height, WHITE);

                DrawLine(0, screen_height / 3, screen_width, screen_height / 3, WHITE);
                DrawLine(0, 2 * screen_height / 3, screen_width, 2 * screen_height / 3, WHITE);


                int x = 0, y = 0;

                for(int i = 0; i < NUM_CELLS; i++, x++) {
                    if(x >= 3) {
                        y++;
                        x = 0;
                    }

                    switch(grid[i]) {
                        case Cell_O: {
                            printf("O: %d, %d\n", x, y);
                            DrawTexture(texture_o.texture, x * square_size, y * square_size, WHITE);
                            break;
                        }
                        case Cell_X: {
                            printf("X: %d, %d\n", x, y);
                            DrawTexture(texture_x.texture, x * square_size, y * square_size, WHITE);
                            DrawTexture(texture_x.texture, x * square_size, y * square_size, WHITE);

                            break;
                        }
                    }
                }
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