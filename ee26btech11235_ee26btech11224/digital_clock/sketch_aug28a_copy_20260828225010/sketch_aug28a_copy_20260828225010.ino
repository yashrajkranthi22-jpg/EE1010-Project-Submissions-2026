// ---------------- Shared 7447 Inputs ----------------
const int PIN_A = A0, PIN_B = A1, PIN_C = A2, PIN_D = A3;

// ---------------- Enable pins for multiplexing ----------------
const int EN[] = {4,5,6,7,8,9};  // sec1, sec10, min1, min10, hr1, hr10

// ---------------- Buttons & Debounce Struct ----------------
struct Button {
  int pin;
  int stableState;
  int rawState;
  unsigned long lastChangeTime;
  unsigned long lastRepeatTime;
  bool actionFired;
};

Button btnPause = {10, HIGH, HIGH, 0, 0, false};
Button btnNext  = {11, HIGH, HIGH, 0, 0, false};
Button btnInc   = {12, HIGH, HIGH, 0, 0, false};
Button btnDec   = {13, HIGH, HIGH, 0, 0, false};

// ---------------- Mode FSM & State ----------------
enum Mode { RUN, EDIT };
Mode clockMode = RUN;

int selectedDigit = 0;  // 0..5 -> sec1..hr10
unsigned long lastBlink = 0;
bool blinkOn = true;

// ---------------- Multiplexing & Timing ----------------
unsigned long lastSecUpdate = 0;
unsigned long lastRefreshTime = 0;
int currentDisp = 0; 

// ---------------- Current Boolean States ----------------
// Seconds
int W1=0, X1=0, Y1=0, Z1=1;       
int W2=1, X2=0, Y2=1;               
// Minutes
int W3=1, X3=0, Y3=0, Z3=1;       
int W4=1, X4=0, Y4=1;               
// Hours
int W5=1, X5=1, Y5=0, Z5=0;       
int W6=0, X6=1, Y6=0;               


// ---------------- Setup ----------------
void setup() {
  pinMode(PIN_A, OUTPUT); pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT); pinMode(PIN_D, OUTPUT);
  for(int i=0; i<6; i++) { 
    pinMode(EN[i], OUTPUT); 
    digitalWrite(EN[i], LOW); 
  }
  
  pinMode(btnPause.pin, INPUT_PULLUP);
  pinMode(btnNext.pin, INPUT_PULLUP);
  pinMode(btnInc.pin, INPUT_PULLUP);
  pinMode(btnDec.pin, INPUT_PULLUP);

  lastSecUpdate = millis();
  lastRefreshTime = millis();
}


// ---------------- Robust Button Logic ----------------
bool checkPress(Button &b, unsigned long currentMillis, bool allowRepeat) {
  bool triggered = false;
  int reading = digitalRead(b.pin);
  
  if (reading != b.rawState) {
    b.lastChangeTime = currentMillis;
    b.rawState = reading;
  }
  
  if ((currentMillis - b.lastChangeTime) > 50) {
    if (reading != b.stableState) {
      b.stableState = reading;
      
      if (b.stableState == LOW) { 
        triggered = true;
        b.lastRepeatTime = currentMillis;
        b.actionFired = true;
      } else {
        b.actionFired = false; 
      }
    } 
    else if (allowRepeat && b.stableState == LOW && b.actionFired) {
      if (currentMillis - b.lastRepeatTime > 500) { 
        triggered = true;
        b.lastRepeatTime = currentMillis - 350; 
      }
    }
  }
  return triggered;
}


// ---------------- Increment Helper ----------------
void incrementDigit(int d){
  int A,B,C,D;
  switch(d){
    case 0: { 
      A = !W1;
      B = (W1 && !X1 && !Z1)||(!W1 && X1);
      C = (!X1 && Y1)||(!W1 && Y1)||(W1 && X1 && !Y1);
      D = (!W1 && Z1)||(W1 && X1 && Y1);
      W1=A; X1=B; Y1=C; Z1=D;
    } break;
    case 1: { 
      A = !W2;
      B = (W2 && !X2 && !Y2)||(!W2 && X2);
      C = (W2 && X2)||(!W2 && !X2 && Y2);
      W2=A; X2=B; Y2=C;
    } break;
    case 2: { 
      A = !W3;
      B = (W3 && !X3 && !Z3)||(!W3 && X3);
      C = (!X3 && Y3)||(!W3 && Y3)||(W3 && X3 && !Y3);
      D = (!W3 && Z3)||(W3 && X3 && Y3);
      W3=A; X3=B; Y3=C; Z3=D;
    } break;
    case 3: { 
      A = !W4;
      B = (W4 && !X4 && !Y4)||(!W4 && X4);
      C = (W4 && X4)||(!W4 && !X4 && Y4);
      W4=A; X4=B; Y4=C;
    } break;
    case 4: { 
      if(X6==0){ 
        A = !W5;
        B = (W5 && !X5 && !Z5)||(!W5 && X5);
        C = (!X5 && Y5)||(!W5 && Y5)||(W5 && X5 && !Y5);
        D = (!W5 && Z5)||(W5 && X5 && Y5);
        W5=A; X5=B; Y5=C; Z5=D;
      } else { 
        A = !W5;
        B = (W5 && !X5)||(!W5 && X5);
        W5=A; X5=B; Y5=0; Z5=0;
      }
    } break;
    case 5: { 
      A = !W6 && !X6;
      B = W6 && !X6;
      W6=A; X6=B; Y6=0;
      
      if(X6==1 && W6==0) { 
        if(Y5==1 || Z5==1) { 
          W5=1; X5=1; Y5=0; Z5=0; 
        }
      }
    } break;
  }
}

// ---------------- Decrement Helper ----------------
void decrementDigit(int d){
  int A,B,C,D;
  switch(d){
    case 0: { 
      A = !W1;
      B = (!X1 && !W1 && ((!Z1 && Y1)||(Z1 && !Y1))) || (!Z1 && W1 && X1); 
      C = (!Z1 && Y1 && (X1||W1)) || (Z1 && !X1 && !W1 && !Y1);
      D = !X1 && !Y1 && ((Z1 && W1) || (!Z1 && !W1));
      W1=A; X1=B; Y1=C; Z1=D;
    } break;
    case 1: { 
      A = !W2;
      B = (Y2 && !X2 && !W2) || (!Y2 && X2 && W2);
      C = !X2 && ((Y2 && W2) || (!Y2 && !W2));
      D = 0;
      W2=A; X2=B; Y2=C;
    } break;
    case 2: { 
      A = !W3;
      B = (!X3 && !W3 && ((!Z3 && Y3)||(Z3 && !Y3))) || (!Z3 && W3 && X3);
      C = (!Z3 && Y3 && (X3||W3)) || (Z3 && !X3 && !W3 && !Y3);
      D = !X3 && !Y3 && ((Z3 && W3) || (!Z3 && !W3));
      W3=A; X3=B; Y3=C; Z3=D;
    } break;
    case 3: { 
      A = !W4;
      B = (Y4 && !X4 && !W4) || (!Y4 && X4 && W4);
      C = !X4 && ((Y4 && W4) || (!Y4 && !W4));
      D = 0;
      W4=A; X4=B; Y4=C;
    } break;
    case 4: { 
      if(X6==0){ 
        A = !W5;
        B = (!X5 && !W5 && ((!Z5 && Y5)||(Z5 && !Y5))) || (!Z5 && W5 && X5);
        C = (!Z5 && Y5 && (X5||W5)) || (Z5 && !X5 && !W5 && !Y5);
        D = !X5 && !Y5 && ((Z5 && W5) || (!Z5 && !W5));
        W5=A; X5=B; Y5=C; Z5=D;
      } else { 
        A = !W5;
        B = (X5 && W5) || (!X5 && !W5);
        W5=A; X5=B; Y5=0; Z5=0;
      }
    } break;
    case 5: { 
      A = X6 && !W6;
      B = !X6 && !W6;
      W6=A; X6=B; Y6=0;
      
      if(X6==1 && W6==0) { 
        if(Y5==1 || Z5==1) { 
          W5=1; X5=1; Y5=0; Z5=0; 
        }
      }
    } break;
  }
}

// ---------------- Display Helper ----------------
void showDigit(int A, int B, int C, int D, int ENpin) {
  for(int i=0; i<6; i++) digitalWrite(EN[i], LOW);
  
  digitalWrite(PIN_A, A); digitalWrite(PIN_B, B);
  digitalWrite(PIN_C, C); digitalWrite(PIN_D, D);
  digitalWrite(ENpin, HIGH);
}

// ---------------- Main Loop ----------------
void loop() {
  unsigned long currentMillis = millis();

  // 1. Process Button Inputs
  bool pausePressed = checkPress(btnPause, currentMillis, false);
  bool nextPressed  = checkPress(btnNext,  currentMillis, false);
  bool incPressed   = checkPress(btnInc,   currentMillis, true);
  bool decPressed   = checkPress(btnDec,   currentMillis, true);

  // 2. Prioritized FSM Routing
  if (pausePressed) {
    if (clockMode == RUN) {
      clockMode = EDIT;
      selectedDigit = 0;
      blinkOn = true;
      lastBlink = currentMillis;
    } else {
      clockMode = RUN;
      lastSecUpdate = currentMillis; 
    }
  } 
  else if (clockMode == EDIT) {
    if (nextPressed) {
      selectedDigit = (selectedDigit + 1) % 6;
      blinkOn = true;
      lastBlink = currentMillis;
    } 
    else if (incPressed) {
      incrementDigit(selectedDigit);
      blinkOn = true;
      lastBlink = currentMillis;
    } 
    else if (decPressed) {
      decrementDigit(selectedDigit);
      blinkOn = true;
      lastBlink = currentMillis;
    }

    if (currentMillis - lastBlink >= 500) {
      blinkOn = !blinkOn;
      lastBlink = currentMillis;
    }
  } 
  else if (clockMode == RUN) {
    if (currentMillis - lastSecUpdate >= 1000) {
      lastSecUpdate += 1000;
      
      int A = !W1;
      int B = (W1 && !X1 && !Z1)||(!W1 && X1);
      int C = (!X1 && Y1)||(!W1 && Y1)||(W1 && X1 && !Y1);
      int D = (!W1 && Z1)||(W1 && X1 && Y1);
      W1=A; X1=B; Y1=C; Z1=D;
      
      if((W1|X1|Y1|Z1)==0){
        A = !W2;
        B = (W2 && !X2 && !Y2)||(!W2 && X2);
        C = (W2 && X2)||(!W2 && !X2 && Y2);
        W2=A; X2=B; Y2=C;
      }
      
      if((W1|X1|Y1|Z1|W2|X2|Y2)==0){
        A = !W3;
        B = (W3 && !X3 && !Z3)||(!W3 && X3);
        C = (!X3 && Y3)||(!W3 && Y3)||(W3 && X3 && !Y3);
        D = (!W3 && Z3)||(W3 && X3 && Y3);
        W3=A; X3=B; Y3=C; Z3=D;
        
        if((W3|X3|Y3|Z3)==0){
          A = !W4;
          B = (W4 && !X4 && !Y4)||(!W4 && X4);
          C = (W4 && X4)||(!W4 && !X4 && Y4);
          W4=A; X4=B; Y4=C;
        }
        
        if((W3|X3|Y3|Z3|W4|X4|Y4)==0){
          if(X6==0){ 
            A = !W5;
            B = (W5 && !X5 && !Z5)||(!W5 && X5);
            C = (!X5 && Y5)||(!W5 && Y5)||(W5 && X5 && !Y5);
            D = (!W5 && Z5)||(W5 && X5 && Y5);
            W5=A; X5=B; Y5=C; Z5=D;
          } else { 
            A = !W5;
            B = (W5 && !X5)||(!W5 && X5);
            W5=A; X5=B; Y5=0; Z5=0;
          }
          
          if((W5|X5|Y5|Z5)==0){
            A = !W6 && !X6;
            B = W6 && !X6;
            W6=A; X6=B; Y6=0;
          }
        }
      }
    }
  }

// 4. Non-Blocking Multiplexing using millis()
  if (currentMillis != lastRefreshTime) {
    lastRefreshTime = currentMillis; 

    bool showThis = true;
    if (clockMode == EDIT && currentDisp == selectedDigit && !blinkOn) {
      showThis = false;
    }

    if (showThis) {
      switch (currentDisp) {
        case 0: showDigit(W1,X1,Y1,Z1,EN[0]); break;
        case 1: showDigit(W2,X2,Y2,0,EN[1]);  break;
        case 2: showDigit(W3,X3,Y3,Z3,EN[2]); break;
        case 3: showDigit(W4,X4,Y4,0,EN[3]);  break;
        case 4: showDigit(W5,X5,Y5,Z5,EN[4]); break;
        case 5: showDigit(W6,X6,Y6,0,EN[5]);  break;
      }
    } else {
      for(int i=0; i<6; i++) digitalWrite(EN[i], LOW);
    }

    currentDisp = (currentDisp + 1) % 6; 
  }
}