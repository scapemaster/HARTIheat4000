class Heatsystem {
  public:
  /*
   * Hier findest du die Werte der Einstellungen wieder
   *
   */
   
  // type     var               val       location               descr                                         Example
  // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
      // Steuerpins festlegen
      int     rel_burn      =   7;       //                      Heizungsbrenner relais
      int     rel_p1        =   6;       //                      Pumpe Heizung --> Pufferspeicher
      int     rel_p2        =   5;       //                      Pumpe Heizkreis 1
      
      // Sensorpin OneWire festlegen
      int     onePin        =   2;
      
      // Temperatur SetUp
      float   b_target      =   60.0;     // Puffer Mitte         Zieltemperatur des Puffers
      float   b_temprange   =   10.0;     //                      Puffertemperatur >> Ein-/Ausschaltbereich     [Ist b_target 50 und b_temprange 10 dann startet der Heizbetrieb bei 40 und endet bei 50]
      float   b_mintemp     =   45.0;     //                      Puffer Mindesttemperatur vor Heizkreisschaltung
      
      float   b_Alarm       =   95.0;     //                      Puffer Prüftempereatur
      float   subheat_Alarm =   95.0;     // Rücklauf Nebenhzng   Nebenheizung MaxTemp
      
      // Mode Variablen
      int     heatmode      =   0;        //                      0 = Im Pufferbetrieb, 1 = Im Heizbetrieb
      int     season        =   0;        //                      0 = Winter --> Heizkreispumpe = an; 1 = Sommer --> Heizkreispumpe bleibt aus.
      int     run_mode      =   0;        //                      Beschreibt das laufende Programm
      int     old           =   0;        //                      Speichert den alten run_mode um eine Änderung festzustellen.
            
      // Device options
      int     season_led    =   3;        //                      Ist an, wenn Sommerbetrieb.
      int     season_but    =   4;        //                      Drücken zum wechseln.

      
      /* Mann könnte jetzt den Pumpe der Nebenheizung auch noch ansteuern.
         Mit der Temp vom Nebenheizungsvorlauf zum Puffer kann man die Brennraumtemp regeln
      */
      
      // Setter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int set_heatmode(int new_heatmode) {
    heatmode = new_heatmode;
  }
      
  int change_season(int season_but, int curr) {
    if(digitalRead(season_but)) {
      if(curr == 0) {
        season = 1;
      }
      if(curr == 1){
        season = 0;
      }
    }
  }

  int set_runmode(float temp_buffer) {
  /* mode       Heizung burn,    Pufferpumpe p1,    HzKr-Pumpe p2       Beschreibung
    -------------------------------------------------------------------------------------------------------
      0         aus               aus               aus                 Abschalten der Anlage
      1         an                an                an  (wenn gesetzt)  Heizbetrieb <<Winter>>
      2         an                an                aus (generell)      Heizbetrieb <<Sommer>>
      3         aus               aus               an  (wenn gesetzt)  Pufferbetrieb HzKr <<Winter>> und Zwangspufferkühlung --> Bei Nebenheizung oder Puffer => HIGH.Alarm!!!
  */
  
  /*
    * heatmode            = der Indikator, ob sich der Brenner gerade in einer Heizphase befindet. Kann über ein Objekt von config geholt werden.
    * season              = der Indikator ( Winter sommerbetrieb) Kann über ein Objekt von config geholt werden. ----> Hardswitch geplant
    * temp_buffer_target  = die Zieltemperatur des Puffers. Kann über ein Objekt von config geholt werden.
  */
  
  /* Hier muss dann noch die automatische Abarbeitung meherer Sensoren gestaltet werden. */
  
  // Programmanzeige
  if(season == 1){digitalWrite(season_led, HIGH);}
  if(season == 0){digitalWrite(season_led, LOW);}
  
  // Programmwahl
    // Heizbetrieb
    if(heatmode == 1) {
      // Winter
      if(season == 0) {
        old = run_mode;
        run_mode = 1;
        digitalWrite(rel_burn, LOW);
        digitalWrite(rel_p1, LOW);
        if(rel_p2 != 0){
          if(temp_buffer >= b_mintemp)  {digitalWrite(rel_p2, LOW);}
          else                          {digitalWrite(rel_p2, HIGH);}
        }
      }
      // Sommer
      if(season == 1) {
        old = run_mode;
        run_mode = 2;
        digitalWrite(rel_burn, LOW);
        digitalWrite(rel_p1, LOW);
        if(rel_p2 != 0){
          digitalWrite(rel_p2, HIGH);
        }
      }
    } // Eo Heatmode
    
    // Pufferbetrieb
    if(heatmode == 0) {
      // Winter
      if(season == 0) {
        old = run_mode;
        run_mode = 3;
        digitalWrite(rel_burn, HIGH);
        if(run_mode != old) {delay(10000);} // Verzögerung von x sec. pPufferbetrieb zur Nachkühlung
        digitalWrite(rel_p1, HIGH);
        if(rel_p2 != 0){
          digitalWrite(rel_p2, LOW);
        }
      }
      // Sommer
      if(season == 1) {
        old = run_mode;
        run_mode = 0;
        digitalWrite(rel_burn, HIGH);
        if(run_mode != old) {delay(10000);} // Verzögerung von x sec. pPufferbetrieb zur Nachkühlung
        digitalWrite(rel_p1, HIGH);
        if(rel_p2 != 0){
          digitalWrite(rel_p2, HIGH);
        }
      }
    } //Eo Pufferbetrieb
    
    // Zwangskühlung (entspricht Programm 3) bei 90% der Alarmtemperatur (i.d.R. 85,5°C)
      if(temp_buffer >= b_Alarm * 0.9) {
        old = run_mode;
        run_mode = 3;
        digitalWrite(rel_burn, HIGH);
        digitalWrite(rel_p1, HIGH);
        if(rel_p2 != 0){
          digitalWrite(rel_p2, LOW);
        }
     }
    return run_mode;  
  } // End of Function
}; // Endo of Class
