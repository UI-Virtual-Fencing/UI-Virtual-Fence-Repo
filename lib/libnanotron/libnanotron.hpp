#ifndef ARDUINO_H
#define ARDUINO_H
#include <Arduino.h>
#endif
#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H
#include <HardwareSerial.h>
#endif



enum hardwaretype{
    tag,
    beacon
},


class nanotron
{
private:
    String ID;
    hardwaretype hardware;

public:
    nanobeacon();
    nanobeacon(String);
    int getsettingself(int);
    String getsettingself();
    bool setsettingself(int, String);
    int range(String tag_id);
    double read_my_input_voltage();

    int read_my_radio_id();
    int read_my_temperature();
    int read_my_voltage();
};



