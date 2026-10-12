#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_PomodoroSetPage.h"
#include "../Inc/ui_PomodoroPage.h"   /* 用 p_work_min 等全局配置 */
#include <stdio.h>

/* 控件指针 */
lv_obj_t * ui_PomodoroSetPage;
static lv_obj_t * val_work;
static lv_obj_t * val_break;
static lv_obj_t * val_rounds;

/* Page 注册 */
Page_t Page_PomodoroSet = {
    ui_PomodoroSetPage_screen_init,
    ui_PomodoroSetPage_screen_deinit,
    &ui_PomodoroSetPage
};

/* ---- 边界保护 ---- */
#define WORK_MIN_MIN   1
#define WORK_MIN_MAX    60
#define BREAK_MIN_MIN   0
#define BREAK_MIN_MAX   10
#define ROUNDS_MIN      1
#define ROUNDS_MAX      10

/* ---- 辅助：刷新数值标签 ---- */
static void refresh_value(lv_obj_t * label, uint16_t val)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%u", val);
    lv_label_set_text(label, buf);
}

/* ---- 六个 +/- 回调（带边界保护） ---- */
static void work_dec_cb(lv_event_t * e)
{
    if(p_work_min > WORK_MIN_MIN) p_work_min--;
    refresh_value(val_work, p_work_min);
}
static void work_inc_cb(lv_event_t * e)
{
    if(p_work_min < WORK_MIN_MAX) p_work_min++;
    refresh_value(val_work, p_work_min);
}
static void break_dec_cb(lv_event_t * e)
{
    if(p_break_min > BREAK_MIN_MIN) p_break_min--;
    refresh_value(val_break, p_break_min);
}
static void break_inc_cb(lv_event_t * e)
{
    if(p_break_min < BREAK_MIN_MAX) p_break_min++;
    refresh_value(val_break, p_break_min);
}
static void rounds_dec_cb(lv_event_t * e)
{
    if(p_rounds > ROUNDS_MIN) p_rounds--;
    refresh_value(val_rounds, p_rounds);
}
static void rounds_inc_cb(lv_event_t * e)
{
    if(p_rounds < ROUNDS_MAX) p_rounds++;
    refresh_value(val_rounds, p_rounds);
}

/* ---- Start 按钮：进入计时页 ---- */
static void start_btn_cb(lv_event_t * e)
{
    Page_Replace_Dir(&Page_Pomodoro, LV_SCR_LOAD_ANIM_FADE_ON);
    pomosetpageback = false;// 设置为false，表示不可以从home回到设置页面

}

/* ---- 辅助：创建一行设置项（演示函数指针当参数） ---- */
static void make_row(lv_obj_t * parent, const char * name, lv_coord_t y,
                     lv_event_cb_t dec_cb, lv_event_cb_t inc_cb,
                     lv_obj_t ** val_label, uint16_t val)
{
    /* 名称 */
    lv_obj_t * nl = lv_label_create(parent);
    lv_label_set_text(nl, name);
    lv_obj_align(nl, LV_ALIGN_TOP_LEFT, 5, y);

    /* - 按钮 */
    lv_obj_t * mb = lv_btn_create(parent);
    lv_obj_set_size(mb, 35, 35);
    lv_obj_align(mb, LV_ALIGN_TOP_MID, -45, y);
    lv_obj_add_event_cb(mb, dec_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * ms = lv_label_create(mb);
    lv_label_set_text(ms, "-");
    lv_obj_center(ms);

    /* 数值标签 */
    *val_label = lv_label_create(parent);
    refresh_value(*val_label, val);
    lv_obj_align(*val_label, LV_ALIGN_TOP_MID, 0, y + 5);

    /* + 按钮 */
    lv_obj_t * pb = lv_btn_create(parent);
    lv_obj_set_size(pb, 35, 35);
    lv_obj_align(pb, LV_ALIGN_TOP_MID, 45, y);
    lv_obj_add_event_cb(pb, inc_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * ps = lv_label_create(pb);
    lv_label_set_text(ps, "+");
    lv_obj_center(ps);
}

/* ---- init / deinit ---- */
void ui_PomodoroSetPage_screen_init(void)
{
    ui_PomodoroSetPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_PomodoroSetPage, LV_OBJ_FLAG_SCROLLABLE);

    /* 标题 */
    lv_obj_t * title = lv_label_create(ui_PomodoroSetPage);
    lv_label_set_text(title, "Pomodoro Setup");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 150);

    /* 三行设置项 */
    make_row(ui_PomodoroSetPage, "Work",    0, work_dec_cb,   work_inc_cb,   &val_work,   p_work_min);
    make_row(ui_PomodoroSetPage, "Break",   40, break_dec_cb,  break_inc_cb,  &val_break,  p_break_min);
    make_row(ui_PomodoroSetPage, "Rounds",  80, rounds_dec_cb, rounds_inc_cb, &val_rounds, p_rounds);

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