/*
  SendATweet

  Demonstrates sending a tweet via a Twitter account using the Temboo Arduino Yun SDK.
  
  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include "Account.h" // contains Temboo account information

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

int numRuns = 1;   // execution count, so this sketch doesn't run forever
int maxRuns = 100;  // the max number of times the Twitter Update Choreo should run

// this constant won't change.  It's the pin number
// of the sensor's output:
const int pingPin = 7;
int initDistance = 0;

void setup() {
  Serial.begin(9600);

  // for debugging, wait until a serial console is connected
  delay(4000);
  while(!Serial);

  Bridge.begin();
}

void loop()
{
  int currentDistance = 0;
  
  if (initDistance == 0)
  {
     initDistance = getDistance();
  }

  // Check who there is
  currentDistance = getDistance();
  Serial.println("Now (" + String(currentDistance) + " cm), Initial (" + String(initDistance) + " cm)");
   if ( currentDistance == 0 || abs(currentDistance - initDistance) < 10 )
  {
    delay(1000);
    return;
  }
   
  // only try to send the tweet if we haven't already sent it successfully
  if (numRuns <= maxRuns) {

    Serial.println("Running SendATweet - Run #" + String(numRuns++) + "...");
  
    // define the text of the tweet we want to send
    //String tweetText("My Arduino Yun has been running for " + String(millis()) + " milliseconds.");
    String tweetText("Sol > Warning, Now (" + String(currentDistance) + " cm), Initial (" + String(initDistance) + " cm)");

    
    TembooChoreo StatusesUpdateChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();
    
    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/ 
    // for complete details about the inputs for this Choreo
 
    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_API_KEY);    
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);

    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);

    // tell the Process to run and wait for the results. The 
    // return code (returnCode) will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();

    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
        Serial.println("Success! Tweet sent!");
    } else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
        char c = StatusesUpdateChoreo.read();
        Serial.print(c);
      }
    } 
    StatusesUpdateChoreo.close();

    // do nothing for the next 90 seconds
    Serial.println("Waiting...");
    delay(5000);
  }
}

int getDistance()
{
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);

  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
