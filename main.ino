// register set/clear macros
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// voltage reference for ADC
#define AREF INTERNAL

// threshold at/above which we trigger a pulse
#define ATHRESH 100

// pulse output pin
#define POUT 5

inline int sample() {  
  // start the conversion
  sbi(ADCSRA, ADSC);

  // ADSC is cleared when the conversion finishes
  while (bit_is_set(ADCSRA, ADSC));

  // ADC macro takes care of reading ADC register.
  // avr-gcc implements the proper reading order: ADCL is read first.
  return ADC;
}

int maxval = 0;
void setup() {
  // enable ADC high speed mode  
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
  ADCSRA |= bit (ADPS0) | bit (ADPS1);                 //   8
  pinMode(A0, INPUT);
  pinMode(5, OUTPUT);
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  ADMUX = 192;
  // set ADMUX as static value for pin 0
  ADMUX = (AREF << 6) | (0 & 0x07);
}

void loop() {
  if (sample() > ATHRESH) {
    // spec from volca sample 2 manual: 5V for 15ms
    PORTD |= B00100000;
    delay(15);
    PORTD ^= B00100000;
  }
}
