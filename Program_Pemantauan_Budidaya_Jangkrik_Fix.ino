#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_BME280.h>
#include <FirebaseESP8266.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

#define DHTPIN 13 // Deklarasi PIN DHT11
#define ledMerah 0
#define ledHijau 2
#define pinBuzzer 14
#define DHTTYPE DHT11
#define FIREBASE_HOST "https://pemantauan-budidaya-jangkrikv3-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "B0Ty5j9tQG7D0AZhLDHAvf1jAcXzZCPuBkyPBM3u"
#define WIFI_SSID "Jarfa_Net"
#define WIFI_PASSWORD "Jarfa1712"
#define SEALEVELPRESSURE_HPA (1013.25) //nilai awal untuk pressure

//data ssid dan password 
const char* ssid     = "Jarfa_Net";
const char* password = "Jarfa1712";
float humi, temp, lux; //deklarasi variabel

//String apikey = "LT1F0GYFYOJ5PW7J"; //write api key
//const char *server = "api.thingspeak.com";
//WiFiClient client;
//unsigned long myChannelNumber = 2133065;
//const char * myWriteAPIKey = "LT1F0GYFYOJ5PW7J";

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
Adafruit_BME280 bme;
int wifiStatus;  // membuat variable wifiStatus utk menyimpan data status wifi
FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  //membuat label status terkoneksi dengan wifi
  WiFi.begin(ssid, password);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("tidak terhubung");
    delay(1000);
  }

  bool status;
  Wire.begin();
  lightMeter.begin();  
  status = bme.begin(0x76);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(ledMerah, OUTPUT);
  pinMode(ledHijau, OUTPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
//  ThingSpeak.begin(client);
}

void loop() {
 float  value_lux = lightMeter.readLightLevel(), 
        value_humi = bme.readHumidity(),
        value_temp = dht.readTemperature();

  String value_status_temp, 
         value_status_humi, 
         value_status_lux,
         value_status_buzzer;


  //if (isnan(lux)||isnan(humi)||isnan(temp)) {
 //   Serial.println("Gagal membaca sensor");
  //  delay(1000);
  //  return;
  //  }
  Serial.print("Suhu : "); 
  Serial.print(value_temp); 
  Serial.println(" C");

  if ((value_temp > 32) || (value_temp < 20)){
    value_status_temp = "Tidak Normal";
    Serial.print("Status Suhu : ");
    Serial.println(value_status_temp);
    }else{
    value_status_temp = "Normal";
    Serial.print("Status Suhu : ");
    Serial.println(value_status_temp);}
  
  Serial.print("Humidity = ");
  Serial.print(value_humi);
  Serial.println(" %");
   if ((value_humi > 80) || (value_humi < 60)){
    value_status_humi = "Tidak Normal";
    Serial.print("Status Kelembaban : ");
    Serial.println(value_status_humi);
    }else{
    value_status_humi = "Normal";
    Serial.print("Status Kelembaban : ");
    Serial.println(value_status_humi);}

  Serial.print("Light: ");
  Serial.print(value_lux);
  Serial.println(" lx");

   if ((value_lux > 50) || (value_lux < 20)){
    value_status_lux = "Tidak Normal";
    Serial.print("Status Intensitas Cahaya : ");
    Serial.println(value_status_lux);
    }else{
    value_status_lux = "Normal";
    Serial.print("Status Intensitas Cahaya : ");
    Serial.println(value_status_lux);}

if ((value_status_temp == "Tidak Normal")||(value_status_humi == "Tidak Normal")||(value_status_lux == "Tidak Normal")){

    tone(pinBuzzer, 10000, 500);
    value_status_buzzer = "ON";
    digitalWrite(ledMerah, HIGH);
    digitalWrite(ledHijau, LOW);
    delay(1000);
    digitalWrite(ledMerah, LOW);
    delay(1000);
    Serial.println("Buzzer : ");
    Serial.println(value_status_buzzer);
  }else{
    digitalWrite(pinBuzzer,LOW);
    value_status_buzzer = "OFF";
    digitalWrite(ledHijau, HIGH);
    digitalWrite(ledMerah, LOW);
    delay(1000);
    Serial.print("Buzzer : ");
    Serial.println(value_status_buzzer);}


//Pengiriman data ke firebase
(Firebase.setFloat(firebaseData, "/value_temp", value_temp));
(Firebase.setString(firebaseData, "/value_status_temp", value_status_temp));

(Firebase.setFloat(firebaseData, "/value_humi", value_humi));
(Firebase.setString(firebaseData, "/value_status_humi", value_status_humi));

(Firebase.setFloat(firebaseData, "/value_lux", value_lux));
(Firebase.setString(firebaseData, "/value_status_lux", value_status_lux));

(Firebase.setString(firebaseData, "/value_status_buzzer", value_status_buzzer)); 
  
  delay(1000);
}
