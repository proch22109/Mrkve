#include <Wire.h>

// Jedno arduino je Slave, druhé Master
#define TYP 'm'
// #define TYP 's'
// #define TYP 'm'
#define x A0
#define y A1

struct data{
  int dataA;
  int dataB;
};


data received;
data sent;

void setup() {
  Serial.begin(9600);
  
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

volatile bool newData = false; // Indikátor dat ke zpracování ve vedlejší funkci, volatile znamena, ze se meni a musi se vzdy kontrolovat (nestaly)

// Slave zavolá funkci po obdržení dat z Master
void onReceive(int numBytes) {
  // NEDÁVAT Serial.print

  // Čtení dat, bude potřeba upravit, zatím čte všechno naráz a do jednoho místa (předělat na čtení do struktury), 
  // ale pozor, získávám data po bytech
  if (numBytes >= sizeof(data)) { //size of je velikost struktury
    // Přečteme bajty přímo do paměti naší struktury
    byte* p = (byte*)&received; //jakoby prejmenovani
    for (size_t i = 0; i < sizeof(data); i++) //size_t je mensi int pro nezaporny cisla
      p[i] = Wire.read(); 
    newData = true; 

    // Vyčištění případného zbytku ve frontě
  while(Wire.available())
    Wire.read();
  }
}

// Slave zavolá funkci po požadavku na data od Master
void onRequest() {
  // NEDÁVAT Serial.print
Wire.write((byte*)&sent, sizeof(data));
}
void loop() {

  // Zatím pro omezení datového toku
  delay(500);

    sent.dataA = analogRead(x);
    sent.dataB = analogRead(y);

  if (TYP == 'm') {

    // Master zahájí přenos (Master -> Slave)

    Wire.beginTransmission(0x08);
    
    Wire.write((byte*)&sent, sizeof(data)); //wire.write muze bud posilat cislo a nebo ukazatel na prvni cislo sturktury a jeji delku

    Wire.endTransmission();

    // Master požádá Slave o data (Slave -> Master)
    Wire.requestFrom(0x08, sizeof(data));

    // Čtení dat
    if (Wire.available() >= sizeof(data)) {
      byte* p = (byte*)&received;
      for (size_t i = 0; i < sizeof(data); i++) 
        p[i] = Wire.read();
      }
    Serial.print("Master prichzi data -> X: ");
      Serial.print(received.dataA);
      Serial.print(" | Y: ");
      Serial.println(received.dataB);
  } else {

    // Pokud je přepínač s daty pro zpracování nastaven, zpracuj data a vypni přepínač.
    if (newData == true) {

     Serial.print("Slave prichzi data -> X: ");
      Serial.print(received.dataA);
      Serial.print(" | Y: ");
      Serial.println(received.dataB);
      
      newData = false;
    }
  }
}

