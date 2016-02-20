#include "Program.h"
#ifndef _EMPTYPROGRAM_H
#define _EMPTYPROGRAM_H
class EmptyProgram : public Program {
public:
    EmptyProgram() : Program() {
    }

    void process() {
        while (!terminate)
        {
            wait_ms(100);
        }

    }

    void parsePad(int pad_gpio) {
    }

    void updateScreen() {
    }

    string name() {
        return "Empty";
    }

};
#endif