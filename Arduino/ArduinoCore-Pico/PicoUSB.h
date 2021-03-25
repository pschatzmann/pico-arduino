#pragma once

#if !defined(TINYUSB_HOST_LINKED) && !defined(TINYUSB_DEVICE_LINKED)
extern "C" {
static bool tud_cdc_connected (void);
}
#endif
