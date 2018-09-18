# Project RFID SD

![alt text](https://raw.githubusercontent.com/naufalboys/Project_RFID_SD/master/Project_RFID_bb.png)

Project ini adalah Project dari SDN Airlangga 1 Surabaya

Cara kerja dari Project ini :
1.  Siswa melakukan Tap Kartu RFID nya pada Alat.
2.  Kemudian sensor RC522 akan melakukan Scanning pada Kartu RFID tersebut.
3.  Microcontroller NodeMCU akan mengirim data tersebut ke database.
4.  Kemudian akan ada tampilan pada LCD bahwa Siswa tersebut telah berhasil masuk/keluar.

Note : 
1. Jangan lupa memilih Board Generic ESP8266 Module pada Arduino dengan Reset Method Nodemcu.
2. Jangan lupa untuk mengganti codingan dengan SSID Wifi dan Password sesuai dengan Wifi yang tersedia.
3. Library yang digunakan adalah ESP8266Wifi, SPI, Wire, LiquidCrystal_I2C, MFRC522.
