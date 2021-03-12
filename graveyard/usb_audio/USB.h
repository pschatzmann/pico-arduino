#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board.h"

#include "tusb.h"
#include "AudioTools/AudioTools.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]     AUDIO | MIDI | HID | MSC | CDC          [LSB]
 */
#define AUDIO_SAMPLE_RATE 44100
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) |  _PID_MAP(MIDI, 3) | _PID_MAP(AUDIO, 4) | _PID_MAP(VENDOR, 5) )
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + CFG_TUD_AUDIO * TUD_AUDIO_MIC_DESC_LEN)
// LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
// 0 control, 1 In, 2 Bulk, 3 Iso, 4 In etc ...
#if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
#define EPNUM_AUDIO   0x03
#else
#define EPNUM_AUDIO   0x01
#endif

/**
 * @brief A easy to use USB Audio implementation
 * 
 */

class USBAudio {
  public:
        // Provides access to the singleton
        static USBAudio &instance(){
          static USBAudio instance_;
          return instance_;
        }

        // starts the USB
        virtual void begin(AudioSource &audio_source, int sampleFrequency=AUDIO_SAMPLE_RATE){
            TU_LOG2("begin\r\n");
            tone_generator = &audio_source;
            audio_init(sampleFrequency);
            while (true) {
              doLoop();
            }
        }

        virtual void doLoop() {
            TU_LOG2("doLoop\r\n");
            tud_task(); // tinyusb device task
            led_blinking_task();
        }

        // Invoked when received GET DEVICE DESCRIPTOR
        // Application return pointer to descriptor
        virtual uint8_t const * usb_descriptor_device_cb(void){
            //--------------------------------------------------------------------+
          // Device Descriptors
          //--------------------------------------------------------------------+
          static tusb_desc_device_t const desc_device = {
              .bLength            = sizeof(tusb_desc_device_t),
              .bDescriptorType    = TUSB_DESC_DEVICE,
              .bcdUSB             = 0x0200,

              // Use Interface Association Descriptor (IAD) for CDC
              // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
              .bDeviceClass       = TUSB_CLASS_MISC,
              .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
              .bDeviceProtocol    = MISC_PROTOCOL_IAD,
              .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

              .idVendor           = 0xCafe,
              .idProduct          = USB_PID,
              .bcdDevice          = 0x0100,

              .iManufacturer      = 0x01,
              .iProduct           = 0x02,
              .iSerialNumber      = 0x03,

              .bNumConfigurations = 0x01
          };
          return (uint8_t const *) &desc_device;
        }

        // TAKE CARE - THE NUMBER OF AUDIO STREAMING INTERFACES PER AUDIO FUNCTION MUST NOT EXCEED CFG_TUD_AUDIO_N_AS_INT - IF IT DOES INCREASE CFG_TUD_AUDIO_N_AS_INT IN tusb_config.h!
        // Invoked when received GET CONFIGURATION DESCRIPTOR
        // Application return pointer to descriptor
        // Descriptor contents must exist long enough for transfer to complete
        virtual uint8_t const * usb_descriptor_configuration_cb(uint8_t index) {
          static uint8_t const desc_configuration[] = {
                // Interface count, string index, total length, attribute, power in mA
                TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

                // Interface number, string index, EP Out & EP In address, EP size
                TUD_AUDIO_MIC_DESCRIPTOR(/*_itfnum*/ ITF_NUM_AUDIO_CONTROL, /*_stridx*/ 0, /*_nBytesPerSample*/ 2, /*_nBitsUsedPerSample*/ 16, /*_epin*/ 0x80 | EPNUM_AUDIO, /*_epsize*/ 48*4)
            };
          return desc_configuration;
        }   

        // Invoked when received GET STRING DESCRIPTOR request
        // Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
        virtual uint16_t const* usb_descriptor_string_cb(uint8_t index, uint16_t langid) {
          int chr_count;
          static uint16_t _desc_str[32];
          // array of pointer to string descriptors
          static char const* string_desc_arr [] = {
              (const char[]) { 0x09, 0x04 }, 	// 0: is supported language is English (0x0409)
              "PaniRCorp",                   	// 1: Manufacturer
              "MicNode",              		    // 2: Product
              "123456",                      	// 3: Serials, should use chip ID
              "UAC2",                       	// 4: Audio Interface
          };

          if ( index == 0) {
            memcpy(&_desc_str[1], string_desc_arr[0], 2);
            chr_count = 1;
          } else {
            // Convert ASCII string into UTF-16

            if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) )
              return NULL;

            const char* str = string_desc_arr[index];

            // Cap at max char
            chr_count = strlen(str);
            if ( chr_count > 31 ) 
              chr_count = 31;

            for(uint8_t i=0; i<chr_count; i++){
              _desc_str[1+i] = str[i];
            }
          }

          // first byte is length (including header), second byte is string type
          _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
          return _desc_str;
        }
     
        // Invoked when device is mounted
        virtual void usb_mount_cb(void){
          blink_interval_ms = BLINK_MOUNTED;
        }

        // Invoked when device is unmounted
        virtual void usb_umount_cb(void){
          blink_interval_ms = BLINK_NOT_MOUNTED;
        }

        // Invoked when usb bus is suspended
        // remote_wakeup_en : if host allow us  to perform remote wakeup
        // Within 7ms, device must draw an average of current less than 2.5 mA from bus
        virtual void usb_suspend_cb(bool remote_wakeup_en){
          (void) remote_wakeup_en;
          blink_interval_ms = BLINK_SUSPENDED;
        }

        // Invoked when usb bus is resumed
        virtual void usb_resume_cb(void){
          blink_interval_ms = BLINK_MOUNTED;
        }

        //--------------------------------------------------------------------+
        // Application Callback API Implementations
        //--------------------------------------------------------------------+

        // Invoked when audio class specific set request received for an EP
        virtual bool usb_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff){
          (void) rhport;
          (void) pBuff;

          // We do not support any set range requests here, only current value requests
          TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          uint8_t ep = TU_U16_LOW(p_request->wIndex);

          (void) channelNum; (void) ctrlSel; (void) ep;

          return false; 	// Yet not implemented
        }


        // Invoked when audio class specific set request received for an interface
        virtual bool usb_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff) {
          (void) rhport;
          (void) pBuff;

          // We do not support any set range requests here, only current value requests
          TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          uint8_t itf = TU_U16_LOW(p_request->wIndex);

          (void) channelNum; (void) ctrlSel; (void) itf;

          return false; 	// Yet not implemented
        }

        virtual bool usb_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff) {
          (void) rhport;

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          uint8_t itf = TU_U16_LOW(p_request->wIndex);
          uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

          (void) itf;

          // We do not support any set range requests here, only current value requests
          TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

          // If request is for our feature unit
          if ( entityID == 2 ) {
            switch ( ctrlSel ) {
              case AUDIO_FU_CTRL_MUTE:
                // Request uses format layout 1
                TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_1_t));
                mute[channelNum] = ((audio_control_cur_1_t*) pBuff)->bCur;
                TU_LOG2("    Set Mute: %d of channel: %u\r\n", mute[channelNum], channelNum);
                return true;

              case AUDIO_FU_CTRL_VOLUME:
                // Request uses format layout 2
                TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_2_t));
                volume[channelNum] = ((audio_control_cur_2_t*) pBuff)->bCur;
                TU_LOG2("    Set Volume: %d dB of channel: %u\r\n", volume[channelNum], channelNum);
                return true;

                // Unknown/Unsupported control
              default:
                TU_BREAKPOINT();
              return false;
            }
          }
          return false;    // Yet not implemented
        }

        // Invoked when audio class specific get request received for an EP
        virtual bool usb_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
          (void) rhport;

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          uint8_t ep = TU_U16_LOW(p_request->wIndex);

          (void) channelNum; (void) ctrlSel; (void) ep;

          //	return tud_control_xfer(rhport, p_request, &tmp, 1);

          return false; 	// Yet not implemented
        }

        // Invoked when audio class specific get request received for an interface
        virtual bool usb_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
          (void) rhport;

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          uint8_t itf = TU_U16_LOW(p_request->wIndex);

          (void) channelNum; (void) ctrlSel; (void) itf;

          return false; 	// Yet not implemented
        }

        // Invoked when audio class specific get request received for an entity
        virtual bool usb_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request) {
          (void) rhport;

          // Page 91 in UAC2 specification
          uint8_t channelNum = TU_U16_LOW(p_request->wValue);
          uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
          // uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we do not need the itf value
          uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

          // Input terminal (Microphone input)
          if (entityID == 1){
            switch (ctrlSel){
              case AUDIO_TE_CTRL_CONNECTOR:;
              // The terminal connector control only has a get request with only the CUR attribute.

              audio_desc_channel_cluster_t ret;

              // Those are dummy values for now
              ret.bNrChannels = 1;
              ret.bmChannelConfig = (audio_channel_config_t)0;
              ret.iChannelNames = 0;

              TU_LOG2("    Get terminal connector\r\n");
              return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, (void*)&ret, sizeof(ret));

              // Unknown/Unsupported control selector
              default: TU_BREAKPOINT(); return false;
            }
          }

          // Feature unit
          if (entityID == 2){
            switch (ctrlSel){
              case AUDIO_FU_CTRL_MUTE:
                // Audio control mute cur parameter block consists of only one byte - we thus can send it right away
                // There does not exist a range parameter block for mute
                TU_LOG2("    Get Mute of channel: %u\r\n", channelNum);
                return tud_control_xfer(rhport, p_request, &mute[channelNum], 1);

              case AUDIO_FU_CTRL_VOLUME:
                switch (p_request->bRequest){
                  case AUDIO_CS_REQ_CUR:
                    TU_LOG2("    Get Volume of channel: %u\r\n", channelNum);
                    return tud_control_xfer(rhport, p_request, &volume[channelNum], sizeof(volume[channelNum]));
                  case AUDIO_CS_REQ_RANGE:
                    TU_LOG2("    Get Volume range of channel: %u\r\n", channelNum);

                    // Copy values - only for testing - better is version below
                    audio_control_range_2_n_t(1) ret;

                    ret.wNumSubRanges = 1;
                    ret.subrange[0].bMin = -90; 	// -90 dB
                    ret.subrange[0].bMax = 90;		// +90 dB
                    ret.subrange[0].bRes = 1; 		// 1 dB steps

                    return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, (void*)&ret, sizeof(ret));

                    // Unknown/Unsupported control
                  default: TU_BREAKPOINT(); return false;
                }

              // Unknown/Unsupported control
                default: TU_BREAKPOINT(); return false;
                }
              }

          // Clock Source unit
          if (entityID == 4) {
            switch (ctrlSel) {
              case AUDIO_CS_CTRL_SAM_FREQ:
                  // channelNum is always zero in this case
                  switch (p_request->bRequest) {
                    case AUDIO_CS_REQ_CUR:
                      TU_LOG2("    Get Sample Freq.\r\n");
                      return tud_control_xfer(rhport, p_request, &sampFreq, sizeof(sampFreq));
                    case AUDIO_CS_REQ_RANGE:
                      TU_LOG2("    Get Sample Freq. range\r\n");
                      return tud_control_xfer(rhport, p_request, &sampleFreqRng, sizeof(sampleFreqRng));

                      // Unknown/Unsupported control
                    default: TU_BREAKPOINT(); return false;
                  }

              case AUDIO_CS_CTRL_CLK_VALID:
                // Only cur attribute exists for this request
                TU_LOG2("    Get Sample Freq. valid\r\n");
                return tud_control_xfer(rhport, p_request, &clkValid, sizeof(clkValid));

                // Unknown/Unsupported control
              default: TU_BREAKPOINT(); return false;
              }
          }
          TU_LOG2("  Unsupported entity: %d\r\n", entityID);
          return false; 	// Yet not implemented
        }

        virtual bool usb_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)  {
          if (buffer_audio_size>0) {
            tud_audio_write ((uint8_t *)buffer_audio, buffer_audio_size);
          }
          return true;
        }

        virtual bool usb_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting) {
          if (clkValid){
            buffer_audio_size = tone_generator->readBytes((uint8_t *)buffer_audio, CFG_TUD_AUDIO_TX_FIFO_SIZE);
          } else {
            buffer_audio_size = 0;
          }
          return true;
        }

        virtual  bool usb_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request){
          (void) rhport;
          (void) p_request;
          return true;
        }

  protected:
    /* Blink pattern
    * - 250 ms  : device not mounted
    * - 1000 ms : device mounted
    * - 2500 ms : device is suspended
    */
    enum  {
      BLINK_NOT_MOUNTED = 250,
      BLINK_MOUNTED = 1000,
      BLINK_SUSPENDED = 2500,
    };

    //--------------------------------------------------------------------+
    // Configuration Descriptor
    //--------------------------------------------------------------------+
    enum {
      ITF_NUM_AUDIO_CONTROL = 0,
      ITF_NUM_AUDIO_STREAMING,
      ITF_NUM_TOTAL
    };

    // Blinking
    uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

    // Audio controls
    bool mute[CFG_TUD_AUDIO_N_CHANNELS_TX + 1]; 						// +1 for master channel 0
    uint16_t volume[CFG_TUD_AUDIO_N_CHANNELS_TX + 1]; 					// +1 for master channel 0

    // Current states
    uint32_t sampFreq;
    uint8_t clkValid;

    // Range states
    audio_control_range_2_n_t(1) volumeRng[CFG_TUD_AUDIO_N_CHANNELS_TX+1]; 			// Volume range state
    audio_control_range_4_n_t(1) sampleFreqRng; 						// Sample frequency range state

    // Audio data
    AudioSource *tone_generator;
    uint16_t buffer_audio[CFG_TUD_AUDIO_TX_FIFO_SIZE/2];
    size_t buffer_audio_size;

    // This is a singletong so, prevent instaniation
    USBAudio(){
    }

    /*------------- MAIN -------------*/
    virtual  void audio_init(int sampleFrequency){
      // Init values
      sampFreq = sampleFrequency;
      clkValid = 1;

      sampleFreqRng.wNumSubRanges = 1;
      sampleFreqRng.subrange[0].bMin = sampFreq;
      sampleFreqRng.subrange[0].bMax = sampFreq;
      sampleFreqRng.subrange[0].bRes = 0;

      board_init();
      tusb_init();
    }

    //--------------------------------------------------------------------+
    // BLINKING TASK
    //--------------------------------------------------------------------+
    virtual  void led_blinking_task(void) {
      static uint32_t start_ms = 0;
      static bool led_state = false;

      // Blink every interval ms
      if ( board_millis() - start_ms < blink_interval_ms) 
        return; // not enough time
      start_ms += blink_interval_ms;

      board_led_write(led_state);
      led_state = 1 - led_state; // toggle
    }    

};






