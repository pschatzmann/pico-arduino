#include "tusb.h"
#include "USBDescriptor.h"


enum {
  ITF_NUM_MIDI = 0,
  ITF_NUM_MIDI_STREAMING,
  ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN)
#define EPNUM_MIDI   0x01


class USBConfig {
  public:
    USBDevice device = USBDevice::instance();

    USBConfig() {
          // define device
          device.idVendor(0xCafe).idProduct(0x0001).bcdDevice(0x0100).
              manufacturer("TinyUSB").product("USBMidi").serialNumber("V0.1");

          // define configuration
          static const uint8_t desc_fs_configuration[] = {
            // Config number, interface count, string index, total length, attribute, power in mA
            TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
            // Interface number, string index, EP Out & EP In address, EP size
            TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, EPNUM_MIDI, 0x80 | EPNUM_MIDI, 64)
          };
          device.setConfigurationDescriptor(desc_fs_configuration, sizeof(desc_fs_configuration));
    }
} midi;


// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
const uint8_t * tud_descriptor_device_cb(void) {
  return (const uint8_t*) midi.device.deviceDescriptor();
}


// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
const uint8_t * tud_descriptor_configuration_cb(uint8_t index){
  return midi.device.configurationDescriptor(index);
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+
// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
const uint16_t* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  return midi.device.string(index);
}

