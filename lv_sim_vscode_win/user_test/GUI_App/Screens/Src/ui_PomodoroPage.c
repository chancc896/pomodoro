#include <stdio.h>
#include <stdbool.h>
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_PomodoroPage.h"
#include "../Inc/ui_PomodoroSetPage.h"
///////////////////// Page Manager //////////////////
Page_t Page_Pomodoro = {ui_PomodoroPage_screen_init, ui_PomodoroPage_screen_deinit, &ui_PomodoroPage};

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_PomodoroPage;
lv_obj_t * ui_PomodoroTimeLabel;
lv_obj_t * ui_PomodoroStartBtn;
lv_obj_t * ui_PomodoroStartLabel;
lv_obj_t * ui_PomodoroResetBtn;
lv_obj_t * ui_PomodoroResetLabel;
lv_obj_t * pomodoro_arc;
lv_obj_t * ui_PomodoroPage;
lv_obj_t * ui_PomodoroTimeLabel;
lv_obj_t * ui_PomodoroStartBtn;
lv_obj_t * ui_PomodoroStartLabel;
lv_obj_t * ui_PomodoroResetBtn;
lv_obj_t * ui_PomodoroResetLabel;
lv_obj_t * ui_PomodoroSetupBtn;    /* 齿轮按钮 */
lv_obj_t * pomodoro_arc;



static lv_obj_t * pomo_tomato;        /* 番茄圆形 */
static lv_obj_t * pomo_state_label;   /* 状态文字 */
static lv_obj_t * pomo_round_label;   /* 轮数显示 */
static lv_obj_t * pomo_leaf;         /* 番茄叶子 */

//工作时间和休息时间和休息轮暂定写死后用变量替代
uint16_t p_work_min   = 25 ;
uint16_t p_break_min  = 5 ; 
uint8_t p_rounds   = 4 ;
uint8_t p_ready_min = 1 ; // 准备阶段的时间


typedef enum//定义一个枚举类型，表示番茄钟的不同阶段
{
    POMODORO_WORK,  // 工作阶段
    POMODORO_BREAK, // 休息阶段
    POMODORO_READY // 准备阶段
}pomo_state_t;

static pomo_state_t state = POMODORO_READY; // 初始当前状态设置为准备阶段 
static uint8_t work_done_cnt = 0; // 已完成工作次数

/*---- 倒计时状态 ----*/
static uint32_t remain_ms;        // 剩余时间
static bool running;              // 是否正在倒计时
static bool pomo_initialized = false;
static bool page_active;
bool pomosetpageback = true;// 用于判断是否能从home回到设置页面,true为可以,false为不可以
static lv_timer_t * pomodoro_timer;

/*---- 把剩余时间刷到标签 ----*/
static void pomodoro_update_label(void)
{
    uint16_t mm = remain_ms / 60000;// 剩余分钟转化
    uint8_t  ss = (remain_ms / 1000) % 60;//剩余秒转化
    char buf[8];
    snprintf(buf, sizeof(buf), "%02u:%02u", mm, ss);   // 注意：用 snprintf，不用 sprintf
    lv_label_set_text(ui_PomodoroTimeLabel, buf);
}

/*---- 刷新环形进度（从12点逆时针填充）----*/
static void pomodoro_update_arc(void)
{
    uint32_t total_ms;
    switch (state)
    {
    case POMODORO_WORK:
        total_ms = (uint32_t)p_work_min * 60 * 1000;
        break;
    case POMODORO_BREAK:
        total_ms = (uint32_t)p_break_min * 60 * 1000;
        break;
    case POMODORO_READY:
        total_ms = (uint32_t)p_ready_min * 60 * 1000;
        break;
    }
    
    uint32_t elapsed_ms = total_ms - remain_ms; //流逝了多少ms
    uint16_t filled_deg = (total_ms == 0) ? 360
                         : (uint16_t)((uint64_t)elapsed_ms * 360 / total_ms);// 计算当前目标角度的值关系是与流逝时间与总时间360的比例关系

    if(filled_deg == 0) {
        /* 还没开始：隐藏指示弧，只露半透明背景 */
        lv_obj_set_style_arc_opa(pomodoro_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);//设置透明度
    } else {
        lv_obj_set_style_arc_opa(pomodoro_arc, LV_OPA_COVER, LV_PART_INDICATOR);
        if(filled_deg >= 360) {
            lv_arc_set_angles(pomodoro_arc, 0, 360);          /* 整圈填满 */
        } else {
            int16_t end_angle = 270 - filled_deg;             /* 逆时针：角度减小 */
            if(end_angle < 0) end_angle += 360;
            /* 指示弧 = 从终点顺时针回到12点(270°)的那段 */
            lv_arc_set_angles(pomodoro_arc, end_angle, 270);
        }
    }
}

/*---- 刷新状态文字和番茄颜色 ----*/
static void pomodoro_update_state_label(void)
{
    const char * txt;
    switch(state) {
        case POMODORO_READY: txt = "Ready"; break;
        case POMODORO_WORK:  txt = "Work";  break;
        case POMODORO_BREAK: txt = "Break"; break;
        default:             txt = "";break;
    }
    lv_label_set_text(pomo_state_label, txt);
}
/*---- 刷新轮数显示 ----*/
static void pomodoro_update_round_label(void)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "Round %u/%u", work_done_cnt + 1, p_rounds);
    lv_label_set_text(pomo_round_label, buf);
}

/*---- 定时器回调：每100ms一次 ----*/
static void pomodoro_timer_cb(lv_timer_t * timer)
{
    if(!running) return;              // 暂停中：直接返回跳出函数

    if(remain_ms > 100)
        remain_ms -= 100;
    else
        remain_ms = 0;
    if(page_active)//只有在番茄钟页面存活的时候才能刷新显示，否则不刷新，避免在其他页面刷新导致闪烁
    {
        pomodoro_update_label();
        pomodoro_update_arc();
        pomodoro_update_state_label();
        pomodoro_update_round_label();
    }
    if(remain_ms > 0) return;//还未倒计时完成，不做状态切换判断，直接返回直至倒计时完成
    switch(state)
    {
        case POMODORO_WORK:
            {
                state = POMODORO_BREAK; // 进入休息阶段
                remain_ms = p_break_min * 60 * 1000;
            }
            break;
        case POMODORO_BREAK:
            work_done_cnt++;// 休息阶段结束，增加已完成工作次数
            if(work_done_cnt >= p_rounds)
            {
                state = POMODORO_READY; // 完成所有轮次，回到准备阶段
                running = false;
                work_done_cnt = 0;
                remain_ms = p_ready_min * 60 * 1000;
                if(page_active)lv_label_set_text(ui_PomodoroStartLabel, LV_SYMBOL_PLAY);
            }
            else
            {
                state = POMODORO_WORK; // 进入工作阶段
                remain_ms = p_work_min * 60 * 1000;
            }
            break;
        case POMODORO_READY:
            state = POMODORO_WORK;
            remain_ms = p_work_min * 60 * 1000; // 准备阶段完成，重置剩余时间为工作阶段的时间
            break;
    }
    if(page_active)
    {
        pomodoro_update_label();
        pomodoro_update_arc();
        pomodoro_update_state_label();
        pomodoro_update_round_label();
    }
}

/*---- 开始/暂停按钮/回调函数 ----*/
static void start_btn_cb(lv_event_t * e)
{
    running = !running;
    lv_label_set_text(ui_PomodoroStartLabel, running ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

/*---- 重置按钮 ----*/
static void reset_btn_cb(lv_event_t * e)
{
    running = false;
    remain_ms = p_ready_min * 60 * 1000;
    state = POMODORO_READY;// 重置后直接进入工作阶段
    work_done_cnt = 0;
    lv_label_set_text(ui_PomodoroStartLabel, LV_SYMBOL_PLAY);
    pomodoro_update_label();
    pomodoro_update_arc();
    pomodoro_update_state_label();
    pomodoro_update_round_label();
}

/*---- 齿轮按钮 ----*/
static void setup_btn_cb(lv_event_t * e)
{
    running = false;//先暂停
    pomo_initialized = false; // 重新初始化番茄钟状态
    Page_Replace_Dir(&Page_PomodoroSet, LV_SCR_LOAD_ANIM_FADE_ON);//淡入动画切换到设置页面

}
///////////////////// SCREEN init ////////////////////
void ui_PomodoroPage_screen_init(void)
{
    ui_PomodoroPage = lv_obj_create(NULL);
    // 设置页面背景颜色
    lv_obj_set_style_bg_color(ui_PomodoroPage, lv_color_hex( 0xFDF2E3 ), 0 );
    /*---- 倒计时进度环 ----*/
    pomodoro_arc = lv_arc_create(ui_PomodoroPage);
    lv_obj_set_size(pomodoro_arc, 180, 180);
    lv_obj_align(pomodoro_arc, LV_ALIGN_CENTER, 0, -40);
    lv_arc_set_bg_angles(pomodoro_arc, 0, 360);                 /* 背景整圈 */
    lv_arc_set_value(pomodoro_arc, 0);
    lv_obj_remove_style(pomodoro_arc, NULL, LV_PART_KNOB);       /* 去掉旋钮小圆点 */
    lv_obj_clear_flag(pomodoro_arc, LV_OBJ_FLAG_CLICKABLE);//禁止滑动事件

    // 设置环形背景环和指示环的颜色、透明度、宽度和圆头样式
    lv_obj_set_style_arc_color(pomodoro_arc, lv_color_hex( 0xC9DCC0 ), LV_PART_MAIN);
    lv_obj_set_style_arc_opa(pomodoro_arc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_arc_width(pomodoro_arc, 20 , LV_PART_MAIN);
    //设置指示环的颜色、宽度和圆头样式
    lv_obj_set_style_arc_color(pomodoro_arc, lv_color_hex( 0x4F7942 ), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(pomodoro_arc, 20 , LV_PART_INDICATOR); 
    /* 端点圆头，两行分别对应背景弧和指示弧 */ 
    lv_obj_set_style_arc_rounded(pomodoro_arc, true , LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(pomodoro_arc, true , LV_PART_INDICATOR);
    
    /* 番茄本体：红椭圆 */
    pomo_tomato = lv_obj_create(ui_PomodoroPage);
    lv_obj_set_size(pomo_tomato, 85, 75);
    lv_obj_align(pomo_tomato, LV_ALIGN_CENTER, -70,20);
    lv_obj_set_style_bg_color(pomo_tomato, lv_color_hex(0xE23B2E), 0);
    lv_obj_set_style_radius(pomo_tomato, LV_RADIUS_CIRCLE, 0); /* 非正方形+全圆角=椭圆 */
    lv_obj_set_style_border_width(pomo_tomato, 0, 0);
    lv_obj_clear_flag(pomo_tomato, LV_OBJ_FLAG_SCROLLABLE);
    /* 叶子：绿圆角矩形，顶在番茄头上 */
    pomo_leaf = lv_obj_create(pomo_tomato);
    lv_obj_set_size(pomo_leaf, 34, 16);
    lv_obj_align(pomo_leaf, LV_ALIGN_TOP_MID, 0, -8);
    lv_obj_set_style_bg_color(pomo_leaf, lv_color_hex(0x3E5C34), 0);
    lv_obj_set_style_radius(pomo_leaf, 8, 0);
    lv_obj_set_style_border_width(pomo_leaf, 0, 0);

    //状态标签和轮数标签
    pomo_state_label = lv_label_create(pomo_tomato);
    pomo_round_label = lv_label_create(pomo_tomato);
    lv_obj_align(pomo_state_label, LV_ALIGN_CENTER, 0, -8);
    lv_obj_align(pomo_round_label, LV_ALIGN_CENTER, 0, 14);
    lv_obj_set_style_text_color(pomo_state_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_color(pomo_round_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(pomo_state_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_font(pomo_round_label, &lv_font_montserrat_14, 0);

    /*---- 时间标签 ----*/
    ui_PomodoroTimeLabel = lv_label_create(ui_PomodoroPage);//创建一个结构体将标签放在页面上，赋值给ui_PomodoroTimeLabel
    // lv_label_set_text(ui_PomodoroTimeLabel, "25:00");
    // lv_obj_set_style_text_font(ui_PomodoroTimeLabel, &lv_font_montserrat_48, 0);
    lv_obj_align(ui_PomodoroTimeLabel, LV_ALIGN_CENTER, 0, -40);//通过ui_PomodoroTimeLabel指针，lv_obj_align函数将标签居中对齐，偏移量为0，-40
    lv_obj_set_style_text_color(ui_PomodoroTimeLabel, lv_color_hex( 0x3E5C34 ), 0 );//设置标签的文字颜色为自定义颜色
    lv_obj_set_style_text_font(ui_PomodoroTimeLabel, &ui_font_Cuyuan48, 0 );//设置字体为自定义字体

    /*---- 开始/暂停按钮 ----*/
    ui_PomodoroStartBtn = lv_btn_create(ui_PomodoroPage);
    lv_obj_set_size(ui_PomodoroStartBtn, 60, 60);                    /* 100x50 改正方形 */
    lv_obj_align(ui_PomodoroStartBtn, LV_ALIGN_CENTER, -60, 100);
    lv_obj_set_style_radius(ui_PomodoroStartBtn, 30, 0);             /* 60的一半 → 正圆 */
    lv_obj_set_style_bg_color(ui_PomodoroStartBtn, lv_color_hex(0x4F7942), 0);  /* 深绿，配设计图 */
    ui_PomodoroStartLabel = lv_label_create(ui_PomodoroStartBtn);
    lv_label_set_text(ui_PomodoroStartLabel, running ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);        /* 播放三角图标 */
    lv_obj_center(ui_PomodoroStartLabel);

    /*---- 重置按钮 ----*/
    ui_PomodoroResetBtn = lv_btn_create(ui_PomodoroPage);
    lv_obj_set_size(ui_PomodoroResetBtn, 60, 60);
    lv_obj_align(ui_PomodoroResetBtn, LV_ALIGN_CENTER, 60, 100);
    lv_obj_set_style_radius(ui_PomodoroResetBtn, 30, 0);/* 60的一半 → 正圆 */
    lv_obj_set_style_bg_color(ui_PomodoroResetBtn, lv_color_hex(0x4F7942), 0);  /* 深绿，配设计图 */
    ui_PomodoroResetLabel = lv_label_create(ui_PomodoroResetBtn);
    lv_label_set_text(ui_PomodoroResetLabel,LV_SYMBOL_REFRESH);
    lv_obj_center(ui_PomodoroResetLabel);

    //设置番茄种齿轮按钮，可以让用户进入设置页面，有两种方式，第一种是点击齿轮，第二种是结束番茄定时后在结束页面选择以当前的设置开始新一轮，或者是进入设置页面修改设置
    /*---- 齿轮按钮：常驻设置入口 ----*/
    ui_PomodoroSetupBtn = lv_btn_create(ui_PomodoroPage);
    lv_obj_set_size(ui_PomodoroSetupBtn, 33, 33);
    lv_obj_align(ui_PomodoroSetupBtn, LV_ALIGN_TOP_RIGHT, -15, 15);   /* 右上角 */
    lv_obj_set_style_radius(ui_PomodoroSetupBtn, 20, 0);              /* 正圆 */
    lv_obj_set_style_bg_color(ui_PomodoroSetupBtn, lv_color_hex(0x4F7942), 0);
    lv_obj_t * setup_label = lv_label_create(ui_PomodoroSetupBtn);
    lv_label_set_text(setup_label, LV_SYMBOL_SETTINGS);                /* 齿轮图标 */
    lv_obj_center(setup_label);


    /*---- 倒计时初始化 ----*/
    if(!pomo_initialized)
    {
        remain_ms = p_ready_min * 60 * 1000;
        running = false;
        state = POMODORO_READY;
        work_done_cnt = 0;
        pomo_initialized = true;
    }

    pomodoro_update_label();
    pomodoro_update_arc();
    pomodoro_update_state_label();
    pomodoro_update_round_label();

    /*---- 注册事件回调 + 创建定时器 ----*/
    lv_obj_add_event_cb(ui_PomodoroStartBtn, start_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_PomodoroResetBtn, reset_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_PomodoroSetupBtn, setup_btn_cb, LV_EVENT_CLICKED, NULL);

    if (pomodoro_timer == NULL ) 
    { 
        pomodoro_timer = lv_timer_create(pomodoro_timer_cb, 100, NULL);
    }
    
    page_active = true;
}

//////////////////// SCREEN deinit ////////////////////
void ui_PomodoroPage_screen_deinit(void)
{
    page_active = false;
}