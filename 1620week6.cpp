#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

DigitalIn button(p5);
PwmOut speaker(p21);

#include <cmath>
#include <stack>
#include <stdlib.h>
#include <time.h>

#define SCREEN_X_LEN 240
#define SCREEN_Y_LEN 240
#define SIZE_TANK_LONG 5
#define SIZE_TANK_WIDTH 3

#define SIZE_TANK_FRONT 4
#define SIZE_TANK_FRONT_WIDTH 2

#define INI_POS_X_PLAYER 120
#define INI_POS_Y_PLAYER 220

#define INI_POS_X_ENEMY 120
#define INI_POS_Y_ENEMY 20

#define BULLET_RADIUS 3

#define X_MOVE_SPEED 8
#define Y_MOVE_SPEED 4
#define BULLET_SPEED 20
#define LIFE 5

stack<int> player_bullet_x;
stack<int> player_bullet_y;
stack<int> temp_player_bullet_x;
stack<int> temp_player_bullet_y;

stack<int> enemy_position_x;
stack<int> enemy_position_y;
stack<int> temp_enemy_position_x;
stack<int> temp_enemy_position_y;

stack<int> enemy_bullet_y;
stack<int> temp_enemy_bullet_y;

TS_StateTypeDef TS_State = {0};

void play_tone(float frequency, float volume, int interval, int rest)
{
    speaker.period(1.0 / frequency);
    speaker = volume;
    wait(interval);
    speaker = 0.0;
    wait(rest);
}
int rand_certain_tank()
{
    int seed;
    seed = rand() % 10;
    return seed;
}
/*This part shoot the random number of bullet*/
int rand_num_bullet(int difficulty)
{
    int seed;
    seed = rand() % 5 - difficulty;
    return seed;
}
/*This returns a random number*/
int rand_num()
{
    int seed;
    srand((unsigned)time(NULL));
    seed = rand() % 240;
    return seed;
}
int setdirection(int x1, int y1, int x_player_position, int y_player_position)
{
    int x_move = 0;
    int y_move = 0;
    int x_diff = x1 - x_player_position;
    int y_diff = y1 - y_player_position;
    if (abs(x_diff) >= abs(y_diff))
    {
        if (x_diff > 0)
        {
            x_move = X_MOVE_SPEED;
        }
        if (x_diff < 0)
        {
            x_move = -X_MOVE_SPEED;
        }
        return x_move;
    }
    if (abs(x_diff) < abs(y_diff))
    {
        if (y_diff > 0)
        {
            y_move = Y_MOVE_SPEED;
        }
        if (y_diff < 0)
        {
            y_move = -Y_MOVE_SPEED;
        }
        return y_move;
    }
}
void draw_caption()
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}
void draw_initial()
{
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"A Tank shooting Game", CENTER_MODE);
    wait_ms(3000);
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"for MBED", CENTER_MODE);
    wait_ms(3000);
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"by GUO ZHANYU", CENTER_MODE);
    wait_ms(3000);
    draw_caption();
}
void ini_touch_screen()
{

    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR)
    {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_SetFont(&Font16);
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}
int **ini_map(int num_enemy)
{
    int **range = new int *[SCREEN_X_LEN];
    for (int i = 0; i < SCREEN_X_LEN; i++)
    {
        range[i] = new int[SCREEN_Y_LEN];
    }
    for (int x = 0; x < SCREEN_X_LEN; x++)
    {
        for (int y = 0; y < SCREEN_Y_LEN; y++)
        {
            if (x == 0 || x == 1 || x == 2 || x == 3 || y == 0 || y == 1 || y == 2 || y == 3 || x == 237 || x == 238 || x == 239 || x == 240 || y == 237 || y == 238 || y == 239 || y == 240)
            {
                range[x][y] = 1;
            }
            else
            {
                range[x][y] = 0;
            }
            int x_player_position = INI_POS_X_PLAYER;
            int y_player_position = INI_POS_Y_PLAYER;
            for (int i = -SIZE_TANK_LONG; i < SIZE_TANK_LONG; i++)
            {
                for (int j = -SIZE_TANK_WIDTH; j < SIZE_TANK_WIDTH; j++)
                {
                    range[x_player_position + i][y_player_position + j] = 2;
                }
            }
            for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
            {
                for (int y = 0; y < SIZE_TANK_FRONT; y++)
                {
                    range[x_player_position + x][y_player_position - SIZE_TANK_WIDTH - y] = 2;
                }
            }
            for (int i = num_enemy; i > 0; i--)
            {
                int x_enemy_position = SCREEN_X_LEN - i * SCREEN_X_LEN / 6;
                int y_enemy_position = INI_POS_Y_ENEMY;
                for (int i = -SIZE_TANK_LONG; i < SIZE_TANK_LONG; i++)
                {
                    for (int j = -SIZE_TANK_WIDTH; j < SIZE_TANK_WIDTH; j++)
                    {
                        range[x_enemy_position + i][y_enemy_position + j] = 3;
                    }
                }
                for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
                {
                    for (int y = 0; y < SIZE_TANK_FRONT; y++)
                    {
                        range[x_enemy_position + x][y_enemy_position + SIZE_TANK_WIDTH + y] = 3;
                    }
                }
                enemy_position_x.push(x_enemy_position);
                enemy_position_y.push(y_enemy_position);
            }
        }
    }
    return range;
}
int **clear_certain_tank(int **range, int x_position, int y_position)
{
    for (int i = -SIZE_TANK_LONG; i < SIZE_TANK_LONG; i++)
    {
        for (int j = -SIZE_TANK_WIDTH; j < SIZE_TANK_WIDTH; j++)
        {
            range[x_position + i][y_position + j] = 0;
        }
    }
    for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
    {
        for (int y = 0; y < SIZE_TANK_FRONT; y++)
        {
            range[x_position + x][y_position - SIZE_TANK_WIDTH - y] = 0;
        }
    }
    for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
    {
        for (int y = 0; y < SIZE_TANK_FRONT; y++)
        {
            range[x_position + x][y_position + SIZE_TANK_WIDTH + y] = 0;
        }
    }
    return range;
}
void creat_map(int **range)
{
    for (int x = 0; x < SCREEN_X_LEN; x++)
    {
        for (int y = 0; y < SCREEN_Y_LEN; y++)
        {
            switch (range[x][y])
            {
            case 0:
                BSP_LCD_DrawPixel(x, y, LCD_COLOR_LIGHTBLUE);
                break;
            case 1:
                BSP_LCD_DrawPixel(x, y, LCD_COLOR_BLACK);
                break;
            case 2:
                BSP_LCD_DrawPixel(x, y, LCD_COLOR_WHITE);
                break;
            case 3:
                BSP_LCD_DrawPixel(x, y, LCD_COLOR_DARKRED);
                break;
            }
        }
    }
}

void win_fail()
{
}

int main()
{
    //initialize the touch screen
    ini_touch_screen();
    int num_enemy = 1;
    int **range = ini_map(num_enemy);
    // have the initial objects position
    int bullet_x_position = 0;
    int bullet_y_position = 0;
    int x_player_position = INI_POS_X_PLAYER;
    int y_player_position = INI_POS_Y_PLAYER;
    int x_enemy_position = INI_POS_X_ENEMY;
    int y_enemy_position = INI_POS_Y_ENEMY;
    int direction_player = 0;
    int flag = 0;
    int score = 0;
    int lives = LIFE;

    draw_initial();
    // use initial range array to creat map
    creat_map(range);

    while (1)
    {
        int bullet_flag = 1;
        int bullet_exist[100];
        int num_bullet = 0;

        int p1 = button.read();
        if (p1 && bullet_flag)
        {
            play_tone(150.0, 0.5, 1, 0);
            BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
            BSP_LCD_FillCircle(x_player_position, y_player_position, BULLET_RADIUS);
            //use stack to govern the value of bullet attribution
            player_bullet_x.push(x_player_position);
            player_bullet_y.push(y_player_position);
            //this ensures one bullet once pressed button
            bullet_flag = 0;
            num_bullet = num_bullet + 1;
        }
        BSP_TS_GetState(&TS_State);
        if (TS_State.touchDetected)
        {
            //get the touch point
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            // This part calculate the distance
            direction_player = setdirection(x1, y1, x_player_position, y_player_position);
            //clear the previous map of player
            range = clear_certain_tank(range, x_player_position, y_player_position);
            // this part move the object
            if (abs(direction_player) > 6)
            {
                int x_move = direction_player;
                x_player_position = x_player_position + x_move;
            }
            else
            {
                int y_move = direction_player;
                y_player_position = y_player_position + y_move;
            }
            // this part upgrade the array
            for (int i = -SIZE_TANK_LONG; i < SIZE_TANK_LONG; i++)
            {
                for (int j = -SIZE_TANK_WIDTH; j < SIZE_TANK_WIDTH; j++)
                {
                    range[x_player_position + i][y_player_position + j] = 2;
                }
            }
            for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
            {
                for (int y = 0; y < SIZE_TANK_FRONT; y++)
                {
                    range[x_player_position + x][y_player_position - SIZE_TANK_WIDTH - y] = 2;
                }
            }
            bullet_flag = 1;
        }
        // This control the enemy tank
        while (!enemy_position_x.empty())
        {
            x_enemy_position = enemy_position_x.top();
            y_enemy_position = enemy_position_y.top();
            //This part clear the previous range of enemy tank
            range = clear_certain_tank(range, x_enemy_position, y_enemy_position);
            // This is part move the tank randomly
            // Move close to player tank
            if (rand_certain_tank() > 2)
            {
                if (x_enemy_position > x_player_position)
                {
                    x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                }
                else
                {
                    x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                }
                if (y_enemy_position > y_player_position)
                {
                    y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                }
                else
                {
                    y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                }
            }
            else
            {
                int rand_pos = rand_num();
                if (rand_pos > x_enemy_position && rand_pos > y_enemy_position)
                {
                    x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                    y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                }
                if (rand_pos > x_enemy_position && rand_pos < y_enemy_position)
                {
                    x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                    y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                }
                if (rand_pos < x_enemy_position && rand_pos > y_enemy_position)
                {
                    x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                    y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                }
                if (rand_pos < x_enemy_position && rand_pos - y_enemy_position)
                {
                    x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                    y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                }
            }

            // This part change the array of new enemy tank
            for (int i = -SIZE_TANK_LONG; i < SIZE_TANK_LONG; i++)
            {
                for (int j = -SIZE_TANK_WIDTH; j < SIZE_TANK_WIDTH; j++)
                {
                    range[x_enemy_position + i][y_enemy_position + j] = 3;
                }
            }
            for (int x = -SIZE_TANK_FRONT_WIDTH; x < SIZE_TANK_FRONT_WIDTH; x++)
            {
                for (int y = 0; y < SIZE_TANK_FRONT; y++)
                {
                    range[x_enemy_position + x][y_enemy_position + SIZE_TANK_WIDTH + y] = 3;
                }
            }

            temp_enemy_position_x.push(x_enemy_position);
            temp_enemy_position_y.push(y_enemy_position);
            enemy_position_x.pop();
            enemy_position_y.pop();
        }

        // this part draw the map again
        creat_map(range);
        // update the position of bullet
        //only y axis is fulfilled here
        while (!player_bullet_y.empty())
        {
            bullet_x_position = player_bullet_x.top();
            bullet_y_position = player_bullet_y.top();
            player_bullet_y.pop();
            bullet_y_position = bullet_y_position - BULLET_SPEED;
            // create new bullet
            BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
            BSP_LCD_FillCircle(bullet_x_position, bullet_y_position, BULLET_RADIUS);
            temp_player_bullet_y.push(bullet_y_position);
        }
        while (!temp_player_bullet_y.empty())
        {
            bullet_y_position = temp_player_bullet_y.top();
            if (bullet_y_position > 0 && bullet_y_position < SCREEN_Y_LEN)
            {
                player_bullet_y.push(bullet_y_position);
            }
            temp_player_bullet_y.pop();
        }
        while (!temp_enemy_position_y.empty())
        {
            x_enemy_position = temp_enemy_position_x.top();
            y_enemy_position = temp_enemy_position_y.top();

            enemy_position_y.push(y_enemy_position);
            enemy_position_x.push(x_enemy_position);

            temp_enemy_position_y.pop();
            temp_enemy_position_x.pop();
        }
        // wait for a while
        wait_ms(10);
    }
}