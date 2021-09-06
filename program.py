import os, math
import serial, sys
import time

def printProgressBar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if iteration == total: 
        print()


def prgramThisShit(port,rom_file):
	arduino = serial.Serial(port,57600)
	progressLength = math.ceil(os.path.getsize(rom_file) / 64)
	x = int(input("Continue? Yes(1) No(0) "))
	if x == 1:
		time.sleep(1)
		command = str.encode('9\n')
		arduino.write(command)

		time.sleep(0.5)
		with open(rom_file, 'rb') as f:
			i = 0
			for chunk in iter(lambda: f.read(64), b''):
				arduino.write(chunk)

				i = i +1
				printProgressBar(i, progressLength, prefix = 'Progress:', suffix = 'Complete', length = 50)
				time.sleep(0.1)
	arduino.close()


def readThisShit(port):
	arduino = serial.Serial(port,57600)
	x = int(input("Continue? Yes(1) No(0) "))
	if x == 1:
		time.sleep(1)
		command = str.encode('3\n')
		arduino.write(command)
		time.sleep(0.5)
		m = input("Inicial Byte Address : ")
		inicio = str.encode(m+'\n')
		arduino.write(inicio)
		memory = []

		flag = True
		i = 0
		while flag:
			i = i + 1
			data = arduino.readline()
			decoded = str(data[0:len(data)].decode("utf-8"))
			printProgressBar(i, 64, prefix = 'Progress:', suffix = 'Complete', length = 50)
			if(decoded == "Z\r\n"):
				flag = False
				print("EEPROM Readed!")
			else:
				memory.append(decoded)
		for v in memory:
			print(v)
	arduino.close()
			


