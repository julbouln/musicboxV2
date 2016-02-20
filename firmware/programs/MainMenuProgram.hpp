#ifndef _MAINMENUPROGRAM_H
#define _MAINMENUPROGRAM_H
// main menu program

class MainMenuProgram : public Program {
    int currentProgram;
public:
    MainMenuProgram() : Program() {
        currentProgram = 1;
    }

    void process() {
        while (!terminate)
        {
            wait_ms(100);
        }
    }

    void parsePad(int pad_gpio) {
        if (pad_gpio == PAD_TOP)
        {
            if (currentProgram > 1)
                currentProgram--;
        }

        if (pad_gpio == PAD_BOTTOM)
        {
            if (currentProgram < programs.size() - 1)
                currentProgram++;
        }

        if (pad_gpio == PAD_SELECT)
        {
            program->terminate = true;
            nextProgram =  programs[currentProgram];
        }

    }

    void updateScreen() {
        /*
                for(int i = 127;i<256;i++) {
//        for(int i = 2;i<128;i++) {
        gfx_putc(i);
            if(i % 20 == 0)
            gfx_printf("\r\n");
        }
        */
        for (int i = 1; i < programs.size(); i++) {
            if (i == currentProgram)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("%s\r\n", programs[i].c_str());
        }
        gfx_setTextColor(OLED_WHITE, OLED_BLACK);

    }

    string name() {
        return "Main menu";
    }

};

#endif