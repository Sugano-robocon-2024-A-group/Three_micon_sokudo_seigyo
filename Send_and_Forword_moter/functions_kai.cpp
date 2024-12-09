#include <Arduino.h>
#include <PS4Controller.h>
#include "functions_kai.h"

// モーターとエンコーダのピン設定
const uint8_t motor_pwm[2] =  {21, 16};
const uint8_t motor_dir1[2] = {18, 22};
const uint8_t motor_dir2[2] = {19, 23};

//PWMのSetUP
//pinMode( 端子番号 , OUTPUT ) ;　// PWM出力を行う端子を出力端子として設定
//ledcSetup ( チャンネル , 周波数 , bit数(分解能) ) ;　//PWM出力波形の初期設定
//ledcAttachPin( 端子番号 , チャンネル ) ;

//Rf P上げる　Lf　Dゲイン　

// PID制御のゲイン 前
//
const float kp[2] = {0.35, 0.5};//(0,1,2,3)  40  0.2  4,0.2 でControlの値が30
const float ki[2] = {0.21, 0.1};//0.2,0.3
const float kd[2] = {1.3, 2.0};//1.2 1.5  //1.3、1.6だとオーバー 1.22 1.52だともっと振動する 1.3,1.4でも振動、、。
int k[2]={70,70};
/*
 * const float kp[2] = {1.0, 1.0};//(0,1,2,3)  40  0.2  4,0.2 でControlの値が30
const float ki[2] = {0.0, 0.0};
const float kd[2] = {15.0, 15.0};
int k[2]={130,105};
 */

//PID制御に関する変数
float integral[2]={0,0};
float previous_error[2]={0,0};

/*
 *const float kp[2] = {1.0, 1.0};//(0,1,2,3)  40  0.2  4,0.2 でControlの値が30
const float ki[2] = {0.0, 0.0};
const float kd[2] = {15.0, 15.0};
int k[2]={130,105};
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
 if (PS4.Right())
      {
        Serial.println("Right");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = travelDistance;
      }
      if (PS4.Down())
      {
        Serial.println("Down");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = -travelDistance;
      }
      if (PS4.Up())
      {
        Serial.println("Up");
        targetDistance[0] = travelDistance;
        targetDistance[1] = travelDistance;
      }
      if (PS4.Left())
      {
        Serial.println("Left");
        targetDistance[0] = travelDistance;
        targetDistance[1] = -travelDistance;
      }

      if (PS4.UpRight())
      {
        Serial.println("Up Right");
        targetDistance[0] = 0.0;
        targetDistance[1] = travelDistance;
      }
      if (PS4.DownRight())
      {
        Serial.println("Down Right");
        targetDistance[0] = -travelDistance;
        targetDistance[1] = 0.0;
      }
      if (PS4.UpLeft())
      {
        Serial.println("Up Left");
        targetDistance[0] = travelDistance;
        targetDistance[1] = 0.0;
      }
      if (PS4.DownLeft())
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
