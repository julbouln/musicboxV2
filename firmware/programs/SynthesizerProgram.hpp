#include "Program.h"
#ifndef _SYNTHESIZERPROGRAM_H
#define _SYNTHESIZERPROGRAM_H
class SynthesizerProgram : public Program {
public:
    SynthesizerProgram() : Program() {
    }

    void process() {
        activateSound();

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
        gfx_setTextColor(OLED_BLACK, OLED_WHITE);
        gfx_printf("Quit\r\n");
        gfx_setTextColor(OLED_WHITE, OLED_BLACK);
    }

    void parseMidi(char *msg, int len) {
        int i;
        for (i = 0; i < len; i++ ) {
            sam2695_write(msg[i]);
        }
    }

    string name() {
        return "Synthesizer";
    }

};
#endif