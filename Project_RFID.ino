#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

#define BUZZER  D4   
#define SS_PIN  D8  
#define RST_PIN D3  
#define CHG_MD  D0  

ESP8266WiFiMulti WiFiMulti;

LiquidCrystal_I2C lcd(0x27, 16, 2);

MFRC522 mfrc522(SS_PIN, RST_PIN);

int serNum[7];
int x = 0;
int y = 0;
int j = 0;

// WiFi parameters to be configured
char ssid[]            = "guru";
const char* password   = "airone198";
char ssid2[]           = "Operator Sekolah";
const char* password2  = "airone198";
char ssid3[]           = "PERPUSTAKAAN";
const char* password3  = "air198oke";
char ssid4[]           = "NEW B401-AP";
const char* password4  = "B401KomputasiMultimedia";
char ssid5[]           = "Aula Pertemuan";
const char* password5  = "airone198";
const char* host       = "sdnairlangga1.com";

void setup()
{
  pinMode(CHG_MD, INPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  //mfrc522.PCD_DumpVersionToSerial();
  //Serial.setDebugOutput(true);

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Connecting to :");
  lcd.setCursor(0, 1);
  lcd.print("WiFi . . .");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  //WiFiMulti.addAP(ssid, password);
  //WiFiMulti.addAP(ssid2, password2);
  //WiFiMulti.addAP(ssid3, password3);
  WiFiMulti.addAP(ssid4, password4);
  WiFiMulti.addAP(ssid5, password5);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFiMulti.run() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi connected !");
    delay(500);
    Serial.println("IP address: ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP Address : ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(1000);
  }
}

void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("Tempelkan Kartu");
  lcd.setCursor(0, 1);
  lcd.print("Absensi Anda !");

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  ////-------------------------------------------------RFID----------------------------------------------

  Serial.println();
  Serial.print(" UID tag :");
  String content = "";
  byte letter, i;
  for (i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();

  //Fungsi Loading pada LCD
  custom_loading();
  lcd.clear();
  loading_lcd();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mengirim Data...");

  String ID = String(mfrc522.uid.uidByte[0], HEX);
  ID += String(mfrc522.uid.uidByte[1], HEX);
  ID += String(mfrc522.uid.uidByte[2], HEX);
  ID += String(mfrc522.uid.uidByte[3], HEX);
  Serial.print("ID = ");
  Serial.println(ID);

  ////-------------------------------------------------SENDING DATA----------------------------------------------

  String url = "";
  if (digitalRead(CHG_MD))
  {
    url =  "http://www.sdnairlangga1.com/absensi/scan.php";
  }
  else
  {
    url =  "http://www.sdnairlangga1.com/absensi/input.php";
  }

  url += "?x=";
  url += ID;

  String url2 = "http://www.sdnairlangga1.com/absensi/lihat.php";
  url2 += "?x=";
  url2 += ID;

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http, http2;

    Serial.print("[HTTP 2] begin...\n");
    if (http2.begin(client, url))
    {
      int httpCode2 = http2.GET();
      http2.end();
    }

    if (http.begin(client, url2))
    {
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Terima Kasih : ");
          lcd.setCursor(0, 1);
          lcd.print(payload);
          digitalWrite(BUZZER, HIGH);
          delay(500);
          digitalWrite(BUZZER, LOW);
          delay(1000);
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        for (int ulang = 0; ulang <= 2; ulang++)
        {
          digitalWrite(BUZZER, HIGH);
          delay(100);
          digitalWrite(BUZZER, LOW);
          delay(100);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HTTP GET");
        lcd.setCursor(0, 1);
        lcd.print("Failed");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Device");
        lcd.setCursor(0, 1);
        lcd.print("Restarting");
        delay(500);
        setup();
      }

      http.end();
    }
    else
    {
      Serial.printf("[HTTP} Unable to connect\n");
      for (int ulang = 0; ulang <= 2; ulang++)
      {
        digitalWrite(BUZZER, HIGH);
        delay(100);
        digitalWrite(BUZZER, LOW);
        delay(100);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HTTP Unable to");
      lcd.setCursor(0, 1);
      lcd.print("Connect");
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Device");
      lcd.setCursor(0, 1);
      lcd.print("Restarting");
      delay(500);
      setup();
    }
  }
  delay(500);
  Serial.println("Silahkan Selanjutnya !");
}

void custom_clear()
{
  byte blank[8] =
  {
    B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000
  };

  for (int n = 0; n < 8; n++)
  {
    lcd.createChar(n, blank);
  }
}

void custom_loading()
{
  custom_clear();
  j = 4;

  byte line1[8] =
  {
    B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000
  };
  byte line2[8] =
  {
    B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000
  };
  byte line3[8] =
  {
    B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100
  };
  byte line4[8] =
  {
    B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110
  };

  lcd.createChar(0, line1);
  lcd.createChar(1, line2);
  lcd.createChar(2, line3);
  lcd.createChar(3, line4);
}

void loading_lcd()
{
  lcd.setCursor(0, 0);
  lcd.print("Loading");

  for (int i = 0; i < j; i++)
  {
    lcd.setCursor(x, 1);
    lcd.write(i);
    delay(10);
  }

  lcd.setCursor(x, 1);
  lcd.write(255);
  delay(10);
  x++;

  if (x < 16)
  {
    loading_lcd();
  }

  if (x > 15)
  {
    x = 0;
    lcd.clear();
  }
}
