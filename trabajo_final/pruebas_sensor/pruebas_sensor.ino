const float max_voltage = 5.0;
const float adc_max_range = 1023.0;
const float voltage_conversion_value = 0.01;

float temperature=0;
float voltage=0;

void setup() {
  // Initialize the serial port at 9600 baud
  Serial.begin(1200);
}

void loop() {

  // takes an analog read from A0 pin
  int raw_value = analogRead(A0);

  // prints the raw value from sensor
 // Serial.print("Raw value: ");
 // Serial.println(raw_value);
 // Serial.print("\n");

  // voltage conversion
  voltage = raw_value * (max_voltage / adc_max_range) ;


 // Serial.print("Current voltage:");
 // Serial.println(voltage);
  
 // Serial.print("\n");

  // temperature conversion
  temperature = voltage / (voltage_conversion_value);


  Serial.print("Current temp:");
  Serial.println(temperature);
  //Serial.print("\n");


  // waits 100ms before another read
  delay(100);
}