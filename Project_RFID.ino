#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

//#define BUZZER  2
#define SS_PIN  15
#define RST_PIN 0
#define SDA_LCD 4
#define SCL_LCD 5

WiFiClient client;
const int httpPort = 80;

MFRC522 mfrc522(SS_PIN,RST_PIN);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

int serNum[7];
int x = 0;
int y = 0;
int j = 0;
int var1,var2;
int frequency = 1;

/*
// WiFi parameters to be configured
char ssid[]           = "AndromaxM3Y";
const char* password  = "matoterbaik";
const char* host      = "b401telematics.com";
*/

// WiFi parameters to be configured
char ssid[]           = "NEW B401-AP";
const char* password  = "LemperAyam";
const char* host      = "b401telematics.com";

void setup()
{ 
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
  //I2C LCD SDA = D2
  //I2C LCD SCL = D1
  Wire.begin(SDA_LCD,SCL_LCD);
  
  lcd.begin();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Connecting to :");
  lcd.setCursor(0,1);
  lcd.print(String(ssid));
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print(".");
  }

  if (!client.connect(host, httpPort))
  {
    Serial.println("Connection Failed");
    return;
  }

  if (client.connect(host, httpPort))
  {
    Serial.println();
    Serial.println("Connected to : ");
    Serial.print(host);
    lcd.setCursor(0,0);
    lcd.print("Connected to :");
    lcd.setCursor(0,1);
    lcd.print(String(host));
  }
  
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println("Silahkan Scan Kartu Anda !");
  lcd.clear();
}

void loop() 
{
  lcd.setCursor(0,0);
  lcd.print("Please scan your");
  lcd.setCursor(0,1);
  lcd.print("Member card !");
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    delay(50);
    return;
  }

////-------------------------------------------------RFID----------------------------------------------
  
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
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
  for(int a = 1; a < 2; a++)
  {
    custom_loading();
    lcd.clear();
    for(int b = 1; b <= 2; b++)
    {
      loading_lcd();
    }
    lcd.clear();
  }

  lcd.setCursor(0,0);
  lcd.print("Mengirim Data...");
  delay(300);
  String ID = String(mfrc522.uid.uidByte[1], DEC);
  Serial.print("ID = ");
  Serial.println(ID);

////-------------------------------------------------SERVER----------------------------------------------

  String url = "/sdairlangga/absensi/input.php";
  url += "?x=";
  url += ID;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");
  delay(10);
  Serial.println();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Data Sukses");
  lcd.setCursor(0,1);
  lcd.print("Dikirim");
  delay(500);
  Serial.println("Silahkan Selanjutnya !");
}

/*void beep(unsigned char repeat, unsigned char delay_beep)
{
  for (int z = 1; z <= repeat; z++)
  {
    noTone(BUZZER);
    delay(delay_beep);
    tone(BUZZER, frequency);
    delay(delay_beep);
  }
}*/

void custom_clear()
{
  byte blank[8] = 
  {
    B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000
  };

  for(int n = 0; n < 8; n++)
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
  lcd.setCursor(0,0);
  lcd.print("Loading");

  for(int i = 0; i < j; i++)
  {
    lcd.setCursor(x,1);
    lcd.write(i);
    delay(25);
  }

  lcd.setCursor(x,1);
  lcd.write(255);
  delay(25);
  x++;

  if(x < 16)
  {
    loading_lcd();
  }

  if(x > 15)
  {
    x = 0;
    lcd.clear();
  }
}

