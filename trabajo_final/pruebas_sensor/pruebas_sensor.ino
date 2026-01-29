#define SENSORS_INTERVAL 100

const float max_voltage = 5.0;
const float adc_max_range = 1023.0;
const float voltage_conversion_value = 0.01;

float temperature=0;
float voltage=0;
unsigned long previous_time_sensor = 0;




void setup() {
  // Initialize the serial port at 9600 baud
  Serial.begin(1200);
}

void loop() {

  // takes current time in milliseconds
  unsigned long current_time = millis(); 
  
  /********************************************* BEGIN SENSOR ROUTINE *********************************************/

  if (current_time - previous_time >= SENSORS_INTERVAL)
    {
      // update previous_time_sensors value (last time into the sensors routine)
      previous_time_sensor = current_time;

      // takes an analog read from A0 pin
      int raw_value = analogRead(A0);

      // prints the raw value from sensor
      // Serial.print("Raw value: ");
      // Serial.println(raw_value);
      // Serial.print("\n");

      // voltage conversion
      voltage = raw_value * (max_voltage / adc_max_range) ;

      // prints the current voltage value 
      // Serial.print("Current voltage:");
      // Serial.println(voltage);
      // Serial.print("\n");

      // temperature conversion
      temperature = voltage / (voltage_conversion_value);

      // prints the current temperature value
      Serial.print("Current temp:");
      Serial.println(temperature);

      // send temperature data to Matlab via Serial Port
      matlab_send(temperature);
    }

    /********************************************* END SENSOR ROUTINE *********************************************/



    
}












/********************************************* Functions *********************************************/

// this function take a float and write it into the Serial Port. Then, Matlab will be reading Serial Port, to take this data
void matlab_send(float data ) {

  // initialize the wrinting in Serial Port using a random string, to avoid errors
  Serial.write("abcd");

  // cast the x1 address to a byte ptr. This allow to handle x1 data as bytes series 
  byte * b = (byte *)& data;

  // using variable b, the data will be sended into the serial port, in a series of 4 bytes data, due to float size
  Serial.write(b, 4);
}

