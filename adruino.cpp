#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "FAOZAN"; // Ganti dengan SSID Wi-Fi Anda
const char* password = "@Bhakti135"; // Ganti dengan kata sandi Wi-Fi Anda

// Pin untuk RC522
#define SS_PIN D8
#define RST_PIN D0

// Pin untuk Buzzer
#define BUZZER_PIN D1

// Pin untuk LED D2 dan D3
#define LED_PIN_D2 D2
#define LED_PIN_D3 D3

// URL skrip PHP di server lokal
const char* serverURL = "http://192.168.1.2/script.php";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inisialisasi RC522

void setup() {
    // Inisialisasi komunikasi serial
    Serial.begin(115200);

    // Inisialisasi SPI dan modul RC522
    SPI.begin();
    mfrc522.PCD_Init();

    // Inisialisasi pin buzzer, LED D2, dan LED D3
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN_D2, OUTPUT);
    pinMode(LED_PIN_D3, OUTPUT);

    // Menghubungkan ke Wi-Fi
    WiFi.begin(ssid, password);
    Serial.println("Menghubungkan ke Wi-Fi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Mencoba koneksi...");
    }

    Serial.println("Terkoneksi ke Wi-Fi!");
    Serial.print("Alamat IP ESP8266: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // Memeriksa apakah kartu RFID terbaca
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    // Membaca UID kartu dan menampilkannya di Serial Monitor
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println("UID Kartu: " + uid);

    // Bunyikan buzzer
    bunyikanBuzzer();

    // Kirim UID ke server lokal menggunakan HTTP POST
    kirimData(uid);

    // Hentikan membaca kartu
    mfrc522.PICC_HaltA();
}

void bunyikanBuzzer() {
    // Bunyikan buzzer dengan frekuensi 1000 Hz selama 100 ms
    tone(BUZZER_PIN, 1500, 100);
}
void nyalakanLED2Detik() {
    // Nyalakan LED di pin D2
    digitalWrite(LED_PIN_D2, HIGH);
    // Tunda program selama 2000 ms (2 detik)
    delay(2000);
    // Matikan LED di pin D2
    digitalWrite(LED_PIN_D2, LOW);
}


void kirimData(String uid) {
    // Membuat objek WiFiClient
    WiFiClient client;

    // Membuat objek HTTPClient
    HTTPClient http;

    // Mulai permintaan HTTP menggunakan WiFiClient
    http.begin(client, serverURL);

    // Mengatur header konten
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Data yang akan dikirim dengan POST
    String postData = "uid=" + uid;
    int httpResponseCode = http.POST(postData);

    // Menampilkan hasil permintaan POST
    if (httpResponseCode > 0) {
        Serial.println("UID berhasil dikirim ke server");
        // Nyalakan LED D3 saat pengiriman berhasil
        digitalWrite(LED_PIN_D3, HIGH);
        // Matikan LED D2
        digitalWrite(LED_PIN_D2, LOW);
        // Nyalakan LED D3 selama 2 detik
        delay(500);
        digitalWrite(LED_PIN_D3, LOW);
    } else {
        Serial.println("Error mengirim UID ke server");
        // Matikan LED D3 saat pengiriman gagal
        digitalWrite(LED_PIN_D3, LOW);
        // Nyalakan LED D2 saat pengiriman gagal
        digitalWrite(LED_PIN_D2, HIGH);
        // Nyalakan LED D2 selama 2 detik
        delay(2000);
        digitalWrite(LED_PIN_D2, LOW);   // Matikan LED D2
    }

    // Tutup koneksi HTTP
    http.end();
}
