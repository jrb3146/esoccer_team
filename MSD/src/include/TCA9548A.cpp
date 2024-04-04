/*   Version: 1.1.3  |  License: Apache 2.0  |  Author: JDWifWaf@gmail.com   */
#include <TCA9548A.h>

// #define DEBUG 0  // Enable Debug Mode

TCA9548A::TCA9548A(uint8_t address) : _address(address) {}

void TCA9548A::begin(TwoWire &inWire)
{
  // this->myWire = &Wire;
    //this->myWire = &inWire;
    //this->myWire->begin();
    //this->myWire->setClock(100000); //400khz clock
}

void TCA9548A::openChannel(uint8_t channel)
{
    uint8_t buff = 0x00;    
    buff = 1 << channel;
    this->_channels |= buff;

    #if DEBUG
    Serial.print("Open Channel: "); Serial.print(channel); Serial.print(" Data: "); Serial.println(buff);
    #endif  

    write(this->_channels);
}

void TCA9548A::closeChannel(uint8_t channel)
{
    uint8_t buff = 0x00;    
    buff = 1 << channel;    
    this->_channels ^= buff;

    #if DEBUG
    Serial.print("Close Channel: "); Serial.print(channel); Serial.print(" Data: "); Serial.println(buff);
    #endif  

    write(this->_channels);
}

void TCA9548A::closeAll()
{
    this->_channels = 0x00;
    write(this->_channels);
}

void TCA9548A::openAll()
{
    this->_channels = 0xFF;
    write(this->_channels);
}

void TCA9548A::scanChannel(int channel)
{
    this->openChannel(channel); // Change communication channel
    
    byte error, address;
    int numDevices;
    Serial.println("Scanning I2C Mux...");
    delay(100);
    numDevices = 0;
    for (address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      delay(50);
      error = Wire.endTransmission();
      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.print(address, HEX);
        Serial.print(" on channel ");
        Serial.print(channel);
        Serial.println(" of the multiplexer TCA9548");
        numDevices++;
        delay(50);
      } else if (error == 4) {
        Serial.print("Unknown error at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      }
    }
    if (numDevices == 0)
      Serial.println("No I2C devices found");
    
    
    this->closeChannel(channel); // Change communication channel
    delay(100);
    
}


void TCA9548A::scanAll()
{
    this->closeAll(); // ensure channel is closed
    for (int ch = 0; ch < 8; ch++) {

      this->scanChannel(ch);
    }
    this->closeAll(); // ensure channel is closed
}

void TCA9548A::writeRegister(uint8_t value)
{
    this->_channels = value;

    #if DEBUG
    Serial.print("Write Control Register: "); Serial.println(value);
    #endif  

    write(this->_channels);
}

void TCA9548A::write(uint8_t inData)
{
    #if DEBUG
    Serial.print("Write Address: "); Serial.print(this->_address, HEX); Serial.print(" Data: "); Serial.println(inData);
    #endif

    Wire.beginTransmission(this->_address);
    Wire.write(inData);
    Wire.endTransmission(true);
}

uint8_t TCA9548A::read()
{
    uint8_t buff = 0;
    Wire.requestFrom((uint16_t)this->_address, (uint8_t)1, (uint8_t)true);

    if(!Wire.available())
        return 255;

    buff = Wire.read();

    #if DEBUG
    Serial.print("Read Address: "); Serial.print(this->_address, HEX); Serial.print(" Data: "); Serial.println(buff);
    #endif
    
    return buff;
}
