// Created by Mir Mikael
// For quastions: MirMikael.github.io

#include <MPU9250_asukiaaa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EEPROM.h>
#include <math.h>
MPU9250_asukiaaa mySensor;

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif
#define CALIB_SEC 20

float GyX, GyY, GyZ, AcX, AcY, AcZ, mgX, mgZ, mgY;
float pitch, roll, yaw;
const int W1_ROWS = 6, W1_COLS = 6, W2_ROWS = 2, W2_COLS = 6;
const int matrixSize1 = W1_ROWS * W1_COLS * sizeof(float);
const int matrixSize2 = W2_ROWS * W2_COLS * sizeof(float);
const int addressMatrix1 = 0;
const int addressMatrix2 = matrixSize2;

// Constants for w1 and w2 arrays
const float w1_values[W1_ROWS][W1_COLS] = {
  {0.8679, 0.8725, -0.6096, -0.1303, -0.1119, -0.0937},
  {0.5902, 0.3545, 0.5575, 0.4289, 0.3481, 0.3718},
  { -0.4553, 0.7235, 0.6340, 0.0596, 0.0612, 0.0484},
  {0.2743, 0.5043, 0.4609, 0.1839, 0.1200, 0.2636},
  { -0.6980, 0.4602, -0.6148, -0.0639, -0.0581, -0.0625},
  {0.6863, 0.6341, 0.3722, 0.5057, 0.4765, 0.3595}
};
const float w2_values[W2_ROWS][W2_COLS] = {
  { -0.4684, -0.5310, 0.7533, 0.2323, 0.6046, -0.2156},
  {0.4162, -0.0499, 0.5606, 0.2189, 0.8406, 0.4740}
};

float out0[W1_COLS], net1[W1_ROWS], out1[W1_ROWS], out2[W2_ROWS], e[W2_ROWS], buffIN[W1_COLS][2200], buffOUT[W2_ROWS][2200];

// Function for sech calculation
inline float sech(float x) {
  return 1.0f / cosh(x);
}

// MLP
void Forward() {
  for (int i = 0; i < W1_ROWS; i++) {
    net1[i] = 0;
    for (int j = 0; j < W1_COLS; j++) {
      net1[i] += w1[i][j] * out0[j];
    }
    out1[i] = tanh(net1[i]);
  }

  for (int i = 0; i < W2_ROWS; i++) {
    out2[i] = 0;
    for (int j = 0; j < W2_COLS; j++) {
      out2[i] += w2[i][j] * out1[j];
    }
  }
}

void Backward() {
  float sech_net1[W1_ROWS];

  for (int j = 0; j < W1_ROWS; j++) {
    sech_net1[j] = sech(  net1[j]);
  }

  // Update w2
  for (int ii = 0; ii < W2_ROWS; ii++) {
    for (int j = 0; j < W2_COLS; j++) {
      w2[ii][j] += e[ii] * out1[j] * eta;
    }
  }

  // Update w1
  for (int ii = 0; ii < W1_COLS; ii++) {
    for (int j = 0; j < W1_ROWS; j++) {
      float sum = 0;
      for (int k = 0; k < W2_ROWS; k++) {
        sum += e[k] * w2[k][j];
      }
      w1[j][ii] += eta * sum * sech_net1[j] * sech_net1[j] * out0[ii];
    }
  }
}

//normalize
float normalize(float * a, float * b, float * c) {

  float length_2, inv_length, xx;
  long ii;
  const float threehalfs = 1.5F;

  length_2 = (*a * *a) + (*b * *b) + (*c * *c);

  //inv_length=0.102; // 1/9.8 --> this would be a huge mistake; it only works in stand-still state, note even a bit of movement!(m.mashaghi)

  xx = length_2 * 0.5F;
  inv_length  = length_2;

  ii  = * ( long * ) &inv_length;                       // evil floating point bit level hacking
  ii  = 0x5f3759df - ( ii >> 1 );               // what the fuck?
  inv_length  = * ( float * ) &ii;
  inv_length  = inv_length * ( threehalfs - ( xx * inv_length * inv_length ) );   // 1st iteration
  //  inv_length  = inv_length * ( threehalfs - ( xx * inv_length * inv_length ) );   // 2nd iteration, this can be removed

  *a *= inv_length;
  *b *= inv_length;
  *c *= inv_length;


  return (inv_length); //--> ino badan ezafe kardam ke beshe fahmid hodoodan cheghadr shetabe dynamic darim...
}

//IMU data readind
void IMUdata() {

  AcZ = 0; AcY = 0; AcX = 0;
  GyX = 0; GyY = 0; GyZ = 0;

  int i = 0;
  for (i = 0; i < 6; i++) {
    mySensor.accelUpdate();
    AcZ += mySensor.accelZ();
    AcY += mySensor.accelY();
    AcX += mySensor.accelX();

    mySensor.gyroUpdate();
    GyX += mySensor.gyroX();
    GyY += mySensor.gyroY();
    GyZ += mySensor.gyroZ();

    //    mySensor.magUpdate();
    //    mgX += mySensor.magX();
    //    mgY += mySensor.magY();
    //    mgZ += mySensor.magZ();

  }

  float invIt = 1.0f / i;
  AcZ *= invIt;
  AcY *= invIt;
  AcX *= invIt;
  GyX *= invIt;
  GyY *= invIt;
  GyZ *= invIt;

  //  mgX *= invIt;
  //  mgY *= invIt;
  //  mgZ *= invIt;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(matrixSize1 + matrixSize2);

#ifdef _ESP32_HAL_I2C_H_
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#else
  Wire.begin();
  mySensor.setWire(&Wire);
#endif

  mySensor.beginAccel();
  mySensor.beginGyro();
  //  mySensor.beginMag();

  for (int_cal = 0; int_cal < 2200; int_cal++) {

    IMUdata();

    normalize(&AcX, &AcY, &AcZ);
    pitch = -(atan2(AcX, sqrt(AcY * AcY + AcZ * AcZ)));
    roll  = (atan2(AcY, sqrt(AcX * AcX + AcZ * AcZ)));

    normalize(&GyX, &GyY, &GyZ);
    pitch = (kalmanY.update(pitch, sqrt(GyY * GyY + GyZ * GyZ)));
    roll = (kalmanX.update(roll, sqrt(GyX * GyX + GyZ * GyZ)));

    buffIN[0][int_cal] = AcX;  buffIN[1][int_cal] = AcY; buffIN[2][int_cal] = AcZ;
    buffIN[3][int_cal] = GyX;  buffIN[4][int_cal] = GyY; buffIN[5][int_cal] = GyZ;
    buffOUT[0][int_cal] = pitch;
    buffOUT[1][int_cal] = roll;

    Forward();
    e[0] = pitch - out2[0];
    e[1] = roll - out2[1];
    Backward();
  }

  Serial.print("time= "); Serial.println((micros() - dt1) * 0.000001);
  for (int iI = 0; iI < 12; iI++) {
    for (int j = 0; j < 2200; j++) {
      for (int i = 0; i <= 5; i++) {
        out0[i] = buffIN[i][j];
      }
      pitch = buffOUT[0][j];
      roll = buffOUT[1][j];
      Forward();
      e[0] = pitch - out2[0];
      e[1] = roll - out2[1];
      Backward();
    }
  }
  Serial.print("end time= "); Serial.println((micros() - dt1) * 0.000001);

  Serial.print("New w1= ");
  for (int i = 0; i < W1_ROWS; i++) {
    for (int j = 0; j < W1_COLS; j++) {
      Serial.print(w1[i][j]);
      Serial.print('\t');
    }
    Serial.println();
  }
  Serial.print("New w2= ");
  for (int i = 0; i < W2_ROWS; i++) {
    for (int j = 0; j < W2_COLS; j++) {
      Serial.print(w2[i][j]);
      Serial.print('\t');
    }
    Serial.println();
  }
}
void loop() {
    
}
