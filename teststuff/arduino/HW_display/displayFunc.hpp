#pragma once
#ifndef H_HW_DISPLAYFUNC
#define H_HW_DISPLAYFUNC


#include <Adafruit_Sensor.h>


void displaySensorDetails(){
    sensor_t sensor;
    accel.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print("Sensor: "); Serial.println(sensor.name);
    Serial.print("DriverVer: "); Serial.println(sensor.version);
    Serial.print("UniqueID: "); Serial.println(sensor.sensor_id);
    Serial.print("MaxValue: "); Serial.print(sensor.max_value); Serial.println("m/s^2");
    Serial.print("MinValue: "); Serial.print(sensor.min_value); Serial.println("m/s^2");
    Serial.print("Resolution: "); Serial.print(sensor.resolution); Serial.println("m/s^2");
    Serial.println("------------------------------------"); 
    Serial.println("");
    delay(500);
} 

void displayDataRate(){ 
    Serial.print("DataRate:");
    switch(accel.getDataRate()) {
        case ADXL345_DATARATE_3200_HZ:
        Serial.print("3200");
        break;
        case ADXL345_DATARATE_1600_HZ:
        Serial.print("1600");
        break;
        case ADXL345_DATARATE_800_HZ:
        Serial.print("800");
        break;
        case ADXL345_DATARATE_400_HZ:
        Serial.print("400");
        break;
        case ADXL345_DATARATE_200_HZ:
        Serial.print("200");
        break;
        case ADXL345_DATARATE_100_HZ:
        Serial.print("100");
        break;
        case ADXL345_DATARATE_50_HZ:
        Serial.print("50");
        break;
        case ADXL345_DATARATE_25_HZ:
        Serial.print("25");
        break;
        case ADXL345_DATARATE_12_5_HZ:
        Serial.print("12.5");
        break;
        case ADXL345_DATARATE_6_25HZ:
        Serial.print("6.25");
        break;
        case ADXL345_DATARATE_3_13_HZ:
        Serial.print("3.13");
        break;
        case ADXL345_DATARATE_1_56_HZ:
        Serial.print("1.56");
        break;
        case ADXL345_DATARATE_0_78_HZ:
        Serial.print("0.78");
        break;
        case ADXL345_DATARATE_0_39_HZ:
        Serial.print("0.39");
        break;
        case ADXL345_DATARATE_0_20_HZ:
        Serial.print("0.20");
        break;
        case ADXL345_DATARATE_0_10_HZ:
        Serial.print("0.10");
        break;
        default:
        Serial.print("????");
        break;
    }
    Serial.println("Hz");
}

void displayRange(void){
    Serial.print("Range:+/-");

    switch(accel.getRange()) {
        case ADXL345_RANGE_16_G: 
        Serial.print("16");
        break;
        case ADXL345_RANGE_8_G:
        Serial.print("8");
        break;
        case ADXL345_RANGE_4_G:
        Serial.print("4");
        break;
        case ADXL345_RANGE_2_G:
        Serial.print("2");
        break;
        default:
        Serial.print("??");
        break;
    }
    Serial.println("g");
}


#endif