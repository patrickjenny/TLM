/*
 * Global_Timer.h
 *
 * Created: 26.02.2017 21:09:44
 *  Author: paddy
 */ 


#ifndef GLOBAL_TIMER_H_
#define GLOBAL_TIMER_H_

#include <stdint.h>

void global_Timer_init(void);

void global_Timer_process(void);

// Timer
typedef uint32_t timer_var_t;
typedef timer_var_t * const timer_array_t[];
extern timer_array_t hal_timers;

#define TIMER_DISABLED (timer_var_t)-1
#define TIMER_MSEC(v) ((v)*1000ul)
#define TIMER_SEC(v) ((v)*1000000ul)
#define TIMER_MINUTE(v) ((v)*60000000ul)
#define TIMERS(...) timer_array_t hal_timers={__VA_ARGS__, 0};


#endif /* GLOBAL_TIMER_H_ */