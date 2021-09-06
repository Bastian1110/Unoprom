# Unoprom
A programmer for the AT28256 EEPROM, based on the Arduino Uno and 8-bit shift register,with a mini-interface in Python.

## Based-On
This project can be considered a mixture of several projects, it is based on the [TommyPROM Programmer](https://github.com/TomNisbet/TommyPROM/tree/master/unlock-ben-eater-hardware) (the part of the management of Arduino registers) and the [Crymaykish Programmer](https://github.com/crmaykish/AT28C-EEPROM-Programmer-Arduino) (for the Arduino-Python interaction).

## Installation

- Software

1. Compile and Run the _Programmer.ino_ file in your Arduino Nano or Uno.
2. Plug yout arduino and detect the serial port, then change the port of _Unoprom.py_
3. Make sure yo gat installed the required libraries to run _Unoprom.py_ (Math,Os,Time and Pyserial)

- Hardware

1. Watch the [Ben Eater EEPROM Programmer Video](https://youtu.be/K88pgWhEb1M) and follow the steps. 
2. Connect the corresponding pins of the 8-bit shift registers to the address pins of the AT28C256

## Usage

1. Move to the directory where you cloned this repository.
```
cd YOURPATH/Unoprom
```
2. Plug your Arduino Uno or Nano to your PC
3. From the terminal, run the following command:
```
python3 unoprom.py
```
4. If all is well, a menu will appear with the programmer options
5. To porgram a.bin or.s make sure you have it in the same folder or change the path (default is a.out)
6. Fallow the minimal GUI
7. :D



