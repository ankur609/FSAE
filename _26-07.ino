
/*
 Data acquistion:
 master arduino 
 signals descriptions:
                                        frame description 
 voltageLV            A0                      A
 Temperature sensor   A1  (motor)             B  
 Acceleration pedal1  A2                      C
 Acceleration pedal2  A3                      D
 Brake pedal -->      A4                      E
 Current sensor -->   A5 (+/- 75A channel)    F  // channel d
 Current sensor -->   A6 (+/- 500A channel)   G   // channel b
 currentvalue   --->                          H
 voltage sensor -->   A7                      I
 speedCar                                     J
 motorRPM                                     K
 CurrentLV                                    L
 amb. temperature                             M                                
 powerLV                                      N
 energyLV                                     O
 SOCLV                                        P
 powerHV                                      Q
 energyHV                                     R
 SOCHV                                        S  
 

 functions:
 1.data logging
 2. communication with dashboard ECU
 3. on board arduino nano communication (to get aceleration /speed)

 Errors:
 1.SD card initlisiation failed
 
 
 Functions:
 convert(String);
 Update()
 
*/
#include <avr/io.h> 
#include <avr/interrupt.h>
// include the SD library:
#include <SPI.h>
#include <SD.h>
// objects
File myFile,myFile1;
Sd2Card card;



//global variables
 double value;
 
 const float gearRatio=5;
 const float radiusTire=.256;
 String motorRPM= "0.000";
 String speedCar= "0.000"; //kmhr 
 String currentLV= "0.000";
 String ambTemp= "25"; //degree celisius
 String ene="";
 
 volatile unsigned long pulses=0;
 volatile unsigned long count=0;
 volatile float value_speed=0;
 unsigned int Tcount=10;// no. of overflow timer2
 
unsigned long long pasttime,curtime;
 
 //master
 String scr="";
 float currentValue=8.0;
 //String ambTemp="-1.0";
 //float powerLV=0; 
 //float voltageLV=0;                        
 //float energyLV=0;                       
 //float SOCLV=0;                           
 float powerHV=0;
 float voltageHV=0;                                      
 float energyHV=0;                                     
 //float SOCHV=0;                            
 //String dashboard;
 bool flag_update=0;
 int flg=0;
 bool curflag=0;
 //name of file
 const int chipSelect = 10;
 char Name[15],Name1[15];
 long long timer=0;
 String T_ms="";
 uint8_t flag; //
float t1;
 
 /*
  * ISR (INT1_vect)
 {
  EIFR&=(~(1<<1));
  pulses++;
 }
 */

 ISR (INT1_vect)
 {
 pulses++;
 }
 ISR (TIMER2_OVF_vect)  // timer0 overflow interrupt
{
  //event to be exicuted every 
  // clear TOV2
  TIFR2&=(0xFE);
  if(count<30)
  count++;
  else{
    count=0;
    value_speed=pulses;
    pulses=0;
  }


  
}

void setup(){

  //debug led
  //DDRB|=(1<<5);
//init serial
Serial.begin(57600);

//init hardware interupt
 pinMode(3, INPUT);
// attachInterrupt(digitalPinToInterrupt(3), speedISR,RISING);
/*
DRDD|=(1<<0); // int1 input
EIMSK|=(1<<1);//enable int1
*/
//interruptv

EICRA|=(1<<3)|(1<<2);// rising interrupt at int1
EIMSK|=(1<<1);
//enable global interrupt variable
SREG|=(1<<7);
//init timer (prescaler 1024)
TCCR2A=0x00;
TCCR2B=0x00;
TCCR2B|=(1<<0)|(1<<1)|(1<<2);
TIMSK2=(1<<0);//overflow interrupt enable

//
  pinMode(2,OUTPUT);
 //sd card initialisation 
 //chip select pin D10
  pinMode(10, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    digitalWrite(2,HIGH);
    return;
  }
  //Serial.println("card initialized.");
  digitalWrite(2,LOW);
  delay(100);
  int value =1;
  String filename ="Y";
  filename+=(char)(value/100+'0');
  filename+=(char)(((value/10)%10)+'0');
  filename+=(char)((value%10)+'0');
  filename+=".csv";
  filename.toCharArray(Name,15);
 // Check to see if the file exists:
  if (SD.exists(Name)) {
   //Serial.println("file exists.");
  }
  else {
    //Serial.println("file doesn't exist.");
    // open a new file and immediately close it:
  //Serial.println("Creating example.txt...");
  myFile = SD.open(Name, FILE_WRITE);
  myFile.close();
  }
  int value1 =1;
  String filename1 ="Erg";
  filename1+=(char)(value1/100+'0');
  filename1+=(char)(((value1/10)%10)+'0');
  filename1+=(char)((value1%10)+'0');
  filename1+=".csv";
  filename1.toCharArray(Name1,15);
 // Check to see if the file exists:
  if (SD.exists(Name1)) {
   //Serial.println("file exists.");
  }
  else {
    //Serial.println("file doesn't exist.");
    // open a new file and immediately close it:
  //Serial.println("Creating example.txt...");
  myFile1 = SD.open(Name1, FILE_WRITE);
  myFile1.close();
  }
  //sd data row intialisation
  //String sd_string="voltageLV,Temperature Motor,THROTTLE1,THROTTLE2,Brake,current CH1,curent CH2,currentSensor,VOLTAGE,motorRPM,speedCar,currentLV,ambTemp,PowerLV,EnergyLV,SOCLV,PowerHV,EnergyHv,SOCHV,TIME(sec)";
  String sd_string="vLV,Temp,T1,T2,Brk,cur1,cur2,cur,vHV,mRPM,spe,curLV,TIME(sec)";
  //String sd_string="test,best";
  myFile = SD.open(Name, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print("Writing to test.txt...");
    myFile.println(sd_string);
    // close the file:
    myFile.close();
   //Serial.println("done.");
   digitalWrite(2,LOW);
  } else {
    // if the file didn't open, print an error:
    digitalWrite(2,HIGH);
   //Serial.println("error opening file");
  }

// Creating file for Storing energy,soc data
 pasttime=millis(); 
}
void loop(){

  // calculate Speed
  value_speed=value_speed/(12.0*.01632*30);
  motorRPM=String(value_speed*60*gearRatio);
  speedCar=String(value_speed*2.0*3.14*radiusTire*18.0/5.0/1000);

  

 //read analog values
   

  String sd_string="";
  String dashboard=""; 
  
  //a0 LV Voltage
  dashboard+='A';
  average(0);
  value=value*26.7/4.7;
  //voltageLV=value;
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";

  //a1 Temperature Sensor
  dashboard+='B';
  average(1);
  value=(value*2200.0)/(12.0-value);
  value=(-3.92+sqrt(3.92*3.92-4.0*0.01101*(498.0-value)))/(2.0*0.01101);
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";
  
  //a2 Acc p1
  dashboard+='C';
  average(2);
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";
 
  //a3 Acc p2
  dashboard+='D';
  average(3);
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";
  
  myFile = SD.open(Name, FILE_WRITE);
  myFile.print(sd_string);
  //Serial.println(sd_string);
  
  
  sd_string="";
  //a4 Brake pedal
  dashboard+='E';
  average(4);
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";
  
  //a5 Current sensor1 500A Channel B
  //dashboard+='F';
  average(6);
  value=(-268.2868)*value+664.5354;
  sd_string+=convert(value);
  //dashboard+=convert(value);
  if(value>-60&&value<60)
  curflag=0;
  else {curflag=0;
    currentValue=value;  }
  sd_string+=",";
    
  //a6 Current sensor 2 75A Channel
  //dashboard+='G';
  average(5);
  value=(-39.2156)*value+95.349+4.6;
    sd_string+=convert(value);
  //dashboard+=convert(value);
  if(curflag==1) currentValue=value;
  sd_string+=","; 
  
  //current value
  dashboard+="FGH";
  sd_string+=convert(currentValue);
  dashboard+=convert(currentValue);
  sd_string+=",";

  //  Serial.println(sd_string);
  
  myFile.print(sd_string);
  sd_string="";

  
  //a7 voltAGE sensor
  dashboard+='I';
  average(7);
  value=200.0/11.0*value-10;
  voltageHV=value;
  sd_string+=convert(value);
  dashboard+=convert(value);
  sd_string+=",";
  
  //motorRPM
  dashboard+='J';
  sd_string+=speedCar;
  dashboard+=speedCar;
  sd_string+=",";
  
  //speedCar
  dashboard+='K';
  sd_string+=String(value_speed);
  dashboard+=String(value_speed);
  sd_string+=",";
  
  //currentLV
  dashboard+='L';
  sd_string+=currentLV;
  dashboard+=currentLV;
  sd_string+=",";
/*  
  //amb. temperature
  dashboard+='M';
  sd_string+=ambTemp;
  dashboard+=ambTemp;
  sd_string+=",";

  powerLV=conv2float(currentLV)*voltageLV;
  dashboard+='N';
  sd_string+=convert(powerLV);
  dashboard+=convert(powerLV);
  
  

  t1=millis()-pasttime;
  energyLV+=powerLV*t1/1000000;
  dashboard+='O';
  sd_string+=convert(energyLV);
  dashboard+=convert(energyLV);
  sd_string+=",";

  
  dashboard+='P';
  SOCLV=(voltageLV-19.0)*100.0/6.0;
  sd_string+=convert(SOCLV);
  dashboard+=convert(SOCLV);
  sd_string+=",";

*/
  powerHV=currentValue*voltageHV;
 // dashboard+='Q';
 // sd_string+=convert(powerHV);
 // dashboard+=convert(powerHV);
 // sd_string+=",";

  curtime=millis();
  t1=(curtime-pasttime)/1000;
  pasttime=curtime;
  energyHV+=powerHV*(t1)/1000;
 // dashboard+='R';
 // sd_string+=convert(energyHV);
  ene=convert(energyHV);
 // sd_string+=",";
/*
  dashboard+='S';
  SOCHV=(voltageHV-68)*100/14;
  sd_string+=convert(SOCHV);
  dashboard+=convert(SOCHV);
  sd_string+=",";
  */  
  dashboard+='A';  
  float time1 = millis()/1000.0;
  sd_string+=convert(time1);
  myFile.println(sd_string);
  myFile.close();
  myFile1 = SD.open(Name1, FILE_WRITE);
  myFile1.println(ene);
  myFile1.close();
 //   Serial.println(sd_string); 
//delay(300);
//dashcomm();
  Serial.write('a');
  Serial.print(dashboard);
  Serial.write('b');

//Serial.println(sd_string);  
  delay(300);
  
}

// average
void average(int pin)
{value=0;
  for(int i=1;i<=5;i++)
  value+= 5*analogRead(pin)/1024.0;
  value=value/5.0;
 // value=2.05;
}
//convert to string
String convert(double value1)
{ String s="";
  long value;
  if (value1==0) {s="0.000";return s;}
  else if(value1<0) value=-value1*1000;
  else value=value1*1000;
  int ctr=0;
  while(value!=0)
  {
  ctr=ctr+1;
  int r=value%10;
  char c=r+'0';
  s=c+s;
  if(ctr==3)
  {
    s='.'+s;
  }
  value=value/10;
  }
  if(value1<0) return '-'+s;
  else return s; 
}
