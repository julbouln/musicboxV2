#include "systick.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
volatile uint32_t system_millis;

/*void SysTick_Handler() {
}
*/
/* Called when systick fires */
void sys_tick_handler(void)
{
    system_millis++;
}

/* sleep for delay milliseconds */
void wait_ms(uint32_t delay)
{
    uint32_t wake = system_millis + delay;
    while (wake > system_millis);
}

/* Set up a timer to create 1mS ticks. */
void systick_setup(void)
{
    /* clock rate / 1000 to get 1mS interrupt rate */
    systick_set_reload(168000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}

#endif
void wait_ms(uint32_t delay)
{
    atomTimerDelay(delay);
}

void systick_setup(void) {
 systick_set_reload(168000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}


#ifdef __cplusplus
}
#endif
