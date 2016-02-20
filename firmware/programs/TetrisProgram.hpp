#include "Program.h"
#ifndef _TETRISPROGRAM_H
#define _TETRISPROGRAM_H
// https://github.com/rbirkby/ArduinoTetris
#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 14
#define TETRIS_BLOCK 4

class TetrisProgram : public Program {

    const int lead_notes[58] = {
        // part 1
        76, 71, 72, 74, 72, 71, 69, 69, 72, 76, 74, 72, 71, 71, 72, 74, 76, 72, 69, 69, 0,
        74, 77, 81, 79, 77, 76, 72, 76, 74, 72, 71, 71, 72, 74, 76, 72, 69, 69, 0,

        // part 2
        64, 60, 62, 59, 60, 57, 56, 59,
        64, 60, 62, 59, 60, 64, 69, 69, 68, 0

    };
    const float lead_times[58] = {
        // part 1
        1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.5, 0.5, 1.0, 0.5, 0.5, 1.5, 0.5, 1.0, 0.5, 0.5, 1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,

        // part 2
        2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
        2.0, 2.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0, 1.0

    };

    const int bass_notes[128] = {
        // part 1
        40, 52, 40, 52, 40, 52, 40, 52, 33, 45, 33, 45, 33, 45, 33, 45, 32, 44, 32, 44, 32, 44, 32, 44, 33, 45, 33, 45, 33, 47, 48, 52,
        38, 50, 38, 50, 38, 50, 38, 50, 36, 48, 36, 48, 36, 48, 36, 48, 35, 47, 35, 47, 35, 47, 35, 47, 33, 45, 33, 45, 33, 45, 33, 45,

        // part 2
        33, 40, 33, 40, 33, 40, 33, 40, 32, 40, 32, 40, 32, 40, 32, 40, 33, 40, 33, 40, 33, 40, 33, 40, 32, 40, 32, 40, 32, 40, 32, 40,
        33, 40, 33, 40, 33, 40, 33, 40, 32, 40, 32, 40, 32, 40, 32, 40, 33, 40, 33, 40, 33, 40, 33, 40, 32, 40, 32, 40, 32, 40, 32, 40


    };
    const float bass_times[128] = {
        // part 1
        0.5,  0.5,  0.5,  0.5,  0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
        0.5,  0.5,  0.5,  0.5,  0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,

        // part 2
        0.5,  0.5,  0.5,  0.5,  0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
        0.5,  0.5,  0.5,  0.5,  0.5,  0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5

    };

    int curLeadNote;
    int curBassNote;
    long int curLeadTime;
    long int curBassTime;

    struct Piece {
        unsigned int *type;
        int x;
        int y;
        int dir;
    };

    struct Block {
        int x;
        int y;
    };

    enum DIR {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        MIN = UP,
        MAX = LEFT
    };

    int blocks[TETRIS_WIDTH][TETRIS_HEIGHT];                               // 2 dimensional array (nx*ny) representing tetris court - either empty block or occupied by a 'piece'
    bool playing = true;                              // true|false - game is in progress
    bool lost = false;
    Piece current, next;                              // the current and next piece
    unsigned int score = 0;                           // the current score
    int dir = 0;

    unsigned int i[4] = {0x0F00, 0x2222, 0x00F0, 0x4444};
    unsigned int j[4] = {0x44C0, 0x8E00, 0x6440, 0x0E20};
    unsigned int l[4] = {0x4460, 0x0E80, 0xC440, 0x2E00};
    unsigned int o[4] = {0xCC00, 0xCC00, 0xCC00, 0xCC00};
    unsigned int s[4] = {0x06C0, 0x8C40, 0x6C00, 0x4620};
    unsigned int t[4] = {0x0E40, 0x4C40, 0x4E00, 0x4640};
    unsigned int z[4] = {0x0C60, 0x4C80, 0xC600, 0x2640};

    unsigned int *pieces[7] = {i, j, l, o, s, t, z};

public:
    TetrisProgram() : Program() {
        uint32_t seed = random_int();
        srand(seed);

        reset();
        curLeadNote = 0;
        curBassNote = 0;
        curLeadTime = 0;
        curBassTime = 0;

    }

    void process() {
        activateSound();

        play();
        curLeadNote = 0;
        curBassNote = 0;
        curLeadTime = 0;
        curBassTime = 0;

        while (!terminate)
        {
            playMusic();
            drop();
            wait_ms(500);

        }

    }

    void parsePad(int pad_gpio) {
        if (pad_gpio == PAD_TOP) {
            rotate();
        }

        if (pad_gpio == PAD_BOTTOM) {
        }

        if (pad_gpio == PAD_PREVIOUS) {
            move(LEFT);
        }

        if (pad_gpio == PAD_NEXT) {
            move(RIGHT);
        }
        if (pad_gpio == PAD_SELECT) {
            stop();
            shutdownSound();
            program->terminate = true;
            nextProgram = "Main menu";

        }
    }

    void updateScreen() {
        draw();
    }

    string name() {
        return "Tetris";
    }
private:

    void playMusic() {
        while (midiWriter.getQueueSize() < 100) {

            int lead_note = lead_notes[curLeadNote];
            float lead_note_time_remaining = lead_times[curLeadNote];
            if (lead_note > 0) {
                midiWriter.sendNoteOn(curLeadTime, 0, 1, lead_note, 127);
                curLeadTime += 60 * lead_note_time_remaining;
                midiWriter.sendNoteOff(curLeadTime, 0, 1, lead_note, 127);
            } else {
                curLeadTime += 60 * lead_note_time_remaining;

            }

            int bass_note = bass_notes[curBassNote];
            float bass_note_time_remaining = bass_times[curBassNote];
            if (bass_note > 0) {
                midiWriter.sendNoteOn(curBassTime, 0, 2, bass_note, 127);
                curBassTime += 60 * bass_note_time_remaining;
                midiWriter.sendNoteOff(curBassTime, 0, 2, bass_note, 127);
            } else {
                curBassTime += 60 * bass_note_time_remaining;
            }

            curLeadNote++;
            curBassNote++;

            if (curLeadNote >= 58 || curBassNote >= 128) {
                curLeadNote = 0;
                curBassNote = 0;

                curBassTime = curLeadTime;
            }
        }
    }

    void lose() { playing = false; lost = true; }

    void addScore(int n) { score = score + n; }
    void clearScore() { score = 0; }
    int getBlock(int x, int y) { return (blocks[x] ? blocks[x][y] : NULL); }
    void setBlock(int x, int y, int type) { blocks[x][y] = type; }
    void clearBlocks() { for (int x = 0; x < TETRIS_WIDTH; x++) { for (int y = 0; y < TETRIS_HEIGHT; y++) { blocks[x][y] = NULL; } } }
    void setCurrentPiece(Piece piece) {current = piece;}
    void setNextPiece(Piece piece) {next = piece;}

    void reset() {
        lost = false;
        playing = true;
        clearBlocks();
        clearScore();
        setCurrentPiece(randomPiece());
        setNextPiece(randomPiece());
    }

    bool move(int dir) {
        int x = current.x, y = current.y;
        switch (dir) {
        case RIGHT: x = x + 1; break;
        case LEFT:  x = x - 1; break;
        case DOWN:  y = y + 1; break;
        }
        if (unoccupied(current.type, x, y, current.dir)) {
            current.x = x;
            current.y = y;
            return true;
        }
        else {
            return false;
        }
    }

    void rotate() {
        int newdir = (current.dir == MAX ? MIN : current.dir + 1);
        if (unoccupied(current.type, current.x, current.y, newdir)) {
            current.dir = newdir;
        }
    }

    void drop() {
        if (!move(DOWN)) {
            addScore(10);
            dropPiece();
            removeLines();
            setCurrentPiece(next);
            setNextPiece(randomPiece());

            if (occupied(current.type, current.x, current.y, current.dir)) {
                lose();
            }
        }
    }

    void dropPiece() {
        Block positionedBlocks[4];
        getPositionedBlocks(current.type, current.x, current.y, current.dir, positionedBlocks);

        for (int i = 0; i < 4; i++) {
            setBlock(positionedBlocks[i].x, positionedBlocks[i].y, -1);
        }
    }

    void removeLines() {
        int x, y, n = 0;
        bool complete;

        for (y = TETRIS_HEIGHT; y > 0; --y) {
            complete = true;

            for (x = 0; x < TETRIS_WIDTH; ++x) {
                if (!getBlock(x, y)) complete = false;
            }

            if (complete) {
                removeLine(y);
                y = y + 1; // recheck same line
                n++;
            }
        }
        if (n > 0) {
            addScore(100 * pow(2, n - 1)); // 1: 100, 2: 200, 3: 400, 4: 800
        }
    }

    void removeLine(int n) {
        int x, y;
        for (y = n; y >= 0; --y) {
            for (x = 0; x < TETRIS_WIDTH; ++x) {
                setBlock(x, y, (y == 0) ? NULL : getBlock(x, y - 1));
            }
        }
    }

//-------------------------------------------------------------------------
// RENDERING
//-------------------------------------------------------------------------

    void draw() {
        gfx_drawRect(6 * 4 - 2, 4 - 2, TETRIS_WIDTH * 4 + 4, TETRIS_HEIGHT * 4 + 4, 1);

        drawCourt();
        drawNext();
        drawScore();

        removeLines();
    }

    void drawCourt() {
        if (playing)
            drawPiece(current.type, current.x + 6, current.y, current.dir);

        int x, y;
        for (y = 0 ; y < TETRIS_HEIGHT ; y++) {
            for (x = 0 ; x < TETRIS_WIDTH ; x++) {
                if (getBlock(x, y))
                    drawBlock(x + 6, y);
            }
        }

        if (lost) {
            gfx_setCursor(0, 2);
            gfx_printf("Game over\r\n");
        }
    }

    void drawNext() {
        if (playing)
            drawPiece(next.type, 0, 3, next.dir);
    }

    void drawScore() {
        gfx_printf("%d\r\n", score);
    }

    void drawPiece(unsigned int type[], int x, int y, int dir) {
        Block positionedBlocks[4];
        getPositionedBlocks(type, x, y, dir, positionedBlocks);

        for (int i = 0; i < 4; i++) {
            drawBlock(positionedBlocks[i].x, positionedBlocks[i].y);
        }
    }

    void drawBlock(int x, int y) {
        gfx_drawRect(x * TETRIS_BLOCK, y * TETRIS_BLOCK + 4, TETRIS_BLOCK, TETRIS_BLOCK, 1);
    }


//------------------------------------------------
// do the bit manipulation and iterate through each
// occupied block (x,y) for a given piece
//------------------------------------------------
    void getPositionedBlocks(unsigned int type[], int x, int y, int dir, Block* positionedBlocks) {
        unsigned int bit;
        int blockIndex = 0, row = 0, col = 0, blocks = type[dir];

        for (bit = 0x8000; bit > 0 ; bit = bit >> 1) {
            if (blocks & bit) {
                Block block = {x + col, y + row};

                positionedBlocks[blockIndex++] = block;
            }
            if (++col == 4) {
                col = 0;
                ++row;
            }
        }
    }

//-----------------------------------------------------
// check if a piece can fit into a position in the grid
//-----------------------------------------------------
    bool occupied(unsigned int type[], int x, int y, int dir) {
        bool result = false;

        Block positionedBlocks[4];
        getPositionedBlocks(type, x, y, dir, positionedBlocks);

        for (int i = 0; i < 4; i++) {
            Block block = positionedBlocks[i];

            if ((block.x < 0) || (block.x >= TETRIS_WIDTH) || (block.y < 0) || (block.y >= TETRIS_HEIGHT) || getBlock(block.x, block.y))
                result = true;
        }

        return result;
    }

    bool unoccupied(unsigned int type[], int x, int y, int dir) {
        return !occupied(type, x, y, dir);
    }


    Piece randomPiece() {
        Piece current = {pieces[rand() % 6], 3, 0, UP};
        return current;
    }

};

#endif