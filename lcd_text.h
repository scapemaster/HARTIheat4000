class LCD_infotext {
  String bt;  // Hilfsvariable für float --> String
  
  public:
  long     ontime        = 0;  // Startwert
  long     curr_ontime   = 0;  // So lange ist das Display schon an
  long     offtime       = 15; // So lange solls an sein
  
  String  line1 = "HARTLheat 4000";
  String  line2 = "Vers 1.2.1";

  settext_displ_1(int run_mode, float buffer_temp) {
    bt = String(buffer_temp);
    
    if(run_mode == 0) {
      line1 = "Anlage STBY     ";
    }
    if(run_mode == 1) {
      line1 = "normal -> heizt ";
    }
    if(run_mode == 2) {
      line1 = "normal ohne Hkr ";
    }
    if(run_mode == 3) {
      line1 = "Pufferbetrieb   ";
    }
    line2 = "T Puffer: " + bt;
    line2 = line2 + "C";
  }//EoF

  set_ontime(){
    ontime = millis() / 1000;
  }
};//EoClass
