#define BLYNK_TEMPLATE_ID "TMPL6nE_qadWl" //mendefinisikan ID template Blynk yang akan digunakan
#define BLYNK_TEMPLATE_NAME "Quickstart Template" //mendefinisikan nama template Blynk yang akan digunakan
#define BLYNK_AUTH_TOKEN "GTU-AiX-CXqZeI3mJFfwWELQWvQntzF5" //mendefinisikan token otentikasi Blynk yang akan digunakan

char ssid[] = "vivo 1820"; //inisialisasi ID wifi
char pass[] = "12345678b"; //inisialisasi password
String token = "6076922870:AAF4jDAV02Hm7B6hI20kezL7SGyxYGlEv7w"; //inisialisasi token bot telegram
const int id = 1997801753; //inisisalisasi ID telegram

#define MQ135     A0 //pin sensor asap

#include "CTBot.h" 
#include <Wire.h>
CTBot mybot;
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;

bool isconnected = false; //inisialisasi conecting wifi
char auth[] = BLYNK_AUTH_TOKEN; // inisialisasi variabel auth sebagai token otentikasi Blynk
int ASAP = 0; //inisialisasi variabel ASAP sebagai nilai awal dari sensor asap
int smokeThreshold = 10; //inisialisasi sebagai ambang batas sensor asap


void setup() //memulai program
{
  Serial.begin(9600);
  pinMode(MQ135, INPUT); //mendeklarasikan MQ135 sebagai inputan
  
  Serial.println("Memulai Program MQ135");
  mybot.wifiConnect(ssid, pass);
  mybot.setTelegramToken(token);
  if (mybot.testConnection()) { //tes koneksi ke telegram
    Serial.println("Koneksi Telegram berhasil");
  } else {
    Serial.println("Koneksi Telegram Gagal");
  }

  WiFi.begin(ssid, pass);  
  Blynk.config(auth);
  if (Blynk.connected()) { //tes koneksi ke Blynk
    Serial.println("Blynk Connected");
  } else {
    Serial.println("Blynk Not Connected");
  }
}

void sendBlynkMessage() { //function untuk mengirim pesan ke Blynk
ASAP = map(analogRead(A0), 0, 4095, 0, 100); // membaca nilai sensor MQ135 menjadi 0-100
  Blynk.virtualWrite(V1, ASAP); 
  Blynk.virtualWrite(V3, ASAP);
  Serial.print("Smoke Level: ");
  Serial.println(ASAP);

  if (ASAP > smokeThreshold) { //memeriksa kadar asap ketika melebihi ambang batas
    Blynk.virtualWrite(V2, 1);
    Blynk.logEvent("gas", "Gas Detected!");
    String bahaya = "BAHAYA \n FireFly: Asap Terdeteksi Berlebih \n Segera Lakukan Tindakan Cepat \n Level asap Terkini : " + String(ASAP) + "%";
    mybot.sendMessage(id, bahaya);

  } else {
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V1, ASAP);
    Blynk.virtualWrite(V3, ASAP);
  }
}

void sendTelegramMessage() { //function mengirim pesan melalui telegram
  TBMessage msg; //mendeklarasikan objek TBMessage dengan nama msg
  if (mybot.getNewMessage(msg)) { //memeriksa inputan user ke serial monitor
    Serial.println("Pesan masuk : " + msg.text);
    String pesan = msg.text;
    if (pesan == "/start") { //output yang dihasilkan ketika user mengirim pesan /start
      String PesanWelcome = 
        "========= HALO PENGGUNA =========== \n"
        "= SELAMAT DATANG di FireFly  = \n"
        "======================================= \n"
        "\n"
        "Kamu dapat gunakan bot ini\n"
        "untuk deteksi asap di tempatmu\n"
        "\n"
        "\n"
        "Kamu dapat gunakan beberapa \n"
        "pesan berikut :\n"
        "/status - Cek Status Sensor\n"
        "/read - Notifikasi Sensor\n"
        "/help - Tampilkan Pesan Bantuan\n"
        "\n"
        "Information : \n"    
        "Kadar asap 0-10% = aman \n"
        "Kadar asap >10% = bahaya \n"
        "\n"             
        "Contact Developer : \n"
        "0812345678922 (Kelompok 3)";
      mybot.sendMessage(id, PesanWelcome);
    } else if (pesan == "/status") { //output yang dihasilkan ketika user mengirim pesan /status
      String PesanStatus = "FireFly : Status Sensor => Online";
      mybot.sendMessage(id, PesanStatus);
    } else if (pesan == "/read") { //output yang dihasilkan ketika user mengirim pesan /read
      
      if (ASAP > smokeThreshold) { //memeriksa kadar asap ketika melebihi ambang batas
        String bahaya = "BAHAYA \n FireFly : Asap Terdeteksi Berlebih \n Segera Lakukan Tindakan Cepat \n Level asap Terkini : " + String(ASAP) + "%";
        mybot.sendMessage(id, bahaya);
        
      } else { 
        String PesanNoDeteksi = "FireFly : Tidak terdeteksi asap, Enjoy!! Kadar Asap: " + String(smokeLevel) + " %";
        mybot.sendMessage(id, PesanNoDeteksi);
      }
    } else if (pesan == "/help") { //output yang dihasilkan ketika user mengirim pesan /help
      String helpMessage = 
        "FireFly Help:\n"
        "/status - Cek Status Sensor\n"
        "/read - Notifikasi Sensor\n"
        "/help - Tampilkan Pesan Bantuan\n";
      mybot.sendMessage(id, helpMessage);
    } else { //output yang dihasilkan ketika user mengirim pesan selain yang dideklarasikan diatas
      String PesanInvalid = 
        "FireFly : Pesan tidak valid.\n" 
        "Kirim /help untuk Bantuan.";
      mybot.sendMessage(id, PesanInvalid);
    }
  }

}

void loop()
{
  sendBlynkMessage();
  sendTelegramMessage(); 
  Blynk.run();
  timer.run();
  delay(50);
}