// GPU fan controller
// temperature-controlled fan controller meant for passive-cooled NVidia GPUs
//
// hardware:
// Seeed XIAO
//
// (per temperature sensor)
// 1x 10K NTC (1%)
// 1x 10K 1%
//
// (per PWM channel)
// 1x small-signal NPN transistor
// 1x 1K resistor (any tolerance OK)
// 1x 2.2K resistor
//
// schematics
// temperature sensor:
//
// GND----[NTC]----[analog input pin]----[10k]----3.3V
//
// glue the thermistor to the mounting plate end of the heatsink
// just below the heatsink fins
//
// PWM output channel:
//
//                12V----[1k]----.----[PWM input to fan]
//                              /
// [PWM output pin]----[2.2k]----[base]
//                              \
//                               [emitter]----GND
//
//

#define R_SERIES 10000
#define B_COEFF 3971
#define TEMP_NOMINAL 25
#define NUM_SAMPLES 5
#define THERM_NOMINAL 10000
#define PWM_FREQ 25000

const int ledPin = 13;

// pins
const int ntc_1_pin = 10;
const int fan_1_pin = 1;
const int ntc_2_pin = 9;
const int fan_2_pin = 2;

// minimum PWM
const int base_pwm = 30;

// temperatures
float ntc_1_temp;
float ntc_2_temp;

// PWM percentages
int fan_1_pwm;
int fan_2_pwm;

void setup(void) {
  Serial.begin(115200);
  //while (!Serial);
  Serial.println(F("OK"));
  analogReadResolution(12);

  // configure pins
  pinMode(fan_1_pin, OUTPUT);
  pinMode(ntc_1_pin, INPUT);
  pinMode(fan_2_pin, OUTPUT);
  pinMode(ntc_2_pin, INPUT);

  // start fans
  set_pwm(100, fan_1_pin);
  set_pwm(100, fan_2_pin);
  delay(3000);
}

// function read_thermistor
//   expects: int analog pin to read
//   returns: float temperature in celcius
float read_thermistor(int t_pin) {
  int i;
  float average;
  int samples[NUM_SAMPLES];
  for (i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = analogRead(t_pin);
    delay(10);
  }
  average = 0;
  for (i = 0; i < NUM_SAMPLES; i++) {
    average += samples[i];
  }
  average /= NUM_SAMPLES;
  average = 4095 / average - 1;
  average = R_SERIES / average;
  float st;
  st = average / THERM_NOMINAL;
  st = log(st);
  st /= B_COEFF;
  st += 1.0 / (TEMP_NOMINAL + 273.15);
  st = 1.0 / st;
  st -= 273.15;
  return st;
}

// function set_pwm
//   expects: int PWM percentage 0 - 100
//   returns: nothing
void set_pwm(int pwm_percent, int pwm_pin) {
  int value;
  float pwm_ratio;
  // sanity checks
  // don't let the fans go below 30% PWM
  // limit max to 100%
  if (pwm_percent < 30)
    pwm_percent = 30;
  if (pwm_percent > 100)
    pwm_percent = 100;
  pwm_ratio = ((100 - float(pwm_percent)) / 100.0);
  value = int(pwm_ratio * 1023);
  Serial.print(F("PIN "));
  Serial.println(pwm_pin);
  Serial.print(F("PWM %: "));
  Serial.println(pwm_percent);
  Serial.print(F("PWM value: "));
  Serial.println(value);
  Serial.println(F("------------"));
  pwm(pwm_pin, 25000, value);
}

void loop(void) {
  // read the thermistor
  ntc_1_temp = read_thermistor(ntc_1_pin);
  Serial.print(F("NTC 1 temp: "));
  Serial.println(ntc_1_temp);
  ntc_2_temp = read_thermistor(ntc_2_pin);
  Serial.print(F("NTC 2 temp: "));
  Serial.println(ntc_2_temp);
  // the NTC reads about 10 C lower than the GPU temperature reported by nvidia-smi
  // anything below 32 C will be considered idle and use 30% PWM
  // anything above 42 C will be considered hot and use 100% PWM
  if (ntc_1_temp < 32)
    ntc_1_temp = 32;
  if (ntc_1_temp > 42)
    ntc_1_temp = 42;
  if (ntc_2_temp < 32)
    ntc_2_temp = 32;
  if (ntc_2_temp > 42)
    ntc_2_temp = 42;
  fan_1_pwm = map(ntc_1_temp, 32, 42, 30, 100);
  set_pwm(fan_1_pwm, fan_1_pin);
  fan_2_pwm = map(ntc_2_temp, 32, 42, 30, 100);
  set_pwm(fan_2_pwm, fan_2_pin);
  delay(500);
}