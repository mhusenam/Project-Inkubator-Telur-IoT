#define BLYNK_TEMPLATE_ID   " "
#define BLYNK_TEMPLATE_NAME "Inkubator Penetas Ayam"
#define BLYNK_AUTH_TOKEN    ""

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <UniversalTelegramBot.h>

// --- Konfigurasi WiFi ---
char ssid[] = "realme 7";
char pass[] = "ojokdigawe";

#define BOTtoken " "
#define CHAT_ID  " "
// --- Google Spreadsheets Webhook URL ---
String GAS_URL = " "; 

// --- Pin Hardware ---
#define DHTPIN          4   // Pin Data DHT22 (GPIO 4)
#define DHTTYPE         DHT22
#define RELAY_LAMPU_PIN 14  // Relay Lampu (GPIO 14)
#define RELAY_KIPAS_PIN 5   // Relay Kipas (GPIO 5)
#define RELAY_HUM_PIN   13  // Relay Humidifier (GPIO 13)
#define SERVO_VENT_PIN  16  // Servo Ventilasi (GPIO 16)
#define SERVO_EGG_PIN   17  // Servo Penggerak Telur (GPIO 17)

// --- Konfigurasi OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21         // Pin SDA Layar OLED (GPIO 21)
#define OLED_SCL 22         // Pin SCL Layar OLED (GPIO 22)
#define OLED_ADDRESS 0x3C

// --- Logika Relay Aktif LOW ---
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

DHT dht(DHTPIN, DHTTYPE);
Servo servoVentilasi;
Servo servoPenggerakTelur;
BlynkTimer timer;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClientSecure clientTelegram;
UniversalTelegramBot bot(BOTtoken, clientTelegram);

// --- Variabel Penyimpan Status Komponen ---
int statusLampu = 0;
int statusKipas = 0;
int statusHumidifier = 0;
int sudutServoTelor = 0;
int sudutServoVentilasi = 0;

float temp = 0.0;
float hum = 0.0;
unsigned long lastTelegramAlert = 0;

// ==========================================
// Virtual Pin Map Blynk:
// V0 : Temp (Suhu - Output)
// V1 : Relay Lampu (Input)
// V2 : Relay Kipas (Input)
// V3 : Relay Humidifier (Input)
// V4 : Servo Penggerak Telur (Input)
// V5 : Servo Ventilasi (Input)
// V6 : Hum (Kelembapan - Output)
// ==========================================

BLYNK_WRITE(V1) {
  statusLampu = param.asInt();
  digitalWrite(RELAY_LAMPU_PIN, statusLampu == 1 ? RELAY_ON : RELAY_OFF);
}

BLYNK_WRITE(V2) {
  statusKipas = param.asInt();
  digitalWrite(RELAY_KIPAS_PIN, statusKipas == 1 ? RELAY_ON : RELAY_OFF);
}

BLYNK_WRITE(V3) {
  statusHumidifier = param.asInt();
  digitalWrite(RELAY_HUM_PIN, statusHumidifier == 1 ? RELAY_ON : RELAY_OFF);
}

BLYNK_WRITE(V4) {
  sudutServoTelor = param.asInt();
  servoPenggerakTelur.write(sudutServoTelor);
}

BLYNK_WRITE(V5) {
  sudutServoVentilasi = param.asInt();
  servoVentilasi.write(sudutServoVentilasi);
}

// --- Fungsi Pengecekan Pesan Telegram ---
void cekPesanTelegram() {
  int numMsg = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < numMsg; i++) {
    if (String(bot.messages[i].chat_id) == CHAT_ID) {
      String text = bot.messages[i].text;
      text.toLowerCase();

      if (text == "/start" || text == "/help") {
        String welcome = "🤖 *BOT INKUBATOR SMART TELUR*\n\n";
        welcome += "Perintah Saklar Teks:\n";
        welcome += "📊 /status - Cek kondisi sensor & alat\n";
        welcome += "💡 /lampu_on atau 'nyalakan lampu'\n";
        welcome += "💡 /lampu_off atau 'matikan lampu'\n";
        welcome += "🌀 /kipas_on atau 'nyalakan kipas'\n";
        welcome += "🌀 /kipas_off atau 'matikan kipas'\n";
        welcome += "💧 /hum_on atau 'nyalakan hum'\n";
        welcome += "💧 /hum_off atau 'matikan hum'\n";
        bot.sendMessage(CHAT_ID, welcome, "Markdown");
      }
      else if (text == "/lampu_on" || text == "nyalakan lampu") {
        statusLampu = 1;
        digitalWrite(RELAY_LAMPU_PIN, RELAY_ON);
        Blynk.virtualWrite(V1, 1);
        bot.sendMessage(CHAT_ID, "💡 Lampu BERHASIL DINYALAKAN!", "");
      }
      else if (text == "/lampu_off" || text == "matikan lampu") {
        statusLampu = 0;
        digitalWrite(RELAY_LAMPU_PIN, RELAY_OFF);
        Blynk.virtualWrite(V1, 0);
        bot.sendMessage(CHAT_ID, "💡 Lampu BERHASIL DIMATIKAN!", "");
      }
      else if (text == "/kipas_on" || text == "nyalakan kipas") {
        statusKipas = 1;
        digitalWrite(RELAY_KIPAS_PIN, RELAY_ON);
        Blynk.virtualWrite(V2, 1);
        bot.sendMessage(CHAT_ID, "🌀 Kipas BERHASIL DINYALAKAN!", "");
      }
      else if (text == "/kipas_off" || text == "matikan kipas") {
        statusKipas = 0;
        digitalWrite(RELAY_KIPAS_PIN, RELAY_OFF);
        Blynk.virtualWrite(V2, 0);
        bot.sendMessage(CHAT_ID, "🌀 Kipas BERHASIL DIMATIKAN!", "");
      }
      else if (text == "/hum_on" || text == "nyalakan hum") {
        statusHumidifier = 1;
        digitalWrite(RELAY_HUM_PIN, RELAY_ON);
        Blynk.virtualWrite(V3, 1);
        bot.sendMessage(CHAT_ID, "💧 Humidifier BERHASIL DINYALAKAN!", "");
      }
      else if (text == "/hum_off" || text == "matikan hum") {
        statusHumidifier = 0;
        digitalWrite(RELAY_HUM_PIN, RELAY_OFF);
        Blynk.virtualWrite(V3, 0);
        bot.sendMessage(CHAT_ID, "💧 Humidifier BERHASIL DIMATIKAN!", "");
      }
      else if (text == "/status" || text == "cek status") {
        String msg = "📊 *STATUS INKUBATOR REAL-TIME*\n\n";
        msg += "🌡 Suhu: " + String(temp, 1) + " °C\n";
        msg += "💧 Kelembapan: " + String(hum, 1) + " %\n";
        msg += "💡 Lampu: " + String(statusLampu == 1 ? "ON" : "OFF") + "\n";
        msg += "🌀 Kipas: " + String(statusKipas == 1 ? "ON" : "OFF") + "\n";
        msg += "💦 Humidifier: " + String(statusHumidifier == 1 ? "ON" : "OFF") + "\n";
        msg += "📐 Servo Vent: " + String(sudutServoVentilasi) + "°\n";
        msg += "📐 Servo Egg: " + String(sudutServoTelor) + "°";
        bot.sendMessage(CHAT_ID, msg, "Markdown");
      }
    }
  }
}

// --- Membaca Sensor DHT22, Kirim Ke Blynk & Update OLED ---
void readDHT() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Gagal membaca sensor DHT22!");
    
    display.clearDisplay();
    display.setCursor(10, 15);
    display.println("ERROR SENSOR");
    display.setCursor(10, 35);
    display.println("Periksa DHT22!");
    display.display();
    return;
  }

  // Kirim ke Blynk
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V6, hum);

  // UPDATE LAYAR OLED
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(10, 0);
  display.println("INKUBATOR SMART IoT");

  display.setCursor(0, 14);
  display.print("Suhu : "); display.print(temp, 1); display.println(" C");
  display.setCursor(0, 24);
  display.print("RH   : "); display.print(hum, 1); display.println(" %");

  display.setCursor(0, 36);
  display.print("S-Vent: "); display.print(sudutServoVentilasi); 
  display.print(" | S-Egg: "); display.println(sudutServoTelor);

  display.setCursor(0, 48);
  if (digitalRead(RELAY_LAMPU_PIN) == RELAY_ON) display.print("LMP:ON  "); else display.print("LMP:OFF ");
  if (digitalRead(RELAY_KIPAS_PIN) == RELAY_ON) display.print("FAN:ON"); else display.print("FAN:OFF");
  
  display.setCursor(0, 57);
  if (digitalRead(RELAY_HUM_PIN) == RELAY_ON) display.print("HUMID:ON"); else display.print("HUMID:OFF");

  display.display();

  // Peringatan Otomatis ke Telegram Jika Suhu / Kelembapan Abnormal (Kirim max 10 menit sekali)
  if (millis() - lastTelegramAlert >= 600000 || lastTelegramAlert == 0) {
    if (temp < 37.5 || temp > 38.5 || hum < 50.0) {
      String alertMsg = "⚠️ *PERINGATAN INKUBATOR!*\n";
      alertMsg += "Kondisi di luar ambang batas ideal:\n";
      alertMsg += "🌡 Suhu: " + String(temp, 1) + " °C\n";
      alertMsg += "💧 Kelembapan: " + String(hum, 1) + " %";
      bot.sendMessage(CHAT_ID, alertMsg, "Markdown");
      lastTelegramAlert = millis();
    }
  }
}

// --- Pengiriman Seluruh Parameter ke Google Spreadsheets ---
void sendDataToGoogleSheets() {
  if (isnan(temp) || isnan(hum)) return;

  WiFiClientSecure client;
  client.setInsecure();

  const char* host = "script.google.com";
  if (!client.connect(host, 443)) {
    Serial.println("Koneksi ke Google Script Gagal!");
    return;
  }

  String url = GAS_URL + "?" +
               "temp=" + String(temp, 1) +
               "&hum=" + String(hum, 1) +
               "&lampu=" + String(statusLampu == 1 ? "ON" : "OFF") +
               "&kipas=" + String(statusKipas == 1 ? "ON" : "OFF") +
               "&humidifier=" + String(statusHumidifier == 1 ? "ON" : "OFF") +
               "&servoTelor=" + String(sudutServoTelor) +
               "&servoVentilasi=" + String(sudutServoVentilasi);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP32\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Seluruh data berhasil dikirim ke Google Sheets.");
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_LAMPU_PIN, OUTPUT);
  pinMode(RELAY_KIPAS_PIN, OUTPUT);
  pinMode(RELAY_HUM_PIN, OUTPUT);
  
  digitalWrite(RELAY_LAMPU_PIN, RELAY_OFF);
  digitalWrite(RELAY_KIPAS_PIN, RELAY_OFF);
  digitalWrite(RELAY_HUM_PIN, RELAY_OFF);

  dht.begin();

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servoVentilasi.setPeriodHertz(50);
  servoPenggerakTelur.setPeriodHertz(50);

  servoVentilasi.attach(SERVO_VENT_PIN, 500, 2400);
  servoPenggerakTelur.attach(SERVO_EGG_PIN, 500, 2400);

  servoVentilasi.write(0);
  servoPenggerakTelur.write(0);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED tidak ditemukan / gagal inisialisasi!");
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(15, 10);
    display.println("INKUBATOR IoT");
    display.setCursor(20, 25);
    display.println("PENETAS TELUR");
    display.setCursor(15, 45);
    display.println("Menghubungkan...");
    display.display();
  }

  clientTelegram.setInsecure();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, readDHT);
  timer.setInterval(4000L, cekPesanTelegram);
  timer.setInterval(60000L, sendDataToGoogleSheets); 
}

void loop() {
  Blynk.run();
  timer.run();
}