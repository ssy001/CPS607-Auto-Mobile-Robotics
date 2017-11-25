#include <QTRSensors.h>

#define NUM_SENSORS             5  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             255  // emitter is 255

#define STARTPIN                48
#define INDICATORPIN            49

//Analog Pins sensor assignment (from left to right)
#define SEN1              2      //back left
#define SEN2              3      //front left
#define SEN3              4      //center
#define SEN4              5      //front right
#define SEN5              6      //back right

//Standard PWM DC control
int S1 = 3;     //M1 Speed Control PWM - Left wheel/motor
int D1 = 12;    //M1 Direction Control
int S2 = 11;    //M2 Speed Control PWM - Right wheel/motor  
int D2 = 13;    //M2 Direction Control

#define mspd1  80
#define mspd2  80
#define dur1   10
#define slimith 500
#define slimitl 250

#define FORWARD    0
#define SEEK 1
#define BACKWARD   2
#define DONE       3
int mode = DONE;

QTRSensorsAnalog qtra((unsigned char[]) {SEN1,SEN2,SEN3,SEN4,SEN5}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int senVal[NUM_SENSORS];

void mstop(){                    
  analogWrite(S1,0);   
  analogWrite(S2,0);      
  digitalWrite(D1,LOW);   
  digitalWrite(D2,LOW);
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
      digitalWrite(D1,LOW);   
      digitalWrite(D2,HIGH);
      break;            
    case 'R':
      digitalWrite(D1,HIGH);   
      digitalWrite(D2,LOW);
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
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  
  Serial.begin(9600);

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

  Serial.println("Line Detect loop start");
  mode = FORWARD;
}

void loop() {
  // put your main code here, to run repeatedly:
  //drive robots and check for line deviation signals
  
//////////----------//////////
  if(mode == FORWARD){
    qtra.read(senVal);
    //black line @ center detected - case 1
    while( senVal[SEN2-2]<=100 && senVal[SEN3-2]>700 && senVal[SEN4-2]<=100 ){
      mmove('F', mspd1, mspd1, dur1);
      qtra.read(senVal);
    }
    //right
    while( senVal[SEN2-2]<=100 && senVal[SEN3-2]<=200 && senVal[SEN4-2]>700 ){    
      mmove('R', mspd2, mspd2, dur1);
      qtra.read(senVal);
    }
    while( senVal[SEN2-2]<=100 && senVal[SEN3-2]>200 && senVal[SEN4-2]>200 ){
      mmove('R', mspd2, mspd2, dur1);
      qtra.read(senVal);
    }
//    while( senVal[SEN2-2]<=300 && senVal[SEN3-2]>700 && senVal[SEN4-2]>300 ){
//      mmove('R', mspd2, mspd2, dur1);
//      qtra.read(senVal);
//    }
    //left
    while( senVal[SEN2-2]>700 && senVal[SEN3-2]<=200 && senVal[SEN4-2]<=100 ){
      mmove('L', mspd2, mspd2, dur1);
      qtra.read(senVal);
    }
    while( senVal[SEN2-2]>200 && senVal[SEN3-2]>200 && senVal[SEN4-2]<=100 ){
      mmove('L', mspd2, mspd2, dur1);
      qtra.read(senVal);
    }
//    while( senVal[SEN2-2]>300 && senVal[SEN3-2]>700 && senVal[SEN4-2]<=300 ){
//      mmove('L', mspd2, mspd2, dur1);
//      qtra.read(senVal);
//    }
    while( senVal[SEN2-2]>300 && senVal[SEN3-2]>700 && senVal[SEN4-2]>300 ){
      if(senVal[SEN2-2] > senVal[SEN4-2]){
        mmove('L', mspd2, mspd2, dur1);
        delay(100);
      }
      else{
        mmove('R', mspd2, mspd2, dur1);
        delay(100);        
      }
      qtra.read(senVal);
    }
    //FINALLY: no black line detected - end of forward stage
    if ( senVal[SEN2-2]<=100 && senVal[SEN3-2]<=100 && senVal[SEN4-2]<=100 ){
      mode = SEEK;        
    }
  }
//////////----------//////////
  else if (mode == SEEK){
    //mmove('B', mspd1, mspd1, dur1);
    //delay(300);
    qtra.read(senVal);
    //black line detected @center
    while( !(senVal[SEN2-2]<=100 && senVal[SEN3-2]>700 && senVal[SEN4-2]<=100) ){
      mmove('R', mspd1, mspd1, dur1);
      qtra.read(senVal);
    }
    mode = FORWARD;
  }
//////////----------//////////
  else if (mode == DONE){
    mstop();
  }
}
