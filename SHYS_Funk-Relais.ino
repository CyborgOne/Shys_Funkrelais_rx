// NRF24L01_Relais
// Empfänger

#include <SPI.h>
#include <RF24.h>

// Zur verfügung stehende Modi für Relais
static const uint8_t RELAIS_MODUS_SWITCH = 0; // Schaltet bei EIN das Relais dauerhaft bis zum nächsten AUS Signal
static const uint8_t RELAIS_MODUS_PULSE  = 1; // Schaltet bei EIN das Relais für 1sec. ein und danach wieder aus. 


//--------------------------------------
// Configuration Start
//--------------------------------------
#define RF_CE_PIN 9
#define RF_CSN_PIN 10

uint8_t clientNummer = 0;                    // Client-ID 0-5

static const uint8_t relaisAnzahl =  2;      // Anzahl der Relais
uint8_t relaisPins[relaisAnzahl] = {6, 7};   // Array der Pins für die Relais 
uint8_t relaisModus[relaisAnzahl] = {RELAIS_MODUS_PULSE, RELAIS_MODUS_SWITCH};   // Array der Pins für die Relais 
//--------------------------------------
// Configuration End
//--------------------------------------



boolean relaisStatus[relaisAnzahl];
static const uint8_t pipes[6] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL};

RF24 radio(RF_CE_PIN, RF_CSN_PIN); 

/**
 * Standard Setup Methode
 */
void setup() {
  Serial.begin(9600);
  
  for(int i=0;i<relaisAnzahl;i++){
      pinMode(relaisPins[i], OUTPUT);
      digitalWrite(relaisPins[i], HIGH);
  }
  
  radio.begin();
  delay(20);
  
  radio.setChannel(1);               // Funkkanal - Mögliche Werte: 0 - 127   
  radio.setAutoAck(0);    
  radio.setPALevel(RF24_PA_LOW);     // Sendestärke darf die gesetzlichen Vorgaben des jeweiligen Landes nicht überschreiten! 
                                     // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm
                                     
  radio.openReadingPipe(0, pipes[clientNummer]);

  radio.startListening();
  delay(20);
  
  Serial.println(F("Starte Empfaenger ..."));  
}
 
 

/**
 * Standard Loop Methode
 */
void loop() {

  if (radio.available((uint8_t*)pipes[clientNummer])){
    long got_message[2] = {-1, -1};
      
    radio.read(&got_message, sizeof(got_message));
    
    int relaisId  = lround(got_message[0]);
    boolean relaisStatus = got_message[1]>0;

    Serial.println();
    Serial.print(F("ClientNummer: "));
    Serial.println(clientNummer);
    Serial.print(F("Relais ID: "));
    Serial.println(relaisId);
    Serial.print(F("Relais Status: "));
    Serial.println(relaisStatus);
    
    if(relaisId>=0 && relaisId<relaisAnzahl){
      Serial.print("Schalte ");
      Serial.println(relaisStatus?"EIN":"AUS");

      // Relais-Status ist invertiert
      // digitalWrite( !EIN ) 
      switch(relaisModus[relaisId]){
        case RELAIS_MODUS_SWITCH:
          digitalWrite(relaisPins[relaisId], relaisStatus?LOW:HIGH);
          break;
        case RELAIS_MODUS_PULSE:
          digitalWrite(relaisPins[relaisId], LOW); 
          delay(1000);
          digitalWrite(relaisPins[relaisId], HIGH);
          break;
      } 
    }
  }
 
}
