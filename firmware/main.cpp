#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "system_setup.h"
#include "systick.h"

#include "usbmidi.h"
#include "ssd1306.h"
#include "interface.h"
#include "gfx.h"
#include "battery.h"

#include "atom.h"
#include "atomport-private.h"
#include "atomtimer.h"
#include "atommutex.h"

#include "CcgMusic.h"

#include "Program.h"

#define IDLE_STACK_SIZE_BYTES 512
#define MAIN_STACK_SIZE_BYTES 1024

#define STACK_SIZE_BYTES 1024

/* Application threads' TCBs */
static ATOM_TCB main_tcb;

static ATOM_TCB interface_tcb;
static ATOM_TCB usbmidi_tcb;
static ATOM_TCB midipool_tcb;

/* Main thread's stack area */
static uint8_t main_thread_stack[MAIN_STACK_SIZE_BYTES];

/* Idle thread's stack area */
static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];


static uint8_t interface_thread_stack[STACK_SIZE_BYTES];
static uint8_t usbmidi_thread_stack[STACK_SIZE_BYTES];
static uint8_t midipool_thread_stack[STACK_SIZE_BYTES];


static ATOM_MUTEX screenMutex;


void renderBattery(int x, int y, int percent)
{
  gfx_drawRect(x, y, 10, 6, OLED_WHITE);
  gfx_fillRect(x + 11, y + 2, 2, 2, OLED_WHITE);
  gfx_fillRect(x, y, (percent) / 10, 6, OLED_WHITE);
}


/* Forward declarations */
static void main_thread_func (uint32_t data);

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

#ifdef __cplusplus
extern "C" {
#endif


void usbmidi_event(char *msg, int len) {
    program->parseMidi(msg,len);
}

// FIXME : does not work
void exti0_isr(void)
{
  exti_reset_request(EXTI0);
//    nvic_disable_irq(NVIC_EXTI0_IRQ);
/*        atomMutexGet(&screenMutex, 0);

              uint8_t pad_gpio = pad_read();
              program->parsePad(pad_gpio);
  //nvic_enable_irq(NVIC_EXTI0_IRQ);
        atomMutexPut(&screenMutex);
*/
}

#ifdef __cplusplus
}
#endif


void interface_task(uint32_t data __maybe_unused) {
    while (1) {
        atomMutexGet(&screenMutex, 0);

        /*
                if (low_battery) {
                    midiOut.shutdown();
                    shdnAmp = 0;
                }
        */
        gfx_fillScreen(OLED_BLACK);
        gfx_setCursor(0, 7);

        if (!program->terminate) {
            /*
            if (pad_pressed()) {
//                nvic_disable_irq(NVIC_EXTI0_IRQ);
                uint8_t pad_gpio = pad_read();

                // only call parsePad once per press
                if (pad_pressed() == 1)
                    program->parsePad(pad_gpio);

                pad_clear();
//                nvic_enable_irq(NVIC_EXTI0_IRQ);
            }
            */
              uint8_t pad_gpio = pad_read();
              program->parsePad(pad_gpio);

        }



        if (!program->terminate) {
            gfx_printf("%s\r\n", program->name().c_str());
            program->updateScreen();
        }

        int percent=battery_percent();
//        gfx_printf("%d %d\n",battery_sense_read(),percent);
        renderBattery(128 - 12, 0, percent);

//        if (midiUsb.suspended) {
//            screen.drawXBitmap(128 - 27, 14, usb_icon_bits, 27, 12, WHITE);
//        }Size());

        ssd1306_display();

        atomMutexPut(&screenMutex);

        wait_ms(200);
    }
}


void usbmidi_task(uint32_t data __maybe_unused) {
    while (1) {
        usbmidi_poll();
    }
}

void midipool_task(uint32_t data __maybe_unused) {
    midiWriter.setPause(false);
    midiWriter.setStop(false);

    while (1) {
        midiWriter.launch();
    }
}

static void main_thread_func (uint32_t data __maybe_unused)
{
    int8_t status = 0;

    initPrograms();
    program = programsMap["Main menu"]();

    if (status == ATOM_OK)
    {
        status = atomThreadCreate(&usbmidi_tcb,
                                  32, usbmidi_task, 0,
                                  &usbmidi_thread_stack[0],
                                  STACK_SIZE_BYTES,
                                  FALSE);
    }

    if (status == ATOM_OK)
    {
        status = atomThreadCreate(&interface_tcb,
                                  32, interface_task, 0,
                                  &interface_thread_stack[0],
                                  STACK_SIZE_BYTES,
                                  FALSE);
    }

    if (status == ATOM_OK)
    {
        status = atomThreadCreate(&midipool_tcb,
                                  24, midipool_task, 0,
                                  &midipool_thread_stack[0],
                                  STACK_SIZE_BYTES,
                                  FALSE);
    }

    while (1)
    {

        atomMutexGet(&screenMutex, 0);

        delete program;
        program = programsMap[nextProgram]();

        atomMutexPut(&screenMutex);

        program->terminate = false;
        program->process();
        wait_ms(500);
    }

}

int main(void)
{

    system_setup();

    atomMutexCreate (&screenMutex);

    int8_t status;
    status = atomOSInit(&idle_thread_stack[0], IDLE_STACK_SIZE_BYTES, FALSE);
    if (status == ATOM_OK)
    {
        /* Create an application thread */
        status = atomThreadCreate(&main_tcb,
                                  16, main_thread_func, 0,
                                  &main_thread_stack[0],
                                  MAIN_STACK_SIZE_BYTES,
                                  TRUE);
        if (status == ATOM_OK)
        {
            /**
             * First application thread successfully created. It is
             * now possible to start the OS. Execution will not return
             * from atomOSStart(), which will restore the context of
             * our application thread and start executing it.
             *
             * Note that interrupts are still disabled at this point.
             * They will be enabled as we restore and execute our first
             * thread in archFirstThreadRestore().
             */
            atomOSStart();
        }
    }

    while (1) {
    }

    return 0;
}