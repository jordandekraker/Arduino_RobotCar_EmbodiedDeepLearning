
/ *
* Copyright (c) 2015, Koji Technology Co., Ltd.
* All rights reserved.
*
* File Name: wifi - robots
* Document ID:
* Abstract: wifi robot smart car control
*
* Current Version: 1.0
* Author: tianyalu
* Completion Date: April 1, 2015
* /


//#include <AFMotor.h> This library is old and deprecated https://github.com/adafruit/Adafruit-Motor-Shield-library
#include <Servo.h>
#include <MsTimer2.h>
#include <EEPROM.h>

int ledpin = 13;//Set system startup light
int ENA = 5;//
int ENB = 6;//
int INPUT2 = 7;//
int INPUT1 = 8;//
int INPUT3 = 12;//
int INPUT4 = 13;//

int inputPin = A0;  // Define the ultrasonic signal receiving pin
int outputPin = A1; // Define the ultrasonic signal transmitting pin
int Input_Detect_LEFT = A2;    //Define car left infrared
int Input_Detect_RIGHT = A3;  //Define car right infrared
int Input_Detect = A4; //Define the car in front of the infrared
int Carled = 2;
int Cruising_Flag = 0;
int Pre_Cruising_Flag = 0 ;

#define MOTOR_GO_FORWARD  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}
#define MOTOR_GO_BACK	  {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}
#define MOTOR_GO_RIGHT	  {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}
#define MOTOR_GO_LEFT	  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}
#define MOTOR_GO_STOP	  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,LOW);}

//AF_DCMotor motor1(1, MOTOR12_64KHZ);//
//AF_DCMotor motor2(2, MOTOR12_64KHZ);//

Servo servo1;//
Servo servo2;//

byte angle1 = 70; //servo1
byte angle2 = 60;

int buffer[3];  //Serial port to receive data cache
int rec_flag;
int serial_data;
int Uartcount;
int IR_R;
int IR_L;
int IR;
unsigned long Pretime;
unsigned long Nowtime;
unsigned long Costtime;

void Open_Light()//Turn on the headlights
{
  digitalWrite(Carled, HIGH);  //Pull low, positive then power, negative access Io mouth
  delay(1000);
}
void Close_Light()//
{
  digitalWrite(Carled, LOW);
  delay(1000);
}

void  Avoiding()
{
  IR = digitalRead(Input_Detect);
  if ((IR == HIGH))
  {
    MOTOR_GO_FORWARD;//Direct line
    return;
  }
  if ((IR == LOW))
  {
    MOTOR_GO_STOP;//stop
    return;
  }
}



void FollowLine()   // Patrol line mode
{
  IR_L = digitalRead(Input_Detect_LEFT);
  IR_R = digitalRead(Input_Detect_RIGHT);

  if ((IR_L == LOW) && (IR_R == LOW)) //Simultaneous detection of obstacles on both sides
  {
    MOTOR_GO_FORWARD;//直行
    return;

  }
  if ((IR_L == LOW) && (IR_R == HIGH)) //On the right encountered obstacles
  {
    MOTOR_GO_LEFT;//
    return;

  }
  if ((IR_L == HIGH) && ( IR_R == LOW)) //On the left encountered obstacles
  {
    MOTOR_GO_RIGHT;//
    return;

  }
  if ((IR_L == HIGH) && (IR_R == HIGH)) //Detected around, just as the video as encountered in a horizontal tape
  {
    MOTOR_GO_STOP;//
    return;
  }


}

void ask_Distance()   // Measure the distance
{
  delay(200);
  digitalWrite(outputPin, LOW);   // Let the ultrasonic wave emit low voltage 2μs
  delayMicroseconds(2);
  digitalWrite(outputPin, HIGH);  // Let the ultrasonic wave launch high voltage 10μs, here is at least 10μs
  delayMicroseconds(10);
  digitalWrite(outputPin, LOW);    // Maintain ultrasound to transmit low voltage
  float Ldistance = pulseIn(inputPin, HIGH);  // Poor reading time difference
  Ldistance = Ldistance / 5.8 / 10;  // Turn time into distance (in centimeters)
  //  Serial.print("L distance:");       //Output distance (unit: cm)
  //  Serial.println(Ldistance);         //Display distance
  if (Ldistance < 15)
  {
    MOTOR_GO_STOP;
  }
  else
  {
    MOTOR_GO_FORWARD;
  }

}

void  Delayed()
{
  int i;
  for (i = 0; i < 20; i++)
  {
    digitalWrite(ledpin, LOW);
    delay(1000);
    digitalWrite(ledpin, HIGH);
    delay(1000);
  }
}


/ *
************************************************* * ************************************************* * *****
** Function Name: setup ()
** Function: System initialization (serial port, motor, servo, LED initialization).
** Entrance parameters: None
** Export parameters: None
************************************************* * ************************************************* * *****
* /

void setup()
{
  pinMode(ledpin, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
  pinMode(INPUT3, OUTPUT);
  pinMode(INPUT4, OUTPUT);
  pinMode(Input_Detect_LEFT, INPUT);
  pinMode(Input_Detect_RIGHT, INPUT);
  pinMode(Carled, OUTPUT);
  pinMode(Input_Detect, INPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  Delayed();
  digitalWrite(ledpin, LOW);

  Serial.begin(9600);//Serial port baud rate is set to 9600 bps
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  servo1.attach(9);//Define the steering gear 7 control port
  servo2.attach(10);//Define the steering gear 8 control port
  Init_Steer();

}

/ *
************************************************* * ************************************************* * *****
** Function Name: loop ()
** Function Function: The main function
** Entrance parameters: None
** Export parameters: None
************************************************* * ************************************************* * *****
* /

void Cruising_Mod()
{

  if (Pre_Cruising_Flag != Cruising_Flag)
  {
    if (Pre_Cruising_Flag != 0)
    {
      MOTOR_GO_STOP;
    }

    Pre_Cruising_Flag =  Cruising_Flag;
  }
  switch (Cruising_Flag)
  {
    //case 1:Send_Distance();//Ultrasonic distance measurement
    case 2: FollowLine(); return; //Patrol line mode
    case 3: Avoiding(); return; //Obstacle avoidance mode
    case 4: ask_Distance(); return; //Ultrasound obstacle avoidance mode
    default: return;
  }

}

void Init_Steer()
{
  servo1.write(angle1);
  servo2.write(angle2);

  angle1 = EEPROM.read(0x01);
  angle2 = EEPROM.read(0x02);
  if (angle1 == 255 && angle2 == 255)
  {
    EEPROM.write(0x01, 70);
    EEPROM.write(0x02, 60);
    return;
  }

  servo1.write(angle1);
  servo2.write(angle2);
}


void loop()
{
  while (1)
  {
    Get_uartdata();
    UartTimeoutCheck();
    Cruising_Mod();
  }
}


/ *
************************************************* * ************************************************* * *****
** Function Name: Communication_Decode ()
** Function: Serial command decoding
** Entrance parameters: None
** Export parameters: None
************************************************* * ************************************************* * *****
* /

void Communication_Decode()
{
  if (buffer[0] == 0x00)
  {
    switch (buffer[1])  //Motor command
    {
      case 0x01: MOTOR_GO_FORWARD; return;
      case 0x02: MOTOR_GO_BACK;    return;
      case 0x03: MOTOR_GO_LEFT;    return;
      case 0x04: MOTOR_GO_RIGHT;   return;
      case 0x00: MOTOR_GO_STOP;    return;
      default: return;
    }
  }
  else if (buffer[0] == 0x01) //Servo command
  {
    if (buffer[2] > 160)return;
    switch (buffer[1])
    {
      case 0x07: angle1 = buffer[2]; servo1.write(angle1); return;
      case 0x08: angle2 = buffer[2]; servo2.write(angle2); return;
      default: return;
    }
  }
  else if (buffer[0] == 0x33) //Read the servo angle from board memory and assign it
  {
    Init_Steer(); return;
  }
  else if (buffer[0] == 0x32) //Save the command to board memory
  {
    EEPROM.write(0x01, angle1);
    EEPROM.write(0x02, angle2);
    return;
  }
  else if (buffer[0] == 0x13) //Mode switch
  {
    switch (buffer[1])
    {
      case 0x02: Cruising_Flag = 2; return;//line follow
      case 0x03: Cruising_Flag = 3; return;//Avoidance
      case 0x04: Cruising_Flag = 4; return;//Radar obstacle avoidance
      case 0x00: Cruising_Flag = 0; return;//Normal mode
      default: Cruising_Flag = 0; return; //
    }
  }
  else if (buffer[0] == 0x05) //lights
  {
    switch (buffer[1])  //
    {
      case 0x00: Open_Light(); return;
      case 0x02: Close_Light(); return;
      default: return;
    }
  }


}

/ *
************************************************* * ************************************************* * *****
** Function Name: Get_uartdata ()
** Function function: read serial command
** Entrance parameters: None
** Export parameters: None
************************************************* * ************************************************* * *****
* /

void Get_uartdata(void)
{
  static int i;

  if (Serial.available() > 0) //Determine if the serial port buffer has data loading
  {
    serial_data = Serial.read();//Read the serial port
    if (rec_flag == 0)
    {
      if (serial_data == 0xff)
      {
        rec_flag = 1;
        i = 0;
        //Uartcount = 0;
        //Pretime = millis();
        Costtime = 0;
      }
    }
    else
    {
      if (serial_data == 0xff)
      {
        rec_flag = 0;
        if (i == 3)
        {
          Communication_Decode();
        }
        i = 0;
      }
      else
      {
        buffer[i] = serial_data;
        i++;
      }
    }
  }
}

/ *
************************************************* * ************************************************* * *****
** Function Name: UartTimeoutCheck ()
** Function: serial timeout detection
** Entrance parameters: None
** Export parameters: None
************************************************* * ************************************************* * *****
* /

void UartTimeoutCheck(void)
{
  if (rec_flag == 1)
  {
    //Nowtime = millis();
    Costtime++;
    if (Costtime == 100000)
    {
      rec_flag = 0;
    }
  }
  // Uartcount++;
}
