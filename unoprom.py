import os
import serial, sys
import time
from program import *



port = '/dev/tty.usbmodem14201'
file = './a.out'




def programEEPROM():
    size = os.path.getsize(file)
    print("Port : ",port," File : ",file, " Size : ",size," bytes")
    lock = int(input("Change file yes(1) no(0) "))
    if(lock == 1):
        filee = input("New File : ")
        size = os.path.getsize(filee)
        print("Port : ",port," File : ",filee, " Size : ",size," bytes")
        if(size != 32768):
            print("The file is smaller than the EEPROM")
        prgramThisShit(port,filee)
        print("EEPROM Programmed! ")
    else:
        if(size != 32768):
            print("The file is smaller than the EEPROM")
        prgramThisShit(port,file)
        print("EEPROM Programmed! ")


def dataProtection():
    print("Port : ",port)
    arduino = serial.Serial(port,57600)
    x = int(input("Enable(0) Disable(1) "))
    if(x == 1):
        time.sleep(1)
        command = str.encode('1\n')
        arduino.write(command)
        print("EEPROM Data Protection Disabled")
    else:
        time.sleep(1)
        command = str.encode('2\n')
        arduino.write(command)
        print("EEPROM Data Protection Enabled")
    arduino.close()



def printMenu():
    print("------------------------------")
    print("1 - Write EEPROM from file")
    print("2 - Read EEPROM (1024 bytes)")
    print("3 - Disable/Enable Data protection")
    print("0 - Quit")
    x = int(input("Option : "))
    return x

flag = True
print("Welcome to EEPROM Programmer !")
while flag:
    option = printMenu()
    if (option == 0):
        flag = False
    elif (option == 1):
        programEEPROM()
    elif (option == 2):
        readThisShit(port)
    elif (option == 3):
        dataProtection()

    


