#include "md_button.h"

#ifndef NULL
#define NULL 0
#endif

#define EVENT_SET_AND_EXEC_CB(btn, evt)                                        \
    do                                                                         \
    {                                                                          \
        btn->event = evt;                                                      \
        if(btn->cb)                                                            \
            btn->cb((md_button_t*)btn);                                      \
    } while(0)

#define BTN_IS_PRESSED(id) (g_btn_status & (1 << id))

enum _BTN_STAGE
{
    _BTN_STAGE_IDLE    = 0,
    _BTN_STAGE_DOWN    = 1,
    _BTN_STAGE_MULTIPLE_CLICK = 2
};

typedef uint32_t _btn_type_t;

static uint32_t button_scan_freq = 100;

static  md_button_t *btn_head = NULL;
static  uint8_t button_cnt = 0;
static  _btn_type_t g_logic_level = (_btn_type_t)0;
static  _btn_type_t g_btn_status = (_btn_type_t)0;

/**
 * @brief add a button
 * 
 * @param btn: button structure instance
 * @return number of keys that have been added -1: already exist
*/
int32_t md_button_add(md_button_t *btn)
{
    md_button_t *curr = btn_head;
    
    if (!btn || (button_cnt > sizeof(_btn_type_t) * 8))
    {
        return -1;
    }

    while (curr)
    {
        if(curr == btn)
        {
            return -1;
        }
        curr = curr->next;
    }

    btn->next = btn_head;
    btn->status = _BTN_STAGE_IDLE;
    btn->event = MD_BTN_EVT_NONE;
    btn->scan_cnt = 0;
    btn->click_cnt = 0;
    btn_head = btn;

    btn->id = button_cnt;
    g_logic_level |= (btn->active_level << btn->id);
    button_cnt ++;

    return button_cnt;
}

/**
 * @brief read all key values by three lines
 * 
 * @param void
 * @return none
*/
static void _button_read(void)
{
    md_button_t* btn = NULL;

    _btn_type_t raw_data = 0;

    for(btn = btn_head; (btn != NULL) && (btn->usr_button_read != NULL); btn = btn->next)
    {
        raw_data = raw_data | ((btn->usr_button_read)(btn) << btn->id);
    }

    g_btn_status = (~raw_data) ^ g_logic_level;
}

/**
 * @brief porcess button events and will exec the cb
 * 
 * @param void
 * @return activated button count
*/
static uint32_t _button_process(void)
{
    uint32_t id = 0;
    uint32_t active_btn_cnt = 0;
    md_button_t* btn = NULL;
    
    for (btn = btn_head; btn != NULL; btn = btn->next)
    {
        id = btn->id;
        if (btn->status > _BTN_STAGE_IDLE)
        {
            btn->scan_cnt ++;
            if (btn->scan_cnt >= ((1 << (sizeof(btn->scan_cnt) * 8)) - 1))
            {
                btn->scan_cnt = btn->long_press_duration;
            }
        }

        switch (btn->status)
        {
            case _BTN_STAGE_IDLE:
            {
                if (BTN_IS_PRESSED(id))
                {
                    btn->scan_cnt = 0;
                    btn->click_cnt = 0;
                    EVENT_SET_AND_EXEC_CB(btn, MD_BTN_PRESS_DOWN);
                    btn->status = _BTN_STAGE_DOWN;
                }
                else
                {
                    btn->event = MD_BTN_EVT_NONE;
                }
            } break; /* _BTN_STAGE_IDLE */

            case _BTN_STAGE_DOWN:
            {
                if (BTN_IS_PRESSED(id))
                {
                    if (btn->click_cnt > 0)
                    {
                        /* 连击结束 */
                        if (btn->scan_cnt > btn->max_multiple_clicks_interval)
                        {
                            EVENT_SET_AND_EXEC_CB(btn, 
                                btn->click_cnt < MD_BTN_REPEAT_CLICK ? 
                                    btn->click_cnt :
                                    MD_BTN_REPEAT_CLICK);

                            btn->status = _BTN_STAGE_DOWN;
                            btn->scan_cnt = 0;
                            btn->click_cnt = 0;
                        }
                    }
                    /* 长按判定 */
                    else if (btn->scan_cnt >= btn->long_press_duration)
                    {
                        if (btn->event != MD_BTN_LONG_PRESS_START)
                        {
                            EVENT_SET_AND_EXEC_CB(btn, MD_BTN_LONG_PRESS_START);
                        }
                    }
                    /* 短按判定 */
                    else if (btn->scan_cnt >= btn->short_press_duration)
                    {
                        if (btn->event != MD_BTN_SHORT_PRESS_START)
                        {
                            EVENT_SET_AND_EXEC_CB(btn, MD_BTN_SHORT_PRESS_START);
                        }
                    }
                }
                else
                {
                    /* 是否是长按弹起 */
                    if (btn->scan_cnt >= btn->long_press_duration)
                    {
                        EVENT_SET_AND_EXEC_CB(btn, MD_BTN_LONG_PRESS_UP);
                        btn->status = _BTN_STAGE_IDLE;
                    }
                    /* 是否是短按弹起 */
                    else if (btn->scan_cnt >= btn->short_press_duration)
                    {
                        EVENT_SET_AND_EXEC_CB(btn, MD_BTN_SHORT_PRESS_UP);
                        btn->status = _BTN_STAGE_IDLE;
                    }
                    /* 连击处理 */
                    else
                    {
                        btn->status = _BTN_STAGE_MULTIPLE_CLICK;
                        btn->click_cnt ++;
                    }
                }
            } break; /* _BTN_STAGE_DOWN */

            case _BTN_STAGE_MULTIPLE_CLICK:
            {
                /* 又按下了一次 */
                if (BTN_IS_PRESSED(id))
                {
                    btn->status = _BTN_STAGE_DOWN;
                    btn->scan_cnt = 0;
                }
                else
                {
                    /* 判断连击是否结束 */
                    if (btn->scan_cnt > btn->max_multiple_clicks_interval)
                    {
                        EVENT_SET_AND_EXEC_CB(btn, 
                            btn->click_cnt < MD_BTN_REPEAT_CLICK ? 
                                btn->click_cnt :
                                MD_BTN_REPEAT_CLICK);

                        btn->status = _BTN_STAGE_IDLE;
                    }
                }
            } break; /* _BTN_STAGE_MULTIPLE_CLICK */
        }
        
        if (btn->status > _BTN_STAGE_IDLE)
        {
            active_btn_cnt ++;
        }
    }

    return active_btn_cnt;
}

/**
 * @brief set button scan freq(Hz)
 * 
 * @param freq: scan frequence, uint:Hz
 * @return none
*/
void md_button_set_scan_freq(uint32_t freq)
{
    button_scan_freq = freq;
}

/**
 * @brief convert ms to scan cnt
 * 
 * @param ms: ms
 * @return scan cnt
*/
uint32_t md_button_ms2cnt(uint32_t ms)
{
    uint32_t scan_cnt = ms / (1000 / button_scan_freq);

    return scan_cnt;
}

/**
 * @brief get target button event
 * 
 * @param btn: button structure instance
 * @return enum md_button_event_t
*/
md_button_event_t md_button_get_event(md_button_t* btn)
{
    return (md_button_event_t)(btn->event);
}

/**
 * @brief periodic call
 * 
 * @param void
 * @return activated button count
*/
uint32_t md_button_scan(void)
{
    _button_read();
    return _button_process();
}
