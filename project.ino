
#include <Servo.h>
Servo myservo1;

#include <LiquidCrystal.h>
#include <stdio.h>
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);

int mos    = A1;
int ir     = A0;
int smoke  = 10;
int relay  = 11;
int buzzer = 13;

const int trigPin1 = A2;
const int echoPin1 = A3;

const int trigPin2 = A4;
const int echoPin2 = A5;


int lvl_wet=0,lvl_dry=0;
int rtr1=0,rtr2=0;
int dista=0;
int dist1=0,dist2=0,dist3,sts1=0,sts2=0;
long duration1;
int distanceCm1, distanceInch1;
long duration2;
int distanceCm2, distanceInch2;
int distancemm1=0;
int distancemm2=0;

unsigned int ultra_dist1()
{int ud1=0;
   digitalWrite(trigPin1, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin1, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin1, LOW);
   duration1 = pulseIn(echoPin1, HIGH);
   //distanceCm= duration*0.034/2;
   //ud = distanceCm;
   distancemm1 = (duration1*0.17);
   ud1 = distancemm1;  
   return ud1;
}
unsigned int ultra_dist2()
{int ud2=0;
   digitalWrite(trigPin2, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin2, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin2, LOW);
   duration2 = pulseIn(echoPin2, HIGH);
   //distanceCm= duration*0.034/2;
   //ud = distanceCm;
   distancemm2 = (duration2*0.17);
   ud2 = distancemm2;  
   return ud2;
}

void beep()
{
  digitalWrite(buzzer, LOW);delay(2500);digitalWrite(buzzer, HIGH);
}

char res[130];

void serialFlush()
{
  while(Serial.available() > 0) 
  {
    char t = Serial.read();
  }
} 

void myserialFlush()
{
  while(mySerial.available() > 0) 
  {
    char t = mySerial.read();
  }
} 

char check(char* ex,int timeout)
{
  int i=0;
  int j = 0,k=0;
  while (1)
  {
    sl:
    if(mySerial.available() > 0)
    {
      res[i] = mySerial.read();
      if(res[i] == 0x0a || res[i]=='>' || i == 100)
      {
        i++;
        res[i] = 0;break;
      }
      i++;
    }
    j++;
    if(j == 30000)
    {
      k++;
     // Serial.println("kk");
      j = 0;  
    }
    if(k > timeout)
    {
      //Serial.println("timeout");
      return 1;
     }
  }//while 1
  if(!strncmp(ex,res,strlen(ex)))
  {
   // Serial.println("ok..");
    return 0;
   }
  else
  {
   // Serial.print("Wrong  ");
   // Serial.println(res);
    i=0;
    goto sl;
   }
} 

char buff[200],k=0;
void upload(unsigned int unt,unsigned int amt,const char *s1);
char readserver(void);
void clearserver(void);

const char* ssid = "iotserver";
const char* password = "iotserver123";

void setup() 
{
    Serial.begin(9600);//serialEvent();
    mySerial.begin(9600);

    pinMode(relay, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(mos, INPUT);
    pinMode(ir, INPUT);
    pinMode(smoke, INPUT);
    pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin1, INPUT); // Sets the echoPin as an Input 
    pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin2, INPUT); // Sets the echoPin as an Input 
   
    digitalWrite(relay, HIGH);
    myservo1.attach(12);  //Base
    
    digitalWrite(buzzer, HIGH);
    
    myservo1.write(100);
      delay(2000); 
    digitalWrite(relay, LOW);
      
    //IOT based automatic waste segregation and monitoring system
    lcd.begin(16, 2);lcd.cursor();
    lcd.print("IOT Based Automa");
    lcd.setCursor(0,1);
    lcd.print("tic Waste Segreg");
      delay(2500);

    lcd.clear();
    lcd.print("ation Monitoring");
    lcd.setCursor(0,1);
    lcd.print("    System      "); 
      delay(2500);

    wifiinit();
      delay(2500);

    lcd.clear();
    lcd.print("M:");//2-3-4,0
    lcd.setCursor(5,0);
    lcd.print("I:"); //7-8-9,0
    lcd.setCursor(11,0);
    lcd.print("S:"); //13-14-15,0

    lcd.setCursor(0,1);
    lcd.print("D:"); //2,1
    lcd.setCursor(8,1);
    lcd.print("W:"); //10,1
}

char bf3[50];
int g=0,f=0,count=0,lc=0;

char mos_string[10];
char gas_string[10];

void loop() 
{
  
 memset(mos_string,'\0',strlen(mos_string));
 if(digitalRead(mos) == LOW)
   {
    lcd.setCursor(2,0);lcd.print("Wet");   
    strcpy(mos_string,"Wet");
   }
 if(digitalRead(mos) == HIGH)
   {
    lcd.setCursor(2,0);lcd.print("Dry");   
    strcpy(mos_string,"Dry"); 
   }

 memset(gas_string,'\0',strlen(gas_string));
 if(digitalRead(smoke) == LOW)
   {
    lcd.setCursor(13,0);lcd.print("ON ");   
    strcpy(gas_string,"ON");
    beep();
       upload(mos_string,lvl_wet,lvl_dry,gas_string);
   }
 if(digitalRead(smoke) == HIGH)
   {
    lcd.setCursor(13,0);lcd.print("OFF");   
    strcpy(gas_string,"OFF"); 
   }


 if(digitalRead(ir) == LOW)
   {
      lcd.setCursor(7,0);lcd.print("ON ");   
       delay(3000);
     memset(mos_string,'\0',strlen(mos_string));        
     if(digitalRead(mos) == LOW)
       {
        lcd.setCursor(2,0);lcd.print("Wet ");
         strcpy(mos_string,"Wet");
        digitalWrite(relay, HIGH);delay(1000);  
        for(int it=100;it>=10;it--) //Left
           {
              myservo1.write(it);
            delay(15);
           }
           
           delay(3000);             
        
        for(int it=10;it<=100;it++) //Center 
           {
              myservo1.write(it);
            delay(15);
           }
        digitalWrite(relay, LOW);delay(1000);  
        upload(mos_string,lvl_wet,lvl_dry,gas_string);
        goto mn;   
       }
       
     if(digitalRead(mos) == HIGH)
       {
        lcd.setCursor(2,0);lcd.print("Dry");  
         strcpy(mos_string,"Dry");
        digitalWrite(relay, HIGH);delay(1000);
        for(int it=100;it<=190;it++) //Right
           {
              myservo1.write(it);
            delay(15);
           }
           
           delay(3000);             
        
        for(int it=190;it>=100;it--) //Center 
           {
              myservo1.write(it);
            delay(15);
           }   
        digitalWrite(relay, LOW);delay(1000);   
        upload(mos_string,lvl_wet,lvl_dry,gas_string);
        goto mn;      
       }
    mn:   
    delay(1500);    
   }
 if(digitalRead(ir) == HIGH)
   {
      lcd.setCursor(7,0);lcd.print("OFF");   
   }  

 dista=0;
 for(rtr1=0;rtr1<5;rtr1++)
   {
    dista = ultra_dist1();
    dist1 = (dist1 + dista);
      delay(10);
   }
 dist1 = (dist1/5);
 //lcd.setCursor(2,0);convertl(dist1);
 if(dist1 > 190)
   {
    dist1 = 190;
   }
 if(dist1 < 30)
   {
    dist1 = 30;  
   }
 lvl_dry = map(dist1,190,30,0,100);
 lcd.setCursor(2,1);convertl(lvl_dry);

 for(rtr2=0;rtr2<5;rtr2++)
   {
    dista = ultra_dist2();
    dist2 = (dist2 + dista);
      delay(10);
   }
 dist2 = (dist2/5);
 //lcd.setCursor(10,0);convertl(dist2);
 if(dist2 > 190)
   {
    dist2 = 190;
   }
 if(dist2 < 30)
   {
    dist2 = 30;  
   }
 lvl_wet = map(dist2,190,30,0,100);
 lcd.setCursor(10,1);convertl(lvl_wet);


 if(lvl_dry > 80)
   {beep();
    sts1++;
    if(sts1 >= 2){sts1=2;}
    if(sts1 == 1)
      {
        upload(mos_string,lvl_wet,lvl_dry,gas_string);
      }
   }
 else
   {
    sts1=0;  
   }

 if(lvl_wet > 80)
   {beep();
    sts2++;
    if(sts2 >= 2){sts2=2;}
    if(sts2 == 1)
      {
        upload(mos_string,lvl_wet,lvl_dry,gas_string);
      }
   }
 else
   {
    sts2=0;  
   }

}

char bf2[50];
void upload(const char *s1,int s2,int s3,const char *s4)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("U");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");
    
  //http://projectsfactoryserver.in/storedata.php?name=pf5&s1=25&s2=35
  //sprintf(buff,"GET http://embeddedspot.top/iot/storedata.php?name=iot139&s1=%u&s2=%u&s3=%u\r\n\r\n",s1,s2);
  
      delay(8000);
     
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot671&s1=%s&s2=%u&s3=%u&s4=%s\r\n\r\n",s1,s2,s3,s4);
      
//      memset(buff,0,strlen(buff));   
  //    sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot4&s1=%s\r\n\r\n",s1);
         
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
         
              delay(2000);
              
              mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" ");  
}

char readserver(void)
{
  char t;
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("R");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //http://projectsfactoryserver.in/last.php?name=amvi001L

      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/last.php?name=iot4L\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
       //read status
        while(1)
        {
           while(!mySerial.available());
            t = mySerial.read();
           // Serial.print(t);
            if(t == '*' || t == '#')
            {
              if(t == '#')return 0;
              while(!mySerial.available());
               t = mySerial.read();
             //  Serial.print(t);
               delay(1000);
                myserialFlush();
               return t;
            }
        }
              delay(2000);
              
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
return t;
}

void clearserver(void)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("C");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s10=0\r\n\r\n");
      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot4&s10=0\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
          delay(2000);
          myserialFlush();
                    
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
}

void wifiinit()
{
 char ret;  
  st:
  mySerial.println("ATE0");
  ret  = check((char*)"OK",50);
  mySerial.println("AT");
  ret  = check((char*)"OK",50);
  if(ret != 0)
  {
    delay(1000);
   goto st; 
  }
  
     lcd.clear();lcd.setCursor(0, 0);lcd.print("CONNECTING");  
  mySerial.println("AT+CWMODE=1");
   ret  = check((char*)"OK",50);
 cagain:
    
  myserialFlush();
  mySerial.print("AT+CWJAP=\"");
  mySerial.print(ssid);
  mySerial.print("\",\"");
  mySerial.print(password);
  mySerial.println("\"");
  if(check((char*)"OK",300))goto cagain;    
  mySerial.println("AT+CIPMUX=1");
  delay(1000);
 

  lcd.clear();lcd.setCursor(0, 0);lcd.print("WIFI READY"); 
}

void converts(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   Serial.write(a);
   Serial.write(c);
   Serial.write(e); 
   Serial.write(g);
   Serial.write(h);
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
   //lcd.write(a);
   //lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}

void convertk(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
  // lcd.write(e); 
  // lcd.write(g);
   lcd.write(h);
}
