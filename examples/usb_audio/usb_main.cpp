/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Reinhard Panhuber
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board.h"
#include "tusb.h"
#include "USB.h"

// make sure that tud_audio_desc_lengths has c binding
extern "C" const uint16_t tud_audio_desc_lengths[];

// These variables are required by the audio driver in audio_device.c
// List of audio descriptor lengths which is required by audio driver - you need as many entries as CFG_TUD_AUDIO - unfortunately this is not possible to determine otherwise
const uint16_t tud_audio_desc_lengths[] = {TUD_AUDIO_MIC_DESC_LEN};


// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void) {
  return USBAudio::instance().usb_descriptor_device_cb();
}


// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index){
  return USBAudio::instance().usb_descriptor_configuration_cb(index);
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+
// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  return USBAudio::instance().usb_descriptor_string_cb(index,langid);
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void){
  return USBAudio::instance().usb_mount_cb();
}

// Invoked when device is unmounted
void tud_umount_cb(void){
  return USBAudio::instance().usb_umount_cb();
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en){
  return USBAudio::instance().usb_suspend_cb(remote_wakeup_en);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void){
  return USBAudio::instance().usb_resume_cb();
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific set request received for an EP
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff){
  return USBAudio::instance().usb_audio_set_req_ep_cb(rhport,p_request, pBuff);
}

// Invoked when audio class specific set request received for an interface
bool tud_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff) {
  return USBAudio::instance().usb_audio_set_req_itf_cb(rhport, p_request, pBuff);
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff){
  return USBAudio::instance().usb_audio_set_req_entity_cb(rhport, p_request, pBuff);
}

// Invoked when audio class specific get request received for an EP
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
  return USBAudio::instance().usb_audio_get_req_ep_cb(rhport, p_request);
}

// Invoked when audio class specific get request received for an interface
bool tud_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
  return USBAudio::instance().usb_audio_get_req_itf_cb(rhport, p_request);
}

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
  return USBAudio::instance().usb_audio_get_req_entity_cb(rhport, p_request);
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting){
  return USBAudio::instance().usb_audio_tx_done_pre_load_cb(rhport, itf, ep_in,cur_alt_setting );
}

bool tud_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting){
  return USBAudio::instance().usb_audio_tx_done_post_load_cb(rhport, n_bytes_copied, itf, ep_in, cur_alt_setting );
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
  return USBAudio::instance().usb_audio_set_itf_close_EP_cb(rhport, p_request );
}


