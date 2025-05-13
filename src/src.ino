const int IN3 = 8; 
const int IN4 = 9;

const int IN1 = 6;   
const int IN2 = 7;   

const int ENCODER_A_PIN = 2;  
const int ENCODER_B_PIN = 3;  
volatile long encoderTicks = 0;

const int TICKS_PER_ROTATION = 611;  

const int TRIG_PIN = 4;
const int ECHO_PIN = A1;

const int S0 = 12;
const int S1 = 5;
const int S2 = 11;
const int S3 = 13;
const int OUT_COLOR = A0;

const int BUTTON_PIN = A2;

bool running = false;
bool last_button_state = LOW;
unsigned long ultimaApasare = 0;
const unsigned long INTERVAL_APASARE_LUNGA = 2000; 

int albRosu = 0, albVerde = 0, albAlbastru = 0;
int negruRosu = 0, negruVerde = 0, negruAlbastru = 0;
int albastruRosu = 0, albastruVerde = 0, albastruAlbastru = 0;
int portocaliuRosu = 0, portocaliuVerde = 0, portocaliuAlbastru = 0;

int etapaCalibrare = 0;  
bool senzorCalibrat = false;

int pragRosuMin = 0, pragRosuMax = 0;
int pragVerdeMin = 0, pragVerdeMax = 0;
int pragAlbastruMin = 0, pragAlbastruMax = 0;

void setup() {
  Serial.begin(9600);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT_COLOR, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT);
  
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), countEncoderTick, RISING);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Sistem initializat. Astept comanda de start.");
  Serial.println("tineti apasat butonul pentru 2 secunde pentru a incepe calibrarea senzorului de culoare.");
  delay(1000);
}

void loop() {
  bool current_button_state = digitalRead(BUTTON_PIN) == HIGH;
  
  if (current_button_state && !last_button_state) {
    ultimaApasare = millis();
  } 
  else if (!current_button_state && last_button_state) {
    if (millis() - ultimaApasare < INTERVAL_APASARE_LUNGA) {
      if (etapaCalibrare == 0) { 
        running = !running;
        if (running) {
          Serial.println("Sistem pornit");
        } else {
          Serial.println("Sistem oprit");
        }
      } else {
        procesareEtapaCalibrare();
      }
    }
  }
  
  if (current_button_state && (millis() - ultimaApasare > INTERVAL_APASARE_LUNGA)) {
    if (etapaCalibrare == 0 && !running) { 
      incepereCalibrare();
      ultimaApasare = millis(); 
    }
  }
  
  last_button_state = current_button_state;

  if (running) {
    motorBanda(true);
    float dist = masurareDistanta();
    
    if (millis() % 500 == 0) {
      Serial.print("Distanta: ");
      Serial.println(dist);
    }
    
    if (dist > 3.5 && dist < 5) { 
      motorBanda(false);
      Serial.println("Obiect detectat! Oprit banda.");
      Serial.println("Distanta:");
      Serial.println(dist);

      delay(200);
      
      String culoare = detecteazaCuloare();
      Serial.println("Culoare detectata: " + culoare);

      if (culoare == "portocaliu") {
        Serial.println("Activare paleta - directia stanga pentru obiect PORTOCALIU");
        paletaStanga();
        delay(1000); 
      } else if (culoare == "albastru") {
        Serial.println("Activare paleta - directia dreapta pentru obiect ALBASTRU");
        paletaDreapta();
        delay(1000); 
      } else {
        Serial.println("Culoare necunoscuta, reluare banda");
      }
      
      delay(1000); 
      motorBanda(true);
    }
  } else {
    motorBanda(false);
  }
}

void incepereCalibrare() {
  etapaCalibrare = 1;
  Serial.println("\n=== MOD CALIBRARE CULORI ===");
  Serial.println("Pozitionati senzorul pe obiectul ALB si apasati scurt butonul");
}

void procesareEtapaCalibrare() {
  switch (etapaCalibrare) {
    case 1: 
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      delay(100);
      albRosu = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      delay(100);
      albVerde = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      delay(100);
      albAlbastru = pulseIn(OUT_COLOR, LOW);
      
      Serial.println("Alb citit:");
      Serial.print("Rosu: "); Serial.println(albRosu);
      Serial.print("Verde: "); Serial.println(albVerde);
      Serial.print("Albastru: "); Serial.println(albAlbastru);
      
      Serial.println("\nAcum pozitionati senzorul pe obiectul NEGRU si apasati scurt butonul");
      etapaCalibrare = 2;
      break;
      
    case 2: 
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      delay(100);
      negruRosu = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      delay(100);
      negruVerde = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      delay(100);
      negruAlbastru = pulseIn(OUT_COLOR, LOW);
      
      Serial.println("Negru citit:");
      Serial.print("Rosu: "); Serial.println(negruRosu);
      Serial.print("Verde: "); Serial.println(negruVerde);
      Serial.print("Albastru: "); Serial.println(negruAlbastru);
      
      Serial.println("\nAcum pozitionati senzorul pe obiectul ALBASTRU si apasati scurt butonul");
      etapaCalibrare = 3;
      break;
      
    case 3: 
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      delay(100);
      albastruRosu = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      delay(100);
      albastruVerde = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      delay(100);
      albastruAlbastru = pulseIn(OUT_COLOR, LOW);
      
      Serial.println("Albastru citit:");
      Serial.print("Rosu: "); Serial.println(albastruRosu);
      Serial.print("Verde: "); Serial.println(albastruVerde);
      Serial.print("Albastru: "); Serial.println(albastruAlbastru);
      
      Serial.println("\nAcum pozitionati senzorul pe obiectul PORTOCALIU si apasati scurt butonul");
      etapaCalibrare = 4;
      break;
      
    case 4: 
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      delay(100);
      portocaliuRosu = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      delay(100);
      portocaliuVerde = pulseIn(OUT_COLOR, LOW);
      
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      delay(100);
      portocaliuAlbastru = pulseIn(OUT_COLOR, LOW);
      
      Serial.println("Portocaliu citit:");
      Serial.print("Rosu: "); Serial.println(portocaliuRosu);
      Serial.print("Verde: "); Serial.println(portocaliuVerde);
      Serial.print("Albastru: "); Serial.println(portocaliuAlbastru);
      
      int difRosu = abs(albastruRosu - portocaliuRosu);
      int difVerde = abs(albastruVerde - portocaliuVerde);
      int difAlbastru = abs(albastruAlbastru - portocaliuAlbastru);
      
      pragRosuMin = albRosu;  
      pragRosuMax = negruRosu;  
      
      pragVerdeMin = albVerde;
      pragVerdeMax = negruVerde;
      
      pragAlbastruMin = albAlbastru;
      pragAlbastruMax = negruAlbastru;
      
      Serial.println("\n=== CALIBRARE COMPLETa ===");
      Serial.println("Valorile de calibrare:");
      Serial.println("Alb - Rosu: " + String(albRosu) + ", Verde: " + String(albVerde) + ", Albastru: " + String(albAlbastru));
      Serial.println("Negru - Rosu: " + String(negruRosu) + ", Verde: " + String(negruVerde) + ", Albastru: " + String(negruAlbastru));
      Serial.println("Portocaliu - Rosu: " + String(portocaliuRosu) + ", Verde: " + String(portocaliuVerde) + ", Albastru: " + String(portocaliuAlbastru));
      Serial.println("Albastru - Rosu: " + String(albastruRosu) + ", Verde: " + String(albastruVerde) + ", Albastru: " + String(albastruAlbastru));
      
      Serial.println("\nDiferente intre culori:");
      Serial.println("Diferenta Rosu: " + String(difRosu));
      Serial.println("Diferenta Verde: " + String(difVerde));
      Serial.println("Diferenta Albastru: " + String(difAlbastru));
      
      etapaCalibrare = 0; 
      senzorCalibrat = true; 
      Serial.println("Puteti porni sistemul prin apasare scurta a butonului");
      break;
  }
}

void motorBanda(bool pornit) {
  if (pornit) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}

void countEncoderTick() {
  if (digitalRead(ENCODER_B_PIN) == HIGH) {
    encoderTicks++;  
  } else {
    encoderTicks--;  
  }
}

void paletaStanga() {
  Serial.println("Stanga - rotatie completa");
  long targetTicks = encoderTicks + TICKS_PER_ROTATION;  
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  unsigned long startTime = millis();
  
  while (encoderTicks < targetTicks) {
    if (millis() % 100 == 0) {  
      Serial.print("Encoder count: ");
      Serial.print(encoderTicks);
      Serial.print(" / Target: ");
      Serial.println(targetTicks);
    }
    
    if (millis() - startTime > 5000) {
      Serial.println("Timeout - oprire fortata");
      break;
    }
    
    delay(5);  
  }
  
  Serial.println("Rotatie completa finalizata");
  oprirePaleta();
}

void paletaDreapta() {
  Serial.println("Dreapta - rotatie completa");
  long targetTicks = encoderTicks - TICKS_PER_ROTATION;  
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  
  unsigned long startTime = millis();
  
  while (encoderTicks > targetTicks) {
    if (millis() % 100 == 0) {  
      Serial.print("Encoder count: ");
      Serial.print(encoderTicks);
      Serial.print(" / Target: ");
      Serial.println(targetTicks);
    }
    
    if (millis() - startTime > 5000) {
      Serial.println("Timeout - oprire fortata");
      break;
    }
    
    delay(5);  
  }
  
  Serial.println("Rotatie completa finalizata");
  oprirePaleta();
}

void oprirePaleta() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

float masurareDistanta() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durata = pulseIn(ECHO_PIN, HIGH, 20000);
  float dist = durata * 0.034 / 2;
  return dist;
}

String detecteazaCuloare() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(50);
  int rosu = pulseIn(OUT_COLOR, LOW);
  
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(50);
  int verde = pulseIn(OUT_COLOR, LOW);
  
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(50);
  int albastru = pulseIn(OUT_COLOR, LOW);
  
  Serial.print("Citire curenta - Rosu: "); Serial.print(rosu);
  Serial.print(", Verde: "); Serial.print(verde);
  Serial.print(", Albastru: "); Serial.println(albastru);
  
  if (senzorCalibrat) {
    long distantaPortocaliu = sq(rosu - portocaliuRosu) + 
                             sq(verde - portocaliuVerde) + 
                             sq(albastru - portocaliuAlbastru);
    
    long distantaAlbastru = sq(rosu - albastruRosu) + 
                           sq(verde - albastruVerde) + 
                           sq(albastru - albastruAlbastru);
    
    bool esteInIntervalRosu = (rosu >= pragRosuMin && rosu <= pragRosuMax);
    bool esteInIntervalVerde = (verde >= pragVerdeMin && verde <= pragVerdeMax);
    bool esteInIntervalAlbastru = (albastru >= pragAlbastruMin && albastru <= pragAlbastruMax);
    
    Serial.print("Distanta fata de portocaliu: "); Serial.println(distantaPortocaliu);
    Serial.print("Distanta fata de albastru: "); Serial.println(distantaAlbastru);
    
    if (!esteInIntervalRosu || !esteInIntervalVerde || !esteInIntervalAlbastru) {
      return "necunoscut"; 
    }
    
    if (distantaPortocaliu < distantaAlbastru) {
      return "portocaliu";
    } else {
      return "albastru";
    }
  } else {
    if (rosu < 340 && verde > 400 && albastru > 200) return "portocaliu";
    else if (rosu > 340 && verde < 400 && albastru < 300) return "albastru";
    else return "necunoscut";
  }
}