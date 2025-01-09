#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Free_Fonts.h>
#include <EEPROM.h>
#define NFIELDS 18
#define BUTTON_0 35
#define BUTTON_1 0
#define TFT_LED 4
#define E1P1 26
#define E1P2 27
#define E2P1 12
#define E2P2 13
#define ENC_BUTTON 15
#define STEP 32
#define DIR 33

// Colors
int TFT_COLOR;
#define BACKGROUND_COLOUR 0xFFFFFF
#define ALARM_COLOUR 0x2020FF

TaskHandle_t Task0;

TFT_eSPI tft = TFT_eSPI();

AccelStepper stepper(AccelStepper::DRIVER, STEP, DIR);

enum type_e {
  INT,
  FLOAT,
  ENUM,
  LABEL
};

typedef struct {
  int xpos;
  int ypos;
  int editable;
  int page;
  int eeprom_address;
  enum type_e type;
  int ival;
  float fval;
  float max;
  float min;
  float step;
  const GFXfont* font;
  long colour;
  char* format;
} field;

typedef struct {
  field* fields;
} fields;

// Global variables
fields F;
float pitch; // internally always in MOD
int page;
int sel = 1;
int edit = 0;
int update_flag = 1;
unsigned long timeout = 0;
long long E1cnt; // Spindle encoder
long long E1offset = 0; // spindle encoder offset
long long steps; // steps accumulated so far
long E2cnt;
// lookup table for encoder transitions
int encoder_states[] = {0,	-1,	1,	0,	1,	0,	0,	-1,	-1,	0,	0,	1,	0,	1,	-1,	0};

void IRAM_ATTR countE1(){
  static int oldE = 0;
  int E = (digitalRead(E1P1) << 1) + digitalRead(E1P2);
  E1cnt = E1cnt + encoder_states[oldE + E];
  oldE = (E & 0x3) << 2;
}

void IRAM_ATTR countE2(){
  static int oldE = 3;
  int E = (digitalRead(E2P1) << 1) + digitalRead(E2P2);
  E2cnt = E2cnt + encoder_states[oldE + E];
  oldE = (E & 0x3) << 2;
}

float fround(float val, float step) {
  int c = (val + step/2) / step;
  return c * step;
}

// Application-specific code
void do_calcs(){
  float depth;
  float mod, PA, shift, f, kf, Zmth, span;
  static int N;
  int num;

  switch (F.fields[1].ival) {
    case 0: // MOD
      pitch = F.fields[0].fval;
      break;
    case 1: // DP
      pitch = 25.4 / F.fields[0].fval;
      break;
    case 2: // MM CP
      pitch = F.fields[0].fval / PI;
      break;
    case 3: // IN CP
      pitch = (25.4 / PI) * F.fields[0].fval;
      break;
  }

  // PCD
  if (N != F.fields[2].ival){
    E1offset = E1cnt;
    stepper.setCurrentPosition(0);
  }
  N = F.fields[2].ival;
  shift = F.fields[4].fval;
  F.fields[5].fval = pitch * N + 2 * shift;
  F.fields[6].fval = (pitch * N + 2 * shift) / 25.4;
  F.fields[7].fval = pitch * (N + 2) + 2 * shift;
  F.fields[8].fval = (pitch * (N + 2) + 2 * shift) / 25.4;
  if (pitch < 1){
    depth = 2.4 * pitch;
  } else {
    depth = 2.25 * pitch;
  }
  F.fields[9].fval = F.fields[7].fval - 2 * depth;
  F.fields[10].fval = F.fields[9].fval / 25.4;

 
  // http://www.sdp-si.com/D805/D805_PDFS/Technical/8050T081.pdf
  f = shift / N;
  PA = (F.fields[3].fval / 180) * PI;
  kf =(1/PI) * ((1/cos(PA))*sqrt(sq(1+2*f)-sq(cos(PA)))-(tan(PA)-PA)-(2*f*tan(PA)));
  Zmth = N * kf+0.5;
  num = (int)(Zmth + .5);
  span = pitch*cos(PA) *(PI * (num-0.5) + N * (tan(PA)-PA)) + 2 * shift*pitch*sin(PA);
  F.fields[11].fval = span;
  F.fields[12].ival = num;

  stepper.setAcceleration((float)F.fields[15].ival);
  stepper.setMaxSpeed((float)F.fields[16].ival);

}

void update(){
  char buffer[128];
  char* ptr = NULL;

  do_calcs(); // Application-specific calculations

  tft.fillScreen(BACKGROUND_COLOUR); // Clear Screen
  for (int i = 0; i < NFIELDS; i++){
    field* D = &F.fields[i];
    //Serial.printf("type %i, xpos = %i ypos = %i ival = %i format = %s\n", D->type, D->xpos, D->ypos, D->ival, D->format);
    if (D->page == page){ // only show elements on the current page
      tft.setCursor(D->xpos, D->ypos);
      tft.setFreeFont(D->font);
      if (i == sel && edit){
        tft.setTextColor(TFT_RED);
      } else {
        tft.setTextColor(D->colour);
      }
      switch(D->type) {
        case INT:
          tft.printf(D->format, D->ival);
          break;
        case FLOAT:
          tft.printf(D->format, D->fval);
          break;
        case ENUM:
          strcpy(buffer, D->format);
          ptr = strtok(buffer,"|");
          for (int j = 0; j < D->ival && ptr != NULL; j++) {
            ptr = strtok(NULL, "|");
          }
          tft.printf("%s", ptr);
          break;
        case LABEL:
          tft.printf(D->format);
          break;
        default:
          break;
      }
    }
  }
}
void setup() {
  long temp;

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  Serial.begin(115200);
  Serial.println("Hobber");

  EEPROM.begin(24);

  pinMode(BUTTON_0, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(ENC_BUTTON, INPUT_PULLUP);
  pinMode(E1P1, INPUT_PULLUP);
  pinMode(E1P2, INPUT_PULLUP);
  pinMode(E2P1, INPUT_PULLUP);
  pinMode(E2P2, INPUT_PULLUP);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(E1P1), countE1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E1P2), countE1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E2P1), countE2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(E2P2), countE2, CHANGE);

  tft.init();
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND_COLOUR); // Clear Screen

  F.fields = (field*)malloc(NFIELDS * sizeof(field));
  // Define the GUI input/output fields
  #include "fields.h"

  // xTaskCreatePinnedToCore(
  //                   task0Code,   /* Task function. */
  //                   "Task0",     /* name of task. */
  //                   32000,       /* Stack size of task */
  //                   NULL,        /* parameter of the task */
  //                   1,           /* priority of the task */
  //                   &Task0,      /* Task handle to keep track of created task */
  //                   0);          /* pin task to core 0 */                  
  // delay(500);

}

void save_nonvol(){
    for (int i = 0; i < NFIELDS; i++){
      field* D = &F.fields[i];
      if (D->eeprom_address >= 0){
        switch (D->type){
          case INT:
          case ENUM:
            EEPROM.put(D->eeprom_address, D->ival);
            Serial.printf("storing INT %i\n", D->ival);
            break;
          case FLOAT:
             EEPROM.put(D->eeprom_address, D->fval);
             Serial.printf("storing FLOAT %f\n", D->fval);
            break;
        }
        EEPROM.commit();
      }
    }
}

void do_GUI(){
  static long button_timer;
  static long oldE2 = E2cnt;
  static int bstate = 0;

  int v = digitalRead(ENC_BUTTON);
  long now = millis();
  if (v == 1) {
    button_timer = now;
    bstate = 0;
  } else {
    if (now - button_timer > 100 && bstate == 0){ // normal debounce
      bstate = 1;
      edit = 1;
      timeout = now;
      update_flag = 1;
      do {
        sel = (sel + 1) % NFIELDS;
      } while (F.fields[sel].editable == 0 || F.fields[sel].page != page);
    }
    if (now - button_timer > 5000 && bstate == 1){ //long press
      page = 1;
      edit = 1;
      update_flag = 1;
      timeout = now;
    }
  }

  if (update_flag){
    update();
    update_flag = 0;
  }

  if (edit){
    int delta = ((E2cnt - oldE2) / 4);
    if (millis() - timeout > 5000) { 
      edit = 0;
      if (page == 1) save_nonvol();
      page = 0;
      sel = 1;
      update_flag = 1;
    }

    if (delta != 0){
      oldE2 = E2cnt;
      timeout = millis();
      field* D = &F.fields[sel];
      switch (D->type){
        case INT:
          D->ival += delta * D->step;
          D->ival = (int)max(D->min, min(D->max, (float)D->ival));
          break;
        case ENUM:
          D->ival += delta;
          if (D->ival > D->max) D->ival = D->min;
          if (D->ival < D->min) D->ival = D->max;
          if (sel = 1) { // Special handling of change of units
            field* P = &F.fields[0];
            switch (D->ival){
              case 0: // MOD
                P->format = "%3.2f";
                P->step = 0.05;
                P->fval = fround(pitch, P->step);
                break;
              case 1: // DP
                P->format = "%5.0f";
                P->step = 1;
                P->fval = fround(25.4 / pitch, P->step);
                break;
              case 2: // MM
                P->format = "%3.2f";
                P->step = 0.01;
                P->fval = fround(PI * pitch, P->step);
                break;
              case 3: // IN
                P->format = "%1.3f";
                P->step = 0.001;
                P->fval = fround(PI * pitch / 25.4, P->step);
                break;
            }
          }
          break;
        case FLOAT:
          D->fval += delta * D->step;
          D->fval = max(D->min, min(D->max, D->fval));
          break;
      }
      update_flag = 1;
    }
  }
}

void task0Code( void * pvParameters ){
  static bool step_state;
  static bool dir_state;
  // target = total edges, so steps * 2
  // 2 * (count - offset) * steps_per_rev / (counts_per_rev * N_teeth)
  long long target = 2 * (E1cnt - E1offset) * F.fields[14].ival / (F.fields[13].ival * F.fields[2].ival);

  while (1){
    Serial.println(uxTaskGetStackHighWaterMark(NULL));
    if (steps < target){
      step_state = ! step_state;
      digitalWrite(DIR, 0);
      digitalWrite(STEP, step_state);
      //Serial.println("Step");
      steps++;
    }
    else if (steps > target) {
      step_state = ! step_state;
      digitalWrite(DIR, 1);
      digitalWrite(STEP, step_state);
      //Serial.println("Antistep");
      steps--;
    }
  }
}

void loop(){

  static bool saturated = 0;
  // target = total edges, so steps * 2
  // 2 * (count - offset) * steps_per_rev / (counts_per_rev * N_teeth)
  long long target = F.fields[17].ival * (E1cnt - E1offset) * F.fields[14].ival / (F.fields[13].ival * F.fields[2].ival);

  stepper.moveTo(target);
  stepper.run();

  if (abs(stepper.speed()) > 0.95 * stepper.maxSpeed() && ! saturated){
    tft.fillScreen(ALARM_COLOUR);
    saturated = 1;
  } else if (abs(stepper.speed()) < 0.94 * stepper.maxSpeed() && saturated){
    tft.fillScreen(BACKGROUND_COLOUR);
    saturated = 0;
    update_flag = 1;
  }
  // if (steps < target){
  //   step_state = ! step_state;
  //   digitalWrite(DIR, 0);
  //   digitalWrite(STEP, step_state);
  //   //Serial.println("Step");
  //   steps++;
  // }
  // else if (steps > target) {
  //   step_state = ! step_state;
  //   digitalWrite(DIR, 1);
  //   digitalWrite(STEP, step_state);
  //   //Serial.println("Antistep");
  //   steps--;
  // }
  do_GUI();
}
