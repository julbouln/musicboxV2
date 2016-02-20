#ifndef _MIDIRAW_H
#define _MIDIRAW_H

#include "CcgMusic.h"

#include "usbmidi.h"
#include "sam2695.h"
#include "systick.h"

class MidiRaw : public MidiDriver {
public:
    MidiRaw() {
        min_queue_size = 100;
        max_queue_size = 200;

    }
    void mutexLock() {
    }
    void mutexUnlock() {
    }
    void msleep(int ms) {
        wait_ms(ms);
    }

    void sendMessage(QueueMessage *qm)
    {
        uint8_t *m = qm->getMessage();
        uint8_t s = qm->getSize();
        for (int i = 0; i < s; i++) {
            sam2695_write(m[i]);
        }

        usbmidi_write(m, s);
        /*      for (int i = 0; i < qm->getSize(); i++) {
                    midiExt.putc(m[i]);
                }
                */
//      printf("MidiMessage: %x %x %x (%d)\n",m[0],m[1],m[2],qm->getSize());

//      if (midiUsb.suspended) {
//          midiUsb.write(MIDIMessage(m,s));
//      }
    }
};

extern MidiRaw midiWriter;

#endif