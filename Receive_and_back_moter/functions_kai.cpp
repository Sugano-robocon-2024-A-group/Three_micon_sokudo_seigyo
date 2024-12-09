#include <Arduino.h>
#include <PS4Controller.h>
#include "functions_kai.h"

// モーターとエンコーダのピン設定
const uint8_t motor_pwm[2] =  {13, 4};
const uint8_t motor_dir1[2] = {25, 32};
const uint8_t motor_dir2[2] = {14, 33};

//Rb P上げる　Lb　Dゲイン　

// PID制御のゲイン 後ろ
const float kp[2] = {1.0, 1.0};//(0,1,2,3)  40  0.2  4,0.2 でControlの値が30
const float ki[2] = {0.0, 0.0};
const float kd[2] = {15.0, 15.0};//
int k[2]={120,110};

//PID制御に関する変数
float integral[2]={0,0};
float previous_error[2]={0,0};

/*
void handleEncoder(int index) {
    if (digitalRead(encoderA[index]) == digitalRead(encoderB[index])) {
        encoderCount[index]++;
    } else {
        encoderCount[index]--;
    }
}*/
float travelDistance=150.0;// 各ホイールの移動距離目標
float targetDistance[2]; // 各ホイールの移動距離目標


void init_pwm_setup() {
   for (int i = 0; i < 2; i++) {
        pinMode(motor_pwm[i], OUTPUT);
        pinMode(motor_dir1[i], OUTPUT);
        pinMode(motor_dir2[i], OUTPUT);
        //ledcAttach(motor_pwm[i], 5000, 8);
       //ledcSetup(i, 1000, 8); 
        //ledcAttachPin(motor_pwm[i], i);
        // 初期デューティを 0 に設定
        //ledcWrite(i, 0);
        ledcSetup(i, 1000, 8);  // チャンネルi、1kHz、8ビット解像度
        ledcAttachPin(motor_pwm[i], i);  // チャンネルiをピンに割り当て
        ledcWrite(i, 0);  // 初期デューティ比を0に設定
        
    }
}

/*float pidCompute(int index, float target, float current) {
    float error = target - current;
    integral[index] += error;
    float derivative = error - previous_error[index];
    float output = (kp[index] * error + ki[index] * integral[index] + kd[index] * derivative);
    previous_error[index] = error;
    Serial.printf("%f ",kp[index]);
    Serial.printf("%f ",ki[index]);
    Serial.printf("%f \n",kd[index]);
    Serial.printf("%f \n",output);
    //ここで整数にする。
    //int output_seisuu=output;
    
    return constrain(output, -250, 250);
}*/

float pidCompute(int index, float target, float current) {
    float error = target - current;
    integral[index] += error;
    float derivative = error - previous_error[index];
    float output = (kp[index] * error + ki[index] * integral[index] + kd[index] * derivative);
    previous_error[index] = error;

    if(output>0){
      output += k[index];
      }else if(output<0){
      output -= k[index];
      }
  
    //Serial.printf("%f \n",output);
    return constrain(output, -255, 255);//PWMの範囲に制約
}

void driveMotor(int index, float controlSignal) {
    if (controlSignal > 0.0) {
        digitalWrite(motor_dir1[index], HIGH);
        digitalWrite(motor_dir2[index], LOW); 
    } else if(controlSignal < 0.0){
        digitalWrite(motor_dir1[index], LOW);
        digitalWrite(motor_dir2[index], HIGH);
        controlSignal = -controlSignal;
    }else{//0だときっちり止まるようにした。
      /*digitalWrite(motor_dir1[index], LOW);
      digitalWrite(motor_dir2[index], LOW);*/
    }
    ledcWrite(index, (int)controlSignal);
}

void stopMotors() {
    for (int i = 0; i < 2; i++) {
        //ledcWrite(motor_pwm[i], 0);
        ledcWrite(i, 0); 
        digitalWrite(motor_dir1[i], LOW);
        digitalWrite(motor_dir2[i], LOW);
    }
}

void resetControlVariables() {
    for (int i = 0; i < 2; i++) {
        encoderCount[i] = 0;
        integral[i] = 0;
        previous_error[i] = 0;
        targetDistance[i] = 0;        
    }
}

//
void handleMoterInput(float* targetDistance, int command){
 if (command==3)
      {
        Serial.println("Right");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = travelDistance;
      }
      if (command==2)
      {
        Serial.println("Down");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = -travelDistance;
      }
      if (command==1)
      {
        Serial.println("Up");
        targetDistance[0] = travelDistance;
        targetDistance[1] = travelDistance;
        //delay(5000);
      }
      if (command==4)
      {
        Serial.println("Left");
        targetDistance[0] = travelDistance;
        targetDistance[1] = -travelDistance;
      }

      if (command==5)
      {
        Serial.println("Up Right");
        targetDistance[0] = 0.0;
        targetDistance[1] = travelDistance;
      }
      if (command==6)
      {
        Serial.println("Down Right");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = 0.0;
      }
      if (command==7)
      {
        Serial.println("Up Left");
        targetDistance[0] = travelDistance;
        targetDistance[1] = 0.0;
      }
      if (command==8)
      {
        Serial.println("Down Left");
        targetDistance[0] = 0.0;
        targetDistance[1] = -travelDistance;
      }
}

  /*
    
      */

      /* for (int i = 0; i < 4; i++)
  {
    // モーターピンを出力に設定
    pinMode(motor_pwm[i], OUTPUT);
    pinMode(motor_dir1[i], OUTPUT);
    pinMode(motor_dir2[i], OUTPUT);
  }

  // PWM設定
  for (int i = 0; i < 4; i++)
  {
    ledcAttach(motor_pwm[i], 1000, 8); // PWM出力ピン、周波数5000Hz、8ビット解像度
  }*/

/*
void init_encoder_setup() {
    for (int i = 0; i < 4; i++) {
        pinMode(encoderA[i], INPUT_PULLUP);
        pinMode(encoderB[i], INPUT_PULLUP);
    }
}*/
