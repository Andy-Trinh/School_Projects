#include <stdio.h>
#include <stdint.h>
#include "eecs388_lib.h"
#include "metal/i2c.h"


struct metal_i2c *i2c;
uint8_t bufWrite[9];
uint8_t bufRead[1];


//The entire setup sequence
void set_up_I2C(){
    uint8_t oldMode;
    uint8_t newMode;
    _Bool success;


    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = MODE1_RESTART;
    printf("%d\n",bufWrite[0]);
    
    i2c = metal_i2c_get_device(0);

    if(i2c == NULL){
        printf("Connection Unsuccessful\n");
    }
    else{
        printf("Connection Successful\n");
    }
    
    //Setup Sequence
    metal_i2c_init(i2c,I2C_BAUDRATE,METAL_I2C_MASTER);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//reset
    delay(100);
    printf("resetting PCA9685 control 1\n");

    //Initial Read of control 1
    bufWrite[0] = PCA9685_MODE1;//Address
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Read success: %d and control value is: %d\n", success, bufWrite[0]);
    
    //Configuring Control 1
    oldMode = bufRead[0];
    newMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
    printf("sleep setting is %d\n", newMode);
    bufWrite[0] = PCA9685_MODE1;//address
    bufWrite[1] = newMode;//writing to register
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sleep
    bufWrite[0] = PCA9685_PRESCALE;//Setting PWM prescale
    bufWrite[1] = 0x79;
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//sets prescale
    bufWrite[0] = PCA9685_MODE1;
    bufWrite[1] = 0x01 | MODE1_AI | MODE1_RESTART;
    printf("on setting is %d\n", bufWrite[1]);
    success = metal_i2c_write(i2c,PCA9685_I2C_ADDRESS,2,bufWrite,METAL_I2C_STOP_DISABLE);//awake
    delay(100);
    printf("Setting the control register\n");
    bufWrite[0] = PCA9685_MODE1;
    success = metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,1,bufRead,1);//initial read
    printf("Set register is %d\n",bufRead[0]);

}

void breakup(int bigNum, uint8_t* low, uint8_t* high){
    *low = bigNum & 0xFF;
    *high = bigNum >> 8;
}

void steering(int angle){
    int valueToBreak = getServoCycle(angle);

    uint8_t low;
    uint8_t high;

    breakup(valueToBreak, &low, &high);

    bufWrite[0] = PCA9685_LED0_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    bufWrite[3] = low;
    bufWrite[4] = high;

    metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
}

void stopMotor(){
    uint8_t low;
    uint8_t high;

    breakup(280, &low, &high);

    bufWrite[0] = PCA9685_LED1_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    bufWrite[3] = low;
    bufWrite[4] = high;

    metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
}

void driveForward(uint8_t speedFlag){
    int speed;

    switch (speedFlag) {
        case 1:
            speed = 313;
            break;
        case 2:
            speed = 315;
            break;
        case 3:
            speed = 317;
            break;
        default:
            speed = 313;
            break;
    }

    uint8_t low;
    uint8_t high;

    breakup(speed, &low, &high);

    bufWrite[0] = PCA9685_LED1_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    bufWrite[3] = low;
    bufWrite[4] = high;

    metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
}

void driveReverse(uint8_t speedFlag){
    int speed;

    switch (speedFlag) {
        case 1:
            speed = 267;
            break;
        case 2:
            speed = 265;
            break;
        case 3:
            speed = 263;
            break;
        default:
            speed = 267;
            break;
    }

    uint8_t low;
    uint8_t high;

    breakup(speed, &low, &high);

    bufWrite[0] = PCA9685_LED1_ON_L;
    bufWrite[1] = 0x00;
    bufWrite[2] = 0x00;
    bufWrite[3] = low;
    bufWrite[4] = high;

    metal_i2c_transfer(i2c,PCA9685_I2C_ADDRESS,bufWrite,5,bufRead,1);
}

int main()
{
    set_up_I2C();

    stopMotor();

    delay(2000);

    steering(-45);
    driveForward(1);

    delay(2000);

    steering(45);

    delay(2000);

    stopMotor();

    delay(2000);

    driveReverse(1);

    delay(2000);

    steering(-45);

    delay(2000);

    stopMotor();
    
    //Fully Controlling the PCA9685
    /*
        -Task 6: using previously defined functions, 
        perform the following sequence of actions
        
        -Configure the motors (wait for 2 seconds)
        -Set the steering heading to 0 degrees 
        -Drive forward (wait for 2 seconds)
        -Change the steering heading to 20 degrees (wait for 2 seconds)
        -Stop the motor (wait for 2 seconds)
        -Drive forward (wait for 2 seconds)
        -Set steering heading to 0 degrees (wait for 2 seconds)
        -Stop the motor
    */

}
