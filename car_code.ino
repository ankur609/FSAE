int accped1,accped2,accavg,brakeped;
//digital pins
int interlock = 3;
int throttle_sw = 4;
int acc_out = 5;

int brake_out = 6;
int prech = 7;
int main_rel = 8;
int buzzer = 9;
int tsal_in = 10;
int ctrl_rel = 11;
int add_sw_in = 12;

//analog pins
int accped1_in = A3;
int accped2_in = A2;
int brakeped_in = A1;
int HV = A0;

  
bool check(){ accped1 = analogRead(A3);
 accped2 = analogRead(A2);
 brakeped = analogRead(A1);
 accavg=(accped1+accped2)/2;
  if ((accped1<(0.9*1023) && accped1>(0.1*1023)) && (accped2<(0.9*1023) && accped2 > (0.1*1023) )&&( brakeped < (0.9*1023) && brakeped > (0.1*1023)) && digitalRead(tsal_in)==HIGH){
    if(abs(accped1-accped2)<103){
      return true;
      }
    else{
      return false;}
    }
    else{
      return false;}
  }

bool plauscheck(){

  if((accavg>0.3*(1023))&&(brakeped>0.35*1023)){
      return false;}
      else{ 
        return true;
        }
    }
void buzer(){
    digitalWrite(buzzer,1);     
    }
void buzzerstop(){
    digitalWrite(buzzer,0); 
    }
void precharge(){
    digitalWrite(prech,1);
    delay(5000);
    digitalWrite(prech,0);
    digitalWrite(main_rel,1);
    //turn off precharge and start mainrelay
    }        
bool startcondn(){
  if(brakeped>(0.2*1023)){
    return true;
    }
    else {
      return false;}
  }
  void start(){
  digitalWrite(ctrl_rel,1);
  buzer();
 
  delay(3000);
  buzzerstop();
  precharge();
  }     
  void startloop(){
  
  precharge();
   digitalWrite(main_rel,1);
  
  } 
void shtdown(){
  digitalWrite(main_rel,0);    

  }
  void tmpshtdown(){
 accped1=analogRead(A3);
 accped2=analogRead(A2);
 
    digitalWrite(main_rel,0);
    while(accavg>0.14*(1023)){
      accped1=analogRead(A3);
      accped2=analogRead(A2);
       accavg=(accped1+accped2)/2;
      }
      
  precharge();
  
    }

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
  pinMode(interlock,OUTPUT);//interlock
  pinMode(throttle_sw,OUTPUT);//throttle switch
  pinMode(acc_out,OUTPUT);//accn
  pinMode(brake_out,OUTPUT);//brake
  pinMode(prech,OUTPUT);//prechareg
  pinMode(main_rel,OUTPUT);//mainrelay
  pinMode(buzzer,OUTPUT);//buzzz
  pinMode(tsal_in,INPUT);//TSAL_OK
  pinMode(ctrl_rel,OUTPUT);//control relay
  pinMode(add_sw_in,INPUT);//additional SW

 digitalWrite(interlock,0);
 digitalWrite(throttle_sw,0);
 digitalWrite(acc_out,0);
 digitalWrite(brake_out,0);
 digitalWrite(prech,0);
 digitalWrite(main_rel,0);  
 digitalWrite(buzzer,0);
 digitalWrite(ctrl_rel,1);
 bailout:
 accped1 = analogRead(A3);
 accped2 = analogRead(A2);
 brakeped = analogRead(A1);
 accavg=(accped1+accped2)/2;
 if(check()&&plauscheck()&&startcondn())
 {
   start();
  }
 else{
  
    goto bailout;
  }

}


void loop() {
  // put your main code here, to run repeatedly:

 accped1=analogRead(A3);
 accped2=analogRead(A2);
 brakeped=analogRead(A1);
 HV=analogRead(A0);  
 Serial.print(accped1);       // prints a label
  Serial.print("\t");              // prints a tab

  Serial.print(accped2);  
  Serial.print("\t");      

  Serial.print(brakeped); 
  Serial.println(" "); 
 accavg=(accped1+accped2)/2;
 if(check()){
  if (plauscheck()){
    
    analogWrite(acc_out,accavg*255/1023);
    analogWrite(brake_out,brakeped*255/1023);
    digitalWrite(throttle_sw,HIGH);
    }
    else{
      tmpshtdown();
      digitalWrite(main_rel,HIGH);
      }
  }
  
  
  else{
    shtdown();
    while(check()==false)
    {
      }
    startloop();
    
    }

}
