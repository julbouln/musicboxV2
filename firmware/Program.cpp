#include "Program.h"

#include "EmptyProgram.hpp"
#include "MainMenuProgram.hpp"
#include "CcgProgram.hpp"
#include "SynthesizerProgram.hpp"
#include "TetrisProgram.hpp"
#include "AboutProgram.hpp"

void shutdownSound() {
	sam2695_shutdown();
	speaker_shutdown();
}

void activateSound() {
	sam2695_reset();
	jack_sense();
}

Program *program;
string nextProgram;

map<string, ProgramCreator> programsMap;
vector<string> programs;

void initPrograms() {
    nextProgram = "Main menu";

    programsMap["Main menu"] = makeProgram<MainMenuProgram>;
    programsMap["Music generator"] = makeProgram<CcgProgram>;
//    programsMap["MIDI file player"] = makeProgram<MidiFileProgram>;
    programsMap["Synthesizer"] = makeProgram<SynthesizerProgram>;
    programsMap["Tetris"] = makeProgram<TetrisProgram>;
    programsMap["About"] = makeProgram<AboutProgram>;

    programs.push_back("Main menu");
    programs.push_back("Music generator");
//    programs.push_back("MIDI file player");
    programs.push_back("Synthesizer");
    programs.push_back("Tetris");
    programs.push_back("About");

};