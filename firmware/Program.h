#include <string>

#ifndef _PROGRAM_H
#define _PROGRAM_H

#include "MidiRaw.h"
#include "interface.h"
#include "rng.h"
#include "gfx.h"
#include "sam2695.h"
#include "ssd1306.h"
#include "system_setup.h"

#define PROGRAM_VERSION "1.0.0"

using namespace std;

class Program {
public:
    bool terminate;
    bool isPlaying;


    Program() {
        terminate = false;
    }

    virtual void init() {}

    virtual void process() {}

    virtual void parsePad(int padio) {}

    virtual void updateScreen() {}

    virtual string name() {
        return "";
    }

    virtual void parseMidi(char *msg, int len)
    {

    }

protected:

    void play() {
        isPlaying = true;
        midiWriter.setPause(false);
        midiWriter.setStop(false);
    }

    void pause() {
        isPlaying = false;
        midiWriter.setPause(true);
    }

    void stop() {
        isPlaying = false;
        midiWriter.setStop(true);
    }



};

void shutdownSound();
void activateSound();

typedef Program *(*ProgramCreator)();

template <typename T>
static Program *makeProgram()
{
    return new T {};
};

extern Program *program;
extern string nextProgram;

extern map<string, ProgramCreator> programsMap;
extern vector<string> programs;

void initPrograms();

#endif