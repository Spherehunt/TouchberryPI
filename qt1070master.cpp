// Started from http://elinux.org/Interfacing_with_I2C_Devices
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;

enum Qt1070Register {
  CHIP_ID = 0,
  FIRMWARE = 1,
  KEY_STATUS = 3,
  RESET = 57
};

enum TouchBerryPiShieldKey {
  UP = 0x04,
  DOWN = 0x08,
  LEFT = 0x01,
  RIGHT = 0x02,
  A = 0x20,
  B = 0x10,
  X = 0x40
};

void slaap(int ms)
{
this_thread::sleep_for(chrono::milliseconds(ms));
}


bool write_register_address(int file, char registerAddress) {
  char writeBuffer[] = { registerAddress };

  // First write address
  if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
      printf("Failed to write to the i2c bus.\r\n");
      return false;
  }

  return true;
}


bool read_register(int file, char registerAddress, char * value) {

    if (!write_register_address(file, registerAddress)) {
      return false;
    }

    slaap(200);

    char readBuffer[1];
    if (read(file, readBuffer, sizeof(readBuffer)) != sizeof(readBuffer)) {
        printf("Failed to read from the i2c bus.\r\n");
        return false;
    } else {
        *value = readBuffer[0];
    }
    return true;
}

bool reset_device(int file) {
    char writeBuffer[] = { RESET };

    // First write address
    if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
        printf("Failed to write to the i2c bus.\r\n");
        return false;
    }
    return true;
}

int writefile(string instruction)
{
        ofstream thumperfile;
        thumperfile.open("/tmp/touch/thumper");
        thumperfile << instruction;
        thumperfile.close();
        return 0;
}


int main(void) {
    int file;
    char filename[40];
    int slaveAddress = 0x1B;        // The 7-bit I2C address of the qt1070 emulator device

    sprintf(filename, "/dev/i2c-1");
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully openend i2c-1 bus\r\n");
    }

    if (ioctl(file, I2C_SLAVE, slaveAddress) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully aquired i2c-1 bus\r\n");
    }

    slaap(1000);

    printf("Writing register address\r\n");
    write_register_address(file, FIRMWARE);

    slaap(1000);


    char chipid = 0x00;
    if (!read_register(file, CHIP_ID, &chipid)) {
      printf("Failed to read chip id\r\n");
      return -1;
    }
    printf("Chip id = 0x%x\r\n", chipid);



    slaap(1000);

    char firmware = 0x00;
    if (!read_register(file, FIRMWARE, &firmware)) {
      printf("Failed to read firmware version\r\n");
      return -1;
    }
    printf("Firmware version = 0x%x\r\n", firmware);

    // return 0;
    // // BREAK ///////////////////////////////////////////////////////////////////

    slaap(1000);

    if (!reset_device(file)) {
      printf("Failed to reset device\r\n");
      return -1;
    }
    printf("Device is resetting\r\n");
    slaap(1000);
    printf("Device is ready\r\n");

    bool keepReading = true;
	int  state = 0;
    while (keepReading) {
      char keys = 0x00;
      
      if (!read_register(file, KEY_STATUS, &keys)) {
        printf("Failed to read firmware version\r\n");
      }
      // printf("Key status = 0x%x\r\n", keys);
	 switch (keys) {
        case UP:
	  state = 1;
          printf("UP ");
	  writefile("forward");
          break;
        case LEFT:
	  state = 1;
          printf("LEFT ");
	  writefile("left");
          break;
        case DOWN:
	  state = 1;
          printf("DOWN ");
	  writefile("reverse");
          break;
        case RIGHT:
	  state = 1;
          printf("RIGHT ");
	  writefile("right");
          break;
        case A:
	  state = 1;
          printf("A ");
	  writefile("alarm");
          break;
        case B:
	  state = 1;
          printf("B ");
	  writefile("strobe");
          break;
        case X:
	  state = 1;
          printf("X ");
	  writefile("randomshift");
          break;
	default:
          if (state == 0){break;}
	  else{
	  writefile("stop");
	  state = 0;
	  printf("Stop");
	  break;}
      }
      fflush(stdout);

      slaap(200);
    }

    printf("Done\r\n");

    return 0;
}

