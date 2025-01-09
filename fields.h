// Tooth size
field* D = &F.fields[0];
D->page = 0;
D->type = FLOAT;
D->xpos = 140;
D->ypos = 90;
D->format = "%5.0f";
D->font = FSB24;
D->colour = TFT_BLACK;
D->fval = 18;
pitch = 25.4/18;
D->step = 1;
D->min = 0;
D->max = 99;
D->editable = 1;
D->eeprom_address = -1;

// Standard
D = &F.fields[1];
D->page = 0;
D->type = ENUM;
D->xpos = 225;
D->ypos = 90;
D->format = "Mod| DP| MM|    IN";
D->max = 3;
D->min = 0;
D->font = FSB12;
D->colour = TFT_BLACK;
D->ival = 1;
D->editable = 1;
D->eeprom_address = -1;

// Tooth Count
D = &F.fields[2];
D->page = 0;
D->type = INT;
D->xpos = 40;
D->ypos = 90;
D->format = "%3.0iT";
D->max = 999;
D->min = 0;
D->step = 1;
D->font = FSB24;
D->colour = TFT_BLACK;
D->ival = 20;
D->editable = 1;
D->eeprom_address = -1;

// PA
D = &F.fields[3];
D->page = 0;
D->type = FLOAT;
D->xpos = 50;
D->ypos = 113;
D->format = "PA %2.1f";
D->max = 50;
D->min = 0;
D->step = 0.5;
D->font = FS12;
D->colour = TFT_BLACK;
D->fval = 20;
D->editable = 1;
D->eeprom_address = -1;

// Shift
D = &F.fields[4];
D->page = 0;
D->type = FLOAT;
D->xpos = 140;
D->ypos = 115;
D->format = "Shift %1.2fmm";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS12;
D->colour = TFT_BLACK;
D->fval = 0;
D->editable = 1;
D->eeprom_address = -1;

// PCD MM
D = &F.fields[5];
D->page = 0;
D->type = FLOAT;
D->xpos = 50;
D->ypos = 132;
D->format = "PCD %3.2f mm";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// PCD IN
D = &F.fields[6];
D->page = 0;
D->type = FLOAT;
D->xpos = 170;
D->ypos = 132;
D->format = "/ %2.3f in";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// OD MM
D = &F.fields[7];
D->page = 0;
D->type = FLOAT;
D->xpos = 50;
D->ypos = 149;
D->format = "OD  %3.2f mm";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// OD IN
D = &F.fields[8];
D->page = 0;
D->type = FLOAT;
D->xpos = 170;
D->ypos = 149;
D->format = "/ %2.3f in";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// ID MM
D = &F.fields[9];
D->page = 0;
D->type = FLOAT;
D->xpos = 50;
D->ypos = 166;
D->format = "RD  %3.2f mm";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// ID IN
D = &F.fields[10];
D->page = 0;
D->type = FLOAT;
D->xpos = 170;
D->ypos = 166;
D->format = "/ %2.3f in";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_BLUE;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// Span MM
D = &F.fields[11];
D->page = 0;
D->type = FLOAT;
D->xpos = 50;
D->ypos = 183;
D->format = "Span %2.3f mm";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_DARKGREEN;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// Over N
D = &F.fields[12];
D->page = 0;
D->type = INT;
D->xpos = 180;
D->ypos = 183;
D->format = "over %1.0i teeth";
D->max = 9.99;
D->min = -9.99;
D->step = 0.05;
D->font = FS9;
D->colour = TFT_DARKGREEN;
D->fval = 0;
D->editable = 0;
D->eeprom_address = -1;

// Encoder CPT
D = &F.fields[13];
D->page = 1;
D->type = INT;
D->xpos = 50;
D->ypos = 75;
D->format = "Enc   %5.0i PPR";
D->max = 100000;
D->min = 0;
D->step = 4;
D->font = FSB12;
D->colour = TFT_BLACK;
D->editable = 1;
D->eeprom_address = 0;
EEPROM.get(D->eeprom_address, temp);
  Serial.printf("Nonvol read %i\n", temp);
if (temp > 10){
  D->ival = temp;
} else {
  D->ival = 1600;
}

// Motor PPR
D = &F.fields[14];
D->page = 1;
D->type = INT;
D->xpos = 50;
D->ypos = 100;
D->format = "Motor %5.0i PPR";
D->max = 1000000;
D->min = 0;
D->step = 100;
D->font = FSB12;
D->colour = TFT_BLACK;
D->editable = 1;
D->eeprom_address = 8;
EEPROM.get(D->eeprom_address, temp);
Serial.printf("Nonvol read %i\n", temp);
if (temp > 10){
  D->ival = temp;
} else {
  D->ival = 90 * 200 * 4;
}

// Motor Accel
D = &F.fields[15];
D->page = 1;
D->type = INT;
D->xpos = 50;
D->ypos = 125;
D->format = "Max Accel %5.0i P/S/S";
D->max = 1000000;
D->min = 0;
D->step = 500;
D->font = FSB12;
D->colour = TFT_BLACK;
D->editable = 1;
D->eeprom_address = 12;
EEPROM.get(D->eeprom_address, temp);
Serial.printf("Nonvol read %i\n", temp);
if (temp > 10){
  D->ival = temp;
} else {
  D->ival = 100000;
}

// Motor speed
D = &F.fields[16];
D->page = 1;
D->type = INT;
D->xpos = 50;
D->ypos = 150;
D->format = "Max Speed %5.0i P/S";
D->max = 1000000;
D->min = 0;
D->step = 500;
D->font = FSB12;
D->colour = TFT_BLACK;
D->editable = 1;
D->eeprom_address = 16;
EEPROM.get(D->eeprom_address, temp);
Serial.printf("Nonvol read %i\n", temp);
if (temp > 10){
  D->ival = temp;
} else {
  D->ival = 50000;
}

// Motor direction
D = &F.fields[17];
D->page = 1;
D->type = INT;
D->xpos = 50;
D->ypos = 175;
D->format = "Direction %i";
D->max = 1;
D->min = -1;
D->step = 1;
D->font = FSB12;
D->colour = TFT_BLACK;
D->editable = 1;
D->eeprom_address = 20;
EEPROM.get(D->eeprom_address, temp);
Serial.printf("Nonvol read %i\n", temp);
if (temp != 0){
  D->ival = temp;
} else {
  D->ival = 1;
}