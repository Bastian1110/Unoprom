#define SHIFT_DATA 2   //SER HC595 
#define SHIFT_CLK 3    //SCLK HC595
#define SHIFT_LATCH 4  //RCLK HC595
#define WRITE_EN 13    //Write Enable of AT28C256

#define EEPROM_SIZE 32768L       
#define STREAM_BUFFER_SIZE 64    //Size of byte page for eeprom

#define CR '\r'            //This 4 need to be ignorate (enter, new line, etc.)
#define LF '\n'
#define BS 0x08
#define DEL 0x7F

const byte cmdBufSize = 128;

int ctxCmd = 0;

char cmdData[cmdBufSize]; // an array to store the received cmd

boolean newCmd = false;

boolean streamComplete = false;
unsigned int streamAddress = 0;

byte bufffer[STREAM_BUFFER_SIZE];
int bytesRead = 0;

void enableWrite(){ digitalWrite(WRITE_EN, LOW);}
void disableWrite(){ digitalWrite(WRITE_EN, HIGH);}


void writeEEPROMPage(byte page[], int sizee){
  setDataBusMode(OUTPUT);

  for(int i = 0; i < sizee; i++){
    setAddress(streamAddress++, false);
    writeDataBus(page[i]);

    disableWrite();
    enableWrite();
    delayMicroseconds(1);
    disableWrite();
  }

  delay(20);
  Serial.print(streamAddress + 1);
  Serial.print(" of ");
  Serial.print(EEPROM_SIZE);
  Serial.print(" (");
  Serial.print(((100L*streamAddress)/(EEPROM_SIZE - 1)));
  Serial.println("%)");
}


void recvCmdWithEndMarker() {
    static byte ndx = 0;
    
    char rc;
   
    while (Serial.available() > 0 && newCmd == false) {
        rc = Serial.read();
        Serial.print(rc);

        if (rc == BS || rc == DEL) { // doesn't handle tabs
          cmdData[ndx] = 0;
          if (ndx > 0) {
              ndx--;
              byte back[] = {BS,32, BS};
              Serial.write(back, 3);
          }
        } else if (rc != CR && rc != LF) {
            cmdData[ndx] = rc;
            ndx++;
            if (ndx >= cmdBufSize) {
                ndx = cmdBufSize - 1;
            }
        } else {
            Serial.println();
            cmdData[ndx] = '\0'; // terminate the string
            ndx = 0;
            newCmd = true;
        }
    }
}


void printMenu() {
    Serial.println("Options:");
    Serial.println("\t1 - Option 1");
    Serial.println("\t2 - Option 2");
    Serial.println("\t3 - Option 3");
}

void processCmd() {
    if (ctxCmd == 0) {
        char op = cmdData[0];
        switch(op) {
        case '1':
            disableSoftwareWriteProtect();
            break;
        case '2':
            enableSoftwareWriteProtect();
            break;
        case '3':
            ctxCmd = 3;
            break;
        case '9':
            ctxCmd = 9;
            break;
        default:
            Serial.println("-EEPROM Programmer----------\n");
            printMenu();
            break;
        }
    } 
    clearCmdBuffer();
}

void clearCmdBuffer() {
    newCmd = false;
    for (int i = 0; i < cmdBufSize; i++) {
       cmdData[i] = 0;
    }
}


void streamEEPROMBytes() {
    byte tempBuffer[STREAM_BUFFER_SIZE];    
    while (Serial.available() > 0 && streamAddress < EEPROM_SIZE) {
        int _bytesRead = Serial.readBytes(tempBuffer, STREAM_BUFFER_SIZE - bytesRead);
        
        memcpy(bufffer + bytesRead, tempBuffer, _bytesRead);

        bytesRead += _bytesRead;

        if (bytesRead == STREAM_BUFFER_SIZE && ((streamAddress + bytesRead) <= EEPROM_SIZE)) {
            writeEEPROMPage(bufffer, bytesRead);

            Serial.write(0x06);

            bytesRead = 0;
        }
    }
    
    if (streamAddress >= EEPROM_SIZE - 1) {
        delay(20);
        streamAddress = 0;
        ctxCmd = 0;
        bytesRead = 0;
        disableWrite();

        Serial.println("Finished writing EEPROM!");
    }
}


void setup() {
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    disableWrite();
    pinMode(WRITE_EN, OUTPUT);
    Serial.begin(57600);

}

void loop() {
  if(ctxCmd == 9){
    streamEEPROMBytes();
  }
  else if(ctxCmd == 3){
    printContents();
  }
  else{
    recvCmdWithEndMarker();
  }

  if(newCmd == true){
    processCmd();
  }
}


























// Output the address bits and outputEnable signal using shift registers.
void setAddress(int addr, bool outputEnable) {
    // Set the highest bit as the output enable bit (active low)
    if (outputEnable) {
        addr &= ~0x8000;
    } else {
        addr |= 0x8000;
    }
    byte dataMask = 0x04;
    byte clkMask = 0x08;
    byte latchMask = 0x10;

    // Make sure the clock is low to start.
    PORTD &= ~clkMask;

    // Shift 16 bits in, starting with the MSB.
    for (uint16_t ix = 0; (ix < 16); ix++)
    {
        // Set the data bit
        if (addr & 0x8000)
        {
            PORTD |= dataMask;
        }
        else
        {
            PORTD &= ~dataMask;
        }

        // Toggle the clock high then low
        PORTD |= clkMask;
        delayMicroseconds(3);
        PORTD &= ~clkMask;
        addr <<= 1;
    }

    // Latch the shift register contents into the output register.
    PORTD &= ~latchMask;
    delayMicroseconds(1);
    PORTD |= latchMask;
    delayMicroseconds(1);
    PORTD &= ~latchMask;
}



// Read a byte from the EEPROM at the specified address.
byte readEEPROM(int address) {
    setDataBusMode(INPUT);
    setAddress(address, /*outputEnable*/ true);
    return readDataBus();
}

// Write a byte to the EEPROM at the specified address.
void writeEEPROM(int address, byte data) {
    setAddress(address, /*outputEnable*/ false);
    setDataBusMode(OUTPUT);
    writeDataBus(data);
    enableWrite();
    delayMicroseconds(1);
    disableWrite();
    delay(10);
}
String number;
int inicio;
bool flag = true;
// Read the first 256 byte block of the EEPROM and dump it to the serial monitor.
void printContents() {
  while(Serial.available()>0){
    number = Serial.readString();
    inicio = number.toInt();
  }
  if(number.length() > 0 && flag == true){
    for (int base = inicio; base <= (inicio + 1023); base += 16) {
      byte data[16];
      for (int offset = 0; offset <= 15; offset += 1) {
        data[offset] = readEEPROM(base + offset);
      }

      char buf[80];
      sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

      Serial.println(buf);
      flag = false;
    } 
    Serial.println("Z");
  }
}

// Write the special six-byte code to turn off Software Data Protection.
void disableSoftwareWriteProtect() {
    disableWrite();
    setDataBusMode(OUTPUT);

    setByte(0xaa, 0x5555);
    setByte(0x55, 0x2aaa);
    setByte(0x80, 0x5555);
    setByte(0xaa, 0x5555);
    setByte(0x55, 0x2aaa);
    setByte(0x20, 0x5555);

    setDataBusMode(INPUT);
    delay(10);
}

// Write the special three-byte code to turn on Software Data Protection.
void enableSoftwareWriteProtect() {
    disableWrite();
    setDataBusMode(OUTPUT);

    setByte(0xaa, 0x5555);
    setByte(0x55, 0x2aaa);
    setByte(0xa0, 0x5555);

    setDataBusMode(INPUT);
    delay(10);
}

// Set the I/O state of the data bus.
// The 8 bits data bus are is on pins D5..D12.
void setDataBusMode(uint8_t mode) {
    // On the Uno and Nano, D5..D12 maps to the upper 3 bits of port D and the
    // lower 5 bits of port B.
    if (mode == OUTPUT) {
        DDRB |= 0x1f;
        DDRD |= 0xe0;
    } else {
        DDRB &= 0xe0;
        DDRD &= 0x1f;
    }
}

// Read a byte from the data bus.  The caller must set the bus to input_mode
// before calling this or no useful data will be returned.
byte readDataBus() {
    return (PINB << 3) | (PIND >> 5);
}

// Write a byte to the data bus.  The caller must set the bus to output_mode
// before calling this or no data will be written.
void writeDataBus(byte data) {
     PORTB = (PORTB & 0xe0) | (data >> 3);
     PORTD = (PORTD & 0x1f) | (data << 5);
}

// Set an address and data value and toggle the write control.  This is used
// to write control sequences, like the software write protect.  This is not a
// complete byte write function because it does not set the chip enable or the
// mode of the data bus.
void setByte(byte value, word address) {
    setAddress(address, false);
    writeDataBus(value);

    delayMicroseconds(1);
    enableWrite();
    delayMicroseconds(1);
    disableWrite();
}
