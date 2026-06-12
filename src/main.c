#include "stdio.h"
#include "math.h"
#include "raylib.h"
#include "assert.h"

#define NUM_CELLS 9
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define switch_player(c) (c == Cell_O ? Cell_X : Cell_O)


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

typedef struct {
    bool active;
    Cell winner;
    Vector2 line_start;
    Vector2 line_end;
} Game_Over;

static bool game_over(Cell grid[NUM_CELLS], Cell player) {
    const int w_cases[8][3] = {
        {0, 4, 8}, {2, 4, 6}, 
        {0, 1, 2}, {3, 4, 5}, 
        {6, 7, 8}, {0, 3, 6},
        {1, 4, 7}, {2, 5, 8}
    };

    for(int i = 0; i < 8; i++) {
        if (grid[w_cases[i][0]] == player && grid[w_cases[i][1]] == player && grid[w_cases[i][2]] == player) {
            return true;
        }
    }

    return false;
}

static bool grid_is_full(Cell grid[NUM_CELLS]) {
    for(int i = 0; i < NUM_CELLS; i++) {
        if(grid[i] == Cell_Empty) {
            return false;
        }
    }

    return true;
}

static int grid_empty_indices(Cell grid[NUM_CELLS], int indices[NUM_CELLS]) {
    int num_indices = 0;
    for(int i = 0; i < NUM_CELLS; i++) {
        if(grid[i] == Cell_Empty) {
            indices[num_indices] = i;
            num_indices++;
        }
    }

    return num_indices;
}

static int grid_minimax(Cell grid[NUM_CELLS], Cell player) {
    Cell opponent = switch_player(player);
 
    if(game_over(grid, opponent)) {
        return opponent == Cell_O ? 1 : -1;
    }

    int score; 
    int indices[NUM_CELLS];

    int num_indices = grid_empty_indices(grid, indices);

    if(num_indices == 0) {
        return 0;
    }

    if(player == Cell_O) {
        score = -100000;

        for(int i = 0; i < num_indices; i++) {
            grid[indices[i]] = player;
            score = MAX(score, grid_minimax(grid, switch_player(player)));
            grid[indices[i]] = Cell_Empty;
        }
    } else {
        score = 100000;
        for(int i = 0; i < num_indices; i++) {
            grid[indices[i]] = player;
            score = MIN(score, grid_minimax(grid, switch_player(player)));
            grid[indices[i]] = Cell_Empty;
        }
    }

    return score;
}

static void grid_update(Cell grid[NUM_CELLS], Cell player) {
    int indices[NUM_CELLS];
    const int num_indices = grid_empty_indices(grid, indices);

    int best_score = -100000;
    int best_index = 0;

    for(int i = 0; i < num_indices; i++) {
        grid[indices[i]] = player;

        if(game_over(grid, player)) {
            return;
        }

        int score = grid_minimax(grid, switch_player(player));

        if(score > best_score) {
            best_score = score;
            best_index = i;
        }

        grid[indices[i]] = Cell_Empty;
    }

    grid[indices[best_index]] = player;

}
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

static void grid_game_over_lines(Cell grid[NUM_CELLS], Cell player, int screen_size, Vector2 *line_start, Vector2 *line_end) {
    const int square_size = screen_size / 3;

    if(grid[0] == player && grid[4] == player && grid[8] == player) {
        line_start -> x = 0;
        line_start -> y = 0;
        line_end -> x = screen_size;
        line_end -> y = screen_size;

        return;
    }

    if(grid[2] == player && grid[4] == player && grid[6] == player) {
        line_start -> x = screen_size;
        line_start -> y = 0;
        line_end -> x = 0;
        line_end -> y = screen_size;

        return;
    }

     for (int row = 0; row < 3; row++)
    {
        int base = row * 3;
        if (grid[base] == player && grid[base + 1] == player &&
            grid[base + 2] == player)
        {
            line_start->x = 0;
            line_start->y = row * square_size + square_size / 2;
            line_end->x = screen_size;
            line_end->y = row * square_size + square_size / 2;
            return;
        }
    }

    for (int col = 0; col < 3; col++)
    {
        if (grid[col] == player && grid[col + 3] == player &&
            grid[col + 6] == player)
        {
            line_start->x = col * square_size + square_size / 2;
            line_start->y = 0;
            line_end->x = col * square_size + square_size / 2;
            line_end->y = screen_size;
            return;
        }
    }
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

    Game_Over game_over_state = {0};

    Centered_Text game_over_text;
    Rectangle game_over_button;
    Centered_Text game_over_button_text;
    Color game_over_button_color = WHITE;

    InitWindow(screen_width, screen_height, "Raylib Template");
    SetTargetFPS(60);

    
    
    menu_start_button.width = 200;
    menu_start_button.height = 100;
    menu_start_button.x = (screen_width - menu_start_button.width) / 2;
    menu_start_button.y = screen_height / 2;
    centered_text_init(&menu_title, "Tic Tac Toe", 40, screen_height / 4, WHITE, screen_width);
    centered_text_init(&menu_start_button_text, "Play", 40, menu_start_button.y + (menu_start_button.height / 4), BLACK, menu_start_button.width);
    menu_start_button_text._x += menu_start_button.x;

    game_over_button.width = 200;
    game_over_button.height = 100;
    game_over_button.x = (screen_width - game_over_button.width) / 2;
    game_over_button.y = screen_height - 180;
    centered_text_init(
        &game_over_button_text,
        "Menu",
        40,
        game_over_button.y + (game_over_button.height / 4),
        BLACK,
        game_over_button.width
    );
    game_over_button_text._x += game_over_button.x;

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


    while (!WindowShouldClose()) {
        Vector2 mouse_pos = GetMousePosition();
        const int m_x = (int) floorf(mouse_pos.x / (float) square_size);
        const int m_y = (int) floorf(mouse_pos.y / (float) square_size);
        const int m_i  = m_y * 3 + m_x;
        switch(scene) {
            case Scene_Menu: {
                Vector2 mouse_pos = GetMousePosition();
                if(CheckCollisionPointRec(mouse_pos, menu_start_button)) {
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        game_over_state.active = false;
                        game_over_state.winner = Cell_Empty;
                        scene = Scene_Game;
                        
                        for (int i = 0; i < NUM_CELLS; i++) {
                            grid[i] = Cell_Empty;
                        }

                        // grid[0] = Cell_X;
                        // grid[3] = Cell_O;
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

    if(!game_over_state.active &&
       IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        if(m_x >= 0 && m_x < 3 &&
           m_y >= 0 && m_y < 3 &&
           grid[m_i] == Cell_Empty) {

            grid[m_i] = player_turn;

            if(game_over(grid, player_turn)) {

                game_over_state.active = true;
                game_over_state.winner = player_turn;

                grid_game_over_lines(
                    grid,
                    player_turn,
                    screen_width,
                    &game_over_state.line_start,
                    &game_over_state.line_end
                );

            } else if(grid_is_full(grid)) {

                game_over_state.active = true;
                game_over_state.winner = Cell_Empty;

            } else {

                grid_update(grid, Cell_O);

                if(game_over(grid, Cell_O)) {

                    game_over_state.active = true;
                    game_over_state.winner = Cell_O;

                    grid_game_over_lines(
                        grid,
                        Cell_O,
                        screen_width,
                        &game_over_state.line_start,
                        &game_over_state.line_end
                    );

                } else if(grid_is_full(grid)) {

                    game_over_state.active = true;
                    game_over_state.winner = Cell_Empty;
                }
            }
        }
    }

   if(game_over_state.active) {

    if(CheckCollisionPointRec(mouse_pos, game_over_button)) {

        game_over_button_color = GREEN;

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            scene = Scene_Menu;
        }

    } else {
        game_over_button_color = WHITE;
    }
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

               if(!game_over_state.active &&
   m_x >= 0 && m_x < 3 &&
   m_y >= 0 && m_y < 3 &&
   grid[m_i] == Cell_Empty) {

    DrawRectangle(
        m_x * square_size,
        m_y * square_size,
        square_size,
        square_size,
        DARKBLUE
    );
}
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

                        case Cell_Empty:
                        default: 
                            break;
                    }
                }

                if(game_over_state.active) {

    if(game_over_state.winner != Cell_Empty) {
        DrawLineEx(
            game_over_state.line_start,
            game_over_state.line_end,
            10,
            RED
        );
    }

    if(game_over_state.winner == Cell_X) {
        centered_text_init(
            &game_over_text,
            "You Win!",
            60,
            20,
            GREEN,
            screen_width
        );
    } else if(game_over_state.winner == Cell_O) {
        centered_text_init(
            &game_over_text,
            "Computer Wins!",
            60,
            20,
            RED,
            screen_width
        );
    } else {
        centered_text_init(
            &game_over_text,
            "Draw!",
            60,
            20,
            YELLOW,
            screen_width
        );
    }

    centered_text_render(&game_over_text);

    DrawRectangleRec(game_over_button, game_over_button_color);
    centered_text_render(&game_over_button_text);
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