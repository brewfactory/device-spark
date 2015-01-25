// https://github.com/br3ttb/Arduino-PID-Library
#include "PID.h"
#include "PhaseRunner.h"

// https://github.com/krvarma/Dallas_DS18B20_SparkCore
#include "DS18B20.h"
#include "OneWire.h"

PhaseRunner *phaseRunner = new PhaseRunner();

double IDLE_POINT_TEMPERATURE = 5.0;
int EVENT_FREQUENCY = 1000;
int CALCULATE_FREQUENCY = 1000;

// Input pin where DS18B20 is connected
int ds18b20Pin = D1;

// Output pin where relay is connected
int relayPin = A6;

// SparkCloud variable outputs
char JSONStatus[64];

// Temperature device on the D1 pin
DS18B20 ds18b20 = DS18B20(ds18b20Pin);

// PID: variables
double pointTemperature, actualTemperature, pidOutput;

// PID: Specify the links and initial tuning parameters
PID myPID(&actualTemperature, &pidOutput, &pointTemperature, 25, 1000, 9, DIRECT);

// Timestamp of the last status events
unsigned long lastEventTimestamp, lastCalculateTimestamp;

 // Setup
void setup() {

    // Start serial on USB
    Serial.begin(9600);

    // SparkCloud: functions
    Spark.function("setSchedule", setSchedule);
    Spark.function("resetSchedule", resetSchedule);

    // Set relay pin mode to output
    pinMode(relayPin, OUTPUT);

    // Point temperature default
    pointTemperature = IDLE_POINT_TEMPERATURE;

    //turn the PID on
    myPID.SetMode(AUTOMATIC);

    // Last event timestamp
    lastEventTimestamp = 0;

    // Last runner timestamp
    lastCalculateTimestamp = 0;
}

 // Loop
void loop() {
    int timeEventDiff = millis() - lastEventTimestamp;
    int timeCalculateDiff = millis() - lastCalculateTimestamp;

    // Searching for the ds18b20 device
    if(actualTemperature == 0 && !ds18b20.search()) {

        // Log to the serial
        Serial.println("No more addresses.");
        Serial.println();

        // Turn off the relay during the scan
        digitalWrite(relayPin, LOW);

        // Restart search
        ds18b20.resetsearch();
        delay(250);

        return;
    }

    // Point temperature
    if (timeCalculateDiff >= CALCULATE_FREQUENCY) {

      // Actual temperature in celsius
      actualTemperature = ds18b20.getTemperature();

      // Compute PID
      if(myPID.Compute()) {

          // Control relay
          analogWrite(relayPin, pidOutput);
      }

      // Step runner
      phaseRunner->onTimeElapsed(timeCalculateDiff);

      if(phaseRunner->getActualPhase()) {
        pointTemperature = phaseRunner->getActualPhase()->getTemperature();
      } else {
        pointTemperature = IDLE_POINT_TEMPERATURE;
      }

      // Save timestamp of the event
      lastCalculateTimestamp = millis();
    }

    // Send event to the SparkCloud
    if (timeEventDiff >= EVENT_FREQUENCY) {

        // Convert status info to JSON
        sprintf(JSONStatus, "{ temp: %2.2f, pointTemp: %2.2f, pwm: %2.2f }",
          actualTemperature, pointTemperature, pidOutput);

        // Log to the serial
        Serial.println(JSONStatus);

        // SparkCore: publish
        Spark.publish("status", JSONStatus);

        // Save timestamp of the event
        lastEventTimestamp = millis();
    }
}

/*
 * Set schedule
 */
int setSchedule(String schedule)
{
    Serial.println("setSchedule called");
    Serial.println(schedule);

    phaseRunner.setSchedule(schedule);

    return 1;
}

/*
 * Reset schedule
 */
int resetSchedule()
{
    Serial.println("resetSchedule called");

    phaseRunner.resetSchedule();

    return 1;
}
