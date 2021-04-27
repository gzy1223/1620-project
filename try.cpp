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
#define SIZE_TANK_LONG 12
#define SIZE_TANK_WIDTH 6
#define WANT_SECOND 30

#define SIZE_TANK_FRONT 10
#define SIZE_TANK_FRONT_WIDTH 3

#define INI_POS_X_PLAYER 120
#define INI_POS_Y_PLAYER 220

#define INI_POS_X_ENEMY 120
#define INI_POS_Y_ENEMY 20

#define BULLET_RADIUS 3

#define X_MOVE_SPEED 8
#define Y_MOVE_SPEED 4
#define BULLET_SPEED 20
#define LIFE 5
#define DISTANCE 50

stack<int> player_bullet_x;
stack<int> player_bullet_y;
stack<int> temp_player_bullet_x;
stack<int> temp_player_bullet_y;

stack<int> enemy_position_x;
stack<int> enemy_position_y;
stack<int> temp_enemy_position_x;
stack<int> temp_enemy_position_y;

stack<int> enemy_bullet_x;
stack<int> enemy_bullet_y;
stack<int> temp_enemy_bullet_x;
stack<int> temp_enemy_bullet_y;

TS_StateTypeDef TS_State = {0};

void play_tone(float frequency, float volume, float interval, int rest)
{
    speaker.period(1.0 / frequency);
    speaker = volume;
    wait(interval);
    speaker = 0.0;
    wait(rest);
}
void draw_player_tank(int x, int y)
{
    BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
    Point Points1[] = {{x, y}, {x + 4, y}, {x + 4, y + 5}, {x + 6, y + 5}, {x + 6, y + 3}, {x + 10, y + 3}, {x + 10, y - 5}, {x + 12, y - 5}, {x + 12, y + 3}, {x + 16, y + 3}, {x + 16, y + 5}, {x + 18, y + 5}, {x + 18, y}, {x + 22, y}, {x + 22, y + 17}, {x + 18, y + 17}, {x + 18, y + 13}, {x + 16, y + 13}, {x + 16, y + 15}, {x + 6, y + 15}, {x + 6, y + 13}, {x + 4, y + 13}, {x + 4, y + 17}, {x, y + 17}};
    BSP_LCD_FillPolygon(Points1, 24);
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
    BSP_LCD_FillRect(x + 10, y + 5, 2, 8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(x, y + 1, 4, 2);
    BSP_LCD_FillRect(x, y + 5, 4, 2);
    BSP_LCD_FillRect(x, y + 9, 4, 2);
    BSP_LCD_FillRect(x, y + 13, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 1, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 5, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 9, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 13, 4, 2);
}
void draw_enemy_tank(int x, int y)
{
    BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
    Point Points2[] = {{x, y}, {x + 4, y}, {x + 4, y + 4}, {x + 6, y + 4}, {x + 6, y + 2}, {x + 16, y + 2}, {x + 16, y + 4}, {x + 18, y + 4}, {x + 18, y}, {x + 22, y}, {x + 22, y + 17}, {x + 18, y + 17}, {x + 18, y + 12}, {x + 16, y + 12}, {x + 16, y + 14}, {x + 12, y + 14}, {x + 12, y + 22}, {x + 10, y + 22}, {x + 10, y + 14}, {x + 6, y + 14}, {x + 6, y + 12}, {x + 4, y + 12}, {x + 4, y + 17}, {x, y + 17}};
    BSP_LCD_FillPolygon(Points2, 24);
    BSP_LCD_SetTextColor(LCD_COLOR_DARKRED);
    BSP_LCD_FillRect(x + 10, y + 4, 2, 8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(x, y + 2, 4, 2);
    BSP_LCD_FillRect(x, y + 6, 4, 2);
    BSP_LCD_FillRect(x, y + 10, 4, 2);
    BSP_LCD_FillRect(x, y + 14, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 2, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 6, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 10, 4, 2);
    BSP_LCD_FillRect(x + 18, y + 14, 4, 2);
}
void draw_tank_underattack(int x, int y)
{
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    Point Points1[] = {{x, y}, {x + 4, y - 2}, {x + 2, y - 6}, {x + 6, y - 4}, {x + 8, y - 8}, {x + 10, y - 4}, {x + 14, y - 6}, {x + 12, y - 2}, {x + 16, y}, {x + 12, y + 2}, {x + 14, y + 4}, {x + 10, y + 4}, {x + 8, y + 8}, {x + 6, y + 4}, {x + 2, y + 4}, {x + 4, y + 2}};
    BSP_LCD_FillPolygon(Points1, 16);
    BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
    Point Points2[] = {{x + 6, y}, {x + 4, y - 2}, {x + 6, y - 2}, {x + 8, y - 4}, {x + 10, y - 2}, {x + 12, y - 2}, {x + 10, y}, {x + 12, y + 2}, {x + 10, y + 2}, {x + 8, y + 4}, {x + 6, y + 2}, {x + 4, y + 2}};
    BSP_LCD_FillPolygon(Points2, 12);
}
class Setting
{
private:
    int *second;
    int want_live;
    int *minute;
    int *score;
    int step;
    int live;

public:
    int show_time(int step);
    void show_score(int score);
    void show_life(int live);
};

int Setting::show_time(int step)
{
    int want_second = WANT_SECOND;
    if (step == 0)
    {
        *second = want_second;
    }
    BSP_LCD_SetFont(&Font8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    //increment the step
    step = step + 1;
    if (step % 5 == 0)
    {
        if (*second != 1)
        {
            *second = *second - 1;
        }
        else
        {
            *second = 59;
        }
    }
    char _SECOND[14];
    sprintf(_SECOND, "%i", *second);
    BSP_LCD_DisplayStringAt(25, 10, (uint8_t *)"time", RIGHT_MODE);
    BSP_LCD_DisplayStringAt(10, 10, (uint8_t *)_SECOND, RIGHT_MODE);
    return step;
}
void Setting::show_score(int score)
{
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    char SCORE[14];
    sprintf(SCORE, "%.0d", score);
    BSP_LCD_DisplayStringAt(SCREEN_X_LEN / 2 + 10, 10, (uint8_t *)"SCORE:", LEFT_MODE);
    BSP_LCD_DisplayStringAt(SCREEN_X_LEN / 2 + 45, 10, (uint8_t *)SCORE, LEFT_MODE);
}
void Setting::show_life(int live)
{
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAt(15, 10, (uint8_t *)"LIFE:", LEFT_MODE);
    char _LIFE[14];
    sprintf(_LIFE, "%i", live);
    BSP_LCD_DisplayStringAt(45, 10, (uint8_t *)_LIFE, LEFT_MODE);
}
class Rand_number
{
private:
    int seed;

public:
    int rand_certain_tank();
    int rand_num_bullet(int difficulty);
    int rand_num();
};

int Rand_number::rand_certain_tank()
{
    int seed;
    seed = rand() % 10;
    return seed;
}
/*This part shoot the random number of score*/
int Rand_number::rand_num_bullet(int difficulty)
{
    int seed;
    seed = rand() % 5 - 3 + difficulty;
    return seed;
}
/*This returns a random number*/
int Rand_number::rand_num()
{
    int seed;
    srand((unsigned)time(NULL));
    seed = rand() % 240;
    return seed;
}
class Move
{
private:
    int x_diff;
    int y_diff;
    int x_move;
    int y_move;
    int distance[3]; // This part store the information of relevant distance
    int abs_distance;
    int relative_x_distance;
    int relative_y_distance;
    int after_move[2];
    int *real_distance;

public:
    int setdirection(int x1, int y1, int x_player_position, int y_player_position);
    int *cal_distance(int x_enemy_position, int y_enemy_position, int x_player_position, int y_player_position);
    int *avoid_collision(int x_enemy_position, int y_enemy_position, int x_player_position, int y_player_position);
};
/*This part move the player tank when touchscreen is touched*/
int Move::setdirection(int x1, int y1, int x_player_position, int y_player_position)
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
int *Move::cal_distance(int x_enemy_position, int y_enemy_position, int x_player_position, int y_player_position)
{
    distance[0] = sqrt(pow(x_enemy_position - x_player_position, 2) + pow(y_enemy_position - y_player_position, 2));
    distance[1] = x_enemy_position - x_player_position;
    distance[2] = y_enemy_position - y_player_position;
    return distance;
}
int *Move::avoid_collision(int x_enemy_position, int y_enemy_position, int x_player_position, int y_player_position)
{
    real_distance = cal_distance(x_enemy_position, y_enemy_position, x_player_position, y_player_position);
    abs_distance = real_distance[0];
    relative_x_distance = real_distance[1];
    relative_y_distance = real_distance[2];
    if (abs_distance < DISTANCE && relative_x_distance < 0)
    {
        x_enemy_position = x_player_position - DISTANCE;
    }
    if (abs_distance < DISTANCE && relative_x_distance > 0)
    {
        x_enemy_position = x_player_position + DISTANCE;
    }
    if (abs_distance < DISTANCE && relative_y_distance < 0)
    {
        y_enemy_position = y_player_position - DISTANCE;
    }
    if (abs_distance < DISTANCE && relative_y_distance > 0)
    {
        y_enemy_position = y_player_position + DISTANCE;
    }
    if (x_enemy_position <= DISTANCE)
    {
        x_enemy_position = x_enemy_position + DISTANCE;
    }
    if (x_enemy_position >= SCREEN_X_LEN - DISTANCE)
    {
        x_enemy_position = x_enemy_position - DISTANCE;
    }
    if (y_enemy_position <= DISTANCE)
    {
        y_enemy_position = y_enemy_position + DISTANCE;
    }
    if (y_enemy_position <= DISTANCE)
    {
        y_enemy_position = y_enemy_position - DISTANCE;
    }
    if (x_enemy_position < 10)
    {
        x_enemy_position = 10;
    }
    if (x_enemy_position > 230)
    {
        x_enemy_position = 240;
    }
    if (y_enemy_position < 10)
    {
        y_enemy_position = 10;
    }
    if (y_enemy_position > 230)
    {
        y_enemy_position = 230;
    }
    after_move[0] = x_enemy_position;
    after_move[1] = y_enemy_position;
    return after_move;
}
class Initial
{
private:
    int tank_position[15];
    int x_player_position;
    int y_player_position;
    int x_enemy_position;
    int y_enemy_position;
    int *received_position;
    int num_enemy;
    int x;
    int y;

public:
    void draw_caption();
    void draw_initial_caption();
    void ini_touch_screen();
    int *ini_tank(int num_enemy);
    void draw_ini_tank(int num_enemy);
    void ini_map();
    void play_again();
};

void Initial::draw_caption()
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}
void Initial::draw_initial_caption()
{
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"A Tank shooting Game", CENTER_MODE);
    wait_ms(1000);
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"for MBED SIMULATOR", CENTER_MODE);
    wait_ms(1000);
    draw_caption();
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"by GUO ZHANYU", CENTER_MODE);
    wait_ms(1000);
    draw_caption();
}
void Initial::ini_touch_screen()
{

    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR)
    {
        printf("BSP_TS_Init error");
    }
    BSP_LCD_SetFont(&Font16);
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}
int *Initial::ini_tank(int num_enemy)
{
    x_player_position = INI_POS_X_PLAYER;
    y_player_position = INI_POS_Y_PLAYER;
    x_enemy_position = 0;
    y_enemy_position = 0;
    tank_position[0] = x_player_position;
    tank_position[1] = y_player_position;
    for (int i = 0; i < num_enemy; i++)
    {
        x_enemy_position = SCREEN_X_LEN - i * SCREEN_X_LEN / 6;
        y_enemy_position = INI_POS_Y_ENEMY;
        tank_position[i + 2] = x_enemy_position;
        tank_position[i + 3] = y_player_position;
    }
    return tank_position;
}
void Initial::draw_ini_tank(int num_enemy)
{
    received_position = ini_tank(num_enemy);
    x = received_position[0];
    y = received_position[1];
    draw_player_tank(x, y);
    for (int i = 2; i <= num_enemy + 2; i = i + 2)
    {
        x = received_position[i];
        y = received_position[i + 1];
        draw_enemy_tank(x, y);
    }
}
void Initial::ini_map()
{
    BSP_LCD_Clear(LCD_COLOR_BLUE);
}
void Initial::play_again()
{
    draw_caption();
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"Touch screen to play again", CENTER_MODE);
}
void ini_stack_enemy(int num_enemy)
{
    for (int i = num_enemy; i > 0; i--)
    {
        int x_enemy_position = SCREEN_X_LEN - i * SCREEN_X_LEN / 6;
        int y_enemy_position = INI_POS_Y_ENEMY;
        enemy_position_x.push(x_enemy_position);
        enemy_position_y.push(y_enemy_position);
    }
}
class End
{
private:
    int win_false;

public:
    int judge_win_fail(int win_false);
    void fail_transition();
    void win_transition();
};

int End::judge_win_fail(int win_false)
{
    if (enemy_position_y.size() == 0)
    {
        win_false = 1;
    }
    else
    {
        win_false = 0;
    }
    return win_false;
}
void End::fail_transition()
{
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"Failure", CENTER_MODE);
}
void End::win_transition()
{
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, SCREEN_Y_LEN / 2, (uint8_t *)"Winner", CENTER_MODE);
}
int main()
{
    Move Move;
    Rand_number Rand_number;
    Initial Initial;
    Setting Setting;
    End End;
    int num_enemy = 3;
    // have the initial objects position
    // 0 is continuing playing, 1 is win, 2 is failure
    int win_false = 0;
    int bullet_x_position = 0;
    int bullet_y_position = 0;
    int x_player_position = INI_POS_X_PLAYER;
    int y_player_position = INI_POS_Y_PLAYER;
    int x_enemy_position = INI_POS_X_ENEMY;
    int y_enemy_position = INI_POS_Y_ENEMY;
    int direction_player = 0;
    int flag = 0;
    int num_score = 0;
    int lives = LIFE;
    int step = 0;
    int live_enemy = 5;
    int num_killed[2];
    int *x_enemy_pos_arr = new int(5);
    int *x_enemy_pos_real_arr = new int(5);
    int *y_enemy_pos_arr = new int(5);

    //initialize the touch screen
    Initial.ini_touch_screen();
    Initial.draw_initial_caption();
    Initial.ini_map();
    Initial.draw_ini_tank(num_enemy);
    ini_stack_enemy(num_enemy);

    while (1)
    {
        int bullet_flag = 1;
        int bullet_exist[100];
        int num_bullet = 0;
        int num_enemy_bullet = 0;
        /*read the value from the button*/
        int p1 = button.read();

        if (p1 && bullet_flag)
        {
            play_tone(300.0, 5, 0.1, 0);
            play_tone(150.0, 5, 0.1, 0);
            //use stack to govern the value of score attribution
            player_bullet_x.push(x_player_position);
            player_bullet_y.push(y_player_position);
            //this ensures one score once pressed button
            bullet_flag = 0;
            num_bullet = num_bullet + 1;
        }

        /*This part move the tank after the screen is touched*/
        BSP_TS_GetState(&TS_State);
        if (TS_State.touchDetected)
        {
            //get the touch score
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            // This part calculate the distance
            direction_player = Move.setdirection(x1, y1, x_player_position, y_player_position);
            // this part move the object
            if (abs(direction_player) > 6)
            {
                int x_move = direction_player;
                x_player_position = x_player_position + x_move;
                if (x_player_position + SIZE_TANK_LONG >= 235)
                {
                    int max = 235 - SIZE_TANK_LONG;
                    x_player_position = max;
                }
                if (x_player_position - SIZE_TANK_LONG <= 5)
                {
                    int min = 5 + SIZE_TANK_LONG;
                    x_player_position = min;
                }
            }
            else
            {
                int y_move = direction_player;
                y_player_position = y_player_position + y_move;
                /*This part avoid the player tank collide with border*/
                if (y_player_position + SIZE_TANK_FRONT + SIZE_TANK_WIDTH >= 235)
                {
                    int max = 235 - SIZE_TANK_FRONT - SIZE_TANK_WIDTH;
                    y_player_position = max;
                }
                if (y_player_position - SIZE_TANK_WIDTH <= 5)
                {
                    int min = 5 + SIZE_TANK_WIDTH;
                    y_player_position = min;
                }
            }

            bullet_flag = 1;
        }

        /*After the area of touching, enemy tank should be moved*/
        // This control the enemy tank
        int num = 1;
        int *after_move = 0;
        int num_position = 1;
        while (!enemy_position_x.empty())
        {
            x_enemy_position = enemy_position_x.top();
            y_enemy_position = enemy_position_y.top();
            // This is part move the tank randomly
            // Move close to player tank
            if (num == 1) //This is move pattern for 1 tank, and it can move randomly
            {
                if (Rand_number.rand_certain_tank() > num * 2)
                {
                    int rand_pos = Rand_number.rand_num();
                    if (rand_pos > x_enemy_position && (rand_pos % 120) > y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    if (rand_pos > x_enemy_position && (rand_pos % 120) < y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                    if (rand_pos < x_enemy_position && (rand_pos % 120) > y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    if (rand_pos < x_enemy_position && (rand_pos % 120) - y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
            }
            // This tank move behind randomly
            if (num == 2)
            {
                int rand_pos = Rand_number.rand_num();
                if (rand_pos > x_enemy_position)
                {
                    x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                    if (Rand_number.rand_certain_tank() > num * 2)
                    {
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    else
                    {
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
                if (rand_pos < x_enemy_position)
                {
                    x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                    if (Rand_number.rand_certain_tank() > num * 2)
                    {
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    else
                    {
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
            }
            // This tank close to player tank
            if (num == 3)
            {
                int *enemy_player_distance = Move.cal_distance(x_enemy_position, y_enemy_position, x_player_position, y_player_position);
                if (enemy_player_distance[0] > DISTANCE)
                {
                    if (enemy_player_distance[1] < 0)
                    {
                        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                    }
                    else
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                    }
                    if (enemy_player_distance[2] < 0)
                    {
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    else
                    {
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
                else
                {
                    x_enemy_position = x_enemy_position;
                    y_enemy_position = y_enemy_position;
                }
            }
            //This part avoid collision with player and enemy
            after_move = Move.avoid_collision(x_enemy_position, y_enemy_position, x_player_position, y_player_position);
            x_enemy_position = after_move[0];
            y_enemy_position = after_move[1];
            // This part have a randun number to shoot the bullet
            /*if (Rand_number.rand_num_bullet(num_enemy) > 2)
            {
                num_enemy_bullet += num_enemy_bullet;
                enemy_bullet_x.push(x_enemy_position);
                enemy_bullet_y.push(y_enemy_position);
            }*/
            //This can change the rate of move
            num = num + 1;
            //This part store the data of enemy position
            temp_enemy_position_x.push(x_enemy_position);
            temp_enemy_position_y.push(y_enemy_position);
            enemy_position_x.pop();
            enemy_position_y.pop();
        }
        /*Next part should draw the map*/
        // clear the previous map
        Initial.ini_map();
        // draw the player tank
        draw_player_tank(x_player_position, y_player_position);
        //This part show the lives, score and remaining time
        Setting.show_life(lives);
        Setting.show_score(num_score);
        step = Setting.show_time(step);
        // update the position of score
        //only y axis is fulfilled here

        while (!temp_enemy_position_y.empty())
        {
            x_enemy_position = temp_enemy_position_x.top();
            y_enemy_position = temp_enemy_position_y.top();
            draw_enemy_tank(x_enemy_position, y_enemy_position);
            enemy_position_y.push(y_enemy_position);
            enemy_position_x.push(x_enemy_position);

            temp_enemy_position_y.pop();
            temp_enemy_position_x.pop();
        }
        while (!enemy_bullet_y.empty())
        {
            bullet_x_position = enemy_bullet_x.top();
            bullet_y_position = enemy_bullet_y.top();

            enemy_bullet_x.pop();
            enemy_bullet_y.pop();
            bullet_y_position = bullet_y_position + BULLET_SPEED;
            // create new bullet
            BSP_LCD_SetTextColor(LCD_COLOR_ORANGE); //This part create the bullet again
            BSP_LCD_FillCircle(bullet_x_position + 8, bullet_y_position, BULLET_RADIUS);
            temp_enemy_bullet_y.push(bullet_y_position);
        }
        //This part judge the remaining number of enemy tank
        win_false = End.judge_win_fail(win_false);
        while (!player_bullet_y.empty())
        {
            int *dis;
            bullet_x_position = player_bullet_x.top();
            bullet_y_position = player_bullet_y.top();
            while (!enemy_position_y.empty())
            {
                x_enemy_position = enemy_position_x.top();
                y_enemy_position = enemy_position_y.top();

                dis = Move.cal_distance(bullet_x_position, bullet_y_position, x_enemy_position, y_enemy_position);
                if (dis[0] < DISTANCE / 4)
                {
                    draw_tank_underattack(x_enemy_position, y_enemy_position);
                    num_score = num_score + 20;
                }
                else
                {
                    temp_enemy_position_y.push(y_enemy_position);
                    temp_enemy_position_x.push(x_enemy_position);
                    bullet_y_position = bullet_y_position - BULLET_SPEED;
                    temp_player_bullet_y.push(bullet_y_position);
                }
                enemy_position_y.pop();
                enemy_position_x.pop();
            }
            player_bullet_y.pop();
            // create new bullet
            BSP_LCD_SetTextColor(LCD_COLOR_ORANGE); //This part create the bullet again
            BSP_LCD_FillCircle(bullet_x_position + 10, bullet_y_position, BULLET_RADIUS);
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
        while (!temp_player_bullet_y.empty())
        {
            bullet_y_position = temp_player_bullet_y.top();
            if (bullet_y_position > 0 && bullet_y_position < SCREEN_Y_LEN)
            {
                player_bullet_y.push(bullet_y_position);
            }
            temp_player_bullet_y.pop();
        }
        while (!temp_enemy_bullet_y.empty())
        {
            bullet_y_position = temp_enemy_bullet_y.top();
            if (bullet_y_position > 0 && bullet_y_position < SCREEN_Y_LEN)
            {
                enemy_bullet_y.push(bullet_y_position);
            }
            temp_enemy_bullet_y.pop();
        }

        if (win_false == 1)
        {
            End.win_transition();
            wait(10);
            while (1)
            {
                BSP_TS_GetState(&TS_State);
                Initial.play_again();
                if (TS_State.touchDetected)
                {
                    uint16_t x1 = TS_State.touchX[0];
                    uint16_t y1 = TS_State.touchY[0];
                    //initialize the touch screen
                    Initial.ini_touch_screen();
                    Initial.ini_map();
                    Initial.draw_ini_tank(num_enemy);
                    ini_stack_enemy(num_enemy);
                    break;
                }
                wait_us(10);
            }
        }
        if (win_false == 2)
        {
            End.win_transition();
            wait(10);
            while (1)
            {
                BSP_TS_GetState(&TS_State);
                Initial.play_again();
                if (TS_State.touchDetected)
                {
                    uint16_t x1 = TS_State.touchX[0];
                    uint16_t y1 = TS_State.touchY[0];
                    //initialize the touch screen
                    Initial.ini_touch_screen();
                    Initial.ini_map();
                    Initial.draw_ini_tank(num_enemy);
                    ini_stack_enemy(num_enemy);
                    break;
                }
                wait_us(10);
            }
        }

        // wait for a while
        wait_ms(10);
    }
}