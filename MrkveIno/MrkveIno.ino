#include <Wire.h>

// Jedno arduino je Slave, druhé Master
#define TYP 's'
// #define TYP 's'
// #define TYP 'm'

byte dataReceived;  // proměnná pro uložení přijatých dat, chtělo by to upgradovat na strukturu

void setup() {
  Serial.begin(9600);
  dataReceived = 0;
  
  // Setup pro Master arduino
  if (TYP == 'm') {
    Wire.begin();

  // Setup pro Slave arduino
  } else {
    Wire.begin(0x08);  // 

    // Hardwarové přerušení (funkce, co se zavolá bezprostředně po nějaké udáosti)
    Wire.onReceive(onReceive); // Slave zavolá funkci po obdržení dat z Master
    Wire.onRequest(onRequest); // Slave zavolá funkci po požadavku na data od Master
  }
}

bool newData = false; // Indikátor dat ke zpracování ve vedlejší funkci

// Slave zavolá funkci po obdržení dat z Master
void onReceive(int numBytes) {
  // NEDÁVAT Serial.print

  // Čtení dat, bude potřeba upravit, zatím čte všechno naráz a do jednoho místa (předělat na čtení do struktury), 
  // ale pozor, získávám data po bytech
  while (Wire.available()) {
    dataReceived = Wire.read();
    newData = true;
  }
}

// Slave zavolá funkci po požadavku na data od Master
void onRequest() {
  // NEDÁVAT Serial.print
  Wire.write(16);
}

void loop() {

  // Zatím pro omezení datového toku
  delay(500);

  if (TYP == 'm') {

    // Master zahájí přenos (Master -> Slave)
    Wire.beginTransmission(0x08);
    Wire.write(17);
    Wire.endTransmission();

    // Master požádá Slave o data (Slave -> Master)
    Wire.requestFrom(0x08, 1);

    // Čtení dat, opět potřeba předělat na strukturu dat
    if (Wire.available()) {
      dataReceived = Wire.read();
    }
    Serial.print("Hello from master: ");
    Serial.println(dataReceived);

  } else {

    // Pokud je přepínač s daty pro zpracování nastaven, zpracuj data a vypni přepínač.
    if (newData == true) {
      Serial.print("Hello from slave: ");
      Serial.println(dataReceived);
      newData = false;
    }
  }
}
