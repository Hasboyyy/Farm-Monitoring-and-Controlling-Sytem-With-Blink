// Nama  : Muhamad Arrizal Hasby
// Nim   : 120290023
// Prodi : Sains Atmosfer dan Keplanetan
// KELOMPOK KKN-PPM ITERA 162


#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>

const long utcOffsetInSeconds = 25200;  // set offset
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);

char Time[ ] = "00:00:00";
char Date[ ] = "00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN "yKwXdg_-ksuKr3VweAmudXie80le55-r"
#define DHTPIN D7
#define DHTTYPE DHT11
#define sensorhujan D6
#define relay D5

SimpleTimer timer;
DHT dht(DHTPIN, DHTTYPE);

int sensorsoil = A0, soil;
const int udara = 737;
const int air = 316;
int interval = (udara - air) / 3;
int timer1;
int mode;

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "POCO X3 Pro";
char pass[   ] = "21062002";

BLYNK_WRITE(V3){
  int pompa = param.asInt();
  if (pompa == LOW){
    digitalWrite(relay, HIGH);
    Blynk.virtualWrite(V7, "Mati");
  } else {
    digitalWrite(relay, LOW);
    Blynk.virtualWrite(V7, "Hidup");
  }
}

void OFF(){
  timer.deleteTimer(timer1);
}

BLYNK_WRITE(V6){
  mode = param.asInt();
  if (mode == 1) {
    timer1=timer.setInterval(1L, autopump);
    timer1;
  }
  else{
    OFF();
  }
}

void testwifi(){
  lcd.setCursor(0, 0);
  lcd.print("Pertanian Cerdas");
  lcd.setCursor(0, 1);
  lcd.print("Desa Talang Jawa");
  delay(5000);
  lcd.clear();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    autopump();
  }
  Blynk.config(auth, "blynk.cloud", 80);
  Blynk.syncAll();
  lcd.setCursor(0, 0);
  lcd.print("   Terhubung    ");
  lcd.setCursor(0, 1);
  lcd.print("    Internet    ");
  delay(5000);
  Blynk.virtualWrite(V3, HIGH);
}

void autopump(){
  soil = analogRead(sensorsoil);
  int soil1 = map(soil, 737, 316, 0, 100);

  lcd.setCursor(0, 0);
  lcd.println(String("SoilMoist = ") + soil1 + (" % "));
  Blynk.virtualWrite(V0, soil1);
  lcd.setCursor(0, 1);
  lcd.print("Kondisi = ");
  if (soil > air && soil < (air + interval)) {
    lcd.println("Basah ");
    digitalWrite(relay, HIGH);
    Blynk.virtualWrite(V3, LOW);
    Blynk.virtualWrite(V4, "Basah");
    Blynk.virtualWrite(V7, "Mati");
  } else if (soil > air && soil < (udara - interval)) {
    lcd.println("Lembab");
    Blynk.virtualWrite(V4, "Lembab");
    digitalWrite(relay, HIGH);
    Blynk.virtualWrite(V3, LOW);
    Blynk.virtualWrite(V7, "Mati");
  } else if (soil < udara && soil > (udara - interval)) {
    lcd.println("Kering");
    Blynk.virtualWrite(V4, "Kering");
    digitalWrite(relay, LOW);
    Blynk.virtualWrite(V3, HIGH);
    Blynk.virtualWrite(V7, "Hidup");
  }
  delay(500);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Gagal membaca sensor!!!"));
    return;
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String("Temp = ") + t + (" C  "));
    lcd.setCursor(0, 1);
    Blynk.virtualWrite(V1, t);
    lcd.print(String("Hum = ") + h + (" %   "));
    Blynk.virtualWrite(V2, h);
    delay(500);
  }

  int hujan = digitalRead(sensorhujan);
  Serial.println(sensorhujan);
  if(hujan == LOW){
    Blynk.virtualWrite(V5, "Hujan");
  }
  else if(hujan == HIGH){
    Blynk.virtualWrite(V5, "Tidak Hujan");
  }
  if (mode==0){
    timer.deleteTimer(timer1);
  }
}

void manupump(){
  soil = analogRead(sensorsoil);
  int soil1 = map(soil, 737, 316, 0, 100);

  lcd.setCursor(0, 0);
  lcd.println(String("SoilMoist = ") + soil1 + (" % "));
  Blynk.virtualWrite(V0, soil1);
  lcd.setCursor(0, 1);
  lcd.print("Kondisi = ");
  if (soil > air && soil < (air + interval)) {
    lcd.println("Basah ");
    Blynk.virtualWrite(V4, "Basah");
  } else if (soil > air && soil < (udara - interval)) {
    lcd.println("Lembab");
    Blynk.virtualWrite(V4, "Lembab");
  } else if (soil < udara && soil > (udara - interval)) {
    lcd.println("Kering");
    Blynk.virtualWrite(V4, "Kering");
  }
  delay(500);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Gagal membaca sensor!!!"));
    return;
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String("Temp = ") + t + (" C  "));
    lcd.setCursor(0, 1);
    Blynk.virtualWrite(V1, t);
    lcd.print(String("Hum = ") + h + (" %   "));
    Blynk.virtualWrite(V2, h);
    delay(500);
  }

  int hujan = digitalRead(sensorhujan);
  Serial.println(sensorhujan);
  if(hujan == LOW){
    Blynk.virtualWrite(V5, "Hujan");
  }
  else if(hujan == HIGH){
    Blynk.virtualWrite(V5, "Tidak Hujan");
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  lcd.backlight();
  pinMode(sensorhujan, INPUT);
  pinMode(sensorsoil, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  testwifi();

  Blynk.virtualWrite(V7, "Mati");
}

void loop() {
  Blynk.virtualWrite(V11, "SMART-FARM DESA TALANG JAWA");
  Blynk.run();
  timer.run();
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();

  second_ = second(unix_epoch);
  if (last_second != second_) {
 

    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);

 

    Time[7] = second_ % 10 + 48;
    Time[6] = second_ / 10 + 48;
    Time[4]  = minute_ % 10 + 48;
    Time[3]  = minute_ / 10 + 48;
    Time[1]  = hour_   % 10 + 48;
    Time[0]  = hour_   / 10 + 48;

 

    Date[0]  = day_   / 10 + 48;
    Date[1]  = day_   % 10 + 48;
    Date[3]  = month_  / 10 + 48;
    Date[4]  = month_  % 10 + 48;
    Date[8] = (year_   / 10) % 10 + 48;
    Date[9] = year_   % 10 % 10 + 48;

    Blynk.virtualWrite(V8, Time);
    Blynk.virtualWrite(V9, Date);
    Blynk.virtualWrite(V10, daysOfTheWeek[timeClient.getDay()]);


    last_second = second_;
   

  }
  manupump();
  while (WiFi.status() != WL_CONNECTED)
    {
      autopump();
    }
}