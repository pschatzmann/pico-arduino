
A dedicated implementation of SoftwareSerial for the Pico which uses the PIO.
I have tested it with a speed up to 115200 and it works like a charm.

If you dont provide any parameters in the constructor it uses

- pio1 
- stateMachineRxIndex = 0
- stateMachineTxIndex = 1
