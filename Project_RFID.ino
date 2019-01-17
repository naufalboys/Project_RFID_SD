#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

#define BUZZER  2   //D4
#define SS_PIN  15  //D8
#define RST_PIN 0   //D3
#define SDA_LCD 4   //D2
#define SCL_LCD 5   //D1
#define CHG_MD  16  //D0

WiFiClient client;
const int httpPort = 80;

MFRC522 mfrc522(SS_PIN,RST_PIN);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

int serNum[7];
int x = 0;
int y = 0;
int j = 0;

/*
// WiFi parameters to be configured
char ssid[]           = "AndromaxM3Y";
const char* password  = "matoterbaik";
const char* host      = "sdnairlangga1.com";
*/

// WiFi parameters to be configured
char ssid[]           = "NEW B401-AP";
const char* password  = "LemperAyam";
const char* host      = "sdnairlangga1.com";


void setup()
{   
  pinMode(CHG_MD,INPUT);
  pinMode(BUZZER,OUTPUT);
  
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  //I2C SDA = D2, I2C SCL = D1
  Wire.begin(SDA_LCD,SCL_LCD);

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  
  lcd.begin();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Connecting to :");
  lcd.setCursor(0,1);
  lcd.print(String(ssid));
  
  // Set WiFi to Station Mode, and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  // Attempt to Connect to WiFi network
  Serial.print("Connecting to : ");
  Serial.println(ssid);
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
    lcd.setCursor(0,0);
    lcd.print("Connection");
    lcd.setCursor(0,1);
    lcd.print("Failed");

    for(int z = 1; z <=5; z++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(50);
      digitalWrite(BUZZER, LOW);
      delay(50);
    }
      
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

    for(int z = 1; z <=3; z++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100);
    }
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

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

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
  delay(700);
  String ID = String(mfrc522.uid.uidByte[0], HEX);
  ID += String(mfrc522.uid.uidByte[1], HEX);
  ID += String(mfrc522.uid.uidByte[2], HEX);
  ID += String(mfrc522.uid.uidByte[3], HEX);
  Serial.print("ID = ");
  Serial.println(ID);

////-------------------------------------------------SENDING DATA----------------------------------------------
  String url = "";
  if(digitalRead(CHG_MD))
  {
    url =  "/absensi/scan.php";
  }
  else
  {
    url =  "/absensi/input.php";
  }
  
  url += "?x=";
  url += ID;
  if (client.connect(host, httpPort))
  {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: close\r\n\r\n");
    Serial.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: close\r\n\r\n");
    delay(10);
  }
  else
  {
    Serial.println("Connection Failed");
    lcd.setCursor(0,0);
    lcd.print("Connection");
    lcd.setCursor(0,1);
    lcd.print("Failed");
    for(int z = 1; z <=2; z++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100);
    }
    setup();
  }
  
  Serial.println();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Data Sukses");
  lcd.setCursor(0,1);
  lcd.print("Dikirim");

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  
  delay(1000);
  Serial.println("Silahkan Selanjutnya !");
}

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
    delay(10);
  }

  lcd.setCursor(x,1);
  lcd.write(255);
  delay(10);
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
