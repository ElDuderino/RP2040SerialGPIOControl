# RP2040SerialGPIOControl
Control various GPIO over Serial connection. 

The purpose of this code is to allow control of GPIO status (HI/LOW) or perhaps eventually other aspects of GPIO (perhaps INPUT direction, PULLUPs, etc.)

The first main purpose though is for relay control. 

We likely also need to READ GPIO status HI/LOW to determine if an action should be taken. 

We need a super simple command type / structure with ASCII support so users can control GPIO over a serial console. 

The main target for now is RP2040.

The folder structure will be such that you use the main folder as the sketchbook location. 
