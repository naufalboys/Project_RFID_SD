#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

#define BUZZER  D3
#define SS_PIN  D4
#define RST_PIN D8

WiFiClient client;
const int httpPort = 80;

MFRC522 mfrc522(SS_PIN,RST_PIN);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

int serNum[7];
int x = 0;
int y = 0;
int j = 0;
int var1;

// WiFi parameters to be configured
char ssid[]           = "Your SSID";
const char* password  = "Your Password";
const char* host      = "b401telematics.com";

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

void setup()
{ 
  Serial.begin(115200);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  //I2C LCD SDA = D2
  //I2C LCD SCL = D1
  Wire.begin(D2,D1);
  
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

    for(var1 = 1; var1 <= 3; var1++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(250);
      digitalWrite(BUZZER, LOW);
    }  
  }
  
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println("Silahkan Scan Kartu Anda !");
  
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() 
{
  lcd.clear();
  
  digitalWrite(BUZZER, HIGH);
  delay(750);
  digitalWrite(BUZZER, LOW);
  
  lcd.setCursor(0,0);
  lcd.print("Please scan your");
  lcd.setCursor(0,1);
  lcd.print("Member card !");
  
  if (client.connect(host, httpPort))
  {
    Serial.println("Client Connected");
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      delay(50);
      return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      delay(50);
      return;
    }

    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    
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
      lcd.setCursor(0,0);
      lcd.print("Success !");
    }

    lcd.setCursor(0,0);
    lcd.print("Mengirim Data...");
   
    String ID = String(mfrc522.uid.uidByte[1], DEC);
    Serial.print("ID = ");
    Serial.println(ID);
    
    String url = "/sdairlangga/absensi/input.php";
    url += "?x=";
    url += ID;
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    client.stop();
    Serial.println();

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
  
  else
  {
    Serial.println("Connection error");
    lcd.setCursor(0,0);
    lcd.print("Connection");
    lcd.setCursor(0,1);
    lcd.print("Error");
    for (var1 = 1; var1 <= 3; var1++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
    }
    setup();
  }
}
