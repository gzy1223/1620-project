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
#define WANT_SECOND 100

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
#define LIFE 20
#define DISTANCE 50

stack<int> player_bullet_x;
stack<int> player_bullet_y;
stack<int> temp_player_bullet_x;
stack<int> temp_player_bullet_y;

stack<int> enemy_position_x;
stack<int> enemy_position_y;
stack<int> temp_enemy_position_x;
stack<int> temp_enemy_position_y;

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
    play_tone(100, 10, 0.1, 0.1);
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
    int *num_enemy;
    int *add_lives;

public:
    int show_time(int step);
    void show_score(int score);
    void show_life(int live);
    int difficult(int *add_live, int *num_enemy);
    void play_again();
};

int Setting::show_time(int step)
{
    int want_second = WANT_SECOND;
    if (step == 0)
    {
        *second = want_second;
    }
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
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
    BSP_LCD_DisplayStringAt(30, 10, (uint8_t *)"TIME:", RIGHT_MODE);
    BSP_LCD_DisplayStringAt(10, 10, (uint8_t *)_SECOND, RIGHT_MODE);
    return step;
}
void Setting::show_score(int score)
{
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    char SCORE[14];
    sprintf(SCORE, "%d", score);
    BSP_LCD_DisplayStringAt(SCREEN_X_LEN / 2 - 30, 10, (uint8_t *)"SCORE:", LEFT_MODE);
    BSP_LCD_DisplayStringAt(SCREEN_X_LEN / 2 + 15, 10, (uint8_t *)SCORE, LEFT_MODE);
}
void Setting::show_life(int live)
{
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAt(15, 10, (uint8_t *)"LIFE:", LEFT_MODE);
    char _LIFE[14];
    sprintf(_LIFE, "%i", live);
    BSP_LCD_DisplayStringAt(55, 10, (uint8_t *)_LIFE, LEFT_MODE);
}
void Setting::play_again()
{
    BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"Touch screen to play again", CENTER_MODE);
}
int Setting::difficult(int *add_lives, int *num_enemy)
{
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Choose Difficulty", CENTER_MODE);
    BSP_LCD_DisplayStringAt(40, 120, (uint8_t *)"EASY", LEFT_MODE);
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"NORMAL", CENTER_MODE);
    BSP_LCD_DisplayStringAt(40, 120, (uint8_t *)"HARD", RIGHT_MODE);
    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected)
    {
        //get the touch score
        uint16_t x1 = TS_State.touchX[0];
        uint16_t y1 = TS_State.touchY[0];
        if (x1 <= 80)
        {
            *num_enemy = 1;
            *add_lives = LIFE;
        }
        if (x1 >= 80 && x1 <= 160)
        {
            *num_enemy = 2;
            *add_lives = LIFE - 5;
        }
        if (x1 >= 160)
        {
            *num_enemy = 3;
            *add_lives = LIFE - 10;
        }
        return 1;
    }
    else
    {
        return 0;
    }
    wait_us(10);
}
class Rand_number
{
private:
    int seed;

public:
    int rand_certain_tank();
    int rand_num1();
    int rand_num2();
    int rand_num_create_tank();
};

int Rand_number::rand_certain_tank()
{
    int seed;
    seed = rand() % 10;
    return seed;
}
/*This part shoot the random number of score*/
int Rand_number::rand_num1()
{
    int seed;
    srand((unsigned)time(NULL));
    seed = rand();
    void srand(unsigned int seed);
    seed = rand() % 240;
    return seed;
}
/*This returns a random number*/
int Rand_number::rand_num2()
{
    int seed;
    srand((unsigned)time(NULL));
    seed = rand() % 240;
    return seed;
}
int Rand_number::rand_num_create_tank()
{
    int seed;
    srand((unsigned)time(NULL));
    seed = rand() % 180 + 40;
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
        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
    }
    if (abs_distance < DISTANCE && relative_x_distance > 0)
    {
        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
    }
    if (abs_distance < DISTANCE && relative_y_distance < 0)
    {
        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
    }
    if (abs_distance < DISTANCE && relative_y_distance > 0)
    {
        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
    }
    if (x_enemy_position <= DISTANCE)
    {
        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
    }
    if (x_enemy_position >= SCREEN_X_LEN - DISTANCE)
    {
        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
    }
    if (y_enemy_position <= DISTANCE)
    {
        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
    }
    if (y_enemy_position <= SCREEN_Y_LEN - DISTANCE)
    {
        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
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
void ini_stack_enemy(int num_enemy)
{
    //clear the previous stack elements
    while (!enemy_position_x.empty())
    {
        enemy_position_y.pop();
        enemy_position_x.pop();
    }
    //push the new position in
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
    int score;

public:
    void fail_transition();
    void win_transition();
    void show_final(int score);
};

void End::fail_transition()
{
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(80, SCREEN_Y_LEN / 2 - 40, (uint8_t *)"Failure", LEFT_MODE);
}
void End::win_transition()
{
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(80, SCREEN_Y_LEN / 2 - 40, (uint8_t *)"Winner", LEFT_MODE);
}
void End::show_final(int score)
{
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    char SCORE[14];
    sprintf(SCORE, "%d", score);
    BSP_LCD_DisplayStringAt(80, SCREEN_Y_LEN / 2 - 10, (uint8_t *)"SCORE:", LEFT_MODE);
    BSP_LCD_DisplayStringAt(140, SCREEN_Y_LEN / 2 - 10, (uint8_t *)SCORE, LEFT_MODE);
}

int main()
{
    Move Move;
    Rand_number Rand_number;
    Initial Initial;
    Setting Setting;
    End End;
    int num_enemy = 3;
    int origin_num_enemy = 0;
    // have the initial objects position
    // 0 is continuing playing, 1 is win, 2 is failure
    int flag = 0;
    int win_false = 0;
    int bullet_x_position = 0;
    int bullet_y_position = 0;
    int x_player_position = INI_POS_X_PLAYER;
    int y_player_position = INI_POS_Y_PLAYER;
    int x_enemy_position = INI_POS_X_ENEMY;
    int y_enemy_position = INI_POS_Y_ENEMY;
    int direction_player = 0;
    int num_score = 0;
    int lives = LIFE;
    int enemy_bullet_x[num_enemy];
    int enemy_bullet_y[num_enemy];
    //control the showing time while looping
    int step = 0;
    int hit_enemy_step = 0;
    int hit_player_step = 0;
    int real_num_enemy = num_enemy;
    int num_kill = 0;

    //initialize the touch screen
    Initial.ini_touch_screen();
    Initial.draw_initial_caption();
    Initial.ini_map();
    int *addr_lives = &lives;
    int *addr_num_enemy = &num_enemy;
    // choose the difficulties
    // easy pattern has 1 enemy, normal pattern has two enemy, while hard pattern has three enemy
    while (1)
    {
        flag = Setting.difficult(addr_lives, addr_num_enemy);
        //draw tank based on the choice
        Initial.draw_ini_tank(num_enemy);
        ini_stack_enemy(num_enemy);
        for (int i = 0; i < sizeof(enemy_bullet_x); i++)
        {
            enemy_bullet_x[i] = 300;
            enemy_bullet_y[i] = 300;
        }
        if (flag)
        {
            break;
        }
    }

    while (1)
    {
        int bullet_exist[10];
        int num_bullet = 0;
        /*read the value from the button*/
        int p1 = button.read();
        if (p1 && num_bullet < 3)
        {
            num_score -= 1;
            play_tone(300.0, 5, 0.1, 0);
            play_tone(150.0, 5, 0.1, 0);
            //use stack to govern the value of score attribution
            player_bullet_x.push(x_player_position);
            player_bullet_y.push(y_player_position);
            //this ensures one score once pressed button
            num_bullet += 1;
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
        }

        /*After the area of touching, enemy tank should be moved*/
        // This control the enemy tank
        int num = 1;
        int *after_move = 0;
        int num_position = 1;
        while (!enemy_position_x.empty())
        {
            //This part inital first several tanks
            x_enemy_position = enemy_position_x.top();
            y_enemy_position = enemy_position_y.top();
            // This is part move the tank randomly
            // Move close to player tank
            if (num == 1) //This is move pattern for 1 tank, and it can move randomly
            {
                if (Rand_number.rand_certain_tank() > num * 2)
                {
                    int rand_pos = Rand_number.rand_num1();
                    if ((rand_pos % 80) > x_enemy_position && (rand_pos % 120) > y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                        if (y_enemy_position > 80)
                        {
                            y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                        }
                    }
                    if ((rand_pos % 80) > x_enemy_position && (rand_pos % 120) < y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                    if ((rand_pos % 80) < x_enemy_position && (rand_pos % 120) > y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                        if (y_enemy_position > 80)
                        {
                            y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                        }
                    }
                    if ((rand_pos % 80) < x_enemy_position && (rand_pos % 120) - y_enemy_position)
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
                // This part show the bullet
                if (enemy_bullet_y[0] > SCREEN_X_LEN)
                {
                    enemy_bullet_x[0] = x_enemy_position;
                    enemy_bullet_y[0] = y_enemy_position;
                }
            }
            // This tank move behind randomly
            if (num == 2)
            {
                int rand_pos = Rand_number.rand_num2();
                if (rand_pos > x_enemy_position)
                {
                    x_enemy_position = x_enemy_position + X_MOVE_SPEED;
                    if (Rand_number.rand_certain_tank() > num)
                    {
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    else
                    {
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                        if (y_enemy_position < 60)
                        {
                            y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                        }
                    }
                }
                if (rand_pos < x_enemy_position)
                {
                    x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                    if (Rand_number.rand_certain_tank() > num)
                    {
                        y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                    }
                    else
                    {
                        y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                    }
                }
                if (y_enemy_position < 80)
                {
                    y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                }

                if (enemy_bullet_y[1] > SCREEN_X_LEN)
                {
                    enemy_bullet_x[1] = x_enemy_position;
                    enemy_bullet_y[1] = y_enemy_position;
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
                    int rand_pos = Rand_number.rand_num2();
                    if (rand_pos > x_enemy_position / 2)
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
                    if (rand_pos < x_enemy_position / 2)
                    {
                        x_enemy_position = x_enemy_position - X_MOVE_SPEED;
                        if (Rand_number.rand_certain_tank() > num * 2)
                        {
                            y_enemy_position = y_enemy_position - Y_MOVE_SPEED;
                        }
                        else
                        {
                            y_enemy_position = y_enemy_position + Y_MOVE_SPEED;
                        }
                    }
                }
                if (enemy_bullet_y[2] > SCREEN_X_LEN)
                {
                    enemy_bullet_x[2] = x_enemy_position;
                    enemy_bullet_y[2] = y_enemy_position;
                }
            }
            //This part avoid collision with player and enemy
            after_move = Move.avoid_collision(x_enemy_position, y_enemy_position, x_player_position, y_player_position);
            x_enemy_position = after_move[0];
            y_enemy_position = after_move[1];
            // This part have a randun number to shoot the bullet

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
        //This part show score and remaining time
        Setting.show_life(lives);
        Setting.show_score(num_score);
        step = Setting.show_time(step);
        // update the position of score
        //only y axis is fulfilled here
        //This part show the number of remaining tank
        num_enemy = temp_enemy_position_y.size();
        //Next part will clear the stack to make sure the movement of the tank
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
        num_kill = 0;
        //This part judge the remaining number of enemy tank
        while (!player_bullet_y.empty())
        {
            int *dis;
            int killed = 0;
            bullet_x_position = player_bullet_x.top();
            bullet_y_position = player_bullet_y.top();
            while (!enemy_position_y.empty())
            {
                killed = 0;
                x_enemy_position = enemy_position_x.top();
                y_enemy_position = enemy_position_y.top();
                /*This part judge if the bullet hit the enemy tank*/
                dis = Move.cal_distance(bullet_x_position, bullet_y_position, x_enemy_position, y_enemy_position);
                if (dis[0] < (DISTANCE / 3 + 10))
                {
                    hit_enemy_step = step;
                    draw_tank_underattack(x_enemy_position, y_enemy_position);
                    int temp_x_enemy_position = Rand_number.rand_certain_tank();
                    int temp_y_enemy_position = 0;
                    num_score = num_score + 20;
                    num_kill += 1;
                }
                else
                {
                    temp_enemy_position_y.push(y_enemy_position);
                    temp_enemy_position_x.push(x_enemy_position);
                    bullet_y_position = bullet_y_position - BULLET_SPEED;
                    killed = 1;
                }

                enemy_position_y.pop();
                enemy_position_x.pop();
            }
            if (killed == 1)
            {
                temp_player_bullet_y.push(bullet_y_position);
            }
            player_bullet_y.pop();
            // create new bullet
            BSP_LCD_SetTextColor(LCD_COLOR_ORANGE); //This part create the bullet again
            BSP_LCD_FillCircle(bullet_x_position + 11, bullet_y_position, BULLET_RADIUS);
        }

        for (int i = 0; i < sizeof(enemy_bullet_y); i++)
        {
            int enemy_bullet_pos_y = enemy_bullet_y[i];
            int enemy_bullet_pos_x = enemy_bullet_x[i];
            enemy_bullet_pos_y = enemy_bullet_pos_y + BULLET_SPEED;
            int *dis = Move.cal_distance(enemy_bullet_pos_x, enemy_bullet_pos_y, x_player_position, y_player_position);
            if (dis[0] < DISTANCE / 3)
            {
                draw_tank_underattack(x_player_position, y_player_position);
                lives -= 1;
                num_score -= 2;
                if (lives <= 0)
                {
                    hit_player_step = step;
                    win_false = 2;
                }
                enemy_bullet_y[i] = 300;
            }
            else
            {
                BSP_LCD_SetTextColor(LCD_COLOR_DARKRED); //This part create the bullet again
                BSP_LCD_FillCircle(enemy_bullet_pos_x + 11, enemy_bullet_pos_y, BULLET_RADIUS);
                enemy_bullet_y[i] = enemy_bullet_pos_y;
            }
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
            num_bullet -= 1;
        }
        if (num_kill >= 1)
        {
            num_kill -= 1;
            x_enemy_position = Rand_number.rand_num_create_tank();
            y_enemy_position = 0;
            temp_enemy_position_y.push(y_enemy_position);
            temp_enemy_position_x.push(x_enemy_position);
        }
        if (WANT_SECOND == (step / 5))
        {
            win_false = 1;
        }

        if (win_false == 1)
        {
            step = 0;
            flag = 0;
            End.win_transition();
            End.show_final(num_score);
            lives = LIFE;
            num_score = 0;
            wait(10);
            while (1)
            {
                BSP_TS_GetState(&TS_State);
                flag = Setting.difficult(addr_lives, addr_num_enemy);
                Setting.play_again();
                if (flag == 1)
                {
                    //initialize the touch screen
                    Initial.ini_touch_screen();
                    Initial.ini_map();
                    Initial.draw_ini_tank(num_enemy);
                    ini_stack_enemy(num_enemy);
                    win_false = 0;
                    break;
                }
                wait_us(10);
            }
        }
        if (win_false == 2 && hit_player_step == (step - 1))
        {
            step = 0;
            End.fail_transition();
            End.show_final(num_score);
            lives = LIFE;
            num_score = 0;
            wait(10);
            while (1)
            {
                BSP_TS_GetState(&TS_State);
                flag = Setting.difficult(addr_lives, addr_num_enemy);
                Setting.play_again();
                if (flag == 1)
                {
                    //initialize the touch screen
                    Initial.ini_touch_screen();
                    Initial.ini_map();
                    Initial.draw_ini_tank(num_enemy);
                    ini_stack_enemy(num_enemy);
                    win_false = 0;
                    break;
                }
                wait_us(10);
            }
        }

        // wait for a while
        wait_ms(10);
    }
}