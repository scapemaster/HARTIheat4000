// Libs einfügen
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "lcd_text.h"


// Heizsystem konfigurieren
#define ONE_WIRE_BUS 2                      //Data ist an Pin 2 des Arduinos angeschlossen

OneWire oneWire(ONE_WIRE_BUS);              //Start des OneWire Bus
DallasTemperature sensors(&oneWire);        //Dallas Temperature referenzieren
Heatsystem heizung;                         //heizung instanzieren
LCD_infotext infotext;                      //lcd textausgabe instanzieren

// LCD-Display konfigurieren
LiquidCrystal_I2C lcd(0x27, 16,2);

void setup() {
  
  //Pinsetup
  pinMode(heizung.rel_burn, OUTPUT);
  pinMode(heizung.rel_p1, OUTPUT);
  pinMode(heizung.rel_p2, OUTPUT);
  pinMode(heizung.season_led, OUTPUT);
  
  pinMode(heizung.season_but, INPUT);

  digitalWrite(heizung.rel_burn, HIGH);
  digitalWrite(heizung.rel_p1, HIGH);
  digitalWrite(heizung.rel_p2, HIGH);
  digitalWrite(heizung.season_led, HIGH);
  
  // Temperaturabtastung einrichten
  Serial.begin(9600);                        // Start der seriellen Konsole
  sensors.begin();                           // Sensor Start 

  // LCD initialisieren
  lcd.init();
}

void loop(void) {
  // Variablen initialisieren
    float temp_buffer;

  // Temp_Sensor Check
    int sensorCount = sensors.getDS18Count();
  
    if(sensorCount == 0) {
      Serial.println("Kein Temp_Sensor erkannt!");
      temp_buffer = 999;
      heizung.heatmode = 0;
      heizung.season   = 1;
    }
  // Durchlafzähler für Backlightsteuerung
  
  /* 
   * ++++++++++++++++++++++++++++++++++++++++Button-Press+++++++++++++++++++++++++++++++
   * 
   * Menüorientierung im LCD-System einrichten
   *
   *
   */
    
  // Temperaturen auslesen
    if(sensorCount != 0) {
      sensors.requestTemperatures();                  // Temperaturen Anfragen
      temp_buffer = sensors.getTempCByIndex(0);       // Puffertemperatur holen
    }

  // Season-Setting (0 = Winter, 1 = Sommer)
    if(digitalRead(heizung.season_but)) {infotext.ontime = millis() / 1000;} // Bei Knopfdruck --> LCD Hintergrundlicht an
    heizung.change_season(heizung.season_but, heizung.season);
  
  // Heizphase/Pufferphase steuern
    int old_heatmode = heizung.heatmode;
    if(temp_buffer <= heizung.b_target - heizung.b_temprange) {heizung.heatmode = 1;}
    if(temp_buffer >= heizung.b_target)                       {heizung.heatmode = 0;}
    
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ heatmode I/O schreiben ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  // run_mode bestimmen
    int old_rmc = heizung.old;
    int run_mode_code = heizung.set_runmode(temp_buffer);

  // Ausgabetext setzen
    infotext.settext_displ_1(run_mode_code, temp_buffer);
    // Backlight LCD on/off
      if(old_rmc != run_mode_code) {infotext.ontime = millis() / 1000;}
      if(old_heatmode != heizung.heatmode) {infotext.ontime = millis() / 1000;}
      infotext.curr_ontime = millis()/1000;
      if(infotext.curr_ontime - infotext.ontime <= infotext.offtime) {lcd.backlight();}
      else {lcd.noBacklight();}

  // Serielle Ausgabe
    Serial.print("Season: ");
    Serial.print(heizung.season);
    Serial.print("\t");
  
    Serial.print("Heatmode: ");
    Serial.print(heizung.heatmode);
    Serial.print("\t");
  
    Serial.print("Mode active: ");
    Serial.print(heizung.run_mode);
    Serial.print("\t");
  
    Serial.print("Temperatur Puffer: ");
    Serial.print(temp_buffer);
    Serial.println("\t");

    Serial.print("LCD Zeile1: ");
    Serial.print(infotext.line1);
    Serial.println("\t");

    Serial.print("LCD Zeile2: ");
    Serial.print(infotext.line2);
    Serial.println("\t");

  // LCD Ausgabe
    lcd.setCursor(0,0);
    lcd.print(infotext.line1);
    lcd.setCursor(0,1);
    lcd.print(infotext.line2);
    
} // End of Loop
