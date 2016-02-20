#include <stdio.h>

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include <libopencm3/usb/midi.h>
#include <libopencm3/cm3/scb.h>

#include "usbmidi.h"

#ifdef __cplusplus
extern "C" {
#endif

usbd_device *usbd_dev;

extern void usbmidi_event(char *msg, int len);

/*
 * All references in this file come from Universal Serial Bus Device Class
 * Definition for MIDI Devices, release 1.0.
 */

/*
 * Table B-1: MIDI Adapter Device Descriptor
 */
static const struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,    /* was 0x0110 in Table B-1 example descriptor */
    .bDeviceClass = 0,   /* device defined at interface level */
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,  /* Prototype product vendor ID */
    .idProduct = 0x5119, /* dd if=/dev/random bs=2 count=1 | hexdump */
    .bcdDevice = 0x0100,
    .iManufacturer = 1,  /* index to string desc */
    .iProduct = 2,       /* index to string desc */
    .iSerialNumber = 0,
    .bNumConfigurations = 1,
};

/*
 * Midi specific endpoint descriptors.
 */
static const struct usb_midi_endpoint_descriptor midi_bulk_endp[] = {{
        /* Table B-12: MIDI Adapter Class-specific Bulk OUT Endpoint
         * Descriptor
         */
        .head = {
            .bLength = sizeof(struct usb_midi_endpoint_descriptor),
            .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
            .bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL,
            .bNumEmbMIDIJack = 1,
        },
        .jack[0] = {
            .baAssocJackID = 0x01,
        },
    }, {
        /* Table B-14: MIDI Adapter Class-specific Bulk IN Endpoint
         * Descriptor
         */
        .head = {
            .bLength = sizeof(struct usb_midi_endpoint_descriptor),
            .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
            .bDescriptorSubType = USB_MIDI_SUBTYPE_MS_GENERAL,
            .bNumEmbMIDIJack = 1,
        },
        .jack[0] = {
            .baAssocJackID = 0x03,
        },
    }
};

/*
 * Standard endpoint descriptors
 */
static const struct usb_endpoint_descriptor bulk_endp[] = {{
        /* Table B-11: MIDI Adapter Standard Bulk OUT Endpoint Descriptor */
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x01,
        .bmAttributes = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize = 0x40,
        .bInterval = 0x00,

        .extra = &midi_bulk_endp[0],
        .extralen = sizeof(midi_bulk_endp[0])
    }, {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x81,
        .bmAttributes = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize = 0x40,
        .bInterval = 0x00,

        .extra = &midi_bulk_endp[1],
        .extralen = sizeof(midi_bulk_endp[1])
    }
};

/*
 * Table B-4: MIDI Adapter Class-specific AC Interface Descriptor
 */
static const struct {
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_body;
} __attribute__((packed)) audio_control_functional_descriptors = {
    .header_head = {
        .bLength = sizeof(struct usb_audio_header_descriptor_head) +
        1 * sizeof(struct usb_audio_header_descriptor_body),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_HEADER,
        .bcdADC = 0x0100,
        .wTotalLength =
        sizeof(struct usb_audio_header_descriptor_head) +
        1 * sizeof(struct usb_audio_header_descriptor_body),
        .binCollection = 1,
    },
    .header_body = {
        .baInterfaceNr = 0x01,
    },
};

/*
 * Table B-3: MIDI Adapter Standard AC Interface Descriptor
 */
static const struct usb_interface_descriptor audio_control_iface[] = {{
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = USB_CLASS_AUDIO,
        .bInterfaceSubClass = USB_AUDIO_SUBCLASS_CONTROL,
        .bInterfaceProtocol = 0,
        .iInterface = 0,

        .extra = &audio_control_functional_descriptors,
        .extralen = sizeof(audio_control_functional_descriptors)
    }
};

/*
 * Class-specific MIDI streaming interface descriptor
 */
static const struct {
    struct usb_midi_header_descriptor header;
    struct usb_midi_in_jack_descriptor in_embedded;
    struct usb_midi_in_jack_descriptor in_external;
    struct usb_midi_out_jack_descriptor out_embedded;
    struct usb_midi_out_jack_descriptor out_external;
} __attribute__((packed)) midi_streaming_functional_descriptors = {
    /* Table B-6: Midi Adapter Class-specific MS Interface Descriptor */
    .header = {
        .bLength = sizeof(struct usb_midi_header_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_MIDI_SUBTYPE_MS_HEADER,
        .bcdMSC = 0x0100,
        .wTotalLength = sizeof(midi_streaming_functional_descriptors),
    },
    /* Table B-7: MIDI Adapter MIDI IN Jack Descriptor (Embedded) */
    .in_embedded = {
        .bLength = sizeof(struct usb_midi_in_jack_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK,
        .bJackType = USB_MIDI_JACK_TYPE_EMBEDDED,
        .bJackID = 0x01,
        .iJack = 0x00,
    },
    /* Table B-8: MIDI Adapter MIDI IN Jack Descriptor (External) */
    .in_external = {
        .bLength = sizeof(struct usb_midi_in_jack_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_IN_JACK,
        .bJackType = USB_MIDI_JACK_TYPE_EXTERNAL,
        .bJackID = 0x02,
        .iJack = 0x00,
    },
    /* Table B-9: MIDI Adapter MIDI OUT Jack Descriptor (Embedded) */
    .out_embedded = {
        .head = {
            .bLength = sizeof(struct usb_midi_out_jack_descriptor),
            .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
            .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK,
            .bJackType = USB_MIDI_JACK_TYPE_EMBEDDED,
            .bJackID = 0x03,
            .bNrInputPins = 1,
        },
        .source[0] = {
            .baSourceID = 0x02,
            .baSourcePin = 0x01,
        },
        .tail = {
            .iJack = 0x00,
        }
    },
    /* Table B-10: MIDI Adapter MIDI OUT Jack Descriptor (External) */
    .out_external = {
        .head = {
            .bLength = sizeof(struct usb_midi_out_jack_descriptor),
            .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
            .bDescriptorSubtype = USB_MIDI_SUBTYPE_MIDI_OUT_JACK,
            .bJackType = USB_MIDI_JACK_TYPE_EXTERNAL,
            .bJackID = 0x04,
            .bNrInputPins = 1,
        },
        .source[0] = {
            .baSourceID = 0x01,
            .baSourcePin = 0x01,
        },
        .tail = {
            .iJack = 0x00,
        },
    },
};

/*
 * Table B-5: MIDI Adapter Standard MS Interface Descriptor
 */
static const struct usb_interface_descriptor midi_streaming_iface[] = {{
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 1,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_AUDIO,
        .bInterfaceSubClass = USB_AUDIO_SUBCLASS_MIDISTREAMING,
        .bInterfaceProtocol = 0,
        .iInterface = 0,

        .endpoint = bulk_endp,

        .extra = &midi_streaming_functional_descriptors,
        .extralen = sizeof(midi_streaming_functional_descriptors)
    }
};

static const struct usb_interface ifaces[] = {{
        .num_altsetting = 1,
        .altsetting = audio_control_iface,
    }, {
        .num_altsetting = 1,
        .altsetting = midi_streaming_iface,
    }
};

/*
 * Table B-2: MIDI Adapter Configuration Descriptor
 */
static const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0, /* can be anything, it is updated automatically
                  when the usb code prepares the descriptor */
    .bNumInterfaces = 2, /* control and data */
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80, /* bus powered */
    .bMaxPower = 0x32,

    .interface = ifaces,
};

static const char * usb_strings[] = {
    "libopencm3.org",
    "Musicbox",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

/* SysEx identity message, preformatted with correct USB framing information */
const uint8_t sysex_identity[] = {
    0x04,   /* USB Framing (3 byte SysEx) */
    0xf0,   /* SysEx start */
    0x7e,   /* non-realtime */
    0x00,   /* Channel 0 */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x7d,   /* Educational/prototype manufacturer ID */
    0x66,   /* Family code (byte 1) */
    0x66,   /* Family code (byte 2) */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x51,   /* Model number (byte 1) */
    0x19,   /* Model number (byte 2) */
    0x00,   /* Version number (byte 1) */
    0x04,   /* USB Framing (3 byte SysEx) */
    0x00,   /* Version number (byte 2) */
    0x01,   /* Version number (byte 3) */
    0x00,   /* Version number (byte 4) */
    0x05,   /* USB Framing (1 byte SysEx) */
    0xf7,   /* SysEx end */
    0x00,   /* Padding */
    0x00,   /* Padding */
};


uint8_t usbmidi_data_end;
uint8_t usbmidi_cur_data;
uint8_t usbmidi_data[256];

static void usbmidi_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
    (void)ep;
    int i, j;
    char buf[64];
    int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

    for (i = 0; i < len; i += 4) {
        uint8_t data_read;
        usbmidi_data_end = 1;
        switch (buf[i]) {
        case 0x2:
            // Two-bytes System Common Message - undefined in USBMidi 1.0
            data_read = 2;
            break;
        case 0x4:
            // SysEx start or continue
            usbmidi_data_end = 0;
            data_read = 3;
            break;
        case 0x5:
            // Single-byte System Common Message or SysEx end with one byte
            data_read = 1;
            break;
        case 0x6:
            // SysEx end with two bytes
            data_read = 2;
            break;
        case 0xC:
            // Program change
            data_read = 2;
            break;
        case 0xD:
            // Channel pressure
            data_read = 2;
            break;
        case 0xF:
            // Single byte
            data_read = 1;
            break;
        default:
            // Others three-bytes messages
            data_read = 3;
            break;
        }

        for (j = 1; j < data_read + 1; j++) {
            usbmidi_data[usbmidi_cur_data] = buf[i + j];
            usbmidi_cur_data++;
        }

        if (usbmidi_data_end) {
            usbmidi_event(usbmidi_data, usbmidi_cur_data);
            usbmidi_cur_data = 0;
        }
    }

}

static void usbmidi_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
    usbmidi_data_end = 0;
    usbmidi_cur_data = 0;
    (void)wValue;

    usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
                  usbmidi_data_rx_cb);
    usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_BULK, 64, NULL);
}

void usbmidi_write(uint8_t *msg, int len) {
    int p;
    for (p = 0; p < len; p += 3) {
        uint8_t buf[4];
        // Midi message to USBMidi event packet
        buf[0] = msg[p] >> 4;
        // SysEx
        if (buf[0] == 0xF) {
            if (p < len - 1) {
                // SysEx start or continue
                buf[0] = 0x4;
            } else {
                switch (len - p) {
                case 1:
                    // SysEx end with one byte
                    buf[0] = 0x5;
                    break;
                case 2:
                    // SysEx end with two bytes
                    buf[0] = 0x6;
                    break;
                case 3:
                    // SysEx end with three bytes
                    buf[0] = 0x7;
                    break;
                }
            }
        }
        buf[1] = msg[p];
        buf[2] = msg[p + 1];
        buf[3] = msg[p + 2];

        usbd_ep_write_packet(usbd_dev, 0x81, buf, 4);
    }
}

void usbmidi_setup() {
    usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
                         usb_strings, 2,
                         usbd_control_buffer, sizeof(usbd_control_buffer));

    usbd_register_set_config_callback(usbd_dev, usbmidi_set_config);
    return usbd_dev;
}

void usbmidi_poll() {
        usbd_poll(usbd_dev);
}

#ifdef __cplusplus
}
#endif
