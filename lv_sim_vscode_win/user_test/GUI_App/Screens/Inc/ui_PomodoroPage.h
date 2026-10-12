#ifndef _UI_POMODOROPAGE_H
#define _UI_POMODOROPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_PomodoroPage;

extern Page_t Page_Pomodoro;

extern bool pomosetpageback;

extern uint16_t p_work_min; 
extern uint16_t p_break_min; 
extern uint8_t p_rounds;

void ui_PomodoroPage_screen_init(void);
void ui_PomodoroPage_screen_deinit(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif