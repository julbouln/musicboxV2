#include "Program.h"
#ifndef _ABOUTPROGRAM_H
#define _ABOUTPROGRAM_H
class AboutProgram : public Program {
public:
    AboutProgram() : Program() {
    }

    void process() {
        while (!terminate)
        {
            wait_ms(100);
        }

    }

    void parsePad(int pad_gpio) {
        if (pad_gpio == PAD_SELECT) {
            stop();
            shutdownSound();
            program->terminate = true;
            nextProgram = "Main menu";
        }
    }

    void updateScreen() {
        gfx_printf("La boite a musique\r\n");
        gfx_printf("Version %s\r\n",PROGRAM_VERSION);
        gfx_setTextColor(OLED_BLACK, OLED_WHITE);
        gfx_printf("Quit\r\n");
        gfx_setTextColor(OLED_WHITE, OLED_BLACK);
    }

    string name() {
        return "About";
    }

};
#endif