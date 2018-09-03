# Project RFID SD
Project ini adalah Project dari SDN Airlangga 1 Surabaya.

Cara kerja dari Project ini :
1.  Siswa melakukan Tap Kartu RFID nya pada Alat.
2.  Kemudian sensor RC522 akan melakukan Scanning pada Kartu RFID tersebut.
3.  Microcontroller NodeMCU akan mengirim data tersebut ke database.
4.  Kemudian akan ada tampilan pada LCD bahwa Siswa tersebut telah berhasil masuk/keluar.

Note : 
1. Jangan lupa memilih Board NodeMCU 1.0 (ESP-12E Module) pada Arduino.
2. Jangan lupa untuk mengganti codingan dengan SSID Wifi dan Password sesuai dengan Wifi yang tersedia.
3. Library yang digunakan adalah ESP8266Wifi, SPI, Wire, LiquidCrystal_I2C, MFRC522.
