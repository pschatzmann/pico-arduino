#pragma once

#include "Arduino.h"
#include "SoftwareSerial.h"

enum BTRate { R1200=1, R2400=2, R4800=3, R9600=4, R19200=5, R38400=6, R57600=7, R115200=8};

/**
 * @brief API for some HC05 AT commands
 * 
 */
class HC05 {
  public:
    HC05(){}

    void begin(SofwareSerial &serial, int key, Stream &out = Serial) {
        stream_ptr = &out;
        serial_ptr = &serial;
        key_pin = key;
        pinMode(key_pin, OUTPUT);    
    }

    bool isATMode() {
        return test();
    }

    void setAtMode(bool isAtMode) {
        digitalWrite(key_pin, isAtMode);
    }

    bool restoreDefaultStatus(){
        Serial.println("Restore to factory default...")
        return cmd("AT+ORGL","",true)
    }

    bool test() {
        return cmd("AT","", false)
    }

    char* version() {
        cmd("AT+VERSION?","",false);
        return cmd_buffer;
    }

    char* name() {
        cmd("AT+NAME?","",false);
        return cmd_buffer;
    }

    bool setName(const char* name){
        serial_ptr->print("Setting name to ");
        serial_ptr->println(name);
        return cmd("AT+NAME=", name, true);
    }

    char* passkey() {
        cmd("AT+ PSWD?","",false);
        return cmd_buffer;
    }

    bool setPasskey(const char *key) {
        serial_ptr->print("Setting passkey to ");
        serial_ptr->println(name);
        return cmd("AT+PSWD=",key, true);
    }

    bool setBaudRate(BTRate rate){
        serial_ptr->print("Setting baud rate to ");
        serial_ptr->println(rate);
        sprintf(cmd_buffer,"AT+UART=%d,1,0",(int)rate, true);
        return cmd(cmd_buffer);
    }

    bool cmd(const char *cmd_str, const char* arg="", bool printReply=true){
        strcpy(cmd_buffer, cmd_str);
        strcat(cmd_buffer, arg);
        SerialS.println(cmd_buffer);
        delay(500);

        SerialS.readln(reply);
        bool result = strncmp(reply,"OK",2)==0;
        if (printReply){
            Serial.println(reply);
        }
        return result
    }


  protected:
    SofwareSerial *serial_ptr;
    Stream *stream_ptr;
    char cmd_buffer[160];
    int key_pin;

};

