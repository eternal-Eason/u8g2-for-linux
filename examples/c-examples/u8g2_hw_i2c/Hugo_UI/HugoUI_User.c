#include "HugoUI_User.h"
#include "Hugo_UI.h"
#include "HugoUI_BMP.h"
#include "u8g2.h"

/******************************变量存放区******************************/

// ui控制变量
extern uint8_t ui_Key_num, ui_Encoder_num; // user可以将自己的实现函数的变量直接赋值给这两个Num

// 开关控件变量
uint8_t SmoothAnimation_Flag = true; // 缓动动画标志位 // 缓动动画flag需要在HugoUI_Control中使用，建议不要改动！（don't change this one!!!）
uint8_t OptionStripFixedLength_Flag = false;

uint8_t InverseColor_flag = false; // oled屏幕反色（白色）标志位

uint8_t BeeperEnable_flag = true;
uint8_t PageListMode_flag = false;
uint8_t SetScreenFlip_flag = false;

// 滑动条变量
float frame_y_speed = 65;
float frame_x_speed = 65;
float frame_width_speed = 65;
float slidbar_y_speed = 65;
float page_x_foricon_speed = 65;
float page_y_forlist_speed = 65;

paramType WS2812_R = 10.0f;
paramType WS2812_G = 10.0f;
paramType WS2812_B = 10.0f;

paramType val1 = 0.1f;
paramType val2 = 0.2f;
paramType val3 = 0.3f;

paramType BeeperLoud = 0;

paramType ScreenBrightness = 255;
paramType MenuScroll = 0;

float pitch, roll, yaw;
short aacx, aacy, aacz;
short gyrox, gyroy, gyroz;

extern float icon_move_x;

u8g2_t u8g2; // 初始化u8g2结构体



/**
 *    @结构体 Slide_Bar Slide_space[]
 *    @brief 滑动条控件 用于存储滑动条控件的值(即储存param)
 *    @param none
 *
 *    @变量
 *        int   val    值
 *        int   min  最小值
 *        int   max  最大值
 *        int   step 步进
 */

struct Slide_Bar Slide_space[] = {
    /* ---------- 屏幕亮度变量 ---------- */
    {(float *)&ScreenBrightness, 1, 255, 16}, // 亮度调整

    /* ---------- 动画速度变量 ---------- */
    {(float *)&frame_x_speed, 20, 200, 1},
    {(float *)&frame_y_speed, 20, 200, 1},
    {(float *)&frame_width_speed, 20, 200, 1},
    {(float *)&slidbar_y_speed, 20, 200, 1},
    {(float *)&page_y_forlist_speed, 20, 200, 1},
    {(float *)&page_x_foricon_speed, 20, 200, 1},

    /* ---------- 用户设置变量 ---------- */
    {(float *)&WS2812_R, 0, 255, 1}, // WS2812
    {(float *)&WS2812_G, 0, 255, 1},
    {(float *)&WS2812_B, 0, 255, 1},

    {(float *)&val1, 0, 10, 0.1}, // param
    {(float *)&val2, 0, 10, 0.5},
    {(float *)&val3, 0, 10, 0.1},

    {(float *)&BeeperLoud, 0, 88, 5}, // 蜂鸣器响度

};


uint8_t *Switch_space[] = {
    &SmoothAnimation_Flag, // 缓动动画flag需要在HugoUI_Control中使用，建议不要改动！（don't change this one!!!）
    &OptionStripFixedLength_Flag,
    &InverseColor_flag,
    &PageListMode_flag,
    &SetScreenFlip_flag,

};
/* -------------------- 用户函数 -------------------- */

void HugoUI_InitLayout(void)
{
    
    
    uint8_t fputc_select_flag = 0; // 0->USART1 / 1->USART6
    /* 注册 Page */
    HugoUIPage_t *pageMain = AddPage(PAGE_CUSTOM, "pageMain");
    HugoUIPage_t *pageSetting = AddPage(PAGE_LIST, "pageSetting");
    HugoUIPage_t *pagePID = AddPage(PAGE_LIST, "pagePID");
    HugoUIPage_t *pageAnimation = AddPage(PAGE_LIST, "pageAnimation");
    HugoUIPage_t *pageWS2812 = AddPage(PAGE_LIST, "pageWS2812");
    //HugoUIPage_t *pageFiles = AddPage(PAGE_LIST, "pageFiles")
     //                             ->SetPgaeFunCallBack(HugoUIPageFilesCallBack);

    /* 注册 Item */
    // PageMain
    pageMain->AddItem(pageMain, "Setting", ITEM_JUMP_PAGE)
        ->SetJumpId(pageSetting->pageId, 0)
        ->SetIconSrc(Setting_BMP);


    pageMain->AddItem(pageMain, "Animation", ITEM_JUMP_PAGE)
        ->SetJumpId(pageAnimation->pageId, 0)
        ->SetIconSrc(Lighting_BMP);



    pageMain->AddItem(pageMain, "RGB", ITEM_JUMP_PAGE)
        ->SetJumpId(pageWS2812->pageId, 0)
        ->SetIconSrc(Unicorn_BMP);

    pageMain->AddItem(pageMain, "???", ITEM_CALL_FUNCTION, EventTreasureBox)
        ->SetIconSrc(TreasureBox_BMP);

    //ageMain->AddItem(pageMain, "MyFiles", ITEM_JUMP_PAGE)
    //    ->SetJumpId(pageFiles->pageId, 0)
    //    ->SetIconSrc(TreasureBox_BMP);

    pageMain->AddItem(pageMain, "About", ITEM_CALL_FUNCTION, EventShowAboutUI)
        ->SetIconSrc(Home_BMP);

    // PageSetting
    pageSetting->AddItem(pageSetting, "Setting", ITEM_PAGE_DESCRIPTION);
    pageSetting->AddItem(pageSetting, "PID Editor", ITEM_JUMP_PAGE)
        ->SetJumpId(pagePID->pageId, 0);

    pageSetting->AddItem(pageSetting, "InverseColor", ITEM_SWITCH, &InverseColor_flag, Oled_EventOledInverseColor);
    pageSetting->AddItem(pageSetting, "FilpScreen", ITEM_SWITCH, Switch_space[SwitchSpace_SetScreenFlip], EventOledSetScreenFlipMode);
    pageSetting->AddItem(pageSetting, "LightLevel", ITEM_CHANGE_VALUE, &ScreenBrightness, Oled_EventUpdateOledLightLevel);
    //pageSetting->AddItem(pageSetting, "BeepEnable", ITEM_SWITCH, &BeeperEnable_flag, EventBeeperEnableConfig);
    //pageSetting->AddItem(pageSetting, "UASRT62Printf", ITEM_SWITCH, &fputc_select_flag, NULL);
    pageSetting->AddItem(pageSetting, "Page2List", ITEM_SWITCH, &PageListMode_flag, NULL);
    //pageSetting->AddItem(pageSetting, "Volume Ctrl", ITEM_CHANGE_VALUE, Slide_space[Slide_space_Volume_Ctrl].val, EventBeeperVolumeControl);
    //pageSetting->AddItem(pageSetting, "SaveAll", ITEM_CALL_FUNCTION, EventSaveSettingConfig);

    pageSetting->AddItem(pageSetting, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageMain->pageId, 0);

    // PageAnimation
    pageAnimation->AddItem(pageAnimation, "Animation", ITEM_PAGE_DESCRIPTION);
    pageAnimation->AddItem(pageAnimation, "SmoothAnim", ITEM_SWITCH, &SmoothAnimation_Flag, NULL);
    pageAnimation->AddItem(pageAnimation, "-PlsSetSpeed--", ITEM_PAGE_DESCRIPTION);
    pageAnimation->AddItem(pageAnimation, "Fre_x", ITEM_CHANGE_VALUE, &frame_x_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "Fre_y", ITEM_CHANGE_VALUE, &frame_y_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "Fre_width", ITEM_CHANGE_VALUE, &frame_width_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "Slidbar_y", ITEM_CHANGE_VALUE, &slidbar_y_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "list_y", ITEM_CHANGE_VALUE, &page_y_forlist_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "icon_x", ITEM_CHANGE_VALUE, &page_x_foricon_speed, NULL);
    pageAnimation->AddItem(pageAnimation, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageMain->pageId, 3);

    // PagePID
    pagePID->AddItem(pagePID, "PID Editor", ITEM_PAGE_DESCRIPTION);
    pagePID->AddItem(pagePID, "PID_Kp", ITEM_CHANGE_VALUE, &val1, NULL);
    pagePID->AddItem(pagePID, "PID_Ki", ITEM_CHANGE_VALUE, &val2, NULL);
    pagePID->AddItem(pagePID, "PID_Kd", ITEM_CHANGE_VALUE, &val3, NULL);
    pagePID->AddItem(pagePID, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageSetting->pageId, 1);

    // PageWS2812
    pageWS2812->AddItem(pageWS2812, "WS2812RGBSet", ITEM_PAGE_DESCRIPTION);

    pageWS2812->AddItem(pageWS2812, "Exit", ITEM_JUMP_PAGE)
        ->SetJumpId(pageMain->pageId, 9);

    //HugoUIPageFilesAddItems(pageFiles, "1:");
    // ItemTail = ItemHead;
    // while(ItemTail != NULL)
    // {
    //     printf("name:%s\r\n", ItemTail->title);
    //     printf("lid:%d\r\n", ItemTail->lineId);
    //     printf("id:%d\r\n", ItemTail->itemId);
    //     ItemTail = ItemTail->next;
    // }
    // pageTail = pageHead;
    // while(pageTail != NULL)
    // {
    //     printf("name:%s\r\n", pageTail->title);
    //     printf("id:%d\r\n", pageTail->pageId);
    //     pageTail = pageTail->next;
    // }

    //printf("malloc used:%d\r\n", my_mem_perused(SRAMIN));
    printf("itemMax:%d\r\n", pageSetting->itemMax);
}


/* Oled设置屏幕翻转的函数 */
void EventOledSetScreenFlipMode(void)
{
    if (*Switch_space[SwitchSpace_SetScreenFlip] == true)
    {
        Oled_u8g2_SetFlipMode(2);
    }
    else
    {
        Oled_u8g2_SetFlipMode(0);
    }
}



/* Large Event */

extern uint8_t mpu6050_set_flag;
extern uint8_t sht30_set_flag;

float Temperature, Humidity;



/* About的应用事件函数 */
void EventShowAboutUI(void)
{
    static float motion_a = 80.0f, motion_a_trg = 0;
    // Oled_SetOledInverseColor(1);
    Oled_u8g2_ShowBMP(motion_a, 0, 40, 50, HeadSculpture_BMP);
    Oled_u8g2_ShowStr(50, FONT_HEIGHT, "HOPE  Pro");
    Oled_u8g2_ShowUTF8(45, FONT_HEIGHT * 2, "版本: Ver1.3");
    Oled_u8g2_ShowUTF8(45, FONT_HEIGHT * 3, "储存: 16 MB");
    Oled_u8g2_ShowUTF8(10, FONT_HEIGHT * 4, " By @kkl_aka科良");
    Oled_u8g2_SetDrawColor(2);
    Oled_u8g2_DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Oled_u8g2_DrawRBox(86, 3, 30, FONT_HEIGHT, 0);
    Oled_u8g2_SetDrawColor(1);

    HugoUI_Animation_Linear(&motion_a, &motion_a_trg, 85);

    if (ui_Key_num == 2)
    {
        // if (*Switch_space[SwitchSpace_OledInverseColor] == false)
        // {
        //     Oled_SetOledInverseColor(0);
        // }
        motion_a = 80.0f;
        icon_move_x = 128; // 倘若跳转到custom界面可以起到一个复位动效
    }
}

/* TreasureBox的事件函数 */
void EventTreasureBox(void)
{
    //Oled_DrawIntensiveComputing();
}

// width: 16, height: 16
/* 波形绘制的测试事件函数 */
void EventChartTest(void)
{ // Oled_DrawIntensiveComputing();
    static float a = 0, a_trg = 64;
    if (a == a_trg)
    {
        if (a == 0)
        {
            a_trg = 64;
        }
        else if (a == 64)
            a_trg = 0;
    }
    HugoUI_Animation_Linear(&a, &a_trg, 30);

    // Oled_DrawSlowBitmapResize(80 - a / 2, 16 - a / 4, LittleHreat_BMP, 16, 16, a / 1.5f, a / 1.5f);

    // Oled_DrawSlowBitmapResize(32 - a / 2, 32 - a / 4, Poet_BMP, 32, 32, 42, 42);
    // u8g2_SetBitmapMode(&u8g2, 0);
    // u8g2_DrawXBMP(&u8g2, 100, 0, 16, 16, LittleHreat_BMP);

    // u8g2_SetBitmapMode(&u8g2, 1);

    u8g2_DrawXBMP(&u8g2, 50, 12, 32, 32, Lighting_BMP);

    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_DrawXBMP(&u8g2, 50, 20, 32, 32, Poet_BMP);
    u8g2_SetDrawColor(&u8g2, 1);

    // u8g2_SetBitmapMode(&u8g2, 0);

    if (ui_Key_num == 2)
    {
        a = 0;
        icon_move_x = 128; // 倘若跳转到custom界面可以起到一个复位动效
    }
}



// 初始化HugoUI,同时播放开场动画
void HugoUIAnimationInit(void)
{
    uint8_t HugoUIEnter_flag = 0;
    uint8_t HugoUIInit_flag = 1;
    float FrameBox_Width, FrameBox_Width_trg = 105;

    u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312a); // 选择字库 // 内存不够就用u8g2_font_profont15_mr

    while (1)
    {
        Oled_u8g2_ClearBuffer();
        // Enter Func
        if (HugoUIEnter_flag)
        {
            Oled_u8g2_DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (HugoUI_Animation_Blur() == 0)
                HugoUIEnter_flag = 0;
            Oled_u8g2_SendBuffer();
            continue;
        }
        if (HugoUIInit_flag == 1)
        {
            Oled_u8g2_DrawRFrame(128 - (FrameBox_Width + 7), 33 - 10, FrameBox_Width, FONT_HEIGHT, 1);
            Oled_u8g2_ShowUTF8((128 - Oled_u8g2_Get_UTF8_ASCII_PixLen(" 请等待初始化完成 ")) / 2, 48 - 10, " 请等待初始化完成 "); // 请等待初始化完成
            Oled_u8g2_ShowUTF8((128 - Oled_u8g2_Get_UTF8_ASCII_PixLen("HelloHOPE")) / 2, FONT_HEIGHT, "HelloHOPE");
            Oled_u8g2_SetDrawColor(2);
            Oled_u8g2_DrawRBox(FrameBox_Width / 8, 35 - 10, FrameBox_Width, FONT_HEIGHT, 0);
            Oled_u8g2_SetDrawColor(1);
        }

        // Init
        if (HugoUI_Animation_Linear(&FrameBox_Width, &FrameBox_Width_trg, 95) == 0 && HugoUIInit_flag == 1)
        {
            HugoUI_InitLayout();
            HugoUIInit_flag = 0;
            break;
        }
        Oled_u8g2_SendBuffer();
    }
}
