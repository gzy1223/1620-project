#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

#define SCREEN_X_LEN 240
#define SCREEN_Y_LEN 240

TS_StateTypeDef TS_State = {0};
struct position
{
    int x;
    int y;
};
/*Touch screen initial h file*/
class iniTouchscreen
{
private:
public:
    void initial();
    int touch();
};
/*Touch screen CPP file*/

void iniTouchscreen::initial()
{
    printf("Touch screen to begin!\n");

    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR)
    {
        printf("BSP_TS_Init error\n");
    }
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}
class Map
{
private:
public:
    void creat_map();
    int *move_range();
    void change_move_range();
    int **create_space_array();
};

int **Map::create_space_array()
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
            range[x][y] = 0;
        }
    }
    return range;
}

void Map::creat_map()
{
    int **range = create_space_array();
    for (int x = 0; x < SCREEN_X_LEN; x++)
    {
        for (int y = 0; y < SCREEN_Y_LEN / 3; y++)
        {
            if (range[x][y] == 0)
            {
                BSP_LCD_DrawPixel(x, y, 51 - 51 - 255);
            }
        }
    }
}
int main()
{
    iniTouchscreen iniTouchscreen;
    Map Map;
    iniTouchscreen.initial();
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"Touch the screen", CENTER_MODE);

    while (1)
    {
        BSP_TS_GetState(&TS_State);
        if (TS_State.touchDetected)
        {
            /* One or dual touch have been detected          */

            /* Get X and Y position of the first touch post calibrated */
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            BSP_LCD_SetTextColor(LCD_COLOR_RED);
            BSP_LCD_FillCircle(x1, y1, 5);
            Map.create_space_array();
            Map.creat_map();
            wait_ms(10);
        }
    }
    delete []range
}
