#pragma once

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "AudioTools/Audiotools.h"

/* Blink pattern
 * - 25 ms   : streaming data
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_STREAMING = 25,
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

enum {
    VOLUME_CTRL_0_DB = 0,
    VOLUME_CTRL_10_DB = 2560,
    VOLUME_CTRL_20_DB = 5120,
    VOLUME_CTRL_30_DB = 7680,
    VOLUME_CTRL_40_DB = 10240,
    VOLUME_CTRL_50_DB = 12800,
    VOLUME_CTRL_60_DB = 15360,
    VOLUME_CTRL_70_DB = 17920,
    VOLUME_CTRL_80_DB = 20480,
    VOLUME_CTRL_90_DB = 23040,
    VOLUME_CTRL_100_DB = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};

typedef struct TU_ATTR_PACKED {
    union {
        struct TU_ATTR_PACKED {
            uint8_t recipient: 5; ///< Recipient type tusb_request_recipient_t.
            uint8_t type: 2; ///< Request type tusb_request_type_t.
            uint8_t direction: 1; ///< Direction type. tusb_dir_t
        } bmRequestType_bit;

        uint8_t bmRequestType;
    };

    audio_cs_req_t bRequest;
    uint8_t bChannelNumber;
    uint8_t bControlSelector;
    union {
        uint8_t bInterface;
        uint8_t bEndpoint;
    };
    uint8_t bEntityID;
    uint16_t wLength;
} audio_control_request_t;


/**
 * @brief USB Audio Headset device (running on core 1). 
 * You can overwrite any callback method !
 * 
 **/

class USBAudio {
    public:
        USBAudio(){
            // setup static instance
            instance(this);
        }

        virtual void processLoop() {
            while(true){
                tud_task(); // TinyUSB device task
                writeBytes();
                led_blinking_task();
            }
        }

        virtual void begin(AudioSource &audioSource){
            audio_source_ptr = &audioSource;
            board_init();
            tusb_init();
            TU_LOG1("Headset running\r\n");
            multicore_launch_core1(processLoopCallback);
        }

        // copy the sound data from the audio source provided in begin
        virtual void writeBytes(void) {
            if (audio_source_ptr!=nullptr){
                size_t max_bytes = buffer_size;
                size_t bytes_eff = audio_source_ptr->readBytes((uint8_t*)buffer, max_bytes);
                tud_audio_write((uint8_t *)buffer, bytes_eff);
            }
        }

        // you can directly copy 16bit PWM sound data to the USB Audio device 
        virtual size_t write(uint8_t* buffer, size_t len){
            return tud_audio_write(buffer, len);
        }

        // you can directly read 16bit PWM sound data to from USB Audio device 
        virtual size_t readBytes(uint8_t* buffer, size_t len){
            return tud_audio_read(buffer, len);
        }

        virtual size_t available() {
            return tud_audio_available();
        }

        virtual void led_blinking_task(void) {
            static uint32_t start_ms = 0;
            static bool led_state = false;

            // Blink every interval ms
            if (board_millis() - start_ms < blink_interval_ms) return;
            start_ms += blink_interval_ms;

            board_led_write(led_state);
            led_state = 1 - led_state;
        }

        virtual void mount_cb(void) {
            blink_interval_ms = BLINK_MOUNTED;
        }

        // Invoked when device is unmounted
        virtual void umount_cb(void) {
            blink_interval_ms = BLINK_NOT_MOUNTED;
        }

        // Invoked when usb bus is suspended
        // remote_wakeup_en : if host allow us  to perform remote wakeup
        // Within 7ms, device must draw an average of current less than 2.5 mA from bus
        virtual void suspend_cb(bool remote_wakeup_en) {
            (void) remote_wakeup_en;
            blink_interval_ms = BLINK_SUSPENDED;
        }

        // Invoked when usb bus is resumed
        virtual void resume_cb(void) {
            blink_interval_ms = BLINK_MOUNTED;
        }


        //--------------------------------------------------------------------+
        // Application Callback API Implementations
        //--------------------------------------------------------------------+

        // Invoked when audio class specific get request received for an entity
        virtual bool audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
            audio_control_request_t *request = (audio_control_request_t *) p_request;

            if (request->bEntityID == UAC2_ENTITY_CLOCK)
                return audio_clock_get_request(rhport, request);
            if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
                return audio_feature_unit_get_request(rhport, request);
            else {
                TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
                        request->bEntityID, request->bControlSelector, request->bRequest);
            }
            return false;
        }

        // Invoked when audio class specific set request received for an entity
        virtual bool audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf) {
            audio_control_request_t const *request = (audio_control_request_t const *) p_request;

            if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
                return audio_feature_unit_set_request(rhport, request, buf);

            TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
                    request->bEntityID, request->bControlSelector, request->bRequest);

            return false;
        }

        virtual bool audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
            (void) rhport;

            uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
            uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

            if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0)
                blink_interval_ms = BLINK_MOUNTED;

            return true;
        }

       virtual  bool audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
            (void) rhport;
            uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
            uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

            TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
            if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0)
                blink_interval_ms = BLINK_STREAMING;

            return true;
        }

        virtual bool audio_rx_done_cb(uint8_t rhport, uint8_t *buffer, uint16_t buf_size) {
            (void) rhport;

            spk_data_size = buf_size;
            memcpy(this->buffer, buffer, buf_size);

            return true;
        }

        virtual bool audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting) {
            (void) rhport;
            (void) itf;
            (void) ep_in;
            (void) cur_alt_setting;

            // This callback could be used to fill microphone data separately
            return true;
        }       


        // setter/getter for instance
        static USBAudio* instance(USBAudio* ptr = nullptr){
            static USBAudio* _instance;
            if (ptr!=nullptr){
                _instance = ptr;
            }
            return _instance;
        }


    protected:
        // Audio controls
        uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
        // Current states
        int8_t mute[CFG_TUD_AUDIO_N_CHANNELS_TX + 1];       // +1 for master channel 0
        int16_t volume[CFG_TUD_AUDIO_N_CHANNELS_TX + 1];    // +1 for master channel 0
        // Buffer for speaker data
        size_t buffer_size = 1000;
        int16_t buffer[1000];
        // Speaker data size received in the last frame
        int spk_data_size;
        // Sound source
        AudioSource *audio_source_ptr;

        // process loop as static method to be used as callback for multicore_launch_core1
        static void processLoopCallback() {
            instance()->processLoop();
        }

        // Helper for clock get requests
        virtual bool audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request) {
            TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

            // Example supports only single frequency, same value will be used for current value and range
            if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ) {
                if (request->bRequest == AUDIO_CS_REQ_CUR) {
                    TU_LOG2("Clock get current freq %u\r\n", AUDIO_SAMPLE_RATE);

                    audio_control_cur_4_t curf = {tu_htole32(AUDIO_SAMPLE_RATE)};
                    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request, &curf,
                                                                    sizeof(curf));
                } else if (request->bRequest == AUDIO_CS_REQ_RANGE) {
                    audio_control_range_4_n_t(1) rangef;                        
                    rangef.wNumSubRanges = tu_htole16(1);
                    rangef.subrange[0] = {tu_htole32(AUDIO_SAMPLE_RATE), tu_htole32(AUDIO_SAMPLE_RATE), 0};
                            
                    TU_LOG2("Clock get freq range (%d, %d, %d)\r\n", (int) rangef.subrange[0].bMin,
                            (int) rangef.subrange[0].bMax, (int) rangef.subrange[0].bRes);
                    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request, &rangef,
                                                                    sizeof(rangef));
                }
            } else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
                    request->bRequest == AUDIO_CS_REQ_CUR) {
                audio_control_cur_1_t cur_valid = {.bCur = 1};
                TU_LOG2("Clock get is valid %u\r\n", cur_valid.bCur);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request, &cur_valid,
                                                                sizeof(cur_valid));
            }
            TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
                    request->bEntityID, request->bControlSelector, request->bRequest);
            return false;
        }

        // Helper for feature unit get requests
        virtual bool audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request) {
            TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

            if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR) {
                audio_control_cur_1_t mute1 = {.bCur = mute[request->bChannelNumber]};
                TU_LOG2("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
                return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request, &mute1,
                                                                sizeof(mute1));
            } else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
                if (request->bRequest == AUDIO_CS_REQ_RANGE) {
                    audio_control_range_2_n_t(1) range_vol;
                    range_vol.wNumSubRanges = tu_htole16(1);
                    range_vol.subrange[0] = {.bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB),tu_htole16(256)};
                    
                    TU_LOG2("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
                            range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256,
                            range_vol.subrange[0].bRes / 256);
                    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request,
                                                                    &range_vol, sizeof(range_vol));
                } else if (request->bRequest == AUDIO_CS_REQ_CUR) {
                    audio_control_cur_2_t cur_vol = {.bCur = tu_htole16(volume[request->bChannelNumber])};
                    TU_LOG2("Get channel %u volume %u dB\r\n", request->bChannelNumber, cur_vol.bCur);
                    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *) request,
                                                                    &cur_vol, sizeof(cur_vol));
                }
            }
            TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
                    request->bEntityID, request->bControlSelector, request->bRequest);

            return false;
        }

        // Helper for feature unit set requests
        virtual  bool audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request,
                                                    uint8_t const *buf) {
            (void) rhport;

            TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
            TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

            if (request->bControlSelector == AUDIO_FU_CTRL_MUTE) {
                TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

                mute[request->bChannelNumber] = ((audio_control_cur_1_t *) buf)->bCur;

                TU_LOG2("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

                return true;
            } else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME) {
                TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

                volume[request->bChannelNumber] = ((audio_control_cur_2_t const *) buf)->bCur;

                TU_LOG2("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

                return true;
            } else {
                TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
                        request->bEntityID, request->bControlSelector, request->bRequest);
                return false;
            }
        }


};
