//edge detection using IR sensors

#include <QTRSensors.h>

#define NUM_SENSORS             4  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             255  // emitter is 255

#define STARTPIN                32
#define INDICATORPIN            34

//Analog Pins sensor assignment
#define FR              2
#define FL              3
#define BL              4
#define BR              5

//Standard PWM DC control
int S1 = 3;     //M1 Speed Control PWM char (0-255)
int D1 = 12;    //M1 Direction Control
int S2 = 11;    //M2 Speed Control PWM
int D2 = 13;    //M2 Direction Control

int mspd = 88, dur1 = 100, dur2 = 200, dur3 = 500, dur4 = 1000;
long rNum;
int rem;

QTRSensorsAnalog qtra((unsigned char[]) {FR, FL, BL, BR}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

void mstop(){                    //Stop
  analogWrite(S1,0);   
  analogWrite(S2,0);      
}   

void mmove(char dir, char spdR, char spdL, int duration){
  switch(dir){
    case 'F':
      digitalWrite(D1,HIGH);    
      digitalWrite(D2,HIGH);
      break;
    case 'B':
      digitalWrite(D1,LOW);   
      digitalWrite(D2,LOW);
      break;
    case 'L':
      digitalWrite(D1,HIGH);   
      digitalWrite(D2,LOW);
      break;            
    case 'R':
      digitalWrite(D1,LOW);   
      digitalWrite(D2,HIGH);
      break;            
  }
  analogWrite(S1, spdR);
  analogWrite(S2, spdL);
  delay(duration);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(STARTPIN, INPUT);
  pinMode(INDICATORPIN, OUTPUT);

  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  
  Serial.begin(9600);

  randomSeed(analogRead(0));

  Serial.println("Waiting to start...");
  while( digitalRead(STARTPIN) == LOW ){}

  Serial.println("Robot start");
  //indicate with lights to start
  for(int i=0; i<3; i++){
    delay(250);
    digitalWrite(INDICATORPIN, HIGH);
    delay(250);
    digitalWrite(INDICATORPIN, LOW);
  }
  delay(2000);

  Serial.println("Edge Detect loop start");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //drive robots and check for off edge signals
  qtra.read(sensorValues);

  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(); 

  rNum = random(1000,4000);
  //no edges detected
  if( sensorValues[FR-2] <= 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] <= 500 ){
    //drive forward
    mmove('F', mspd, mspd, dur1);
  }

  //front has edge
  else if ( sensorValues[FR-2] > 500 && sensorValues[FL-2] > 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] <= 500 ){
    //reverse until edge not detected, turn right 90 deg    
    mmove('B', mspd, mspd, dur3);
    if ( rNum > 4000){
      mmove('R', mspd, mspd, 3000);
    }
    rem = 1000;
    qtra.read(sensorValues);
    while( sensorValues[BL-2] <= 500 && sensorValues[BR-2] <= 500 && rem ){ 
      mmove('R', mspd, mspd, 100);
      rem -=100;
    }
  }
  //front right has edge
  else if ( sensorValues[FR-2] > 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] <= 500 ){
    //reverse until edge not detected, turn left 45 deg
    mmove('B', mspd, mspd, dur3);
    if ( rNum > 4000){
      mmove('L', mspd, mspd, 3000);
    }
    rem = 1000;
    qtra.read(sensorValues);
    while( sensorValues[BR-2] <= 500 && rem ){ 
      mmove('L', mspd, mspd, 100);
      rem -=100;
    }
  }
  //front left has edge
  else if ( sensorValues[FR-2] <= 500 && sensorValues[FL-2] > 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] <= 500 ){
    //reverse until edge not detected, turn right 45 deg    
    mmove('B', mspd, mspd, dur3);
    if ( rNum > 4000){
      mmove('R', mspd, mspd, 3000);
    }
    rem = 1000;
    qtra.read(sensorValues);
    while( sensorValues[BL-2] <= 500 && rem ){ 
      mmove('R', mspd, mspd, 100);
      rem -=100;
    }
  }
  
  //back has edge
  else if ( sensorValues[FR-2] <= 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] > 500 && sensorValues[BR-2] > 500 ){
    //forward until edge not detected    
    mmove('F', mspd, mspd, dur1);
  }
  //back right has edge
  else if ( sensorValues[FR-2] <= 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] > 500 ){
    //forward until edge not detected, turn left 45 deg
    mmove('F', mspd, mspd, dur3);
    mmove('L', mspd, mspd, rNum);
  }
  //back left has edge
  else if ( sensorValues[FR-2] <= 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] > 500 && sensorValues[BR-2] <= 500 ){
    //forward until edge not detected, turn right 45 deg    
    mmove('F', mspd, mspd, dur3);
    mmove('R', mspd, mspd, rNum);
  }

//NOTE: These cases should not happen
  //right side has edge
  else if ( sensorValues[FR-2] > 500 && sensorValues[FL-2] <= 500 && sensorValues[BL-2] <= 500 && sensorValues[BR-2] > 500 ){
    //stop robot    
    mstop();
  }

  //left side has edge
  else if ( sensorValues[FR-2] <= 500 && sensorValues[FL-2] > 500 && sensorValues[BL-2] > 500 && sensorValues[BR-2] <= 500 ){
    //stop robot
    mstop();
  }

  //all sides have edges
  else if ( sensorValues[FR-2] > 500 && sensorValues[FL-2] > 500 && sensorValues[BL-2] > 500 && sensorValues[BR-2] > 500 ){
    //stop robot
    mstop();
  }

  //delay(1000);

}
