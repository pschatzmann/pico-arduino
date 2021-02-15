#include "USB.h"

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void) {
    return USBMidi::instance().get_descriptor_device();
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index){
    return USBMidi::instance().get_descriptor_configuration(index);
}

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    return USBMidi::instance().get_descriptor_string(index, langid);
}

// Invoked when device is mounted
void tud_mount_cb(void){
    USBMidi::instance().onMount();
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    USBMidi::instance().onUnmount();
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en){
    USBMidi::instance().onSuspend(remote_wakeup_en);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    USBMidi::instance().onResume();
}

// define variable for function pointer
//void (* tud_midi_cpp_rx_cb)();

// // assigned from Midi c++ class
// void __setOnRxCallback(void (*fp)()) {
//     tud_midi_cpp_rx_cb = fp;
// }

// // Callback provided by USB Framework
// void tud_midi_rx_cb(uint8_t itf){
//     tud_midi_cpp_rx_cb();
// }
