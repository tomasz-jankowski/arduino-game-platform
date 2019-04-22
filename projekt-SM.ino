// BIBLIOTEKI
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>



// REJESTR PRZESUWNY
int latchPin = 10;
int dataPin = 11;
int clockPin = 12;

// PRZYCISKI
int key[] = {2, 3, 4, 5, 6, 7, 8, 9};
int button = A2;

// LCD
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// ZMIENNE
int mode = 0;
bool state = 0;
int tab[15];


// ---------- SETUP ----------
void setup()
{
  // lcd
  lcd.begin(16, 2);

  // rejestr
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  // przyciski
  for (int i = 0; i < 8; i++)
  {
    pinMode(key[i], INPUT);
  }

  // GENERATOR PSEUDO-LOSOWY
  randomSeed(analogRead(0));

  // START
  Reset();
  lightShiftRight();
  lightShiftLeft();
  blinkAll(1,200);
  Reset();
  chooseMode();
}


// ---------- LOOP ----------
void loop(){}


// -------------------------

// chooseMode()
void chooseMode()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wybierz tryb");
  delay(2500);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("<-----");
  lcd.setCursor(1, 1);
  lcd.print("Akceptuj tryb");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Przycisk 1-lewo");
  lcd.setCursor(0, 1);
  lcd.print("Przycisk 2-prawo");
  delay(2500);
  lcd.clear();

  chooseMode1();
}


// ---------- FUNKCJE TRYBÓW ----------



// TRYB 1 - TRENING

void Mode1()
{
  unsigned long time1, time2, czas;
  int j = 0;
  blinkAll(1, 500);
  delay(200);
  for (int i = 0; i < 10; i++)
  {
    state = 0;
    j = random(0, 8);
    lightPin(j);
    time1 = micros();
    while (!state)
    {
      if (digitalRead(key[j]) == HIGH)
      {
        time2 = micros();
        czas=(time2-time1)/1000;
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Czas [ms]:");
        lcd.setCursor(6,1);
        lcd.print(czas);
        delay(500);
        blinkAll(1, 500);
        delay(200);
        state = 1;
      }
    }
  }
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("KONIEC");
  lcd.setCursor(4,1);
  lcd.print("TRENINGU");
  lightPin(6);
  delay(3500);
  chooseMode1();
}



// TRYB 2 - CZAS REAKCJI

void Mode2()
{
  unsigned long time1, time2, czas;
  int j=0;
  blinkAll(1,500);
  delay(200);
  for (int i=0; i<15; i++)
  {
    state=0;
    j=random(0,8);
    lightPin(j);
    time1=micros();
    while (!state)
    {
      if (digitalRead(key[j]) == HIGH)
      {
        time2=micros();
        czas=(time2-time1)/1000;
        tab[i]=czas;
        if (czas < 150)
        {
          lcd.clear();
          lcd.setCursor(4,0);
          lcd.print("OSZUSTWO");
          lightPin(7);
          delay(3000);
          chooseMode1();
        }
        else
        {
          delay(500);
          blinkAll(1,500);
          delay(200);
          state = 1;
        }
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Twoj wynik [ms]:");
  lcd.setCursor(6,1);
  int suma=sumAll();
  lcd.print(suma);
  delay(3500);
  int x = EEPROM.read(8);
  x+=1;
  EEPROM.write(8, x);
  int average=(EEPROMReadlong(4)+suma)/x;
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Sredni czas:");
  lcd.setCursor(6,1);
  lcd.print(average);
  lightPin(6);
  delay(3500);
  EEPROMWritelong(4, average/x);
  
  if (suma < EEPROMReadlong(0) || EEPROMReadlong(0) == 0)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Nowy rekord!");
    lcd.setCursor(0,1);
    lcd.print("Poprzedni: ");
    lcd.setCursor(11,1);
    lcd.print(EEPROMReadlong(0));
    delay(3500);

    EEPROMWritelong(0, suma);
  }
  chooseMode1();
}


// TRYB 3 - MULTIPLAYER

void Mode3()
{
  state=0;
  int j=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Gracz 1-button 1");
  lcd.setCursor(0,1);
  lcd.print("Gracz 2-button 8");
  Reset();
  delay(3500);
  int points1=0, points2=0;
  unsigned long czas, time1, time2;
  
  while (points1!=10 && points2!=10)
  {
    state=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("G1");
    lcd.setCursor(14,0);
    lcd.print("G2");
    lcd.setCursor(0,1);
    lcd.print(points1);
    lcd.setCursor(14,1);
    lcd.print(points2);
    
    j = random(0, 8);
    lightPin(j);
    czas = micros();
    
    while (!state)
    {
    if (digitalRead(key[7]) == HIGH)
    {
      time1=micros();
      if (time1-czas < 150)
      {
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("OSZUSTWO");
        lightPin(7);
        points1-=1;
        delay(1000);
        blinkAll(1,500);
        delay(200);
        state=1;
      }
      else
      points1+=1;
      blinkAll(1,500);
      delay(200);
      state=1;
    }


    if (digitalRead(key[0]) == HIGH)
    {
      time2=micros();
      if (time2-czas < 150)
      {
        lcd.clear();
        lcd.setCursor(4,0);
        lcd.print("OSZUSTWO");
        lightPin(7);
        points2-=1;
        delay(1000);
        blinkAll(1,500);
        delay(200);
        state=1;
      }
      else
      points2+=1;
      blinkAll(1,500);
      delay(200);
      state=1;
    }
  }
  }

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Zwyciezca:");
  lcd.setCursor(4,1);
    
  if (points1 > points2)
  {
    lcd.print("Gracz 1");
  }

  else
  {
    lcd.print("Gracz 2");
  }
  lightPin(6);
  delay(3500);
  chooseMode1();
}



// --------------------------------------------------

int sumAll()
{
  int sum=0;
  for (int i=0; i<15; i++)
  {
    sum+=tab[i];
  }
  return sum;
}

void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }

void clearEEPROM()
{
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("EEPROM");
  lcd.setCursor(2,1);
  lcd.print("Clearing");
  delay(500);
  lcd.setCursor(10,1);
  lcd.print(".");
  delay(500);
  lcd.setCursor(11,1);
  lcd.print(".");
  delay(500);
  lcd.setCursor(12,1);
  lcd.print(".");
  delay(500);
  EEPROMWritelong(0,0);
  EEPROMWritelong(4,0);
  EEPROM.write(8,0);
  setup();
}

// ---------- Funkcje wyboru trybu ----------
void chooseMode1()
{
  state=0; 
  delay(200);
  lightPin(7);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Tryb 1");
  lcd.setCursor(4, 1);
  lcd.print("Trening");

  while (!state)
  {
    if (digitalRead(key[6]) == HIGH)
      chooseMode2();

    if (digitalRead(key[7]) == HIGH)
      chooseMode3();

    if (analogRead(button) == 1023)
      Mode1();
  }
}

void chooseMode2()
{
  delay(200);
  lightPin(6);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Tryb 2");
  lcd.setCursor(2, 1);
  lcd.print("Czas reakcji");

  while (!state)
  {
    if (digitalRead(key[6]) == HIGH)
      chooseMode3();

    if (digitalRead(key[7]) == HIGH)
      chooseMode1();

    if (digitalRead(key[2]) == HIGH && digitalRead(key[0]) == HIGH)
      clearEEPROM();

    if (analogRead(button)==1023)
      Mode2();
  }
}

void chooseMode3()
{
  delay(200);
  lightPin(5);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Tryb 3");
  lcd.setCursor(2, 1);
  lcd.print("Multiplayer");

  while (!state)
  {
    if (digitalRead(key[6]) == HIGH)
      chooseMode1();

    if (digitalRead(key[7]) == HIGH)
      chooseMode2();

    if (analogRead(button)==1023)
      Mode3();
  }
}

// ---------- FUNKCJE LED ----------

// ----- Pojedyczny LED -----
void lightPin(int p)
{
  int pin;
  pin = 1 << p;   // mnożenie przez 2
  digitalWrite(latchPin, LOW);    // włączenie nasłuchiwania
  shiftOut(dataPin, clockPin, MSBFIRST, pin);   // przesunięcie bitów
  digitalWrite(latchPin, HIGH);   // wyłączenie nasłuchiwania
}

// ----- Wszystkie jednocześnie -----
void blinkAll(int n, int d)   // n - ilość, d - delay
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  delay(200);
  for (int x = 0; x < n; x++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 255);
    digitalWrite(latchPin, HIGH);
    delay(d);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    delay(d);
  }
}

// ----- Przesuń diody w prawo -----
void lightShiftRight()
{
  for (int i = 7; i >= 0; i--) {
    lightPin(i);
    delay(75);
  }
}

// ----- Przesuń diody w lewo -----
void lightShiftLeft()
{
  for (int i = 0; i < 8; i++) {
    lightPin(i);
    delay(75);
  }
}

// ----- Wyzerowanie rejestru -----
void Reset()
{
  for (int x = 0; x < 8; x++)
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
  }
}
