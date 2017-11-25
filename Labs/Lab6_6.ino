#include <DistanceGP2Y0A41SK.h>
#include <NewPing.h>

#define SONAR_NUM     3 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define mspd1  100
#define mspd2  120
#define mdur1   10

long randdur;
unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.
unsigned int Ldist, Cdist, Rdist;
const unsigned int distlim = 20, sdistlim = 12; //threshold for obstacles (in cm)

//Standard PWM DC control
int S1 = 3;     //M1 Speed Control PWM - Left wheel/motor
int D1 = 12;    //M1 Direction Control
int S2 = 11;    //M2 Speed Control PWM - Right wheel/motor  
int D2 = 13;    //M2 Direction Control

int pingPinT1  = 40;  //left sensor
int pingPinE1  = 41;
int pingPinT2  = 42;  //center sensor
int pingPinE2  = 43;
int pingPinT3  = 44;  //right sensor
int pingPinE3  = 45;

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(pingPinT1, pingPinE1, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(pingPinT2, pingPinE2, MAX_DISTANCE),
  NewPing(pingPinT3, pingPinE3, MAX_DISTANCE),
};

DistanceGP2Y0A41SK SDistL, SDistR;
int sdl, sdr;

// Robot internal map - 5x5 maze
char LHW[11][11] = { {'.', '-', '.', '-', '.', '-', '.', '-', '.', '-', '.'},
                     {'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'},
                     {'.', ' ', '.', ' ', '.', ' ', '.', ' ', '.', ' ', '.'},
                     {'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'},
                     {'.', ' ', '.', ' ', '.', ' ', '.', ' ', '.', ' ', '.'},
                     {'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'},
                     {'.', ' ', '.', ' ', '.', ' ', '.', ' ', '.', ' ', '.'},
                     {'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'},
                     {'.', ' ', '.', ' ', '.', ' ', '.', ' ', '.', ' ', '.'},
                     {'|', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '|'},
                     {'.', '-', '.', '-', '.', '-', '.', '-', '.', '-', '.'} };
                     
void mstop(){                    
  analogWrite(S1,0);   
  analogWrite(S2,0);      
  digitalWrite(D1,LOW);   
  digitalWrite(D2,LOW);
}   

void mmove(char dir, char spdL, char spdR, int duration){
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
  analogWrite(S1, spdL);
  analogWrite(S2, spdR);
  delay(duration);
}

void setup() {
  Serial.begin(115200);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  SDistL.begin(A8);
  SDistR.begin(A9);
  randomSeed(analogRead(0));
  Serial.println("5sec delay b4 start...");
  delay(5000);
  Serial.println("Robot start");
}

void loop() {

  Ldist = sonar[0].ping()/ US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS) and convert to cm
  Cdist = sonar[1].ping()/ US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS) and convert to cm
  Rdist = sonar[2].ping()/ US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS) and convert to cm
  sdl = SDistL.getDistanceCentimeter();
  sdr = SDistR.getDistanceCentimeter();

  if((Ldist > distlim || Ldist == 0) && (Cdist > distlim || Cdist == 0) && (Rdist > distlim || Rdist == 0) && sdl > sdistlim && sdr > sdistlim){
    mmove('F',mspd1,mspd1,10);
  }
  else{
    if((Ldist <= distlim && Ldist != 0) && (Cdist <= distlim && Cdist != 0) && (Rdist <= distlim && Rdist != 0)){  //corners!!!
      mmove('L',mspd1,mspd1,1000);
    }
    else if(Cdist <= distlim && Cdist != 0){
        //randdur = random(100,250);
        mmove('L',mspd1,mspd1,400);
        //randdur = random(10,30);
        //mmove('B',mspd2,mspd1,randdur);
    }
    else if(Ldist <= distlim && Ldist != 0){
        randdur = random(200,350);
        mmove('R',mspd1,mspd1,randdur);
    }
    else if(Rdist <= distlim && Rdist != 0){
        randdur = random(200,350);
        mmove('L',mspd1,mspd1,randdur);
        //randdur = random(10,30);
        //mmove('B',mspd2,mspd1,randdur);
    }
    else if((sdl <= sdistlim) && (sdr <= sdistlim)){  //corners!!!
        mmove('R',mspd1,mspd1,1000);
    }
    else if(sdl <= sdistlim){      
        mmove('R',mspd1,mspd1,500);
    }
    else if(sdr <= sdistlim){
        mmove('L',mspd1,mspd1,500);
    }
    
  }
}


