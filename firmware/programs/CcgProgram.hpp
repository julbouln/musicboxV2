#include "Program.h"
#ifndef _CCGPROGRAM_H
#define _CCGPROGRAM_H

// Ccgmusic program

#define ARRANGEMENTS_COUNT 9
#define STRUCTURES_COUNT 2
#define TEMPO_COUNT 8

enum {
    SEL_MAIN_PLAYPAUSE,
    SEL_MAIN_STOP,
    SEL_MAIN_NEXT,
    SEL_MAIN_SETTINGS,
    SEL_MAIN_QUIT,
    SEL_MAIN_COUNT
};

enum {
    SEL_SETTINGS_ARRANGEMENT,
    SEL_SETTINGS_STRUCTURE,
    SEL_SETTINGS_TEMPO,
    SEL_SETTINGS_RETURN,
    SEL_SETTINGS_COUNT
};

enum {
    SUBMENU_MAIN,
    SUBMENU_SETTINGS,
    SUBMENU_ARRANGEMENT,
    SUBMENU_STRUCTURE,
    SUBMENU_TEMPO
};

class CcgProgram : public Program {
    uint8_t structureScript;
    uint8_t arrangementScript;
    uint8_t tempo;
    uint32_t seed;

    uint8_t submenu;

    int confStructure;
    int confArrangement;
    int confTempo;

    bool nextSeed;

    const string arrangements[ARRANGEMENTS_COUNT] {
        "Simple Arrangement",
//      "Piano Advanced Swinging Blues",
//      "Piano Advanced Boogie Woogie",
        "Piano Advanced Disco",
        "Piano Advanced Classical",
        "Simple Latin Style Arrangement",
        "Simple Dance Style Arrangement",
        "Simple Instrumental March Arrangement",
        "Simple Ballad Style Arrangement",
        "Simple Punk Rock Style Arrangement",
        "Random Electro Rock"
    };

    const string arrangements_s[ARRANGEMENTS_COUNT] {
        "Simple",
        "Piano Disco",
        "Piano Classic",
        "Latin",
        "Dance",
        "March",
        "Ballad",
        "Punk Rock",
        "Electro Rock"

    };

    const string structures[STRUCTURES_COUNT] = {
//      "Classical Structure Small",
        "Classical Structure Big",
        "Modern Song Structure"
//      "Random Structure"
    };

    const string structures_s[STRUCTURES_COUNT] = {
        "Classical",
        "Modern"
    };
    const int tempos[TEMPO_COUNT] = {60, 80, 100, 120, 160, 180, 200, 240};

    int currentSel;

public:
    CcgProgram() : Program() {
        structureScript = 0;
        arrangementScript = 2;
        tempo = 3;
        confStructure = -1; // random
        confArrangement = -1; // random
        confTempo = -1; // random
        setRandomSeed();
        isPlaying = true;
        nextSeed = true;
        currentSel = SEL_MAIN_PLAYPAUSE;
        submenu = SUBMENU_MAIN;
    }

    void process() {
        activateSound();

        currentSel = SEL_MAIN_PLAYPAUSE;

        play();
        while (!terminate) {
            if (isPlaying) {
                midiWriter.setStop(false);

                SongCreator *songCreator = new SongCreator();
                songCreator->createSong(seed, tempos[tempo], structures[structureScript], arrangements[arrangementScript], &midiWriter);

                while (midiWriter.getQueueSize() > 0) {
                    wait_ms(100);
                }
                delete songCreator;

                if (nextSeed)
                    setRandomSeed();

            } else {
                wait_ms(100);
            }
        }
    }

    void parsePad(int pad_gpio) {

        if (pad_gpio == PAD_PREVIOUS || pad_gpio == PAD_TOP) {
            if (currentSel > 0)
                currentSel--;
        }

        int sel_max = 1;
        switch (submenu) {
        case SUBMENU_MAIN:
            sel_max = SEL_MAIN_COUNT;
            break;
        case SUBMENU_SETTINGS:
            sel_max = SEL_SETTINGS_COUNT;
            break;
        case SUBMENU_ARRANGEMENT:
            sel_max = ARRANGEMENTS_COUNT + 2; // random + return
            break;
        case SUBMENU_STRUCTURE:
            sel_max = STRUCTURES_COUNT + 2; // random + return
            break;
        case SUBMENU_TEMPO:
            sel_max = TEMPO_COUNT + 2; // random + return
            break;
        }

        if (pad_gpio == PAD_NEXT || pad_gpio == PAD_BOTTOM) {
            if (currentSel < sel_max - 1)
                currentSel++;
        }

        // select
        if (pad_gpio == PAD_SELECT) {
            switch (submenu) {
            case SUBMENU_MAIN:
                switch (currentSel) {
                case SEL_MAIN_PLAYPAUSE:
                    if (isPlaying) {
                        pause();
                    }
                    else {
                        play();
                    }
                    break;
                case SEL_MAIN_STOP:
                    stopSong();
                    pause();
                    currentSel = SEL_MAIN_PLAYPAUSE;
                    break;
                case SEL_MAIN_NEXT:
                    nextSong();
                    break;
                case SEL_MAIN_SETTINGS:
                    currentSel = 0;
                    submenu = SUBMENU_SETTINGS;
                    break;
                case SEL_MAIN_QUIT:
                    stop();
                    shutdownSound();
                    program->terminate = true;
                    nextProgram = "Main menu";

                    break;
                }
                break;
            case SUBMENU_SETTINGS:
                switch (currentSel) {
                case SEL_SETTINGS_ARRANGEMENT:
                    currentSel = confArrangement + 1;
                    submenu = SUBMENU_ARRANGEMENT;
                    break;
                case SEL_SETTINGS_STRUCTURE:
                    currentSel = confStructure + 1;
                    submenu = SUBMENU_STRUCTURE;
                    break;
                case SEL_SETTINGS_TEMPO:
                    currentSel = confTempo + 1;
                    submenu = SUBMENU_TEMPO;
                    break;
                case SEL_SETTINGS_RETURN:
                    currentSel = SEL_MAIN_SETTINGS;
                    submenu = SUBMENU_MAIN;
                    break;
                }
                break;
            case SUBMENU_ARRANGEMENT:
                if (currentSel < ARRANGEMENTS_COUNT + 1) {
                    confArrangement = currentSel - 1;
                    currentSel = 0;
                    submenu = SUBMENU_MAIN;
                    nextSong();
                } else {

                    currentSel = SEL_SETTINGS_ARRANGEMENT;
                    submenu = SUBMENU_SETTINGS;
                }
                break;
            case SUBMENU_STRUCTURE:
                if (currentSel < STRUCTURES_COUNT + 1) {
                    confStructure = currentSel - 1;
                    currentSel = 0;
                    submenu = SUBMENU_MAIN;
                    nextSong();
                } else {

                    currentSel = SEL_SETTINGS_STRUCTURE;
                    submenu = SUBMENU_SETTINGS;
                }
                break;
            case SUBMENU_TEMPO:
                if (currentSel < TEMPO_COUNT + 1) {
                    confTempo = currentSel - 1;
                    currentSel = 0;
                    submenu = SUBMENU_MAIN;
                    nextSong();
                } else {

                    currentSel = SEL_SETTINGS_TEMPO;
                    submenu = SUBMENU_SETTINGS;
                }
                break;
            }
        }
    }

    void updateScreen() {
        int i;
        switch (submenu) {
        case SUBMENU_MAIN:
            gfx_printf("Seed %d\r\n", seed);

            gfx_printf("%s", arrangements_s[arrangementScript].c_str());
            if (confArrangement < 0)
                gfx_printf(" (random)");

            gfx_printf("\r\n");
            gfx_printf("%s", structures_s[structureScript].c_str());
            if (confStructure < 0)
                gfx_printf(" (random)");
            gfx_printf("\r\n");

            gfx_printf("Tempo %d", tempos[tempo]);
            if (confTempo < 0)
                gfx_printf(" (random)");
            gfx_printf("\r\n");


            if (currentSel == SEL_MAIN_PLAYPAUSE)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            if (isPlaying) {
                gfx_printf("Pause");
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);
                gfx_printf(" ");
            }
            else {
                gfx_printf("Play");
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);
                gfx_printf("  ");
            }


            if (currentSel == SEL_MAIN_STOP)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Stop");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf(" ");

            if (currentSel == SEL_MAIN_NEXT)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Next");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            if (currentSel == SEL_MAIN_SETTINGS)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Settings");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf(" ");

            if (currentSel == SEL_MAIN_QUIT)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Quit\r\n");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            break;
        case SUBMENU_SETTINGS:
            if (currentSel == SEL_SETTINGS_ARRANGEMENT)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Arrangement");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            if (currentSel == SEL_SETTINGS_STRUCTURE)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Structure");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            if (currentSel == SEL_SETTINGS_TEMPO)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Tempo");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");
            if (currentSel == SEL_SETTINGS_RETURN)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Return");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            break;
        case SUBMENU_ARRANGEMENT:
            i = 0;
            if (currentSel == 0)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Random");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");
            for (i = 1; i < ARRANGEMENTS_COUNT + 1; i++) {
                if (currentSel == i)
                    gfx_setTextColor(OLED_BLACK, OLED_WHITE);
                else
                    gfx_setTextColor(OLED_WHITE, OLED_BLACK);

                gfx_printf("%s", arrangements_s[i - 1].c_str());

                gfx_setTextColor(OLED_WHITE, OLED_BLACK);
                if (i % 2 == 0)
                    gfx_printf("\r\n");
                else
                    gfx_printf("  ");
            }

            if (currentSel == i)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Return");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            break;
        case SUBMENU_STRUCTURE:
            i = 0;
            if (currentSel == 0)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Random");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");
            for (i = 1; i < STRUCTURES_COUNT + 1; i++) {
                if (currentSel == i)
                    gfx_setTextColor(OLED_BLACK, OLED_WHITE);
                else
                    gfx_setTextColor(OLED_WHITE, OLED_BLACK);

                gfx_printf("%s", structures_s[i - 1].c_str());

                gfx_setTextColor(OLED_WHITE, OLED_BLACK);
                if (i % 2 == 0)
                    gfx_printf("\r\n");
                else
                    gfx_printf("  ");
            }

            if (currentSel == i)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Return");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            break;
        case SUBMENU_TEMPO:
            i = 0;
            if (currentSel == 0)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Random");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");
            for (i = 1; i < TEMPO_COUNT + 1; i++) {
                if (currentSel == i)
                    gfx_setTextColor(OLED_BLACK, OLED_WHITE);
                else
                    gfx_setTextColor(OLED_WHITE, OLED_BLACK);

                gfx_printf("%d", tempos[i - 1]);

                gfx_setTextColor(OLED_WHITE, OLED_BLACK);
                if (i % 2 == 0)
                    gfx_printf("\r\n");
                else
                    gfx_printf("  ");
            }

            if (currentSel == i)
                gfx_setTextColor(OLED_BLACK, OLED_WHITE);
            else
                gfx_setTextColor(OLED_WHITE, OLED_BLACK);

            gfx_printf("Return");

            gfx_setTextColor(OLED_WHITE, OLED_BLACK);
            gfx_printf("\r\n");

            break;
        }

    }

    string name() {
        return "Music generator";
    }

private:

    void nextSong()
    {
        midiWriter.setStop(true);
        nextSeed = true;
    }

    void stopSong() {
        midiWriter.setStop(true);
        nextSeed = false;
    }

    void setRandomSeed() {
        seed = random_int() / 2; // 0-2147483647
        srand(seed);
        if (confArrangement < 0)
            arrangementScript = Utils::getRandomInt(0, ARRANGEMENTS_COUNT - 1);
        else
            arrangementScript = confArrangement;

        if (confStructure < 0)
            structureScript = Utils::getRandomInt(0, STRUCTURES_COUNT - 1);
        else
            structureScript = confStructure;

        if (confTempo < 0)
            tempo = Utils::getRandomInt(0, TEMPO_COUNT - 1);
        else
            tempo = confTempo;
    }

};
#endif