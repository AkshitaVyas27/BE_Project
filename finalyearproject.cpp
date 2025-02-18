#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>//I2c
#include <LiquidCrystal_I2C.h>
#include <I2Cdev.h>
#include <MPU6050.h>

MPU6050 mpu;
intsensorValue = 0;
int16_t ax, ay, az; // raw accelerometer data
int16_tgx, gy, gz; // raw gyroscope data
floataccelMagnitude; // accelerometer magnitude
floatimpactThreshold = 5000; // set the impact threshold
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <EEPROM.h>
//sender phone number with country code
const String PHONE = "+916375699008";
constinttotalPhoneNo = 5;
String phoneNo[totalPhoneNo] = {"","","","",""};
intoffsetPhone[totalPhoneNo] = {0,13,26,39,52};
String tempPhone = "";

//GPS Module TX pin to NodeMCU D1
//GPS Module RX pin to NodeMCU D2
#define rxGPS D3 //D1
#define txGPS D4 //D2
SoftwareSerialneogps(rxGPS,txGPS);
TinyGPSPlusgps;

//GSM Module TX pin to NodeMCU D3
//GSM Module RX pin to NodeMCU D4
#define rxGSM D6 //D3
#define txGSM D5 //D4
SoftwareSerial sim800(rxGSM,txGSM);

String smsStatus;
String senderNumber;
String receivedDate;
String msg;
boolean DEBUG_MODE = 1;
/* setup function */
voidsetup() {
    lcd.init();   // initialize the lcd 
  lcd.init();

    Wire.begin();//I2c
  //Serial.begin(9600);
  mpu.initialize();
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
  pinMode(D0, OUTPUT);
         digitalWrite(D0, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(200);                      // wait for a second
  digitalWrite(D0, HIGH);   // turn the LED off by making the voltage LOW
  delay(200); 
  digitalWrite(D0, LOW); 
    delay(200);                      // wait for a second
  digitalWrite(D0, HIGH);   // turn the LED off by making the voltage LOW
  delay(1000); 

  // Print a message to the LCD.
  lcd.backlight();
pinMode(D0, OUTPUT);pinMode(D4, OUTPUT);
         digitalWrite(D0, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(D0, HIGH);   // turn the LED off by making the voltage LOW
  delay(1000); 
  digitalWrite(D0, LOW); 
    delay(1000);                      // wait for a second
  digitalWrite(D0, HIGH);   // turn the LED off by making the voltage LOW
  delay(1000); 

  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("WELCOME");

  Serial.begin(115200);
  Serial.println("NodeMCU USB serial initialize");
  sim800.begin(9600);
  Serial.println("SIM800L serial initialize");
  neogps.begin(9600);
  Serial.println("NEO6M serial initialize");
  sim800.listen();
  neogps.listen();

 
  EEPROM.begin(512);

  Serial.println("List of Registered Phone Numbers");
  for (inti = 0; i<totalPhoneNo; i++){
    phoneNo[i] = readFromEEPROM(offsetPhone[i]);
    if(phoneNo[i].length() != 13)
      {phoneNo[i] = "";Serial.println(String(i+1)+": empty");}
    else
      {Serial.println(String(i+1)+": "+phoneNo[i]);}
  }

  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";

  delay(9000);
  sim800.println("AT+CMGF=1"); //SMS text mode
  delay(1000);
  sim800.println("AT+CLIP=1"); //Enable Caller ID
  delay(500);

        phoneNo[0] = "+916375699008";
      phoneNo[1] = "+917014689809";
      phoneNo[2] = "+917627029203";
      phoneNo[3] = "+916378638280";
      phoneNo[4] = "+916375699008";

       writeToEEPROM(offsetPhone[0],phoneNo[4]);
       writeToEEPROM(offsetPhone[1],phoneNo[4]);
       writeToEEPROM(offsetPhone[2],phoneNo[4]);
       writeToEEPROM(offsetPhone[3],phoneNo[4]);
       writeToEEPROM(offsetPhone[4],phoneNo[4]);
   digitalWrite(D4, HIGH); 
delay(20000);
  
}

voidloop() {
  //sim800.listen();
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sensorValue = analogRead(A0);
               lcd.setCursor(0, 1);
  lcd.print("MQ3  ");  lcd.print(sensorValue);
    if(sensorValue>1000){//MQ3
     digitalWrite(D0, LOW); 
           lcd.setCursor(0, 1);
  lcd.print("Alcohol Detected");
       String phoneNumber1 = "+916375699008";
       digitalWrite(D4, LOW); 
    sendLocation2(phoneNumber1);
    lcd.clear();
    }
    Serial.println(sensorValue);
  accelMagnitude = sqrt(ax*ax + ay*ay + az*az);
                 lcd.setCursor(0, 0);
  lcd.print("Impect  ");  lcd.print(accelMagnitude);
   if (accelMagnitude>impactThreshold) {       digitalWrite(D4, LOW); 
    Serial.println("Impact detected!");
    digitalWrite(D0, LOW);  delay(200);
      lcd.setCursor(0, 0);
  lcd.print("Accident Aleart");
  String phoneNumber1 = "+916375699008";
    sendLocation1(phoneNumber1);
    delay(1500);
      lcd.clear();
digitalWrite(D0, HIGH); 
  lcd.clear();
  }
  
  delay(10);

while(sim800.available()){
  parseData(sim800.readString());
}

while(Serial.available())  {
  sim800.println(Serial.readString());
}

} 

voidparseData(String buff){
  Serial.println(buff);

  if(buff.indexOf("RING") > -1)
  {
    boolean flag = 0;
    String callerID = "";

    //+CLIP: "03001234567",129,"",0,"",0
    if(buff.indexOf("+CLIP:")){
     unsignedint index, index1;
     
      index = buff.indexOf("\"");
      index1 = buff.indexOf("\"", index+1);
      callerID = buff.substring(index+2, index1);
      callerID.trim();
      if(callerID.length() == 13){

        flag = comparePhone(callerID);
      }

      elseif(callerID.length() == 10){
        
        flag = compareWithoutCountryCode(callerID);
        callerID = "0" + callerID;
      }
   }
    if(flag == 1){
      sim800.println("ATH");
      delay(1000);
      sendLocation(callerID);
    }
    else{
      sim800.println("ATH");
      debugPrint("The phone number is not registered.");
    }
    return;
  }
  
  unsignedintlen, index;
   index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();

  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);

    if(cmd == "+CMTI"){
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      sim800.println(temp); 
    }

    elseif(cmd == "+CMGR"){
      extractSms(buff);

      if(msg.equals("r") &&phoneNo[0].length() == 13) {
        writeToEEPROM(offsetPhone[0],senderNumber);
        phoneNo[0] = senderNumber;
        String text = "Number is Registered: ";
        text = text + senderNumber;
        debugPrint(text);
        Reply("Number is Registered", senderNumber);
      }

      if(comparePhone(senderNumber)){
        doAction(senderNumber);
        //sendLocation();
      }
    }
  }
  else{
  }
}

voiddoAction(String phoneNumber){

  if(msg == "send location"){  
      sendLocation(phoneNumber);
  }

  if(msg == "r2="){  

      Serial.println(offsetPhone[1]);
      writeToEEPROM(offsetPhone[1],tempPhone);
      phoneNo[1] = tempPhone;
      String text = "Phone2 is Registered: ";
      text = text + tempPhone;
      debugPrint(text);
      Reply(text, phoneNumber);
  }

  elseif(msg == "r3="){  
      writeToEEPROM(offsetPhone[2],tempPhone);
      phoneNo[2] = tempPhone;
      String text = "Phone3 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }

  elseif(msg == "r4="){  
      writeToEEPROM(offsetPhone[3],tempPhone);
      phoneNo[3] = tempPhone;
      String text = "Phone4 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }

  elseif(msg == "r5="){  
      writeToEEPROM(offsetPhone[4],tempPhone);
      phoneNo[4] = tempPhone;
      String text = "Phone5 is Registered: ";
      text = text + tempPhone;
      Reply(text, phoneNumber);
  }

  elseif(msg == "list"){  
      String text = "";
      if(phoneNo[0])
        text = text + phoneNo[0]+"\r\n";
      if(phoneNo[1])
        text = text + phoneNo[1]+"\r\n";
      if(phoneNo[2])
        text = text + phoneNo[2]+"\r\n";
      if(phoneNo[3])
        text = text + phoneNo[3]+"\r\n";
      if(phoneNo[4])
        text = text + phoneNo[4]+"\r\n";
        
      debugPrint("List of Registered Phone Numbers: \r\n"+text);
      Reply(text, "+916375699008");
  }
  elseif(msg == "del=1"){  
      writeToEEPROM(offsetPhone[0],"");
      phoneNo[0] = "";
      Reply("Phone1 is deleted.", phoneNumber);
  }
  elseif(msg == "del=2"){  
      writeToEEPROM(offsetPhone[1],"");
      phoneNo[1] = "";
      debugPrint("Phone2 is deleted.");
      Reply("Phone2 is deleted.", phoneNumber);
  }
  elseif(msg == "del=3"){  
      writeToEEPROM(offsetPhone[2],"");
      phoneNo[2] = "";
      debugPrint("Phone3 is deleted.");
      Reply("Phone3 is deleted.", phoneNumber);
  }
  elseif(msg == "del=4"){  
      writeToEEPROM(offsetPhone[3],"");
      phoneNo[3] = "";
      debugPrint("Phone4 is deleted.");
      Reply("Phone4 is deleted.", phoneNumber);
  }
  elseif(msg == "del=5"){  
      writeToEEPROM(offsetPhone[4],"");
      phoneNo[4] = "";
      debugPrint("Phone5 is deleted.");
      Reply("Phone5 is deleted.", phoneNumber);
  }

  if(msg == "del=all"){  
      writeToEEPROM(offsetPhone[0],"");
      writeToEEPROM(offsetPhone[1],"");
      writeToEEPROM(offsetPhone[2],"");
      writeToEEPROM(offsetPhone[3],"");
      writeToEEPROM(offsetPhone[4],"");
      phoneNo[0] = "";
      phoneNo[1] = "";
      phoneNo[2] = "";
      phoneNo[3] = "";
      phoneNo[4] = "";
      offsetPhone[0] = NULL;
      offsetPhone[1] = NULL;
      offsetPhone[2] = NULL;
      offsetPhone[3] = NULL;
      offsetPhone[4] = NULL;
      debugPrint("All phone numbers are deleted.");
      Reply("All phone numbers are deleted.", phoneNumber);
  }
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  tempPhone = "";
}
voidextractSms(String buff){
   unsignedint index;
   
    index = buff.indexOf(",");
    smsStatus = buff.substring(1, index-1); 
    buff.remove(0, index+2);
    
    senderNumber = buff.substring(0, 13);
    buff.remove(0,19);
   
    receivedDate = buff.substring(0, 20);
    buff.remove(0,buff.indexOf("\r"));
    buff.trim();
    
    index =buff.indexOf("\n\r");
    buff = buff.substring(0, index);
    buff.trim();
    msg = buff;
    buff = "";
    msg.toLowerCase();

    String tempcmd = msg.substring(0, 3);
    if(tempcmd.equals("r1=") || tempcmd.equals("r2=") ||
       tempcmd.equals("r3=") || tempcmd.equals("r4=") ||
       tempcmd.equals("r5=")){
        
        tempPhone = msg.substring(3, 16);
        msg = tempcmd;

    }       
}

voidReply(String text, String Phone)
{
  //return;
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\""+Phone+"\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("SMS Sent Successfully.");
\
 
  smsStatus = "";
  senderNumber="";
  receivedDate="";
  msg="";
  tempPhone = "";  
}

voidwriteToEEPROM(intaddrOffset, const String &strToWrite)
{
  byte len = 13; //strToWrite.length();
   for (inti = 0; i<len; i++)
  {
    EEPROM.write(addrOffset + i, strToWrite[i]);
  }

  EEPROM.commit();

}

String readFromEEPROM(intaddrOffset)
{
  intlen = 13;
  chardata[len + 1];
  for (inti = 0; i<len; i++)
  {
    data[i] = EEPROM.read(addrOffset + i);
  }
  data[len] = '\0';
  return String(data);
}


booleancomparePhone(String number)
{
  boolean flag = 0;
  //--------------------------------------------------
  for (inti = 0; i<totalPhoneNo; i++){
    phoneNo[i] = readFromEEPROM(offsetPhone[i]);
    if(phoneNo[i].equals(number)){
      //Serial.println(phoneNo[i]);
      flag = 1;
      break;
    }
  }
  return flag;
}


booleancompareWithoutCountryCode(String number)
{
  boolean flag = 0;
  for (inti = 0; i<totalPhoneNo; i++){
    phoneNo[i] = readFromEEPROM(offsetPhone[i]);
    //remove first 3 digits (country code)
    phoneNo[i].remove(0,3);
    //Serial.println("meee1: "+phoneNo[i]);
    if(phoneNo[i].equals(number)){
      //Serial.println(phoneNo[i]);
      flag = 1;
      break;
    }
  }
  return flag;
}

 voiddebugPrint(String text){
  if(DEBUG_MODE == 1)
    Serial.println(text);
}

voidsendLocation(String phoneNumber)
{
  // Can take up to 60 seconds
  booleannewData = false;
  for (unsignedlong start = millis(); millis() - start <2000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
        {newData = true;break;}
    }
  }
 
  //If newData is true
  if(newData)
  {
    newData = false;
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    //String speed = String(gps.speed.kmph());
    
    String text = "Latitude= " + latitude;
    text += "\n\r";
    text += "Longitude= " + longitude;
    text += "\n\r";
    text += "Speed= " + String(gps.speed.kmph()) + " km/h";
    text += "\n\r";
    text += "Altitude= " + String(gps.altitude.meters()) + " meters";
    //text += "\n\r";
    //text += "Connected Satellites= " + String(gps.satellites.value());
    text += "\n\r";
    text += "http://maps.google.com/maps?q=loc:" + latitude + "," + longitude;
         
    debugPrint(text);
    //delay(300);
    Reply(text, phoneNumber);
  }
}

void sendLocation1(String phoneNumber)
{
  Serial.print("aaya");
  // Can take up to 60 seconds
  booleannewData = false;
  for (unsignedlong start = millis(); millis() - start <2000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
        {newData = true;break;}
    }
  }
 
  //If newData is true
  if(newData)
  {
    newData = false;
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    //String speed = String(gps.speed.kmph());
   
      String text1 = "Emergency.... Accident alert " +    text1 += "http://maps.google.com/maps?q=loc:" + latitude + "," + longitude;
   
          Serial.println(text1);
    //debugPrint(text1);
    //delay(300);
    Reply(text1, phoneNumber);
  }
}

void sendLocation2(String phoneNumber)
{
 
  // Can take up to 60 seconds
  booleannewData = false;
  for (unsignedlong start = millis(); millis() - start <2000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
        {newData = true;break;}
    }
  }

  //If newData is true
  if(newData)
  {
    newData = false;
    String latitude = String(gps.location.lat(), 6);
    String longitude = String(gps.location.lng(), 6);
    //String speed = String(gps.speed.kmph());

      String text1 = "Alcohol Detected " +    text1 += "http://maps.google.com/maps?q=loc:" + latitude + "," + longitude;
   
          Serial.println(text1);
    //debugPrint(text1);
    //delay(300);
    Reply(text1, phoneNumber);
  }
}


 

