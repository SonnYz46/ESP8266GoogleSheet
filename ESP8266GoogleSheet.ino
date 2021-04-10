#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//----------------------------------------

#define ON_Board_LED 2  //--> Defining an On Board LED(GPIO2), used for indicators when the process of connecting to a wifi router

const char* ssid = "WIFI_NAME"; //--> ชื่อ WIFI
const char* password = "WIFI_PASSWORD"; //-->  รหัสผ่าน WIFI 
//----------------------------------------

//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; //--> Create a WiFiClientSecure object.

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

String GAS_ID = "YOUR_SPREADSHEET_SCRIPT_ID"; //--> spreadsheet *SCRIPT* ID //ใส่ Script ID ของ Spreadsheet

//============================================ void setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //ตั้ง baudrate เป็น 115200
  delay(500);

  WiFi.begin(ssid, password); //--> Connect to your WiFi router //เริ่มเชื่อมต่อกับ WIFI ที่เรากำหนดให้
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> 

  //---------------------------------------- Wait for connection //รอการเชื่อมต่อกับ WIFI
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------ไฟ Onboard จะกระพริบตอนพยายามเชื่อมต่อกับ WIFI
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> ไฟ Onboard จะดับตอนเชื่อมต่อสำเร็จแล้ว
  //----------------------------------------ถ้าเชื่อมต่อสำเร็จ จะสามารถดู IP ได้ใน Serial Monitor
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}
//==============================================================================
//============================================================================== void loop
void loop() {

  now = millis();
  // Publishes new data to google sheet every 10 seconds //ส่งข้อมูลไปทุกๆ 10 วินาที (หน่วย ms)
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
   
   /* EXAMPLE OF DATA SENDING ::::::::::::::::
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Humidity2: ");
    Serial.print(h);
    Serial.print(" %\t Temperature2: ");
    Serial.print(t);
    Serial.print(" *C ");*/ 
    sendData(t,h,t,h,t); //sendData(ข้อมูลที่จะส่ง1,ข้อมูลที่จะส่ง2,ข้อมูลที่จะส่ง3,ข้อมูลที่จะส่ง4,ข้อมูลที่จะส่ง5)
    
  }

  
}
//*****
//==============================================================================

void sendData(float value,float value2, float value3, float value4, float value5) { //ใช้เพิ่มตัวแปรและกำหนดตัวแปรที่จะส่งข้อมูล
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host //เชื่อมต่อกับ Host ของ Google
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Process Data //ในตอนนี้บรรทัดที่ 103-107 เป็นการกำหนดชื่อตัวแปรข้อมูลที่จะส่ง และ บรรทัดที่ 108 เป็นการทำ url ที่จะส่งข้อมูล

  float string_temp = value; 
  float string_humi = value2;
  float string_temp2 = value3;
  float string_humi2 = value4;
  float string_temp3 = value5;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi + "&temp2=" + string_temp2 + "&humi2=" + string_humi2 + "&temp3=" + string_temp3; //  4 variables 
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //---------------------------------------
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 
//===============================================
