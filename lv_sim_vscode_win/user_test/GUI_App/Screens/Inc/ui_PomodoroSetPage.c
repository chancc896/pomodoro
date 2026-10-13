#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_PomodoroSetPage.h"
#include "../Inc/ui_PomodoroPage.h"   /* 用 p_work_min 等全局配置 */
#include <stdio.h>
#include <stdlib.h>   /* atoi */
/* 控件指针 */
static lv_obj_t * roller_work;
static lv_obj_t * roller_break; 
static lv_obj_t * roller_rounds; 
static lv_obj_t * roller_ready;

/* Page 注册 */
lv_obj_t * ui_PomodoroSetPage;
Page_t Page_PomodoroSet = {
    ui_PomodoroSetPage_screen_init,
    ui_PomodoroSetPage_screen_deinit,
    &ui_PomodoroSetPage
};

/* 选项字符串 */
static const char * opts_work   = "5\n10\n15\n20\n25\n30\n35\n40\n45\n50\n55\n60";
static const char * opts_break  = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10";
static const char * opts_rounds = "1\n2\n3\n4\n5\n6";
static const char * opts_ready  = "0\n1\n2\n3\n4\n5";

/* 读取选中值并更新全局变量 */
static void roller_work_cb(lv_event_t * e)
{
    char buf[8];
    lv_roller_get_selected_str(roller_work, buf, sizeof(buf));
    p_work_min = (uint16_t)atoi(buf);
}

static void roller_break_cb(lv_event_t * e)
{
    char buf[8];
    lv_roller_get_selected_str(roller_break, buf, sizeof(buf));
    p_break_min = (uint16_t)atoi(buf);
}

static void roller_rounds_cb(lv_event_t * e)
{
    char buf[8];
    lv_roller_get_selected_str(roller_rounds, buf, sizeof(buf));
    p_rounds = (uint8_t)atoi(buf);
}

static void roller_ready_cb(lv_event_t * e)
{
    char buf[8];
    lv_roller_get_selected_str(roller_ready, buf, sizeof(buf));
    p_ready_min = (uint8_t)atoi(buf);
}

/* ---- Start 按钮：进入计时页 ---- */
static void start_btn_cb(lv_event_t * e)
{
    Page_Replace_Dir(&Page_Pomodoro, LV_SCR_LOAD_ANIM_FADE_ON);
    pomosetpageback = false;// 设置为false，表示不可以从home回到设置页面

}

/* ---- init / deinit ---- */
void ui_PomodoroSetPage_screen_init(void)
{
    ui_PomodoroSetPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_PomodoroSetPage, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题 */
    lv_obj_t * title = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(title, "Pomodoro Setup");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 107);

    /*---- Work 轮盘 ----*/
    roller_work = lv_roller_create(ui_PomodoroSetPage);
    lv_roller_set_options(roller_work, opts_work, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_work, 3);
    lv_obj_set_size(roller_work, 60, 90);
    lv_obj_align(roller_work, LV_ALIGN_TOP_LEFT, 15, 10);
    lv_obj_add_event_cb(roller_work, roller_work_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /* 选中行：白色不透明 */
    lv_obj_set_style_text_color(roller_work, lv_color_hex(0xFFFFFF), LV_PART_SELECTED);
    lv_obj_set_style_text_opa(roller_work, LV_OPA_COVER, LV_PART_SELECTED);
    /* 未选中行：灰色半透明 */
    lv_obj_set_style_text_color(roller_work, lv_color_hex(0x8E8E93), LV_PART_MAIN);
    lv_obj_set_style_text_opa(roller_work, LV_OPA_60, LV_PART_MAIN);

    /*---- Break 轮盘 ----*/
    roller_break = lv_roller_create(ui_PomodoroSetPage);
    lv_roller_set_options(roller_break, opts_break, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_break, 3);
    lv_obj_set_size(roller_break, 60, 90);
    lv_obj_align(roller_break, LV_ALIGN_TOP_RIGHT, -15, 10);
    lv_obj_add_event_cb(roller_break, roller_break_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /* 选中行：白色不透明 */
    lv_obj_set_style_text_color(roller_break, lv_color_hex(0xFFFFFF), LV_PART_SELECTED);
    lv_obj_set_style_text_opa(roller_break, LV_OPA_COVER, LV_PART_SELECTED);
    /* 未选中行：灰色半透明 */
    lv_obj_set_style_text_color(roller_break, lv_color_hex(0x8E8E93), LV_PART_MAIN);
    lv_obj_set_style_text_opa(roller_break, LV_OPA_60, LV_PART_MAIN);

    /*---- Rounds 轮盘 ----*/
    roller_rounds = lv_roller_create(ui_PomodoroSetPage);
    lv_roller_set_options(roller_rounds, opts_rounds, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_rounds, 3);
    lv_obj_set_size(roller_rounds, 60, 90);
    lv_obj_align(roller_rounds, LV_ALIGN_BOTTOM_LEFT, 15, -70);
    lv_obj_add_event_cb(roller_rounds, roller_rounds_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /* 选中行：白色不透明 */
    lv_obj_set_style_text_color(roller_rounds, lv_color_hex(0xFFFFFF), LV_PART_SELECTED);
    lv_obj_set_style_text_opa(roller_rounds, LV_OPA_COVER, LV_PART_SELECTED);
    /* 未选中行：灰色半透明 */
    lv_obj_set_style_text_color(roller_rounds, lv_color_hex(0x8E8E93), LV_PART_MAIN);
    lv_obj_set_style_text_opa(roller_rounds, LV_OPA_60, LV_PART_MAIN);

    /*---- Ready 轮盘 ----*/
    roller_ready = lv_roller_create(ui_PomodoroSetPage);
    lv_roller_set_options(roller_ready, opts_ready, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_ready, 3);
    lv_obj_set_size(roller_ready, 60, 90);
    lv_obj_align(roller_ready, LV_ALIGN_BOTTOM_RIGHT, -15, -70);
    lv_obj_add_event_cb(roller_ready, roller_ready_cb, LV_EVENT_VALUE_CHANGED, NULL);
    /* 选中行：白色不透明 */
    lv_obj_set_style_text_color(roller_ready, lv_color_hex(0xFFFFFF), LV_PART_SELECTED);
    lv_obj_set_style_text_opa(roller_ready, LV_OPA_COVER, LV_PART_SELECTED);
    /* 未选中行：灰色半透明 */
    lv_obj_set_style_text_color(roller_ready, lv_color_hex(0x8E8E93), LV_PART_MAIN);
    lv_obj_set_style_text_opa(roller_ready, LV_OPA_60, LV_PART_MAIN);

    /*---- 选中区域：透明背景 + 上下横线 ----*/ 
    lv_obj_set_style_bg_opa(roller_work, LV_OPA_TRANSP, LV_PART_SELECTED); /* 背景透明 */ 
    lv_obj_set_style_border_color(roller_work, lv_color_hex( 0x4F7942 ), LV_PART_SELECTED); /* 线颜色深绿 */ 
    lv_obj_set_style_border_width(roller_work, 2 , LV_PART_SELECTED); /* 线宽 2px */
    lv_obj_set_style_border_side(roller_work, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_SELECTED); /* 只上下 */
    /*---- 轮盘整体背景：去掉 ----*/ 
    lv_obj_set_style_bg_opa(roller_work, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller_work, 0 , LV_PART_MAIN);

    lv_obj_set_style_bg_opa(roller_ready, LV_OPA_TRANSP, LV_PART_SELECTED); /* 背景透明 */ 
    lv_obj_set_style_border_color(roller_ready, lv_color_hex( 0x4F7942 ), LV_PART_SELECTED); /* 线颜色深绿 */ 
    lv_obj_set_style_border_width(roller_ready, 2 , LV_PART_SELECTED); /* 线宽 2px */
    lv_obj_set_style_border_side(roller_ready, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_SELECTED); /* 只上下 */
    /*---- 轮盘整体背景：去掉 ----*/ 
    lv_obj_set_style_bg_opa(roller_ready, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(roller_ready, 0 , LV_PART_MAIN);

    lv_obj_set_style_bg_opa(roller_rounds, LV_OPA_TRANSP, LV_PART_SELECTED); /* 背景透明 */ 
    lv_obj_set_style_border_color(roller_rounds, lv_color_hex( 0x4F7942 ), LV_PART_SELECTED); /* 线颜色深绿 */ 
    lv_obj_set_style_border_width(roller_rounds, 2 , LV_PART_SELECTED); /* 线宽 2px */
    lv_obj_set_style_border_side(roller_rounds, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_SELECTED); /* 只上下 */
    lv_obj_set_style_bg_opa(roller_rounds, LV_OPA_TRANSP, LV_PART_MAIN); /* 背景透明 */
    lv_obj_set_style_border_width(roller_rounds, 0 , LV_PART_MAIN);

    lv_obj_set_style_bg_opa(roller_break, LV_OPA_TRANSP, LV_PART_SELECTED); /* 背景透明 */ 
    lv_obj_set_style_border_color(roller_break, lv_color_hex( 0x4F7942 ), LV_PART_SELECTED); /* 线颜色深绿 */ 
    lv_obj_set_style_border_width(roller_break, 2 , LV_PART_SELECTED); /* 线宽 2px */
    lv_obj_set_style_border_side(roller_break, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM, LV_PART_SELECTED); /* 只上下 */
    lv_obj_set_style_bg_opa(roller_break, LV_OPA_TRANSP, LV_PART_MAIN); /* 背景透明 */
    lv_obj_set_style_border_width(roller_break, 0 , LV_PART_MAIN);


    //给轮盘设置名字标签
    lv_obj_t * lbl_work = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(lbl_work, "Work");
    lv_obj_align(lbl_work, LV_ALIGN_TOP_LEFT, 24, 62);

    lv_obj_t * lbl_break = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(lbl_break, "Break");
    lv_obj_align(lbl_break, LV_ALIGN_TOP_RIGHT, -25, 62);

    lv_obj_t * lbl_rounds = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(lbl_rounds, "Rounds");
    lv_obj_align(lbl_rounds, LV_ALIGN_BOTTOM_LEFT, 18, -91);

    lv_obj_t * lbl_ready = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(lbl_ready, "Ready");
    lv_obj_align(lbl_ready, LV_ALIGN_BOTTOM_RIGHT, -20, -91);


    /* Start 按钮 */
    lv_obj_t * start_btn = lv_btn_create(ui_PomodoroSetPage);
    lv_obj_set_size(start_btn, 100, 40);
    lv_obj_align(start_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(start_btn, start_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * sl = lv_label_create(start_btn);
    lv_label_set_text(sl, "Start");
    lv_obj_center(sl);

    
}

void ui_PomodoroSetPage_screen_deinit(void)
{
    /* 控件随页面销毁自动释放，无额外资源需清理 */
}