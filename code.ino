#include "bsec.h"
#include <U8g2lib.h>
#include <U8x8lib.h>
U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 34);
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;

// Entry point for the example
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();

  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.setColorIndex(1);  

  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);
}

// Function that is looped forever
void loop(void)
{
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
     u8g2.firstPage();
  do {
    u8g2.drawFrame(0,0,128,31);         
    u8g2.drawFrame(0,33,128,31);           
    u8g2.drawStr( 1, 13, "T:");
    u8g2.setCursor(14,13);
    u8g2.print(iaqSensor.temperature,2);
    u8g2.drawUTF8(50, 13, DEGREE_SYMBOL);
    u8g2.drawUTF8(54, 13, "C");

    u8g2.drawStr( 65, 13, "H:");
    u8g2.setCursor(80,13);
    u8g2.print(iaqSensor.humidity,2); 
    u8g2.drawUTF8(115, 13, "%");

    u8g2.drawStr(1,28, "CO2:");        
    u8g2.setCursor(41,28);
    u8g2.print(iaqSensor.co2Equivalent,0);
    u8g2.setCursor(96,26);
    u8g2.print("ppm");
    u8g2.setCursor(1,46);
    u8g2.print("VBOC:"); 
    u8g2.setCursor(51,46);
    u8g2.print(iaqSensor.breathVocEquivalent*1000,0);
    u8g2.setCursor(100,46);
    u8g2.print("ppb");   

    u8g2.setCursor(1,61);
    u8g2.print("IAQ:");      
    u8g2.setCursor(41,61);
    u8g2.print(iaqSensor.staticIaq,0);        
    u8g2.setCursor(75,61);
    u8g2.print("Acu:");      
    u8g2.setCursor(108,61);
    u8g2.print(iaqSensor.iaqAccuracy);        
    } while( u8g2.nextPage() );
    delay(100);
    
//    output = String(time_trigger);
      output = String("Temperatura = ");
      output += String(iaqSensor.temperature);
      output += String(" ºC | Humedad = ");
      output += String(iaqSensor.humidity);
      output += String(" %H | Presion = ");
//    output += String(iaqSensor.rawTemperature);
      output += String(iaqSensor.pressure);
      output += String(" Pa | IAQ = ");
//    output += ", " + String(iaqSensor.rawHumidity);
//    output += ", " + String(iaqSensor.gasResistance);
//    output += ", " + String(iaqSensor.iaq);
//    output += ", " + String(iaqSensor.iaqAccuracy);
      output += String(iaqSensor.staticIaq);
      output += String(" | CO2 = ");
      output += String(iaqSensor.co2Equivalent);
      output += String(" ppm | BVOC = ");
      output += String(iaqSensor.breathVocEquivalent);
      output += String(" ppb ");
      output += "iaq accuracy" + String(iaqSensor.iaqAccuracy);
    Serial.println(output);
  } else {
    checkIaqSensorStatus();
  }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
