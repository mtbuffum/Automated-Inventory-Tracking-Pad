
#include "HX711.h"
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>

#include <WiFi.h>

#include <ESP_Mail_Client.h>
#include <algorithm>
#include <Preferences.h>


#define DOUT 22
#define CLK  21

#define PROJECT_ID "iot-scale-tracker"
#define CLIENT_EMAIL "iot-scale-datalogging@iot-scale-tracker.iam.gserviceaccount.com"
// Email Credentials
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "esp9322@gmail.com"
#define AUTHOR_PASSWORD "pmzr wyim mxuw emif"

// Recipient's email
#define RECIPIENT_EMAIL "mtbuffum9@gmail.com"

// Declare the global used SMTPSession object for SMTP transport
//SMTPSession smtp;

//Table of Contents
float getAdaptiveWeight();
float weigh();
float Calibration();
float smoothWeight(float rawWeight, float alpha);
bool CheckWeight(float filteredWeight, float OneItem);
void setup();
void AppendGsheets(String range, float data);
void saveWeightToFlash(); 
// Callback function to get the Email sending status
//void smtpCallback(SMTP_Status status);
//void sendEmail(const char* subject, const char* message);

// Define global variables
const int buttonPin = 13;  // GPIO pin for button
bool buttonHeld = false;
unsigned long buttonPressTime = 0;  // Track button press duration

unsigned long previousMillis = 0;  // Store last recorded time
const long interval = 60000;  // 10 seconds interval for weight check

float OneItem = 0;  // Store calibrated weight
const int redPin = 15;
const int greenPin = 12;
const int bluePin = 4;

Preferences preferences;

int buttonState;
int lastButtonState = LOW;
unsigned long lastPressTime = 0;
unsigned long debounceDelay = 50;   // Debounce delay
unsigned long doubleClickDelay = 1000; // Max time between clicks to be considered a double-click
int clickCount = 0;

const char PRIVATE_KEY[] = R"KEY(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCnvB8qEplgSV6c
Vq1yVOfKhuOwbB1cPFwpyWBntDJMxJwo+FGrTBfyJagKtNxWbTO4YVAsXg7Tve5h
XkhOgONzGp6A8//UhweZfOyfPyeuRonmmCbS/2bgM3t6jdxgLorgIdfhVhajBpwc
sqYP+s9erW0O4xUgRmTVf+KKDxN/2TvnEc8BzjAkiAduiym/A4ynr5gBKXSFbm7s
soYeYWCtRhDctGWaOTiW4UbHhEgEnOk/FV2EyuAEJ67y6i/sKEyMmdRUrPDEcfuB
I/r1FQUHUZQV/JurnDVOuff7iTUnXuziRFpl7qJer5X8S8omUbxP3tE40LS9v7Jy
hHAbT4NLAgMBAAECggEAAxc5g8iMwI3Ziz8ibXIYq05JyvbTCHlCp+QPPYBZuB+c
H20j8OiACg6dLN7t4gtoSCqselGcke5BnDp+hP1lZrn3Pbna3VgRN2GdgpNJVrHz
piboYDu9WFhA6DgA9yZXmY9GFd+rBYOaEM7oT6PeRyJVWVB54GErrntvkzFGUxuH
/MckWKNebn8yM/QPavxjPwweqTTbP/E4WmjhZqKIlC+q0rXo8GXwBPtoYhKx3yf3
m8k6vYi8DKimWilyaDtNtMb+aHoSmCtxeGfukfGmRvPmXJYFaLrzAIJHUOew7ch0
XUDDtKL2JXEmXMXGV0buLo/TyWGbvWhEYxxFRa0xAQKBgQDT0o3JW/gb1tYDsOrx
p2CA6uv5Est3ggYu+AGNQK6fIq5z0gQXRDzvqWVlq8qfSlV6WCKnyh172czmY8v4
O+ALfDLhEkQw3vzNKxC1LwXXuhy9CQ1YIz0i1N2NGZ1RsL2FrggRx8nquRM9BWcD
tsbHFFVu6KUxafUXkJW24HF8wQKBgQDKt63CBudSEEwGom9syJAr/rNRJWrdjd1g
LA5ffMo93qhxi8NKM0mdh20BRXiMGR2NJFUhrr9fHxkbO6LjTLU7BsGvcyZhouJ5
lR7v5r59IwZupeko1Qk69DR5IbqoN6w50LMiarNDeap3FWQoUnFLaI/BMsoDlBDG
biGFLR3nCwKBgQCv9ldGkJn5NmyuwEdL28k1DvJcBJr1ehP87JUNbFAjbkIRgMNZ
OJyOUuAip6U40h1T9udDD4mSmwm10e1dFxA1OkVh5zSyjGrhXQcSHC/0V20MM/rb
3tool0A8uKyRcgrehAW8D4XS8U/j/xrmYO2b725RJf4koGqz2q4kLyrXgQKBgQCo
1CLFRiSfBu2aVOwBqGPSEDTdbxE6Za0qLGboBbEVp/uXXTQO9vnnVWNmPKpx23Tu
GIVCIRxGp0qskJVPQpJpD259paQgipydb6EtYXniFCqGOm0d0Zb3X2zYOUwQTxWD
rRmTn1Oe7ZQXy1v37PloOTQITjlKk6UI7a3DEVknEwKBgFTMLUIY6Tp/bS4uWYn/
vZpDhUFWVySOq5TduJd5FTBmYfqe/+a1VYACOCHRFPKB1znZOEBn5CKj7uDk4miD
ZNp/KkVGisgRisloADAeIOM01Exu9luYzQs8S4M8k69DGNoZu7zCKc3V3uBbwJkU
+cKWBQDSSNA911L+0ljeNNiE
-----END PRIVATE KEY-----
)KEY";

HX711 scale; // Create HX711 scale object
const char spreadSheetID[] = "1a_EaMT9_jKE_hE3MMUWwmqx9KbIUirzaJi1VtkIdjog";

SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

// Calibration Factor (adjust as needed)
float calibration_factor = 103.6/2;
float totalWeight = 0;


// WiFi Connection
void ConnectToWIFI() {
    const char* ssid = "Jack's iPhone";
    const char* password = "11/05/05";

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {  
        delay(1000);
        setColor(0,0,255);
        Serial.print(".");
        attempts++;
        delay(1000);
        setColor(0,0,0);
        
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi Connection Failed! Restarting...");
        ESP.restart();
    }
    
}

// Google Sheets Authentication
void tokenStatusCallback(TokenInfo info) {
    GSheet.printf("Token info: type = %s, status = %s\n", 
        GSheet.getTokenType(info).c_str(), 
        GSheet.getTokenStatus(info).c_str());
        
    if (info.status == token_status_error) {
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
}

// Append data to Google Sheets
void AppendGsheets(float oneItem, float data) {
    FirebaseJson valueRange, response;
    valueRange.add("majorDimension", "COLUMNS");
    valueRange.set("values/[0]/[0]", String(oneItem));
    valueRange.set("values/[1]/[0]", String(data)); 

    if (GSheet.values.update(&response, spreadSheetID, "Sheet1!A3:B3", &valueRange)) {
        response.toString(Serial, true);
    } else {
        Serial.println("Error appending data: " + GSheet.errorReason());
    }

    valueRange.clear();
    response.clear();
}

String ReadFromGsheets(String range) {
    FirebaseJson response;

    if (GSheet.values.get(&response, spreadSheetID, range)) {
        Serial.println("Data retrieved successfully!");
        response.toString(Serial, true);  // Print full JSON response
        
        // Extract the value
        FirebaseJsonData jsonData;
        response.get(jsonData, "values/[0]/[0]");  // Read first value

        if (jsonData.success) {
            Serial.print("Value from Google Sheets (" + range + "): ");
            Serial.println(jsonData.stringValue);  // Print value
            return jsonData.stringValue;  // Return the value
        } else {
            Serial.println("Failed to parse value.");
            return "";  // Return empty string if parsing fails
        }
    } else {
        Serial.println("Error retrieving data: " + GSheet.errorReason());
        return "";  // Return empty string if retrieval fails
    }
}

//
void sendEmail(String Subject, String text) {
  MailClient.networkReconnect(true);

  /** Enable the debug via Serial port
   * 0 for no debugging
   * 1 for basic level debugging
   *
   * Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
   */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the Session_Config for user defined session credentials */
  Session_Config config;

  /* Set the session config */
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  /*
  Set the NTP config time
  For times east of the Prime Meridian use 0-12
  For times west of the Prime Meridian add 12 to the offset.
  Ex. American/Denver GMT would be -6. 6 + 12 = 18
  See https://en.wikipedia.org/wiki/Time_zone for a list of the GMT/UTC timezone offsets
  */
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = Subject;
  message.addRecipient(F("Sara"), RECIPIENT_EMAIL);
    
  /*Send HTML message*/
  /*String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;*/

   
  //Send raw text message
  String textMsg = text;
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;


  /* Connect to the server */
  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());

}

//}
//
//
//// Callback function to get the Email sending status
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

// Adaptive Weight Measurement
float getAdaptiveWeight() {
    static float lastWeight = 0;
    float newWeight = 0;
    int numReadings = 5;  // Multiple readings for accuracy

    for (int i = 0; i < numReadings; i++) {
        newWeight += abs(scale.get_units());
    }
    newWeight /= numReadings;

    // Dynamic threshold: 5% of the lastWeight (or at least 0.5g)
    float threshold = max(0.05 * lastWeight, 0.5);

    if (abs(newWeight - lastWeight) > threshold) {  
        lastWeight = newWeight;
    }

    return lastWeight;
}



void setColor(int red, int green, int blue) {
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
}

float Calibration() {
    Serial.println("Place one item on scale...");
    scale.tare();
    for(int i = 0; i < 3; i++){
    setColor(125,0,125);
    delay(200);
    setColor(0, 0, 0);  // Turn off momentarily
    delay(200);
    }
    // Start with RED indicator
    setColor(255, 0, 0);  // Red (Start Calibration)

    const int numSamples = 20;  // More samples for accuracy
    float weightReadings[numSamples];
    float sumWeights = 0;
    float lastStableWeight = 0;
delay(3000);
    for (int i = 0; i < numSamples; i++) {
        delay(1000);  // Allow scale to stabilize

        // Switch to yellow at halfway point
        if (i == numSamples / 2) setColor(255, 255, 0); // Yellow (Midway)

        // Reduce power drift while ensuring reliability
        if (i % 5 == 0) {  
            scale.power_down();
            delay(5);
            scale.power_up();
        }

        float rawWeight = getAdaptiveWeight();
        float filteredWeight = smoothWeight(rawWeight);

        if (filteredWeight < 1.0) filteredWeight = 0;  // Remove ghost weights

        weightReadings[i] = filteredWeight;
        sumWeights += filteredWeight;

        Serial.print("Reading ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(filteredWeight, 2);
        Serial.println(" g");

        // Small LED blink for indication
        setColor(0, 0, 0);  // Turn off momentarily
        delay(200);
        if (i < numSamples / 2) setColor(255, 0, 0); // Red
        else setColor(255, 255, 0); // Yellow
    }

    // Compute the median to filter out any noise or outliers
    std::sort(weightReadings, weightReadings + numSamples);
    lastStableWeight = weightReadings[numSamples / 2];

    // Set color to GREEN to indicate completion
    setColor(0, 255, 0); // Green (Done)
    Serial.println("Calibration done!");
    Serial.print("Calibrated Weight: ");
    Serial.println(lastStableWeight, 2);

    totalWeight = lastStableWeight;
    return lastStableWeight;
}

// Smooth Weight Function
float smoothWeight(float newReading) {
    static float smoothedWeight = 0;
    float alpha = 0.8;  // Default smoothing factor

    if (abs(newReading - smoothedWeight) > 5) {  // Detect large jumps
        alpha = 0.95;  // Adapt faster for large changes
    } else if (abs(newReading - smoothedWeight) < 1) {
        alpha = 0.6;  // Smoother for tiny changes
    }

    smoothedWeight = alpha * newReading + (1 - alpha) * smoothedWeight;
    return smoothedWeight;
}




// Weight Checking Function
bool CheckWeight(float filteredWeight, float OneItem) {
  float tolerance = OneItem *0.3;
  if(abs(filteredWeight) > (abs(totalWeight + OneItem)-tolerance)){
    return true;
  }
  else if(abs(filteredWeight) < (abs(totalWeight - OneItem)+tolerance)){
    return true;
  }
  else{
    return false;
  }
    
}

// Weighing Function
float weigh() {
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();

    if (now - lastUpdate < 50) {  // Check every 50ms for quick updates
        return -1;  // Skip unnecessary calls
    }
    lastUpdate = now;

    float rawWeight = getAdaptiveWeight();
    float filteredWeight = smoothWeight(rawWeight);
    
    if (filteredWeight < 1.0) {  // Remove ghost weight
        filteredWeight = 0;
    }

    Serial.print("Weight: ");
    Serial.print(filteredWeight, 2);
    Serial.println(" g");

    return filteredWeight;
}
float weight = 0.0;
float lastSavedWeight = 0.0;  // Track the last saved value

// Setup Function
void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
    Serial.begin(115200);
    scale.begin(DOUT, CLK);
    Serial.println("Initializing...");
    
    delay(2000);
    scale.tare();
    scale.set_scale(calibration_factor);

    ConnectToWIFI();

    pinMode(buttonPin, INPUT_PULLUP);  

    GSheet.setTokenCallback(tokenStatusCallback);
    GSheet.setPrerefreshSeconds(10 * 60);
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

    Serial.println("Google Sheets Connected!");

    preferences.begin("my-app", true); // 'true' means read-only mode

    // Retrieve the last stored float value or default to 0.0 if no value is found
    weight = preferences.getFloat("weight", 0.0);
  
    Serial.print("Restored float value: ");
    Serial.println(weight);
  
    preferences.end();
}



// Main Loop
bool EmailSent = false;
bool firstRun = true;

void loop() {
    bool doublePressed = false;
    if(WiFi.status()!= WL_CONNECTED){
      ConnectToWIFI();
    }
    
    bool ready = GSheet.ready();
    unsigned long currentMillis = millis();
    int reading = digitalRead(buttonPin);
    

    // Detect button press
    if (reading == LOW && lastButtonState == HIGH) {
        if (currentMillis - lastPressTime > debounceDelay) {
            clickCount++;
            lastPressTime = currentMillis;
        }
    }

    // Check for double-click within the allowed time frame
    if (clickCount == 2 && (currentMillis - lastPressTime <= doubleClickDelay)) {
        doublePressed = true;
        Serial.println("Double Click Detected! Uploading Now");
        clickCount = 0;  // Reset after successful detection
    }

    // Reset click count if time exceeds double-click delay
    if (currentMillis - lastPressTime > doubleClickDelay) {
        clickCount = 0;
    }

    lastButtonState = reading;

    // Button Handling for Calibration (Hold for 2 seconds)
    if (digitalRead(buttonPin) == LOW) {  
        if (!buttonHeld) {
            buttonPressTime = currentMillis;
            buttonHeld = true;
        }
        if (currentMillis - buttonPressTime >= 3000) {
            Serial.println("Button held for 3 seconds, starting calibration...");
            OneItem = Calibration();  
            Serial.println("Calibration complete!");
            float OWeight = weigh();
            AppendGsheets(OneItem, OWeight); 
            buttonHeld = false;
            delay(2000);  
        }
    } 
    else {  
        buttonHeld = false;  
    }
    

    // Every 10 seconds, check weight and update Google Sheets if changed
    if((currentMillis - previousMillis >= interval)||doublePressed||firstRun){
        previousMillis = currentMillis;  
        if(firstRun == true){
          setColor(125,0,125);
         }
        weight = weigh();  
        Serial.print("Current Weight: ");
        Serial.println(weight);
        
        //***NEED TO ADD A DOUBLE CHECK TO MAKE SURE WEIGHT CHANGED***
        
        if (CheckWeight(weight, OneItem)){
          if(OneItem == 0){
            Serial.print("Waiting Calibration");
              
          }
          else{
            delay(1000);
            weight = weigh();
            if(CheckWeight(weight,OneItem)){
              AppendGsheets(OneItem, weight);  
              totalWeight = weight;
              Serial.println("Weight change detected, updating Google Sheets.");
              saveWeightToFlash();  // Save the current state to flash memory
              lastSavedWeight = weight;
            }
          }
        }
    
        String LowValue = ReadFromGsheets("Sheet1!D3");
        String HighValue = ReadFromGsheets("Sheet1!F3");
        String Inventory = ReadFromGsheets("Sheet1!G3");
        String Item = ReadFromGsheets("Sheet1!C3");
        String Link = ReadFromGsheets("Sheet1!H3");

        if(OneItem != 0){
          if(LowValue.endsWith("%") && HighValue.endsWith("%")){
            LowValue.remove(LowValue.length() - 1);
            HighValue.remove(HighValue.length() - 1);
            int percentageWeight = (weight/OneItem)*100;
            if(percentageWeight >= (HighValue.toInt()-5)){
              setColor(0,255,0);
                EmailSent = false;
            }
            else if(percentageWeight <= (LowValue.toInt()+5)){
                
                setColor(255, 0, 0);
                String Message = "Hi, Your Current inventory of " + Item + " Is low you probably need to order more!\n REMEMBER WITH TAKING A PERCENTAGE OF AN ITEM CALCULATIONS ARE LESS ACCURATE\n\nHere is the link to do so:\n" + Link;
                // send email
                const char* subject = "LOW INVENTORY";
                if(!EmailSent){
                sendEmail(subject ,Message);
                EmailSent = true;
                }
                 
              }
              else{
                setColor(255, 255, 0);
                EmailSent = false;
              }
            
          }
          else{
            
             if(Inventory.toInt() >= HighValue.toInt()){
                setColor(0,255,0);
                EmailSent = false;
              }
              else if(Inventory.toInt() <= LowValue.toInt()){
                setColor(255, 0, 0);
                String Message = "Hi, Your Current inventory of " + Item + " Is low you probably need to order more!\n Here is the link to do so:\n" + Link;
                // send email
                const char* subject = "LOW INVENTORY";
                if(!EmailSent){
                sendEmail(subject ,Message);
                EmailSent = true;
                }
                
              }
              else{
                setColor(255, 255, 0);
                EmailSent = false;
              }
            }
        }
        firstRun = false;
    }
    
}




void saveWeightToFlash() {
  preferences.begin("my-app", false);  // 'false' means read/write mode

  // Store the current value of the weight in flash memory (this will replace the old value)
  preferences.putFloat("weight", weight);

  Serial.println("Weight value saved to flash memory.");

  preferences.end();  // Close Preferences after writing
}