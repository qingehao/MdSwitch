#ifndef MD_BUTTON_H
#define MD_BUTTON_H

#include "stdint.h"

typedef void (*md_button_event_callback)(void*);

typedef enum
{
    MD_BTN_PRESS_DOWN = 0,
    MD_BTN_CLICK,
    MD_BTN_DOUBLE_CLICK,
    MD_BTN_REPEAT_CLICK,
    MD_BTN_SHORT_PRESS_START,
    MD_BTN_SHORT_PRESS_UP,
    MD_BTN_LONG_PRESS_START,
    MD_BTN_LONG_PRESS_UP,
    MD_BTN_EVT_MAX,
    MD_BTN_EVT_NONE,
} md_button_event_t;

typedef struct md_button
{
    struct md_button* next;

    uint8_t  (*usr_button_read)(void *);
    md_button_event_callback  cb;

    uint16_t scan_cnt;
    uint16_t click_cnt;
    uint16_t max_multiple_clicks_interval;

    uint16_t debounce_tick;
    uint16_t short_press_duration;
    uint16_t long_press_duration;

    uint8_t id;
    uint8_t active_level : 1;
    uint8_t event        : 4;
    uint8_t status       : 3;
} md_button_t;

#ifdef __cplusplus
extern "C" {
#endif

void md_button_set_scan_freq(uint32_t freq);
uint32_t md_button_ms2cnt(uint32_t ms);
int32_t md_button_add(md_button_t *btn);
md_button_event_t md_button_get_event(md_button_t* btn);
uint32_t md_button_scan(void);

#ifdef __cplusplus
}
#endif  
#endif
