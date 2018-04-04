#include <UTFT.h>
# include<UTFT_tinyFAT.h>
# include<URTouch.h>
# include<UTFT_Geometry.h>
# include<EEPROM.h>
int addr=0;
#define TOUCH_ORIENTATION  LANDSCAPE
int b=0,s,f;
const float pi=3.14;
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t UbuntuBold[];
bool fl=0;
int flg,flag=0;

UTFT myGLCD(CTE50, 38, 39, 40, 41);
UTFT_tinyFAT myFiles(&myGLCD);
UTFT_Geometry geo(&myGLCD);
//URTouch  myTouch( 6, 5, 4, 3, 2);

String volLV="NA";
String temp="NA";
String acc1="NA";
String acc2="NA";
String bra="NA";
String cur1="NA";
String cur2="NA";
String cur="NA";
String vol="NA";
String spe="000";
String rpm="NA";
String curLV="NA";
String ambtemp="NA";
float powerHV;
float powerLV;
float energyLV=0,p1=0;
float energyHV=0,p2=0;
float distance=0,p3=0;
float SOCHV;
float SOCLV;
String str="";
String scrape="";
float voltageLV;
float currentLV;
float currentHV;
float voltageHV;
float tempint; 

unsigned long long pasttime=0;
unsigned long long curtime=0;
unsigned long long t1=0;


void dashsetup()
{
word res1,res2,res3;
/*res1=myFiles.loadBitmap(156, 0, 488, 480,"IITR.RAW");
   if (res1!=0)
      {
        if (res1==0x10)
        {
          myGLCD.print("File not found...", 0, 0);
          myGLCD.print("IITR.RAW", 0, 14);
        }
        else
        {
          myGLCD.print("ERROR: ", 0, 0);
          myGLCD.printNumI(res1, 56, 0);
        }
        delay(3000);
        myGLCD.clrScr();
      }
      myGLCD.fillScr(VGA_WHITE);
  res2=myFiles.loadBitmap(0, 105, 800,271 ,"IITRMS1.RAW");
   if (res2!=0)
      {
        if (res2==0x10)
        {
          myGLCD.print("File not found...", 0, 0);
          myGLCD.print("IITRMS1.RAW", 0, 14);
        }
        else
        {
          myGLCD.print("ERROR: ", 0, 0);
          myGLCD.printNumI(res2, 56, 0);
        }
        delay(3000);
        myGLCD.clrScr();
      }
      myGLCD.fillScr(VGA_WHITE);  */
   res3=myFiles.loadBitmap(0, 0, 800,480 ,"spee800.RAW");
 
   if (res3!=0)
      {
        if (res3==0x10)
        {
          myGLCD.print("File not found...", 0, 0);
          myGLCD.print("spee800.RAW", 0, 14);
        }
        else
        {
          myGLCD.print("ERROR: ", 0, 0);
          myGLCD.printNumI(res3, 56, 0);
        }
        delay(3000);
        myGLCD.clrScr();
      }
myGLCD.setFont(UbuntuBold);
myGLCD.print("HV",657,310);
myGLCD.print("LV",102,310);

}

void Update(){
  //long long loca=lTime_elapsed=millis();
  String str="";
  int strcount=0;
    while(Serial.available()){
      char ch1=Serial.read();
      str+=ch1;}
    flag=0;
    flg=0;
    strcount=str.length();
   for(int i=0;i<strcount;i++)
    {    
     char ch=str.charAt(i);
     if(ch<60&&ch>=45){
       if(ch==':') {
   
      flg=1;
      scr="";
      continue;
       }

       else{
  //delay(100);
        if(flg==1){
         if(ch=='/'){
         flg++;
         volLV=scr;
         scr="";
         continue; 
        }
        scr+=ch;
       }
    else if(flg==2){
   if(ch=='/'){
      flg++;
      temp=scr;
      scr="";
      continue; 
     }
     scr+=ch;
     }
   else if(flg==3){
   if(ch=='/'){
      flg++;
      acc1=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }

   else if(flg==4){
   if(ch=='/'){
      flg++;
      acc2=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }

   else if(flg==5){
   if(ch=='/'){
      flg++;
      bra=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }

   else if(flg==6){
   if(ch=='/'){
      flg++;
      cur1=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }
   else if(flg==7){
   if(ch=='/'){
      flg++;
      cur2=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }
   else if(flg==8){
   if(ch=='/'){
      flg++;
      cur=scr;
      scr="";
      continue; 
     }

   else if(flg==9){
   if(ch=='/'){
      flg++;
      vol=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     } 
      scr+=ch;
     }

   else if(flg==10){
   if(ch=='/'){
      flg++;
      spe=scr;
      scr="";
      continue; 
     }
      scr+=ch;
     }
   else if(flg==11){
   if(ch==';'){
      flg=0;
      rpm=scr;
      scr="";
      break; 
     }
      scr+=ch;
     }
   }  
     }//charchk
    } 

    
}
void setup() {
  // put your setup code here, to run once:
//Serial.begin(9600);
pinMode(7,OUTPUT);
digitalWrite(7,LOW);
Serial1.begin(9600);
int picsize_x, picsize_y;
myGLCD.InitLCD(); 
myGLCD.clrScr();
myGLCD.setFont(BigFont);
//myTouch.InitTouch(TOUCH_ORIENTATION);
myGLCD.fillScr(VGA_WHITE);
file.initFAT();
dashsetup();

pasttime=millis();

/*EEPROM.put(addr,p1);
EEPROM.put(addr+sizeof(float),p2);
EEPROM.put(addr+2*sizeof(float),p3);
*/
/*
EEPROM.get(addr,p1);
energyLV=p1;
EEPROM.get(addr+sizeof(float),p2);
energyHV=p2;
EEPROM.get(addr+2*sizeof(float),p3);
distance=p3;
  */
  
}




void sercomm(){
//  t1=millis()-pasttime;
   str="";
   while (true)
 {
    
   while (!Serial1.available())
  {if(millis()-pasttime>500)
   fl=0;
       }
      
  char c=Serial1.read();
  if(c=='a') {break;}
  }


while (true)
 {
    
   while (!Serial1.available())
  {
    if(millis()-pasttime>500)
   fl=0;
       }
  char c=Serial1.read();
  if(c=='b') {break;}
  str+=c;
  }
if(fl==0)
pasttime=millis();

  }


  

void printlcd()
{
// b = speed previous value
if(b!=s)
{
  myGLCD.setColor(200,200,200);
  geo.fillTriangle(395,230,405,230,400-130*cos(-1*pi/6.0+pi*b/120.0),230-130*sin(-1*pi/6.0+pi*b/120.0));
  //myGLCD.drawBitmap(310,130,180,59,logo);
  myGLCD.setColor(VGA_RED);
  myGLCD.fillCircle(400,230,15);
  geo.fillTriangle(395,230,405,230,400-130*cos(-1*pi/6.0+pi*s/120.0),230-130*sin(-1*pi/6.0+pi*s/120.0));
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(s,320,327);
  myGLCD.setFont(BigFont);
  myGLCD.print("Km/hr",420,350);
  b=s;
}

/*myGLCD.setColor(22,26,33);
myGLCD.fillRect(92,90,156,217);
myGLCD.fillRect(640,90,711,217);
*/
myGLCD.setColor(VGA_WHITE);
myGLCD.setBackColor(22,26,33);

myGLCD.setFont(SmallFont);
myGLCD.print("Current(A)",72,90); 
myGLCD.print("Voltage(V)",72,130);
myGLCD.print("Energy(Whr)",76,170);
myGLCD.print("Current(A)",627,90);
myGLCD.print("Voltage(V)",627,130);   
myGLCD.print("Energy(Whr)",631,170);

myGLCD.setFont(BigFont);
myGLCD.print("    ",1,92,105);
myGLCD.printNumF(currentLV,1,92,105,'.',4,'0');
myGLCD.print("    ",1,92,145);
myGLCD.printNumF(voltageLV,1,92,145,'.',4,'0');
myGLCD.print("    ",92,185);
myGLCD.printNumI(energyLV,92,185,4,'0');
myGLCD.print("     ",1,640,105);
myGLCD.printNumF(currentHV,1,640,105,'.',5,'0');
myGLCD.print("    ",1,647,145); 
myGLCD.printNumF(voltageHV,1,647,145,'.',4,'0');
myGLCD.print("    ",647,185);    
myGLCD.printNumI(energyHV,647,185,4,'0');

myGLCD.setBackColor(VGA_BLACK);
myGLCD.print("   ",715,413);
myGLCD.printNumI(tempint,715,413,3,'0');
myGLCD.print("     ",350,380);
myGLCD.printNumF(distance,1,350,380,'.',5,'0');
myGLCD.print("Km",440,380);

}


void loop() {
  // put your main code here, to run repeatedly:
//str="A21.0B7.5C5.7D69E4F3G2H100I69J54K8L3A";
//checkch();
sercomm();
//myGLCD.print(str,CENTER,40);
if(tempint>40) digitalWrite(7,HIGH);
else digitalWrite(7,LOW);
update1();

fl=1;

//printcheck();
  
}

void printcheck(){
  myGLCD.print(" volLV= "+volLV,CENTER,0);
  myGLCD.print(" temp= "+temp,CENTER,20);
  myGLCD.print(" acc1= "+acc1,CENTER,40);
  myGLCD.print(" acc2= "+acc2,CENTER,60);
  myGLCD.print(" bra= "+bra,CENTER,80);
 // myGLCD.print(" cur1= "+cur1,CENTER,100);
//  myGLCD.print(" cur2= "+cur2,CENTER,120);
  myGLCD.print(" cur= "+cur,CENTER,140);
  myGLCD.print(" vol= "+vol,CENTER,160);
  myGLCD.print("spe= "+spe,CENTER,180);
  myGLCD.print(" rpm= "+rpm,CENTER,200);
  myGLCD.print(" curLV= "+curLV,CENTER,220);
 // myGLCD.print(" ambtemp= "+ambtemp,CENTER,240);
  myGLCD.printNumF(powerLV,3,CENTER,260); 
  myGLCD.printNumF( energyLV,3,CENTER,280);
  myGLCD.printNumF(SOCLV,3,CENTER,300);
  myGLCD.printNumF(powerHV,3,CENTER,320);
  myGLCD.printNumF(energyHV,3,CENTER,340);
  myGLCD.printNumF(SOCHV,3,CENTER,360);
  
  }



float conv2float(String s1)
{
  if (s1.charAt(0)=='-') 
  { 
    s1=s1.substring(1,s1.length());
    return -1.0*s1.toFloat();
  }
  else
  { return s1.toFloat();}
}

