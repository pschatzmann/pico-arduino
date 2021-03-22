/**
 * PWM fading LED example using Pico api calls 
 */
#include "Arduino.h"

const int pin = 15; // PICO_DEFAULT_LED_PIN;

void setup() {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 255);
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num, &config, true);
}
  
 void loop() {
  //Fading the LED
  for(int i=0; i<255; i++){
    pwm_set_gpio_level(pin, i);
    delay(5);
  }
  for(int i=255; i>0; i--){
    pwm_set_gpio_level(pin, i);
    delay(5);
  }
}
