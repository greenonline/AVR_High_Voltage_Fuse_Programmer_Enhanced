# AVR_High_Voltage_Fuse_Programmer_Enhanced
Using Arduino Uno as a High Voltage Fuse Programmer for Atmel ATTiny, with Buzzer and power LED

Basically the same as [GadgetReboot](https://github.com/GadgetReboot)/[Arduino](https://github.com/GadgetReboot/Arduino)/[Uno](https://github.com/GadgetReboot/Arduino/tree/master/Uno)/[AVR_High_Voltage_Fuse_Programmer](https://github.com/GadgetReboot/Arduino/tree/master/Uno/AVR_High_Voltage_Fuse_Programmer)/[AVR_HV_Fuse_Programmer.ino](https://github.com/GadgetReboot/Arduino/blob/master/Uno/AVR_High_Voltage_Fuse_Programmer/AVR_HV_Fuse_Programmer.ino), but with Ralph Bacon's buzzer and power LED code reinistated, see [RalphBacon](https://github.com/RalphBacon)/[ATTiny85_Fuse_Resetter](https://github.com/RalphBacon/ATTiny85_Fuse_Resetter)/[ATTiny85_Reset.ino](https://github.com/RalphBacon/ATTiny85_Fuse_Resetter/blob/master/ATTiny85_Reset.ino)

The differences from  Ralph's code, is that the buzzer is now on pin 4 (rather than pin 7), to accomdate the RESET being on pin 7 (rather than pin 13). The reset was moved by GedgetReboot in order to avoid using the Arduino's built-in LED pin, that caused issues when the LED flashed.
