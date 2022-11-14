/* Basic usage example for EmbAJAX library:
   Provide a web interface to set built-in LED on, off, or blinking.

   Note that ESP boards seems to be no real standard on which pin the builtin LED is on, and
   there is a very real chance that LED_BUILTIN is not defined, correctly, for your board.
   If you see no blinking, try changing the LEDPIN define (with an externally connected LED
   on a known pin being the safest option). Similarly, on and off states are sometimes reversed.

   This example code is in the public domain (CONTRARY TO THE LIBRARY ITSELF). */

#include <EmbAJAX.h>

#define LEDCONNECTIONPIN LED_BUILTIN
#define LED_BRIGHTNESS_ON  LOW
#define LED_BRIGHTNESS_OFF HIGH
long last_activity_message;

// Set up web server, and register it with EmbAJAX. Note: EmbAJAXOutputDriverWebServerClass is a
// convenience #define to allow using the same example code across platforms
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

// Define the main elements of interest as variables, so we can access to them later in our sketch.
EmbAJAXSlider slider("slider", 0, 1000, 100);   // slider, from 0 to 1000, initial value 100

// Define a page (named "page") with our elements of interest, above, interspersed by some uninteresting
// static HTML. Note: MAKE_EmbAJAXPage is just a convenience macro around the EmbAJAXPage<>-class.
MAKE_EmbAJAXPage(page, "EmbAJAX example - Connection events", "",
                 new EmbAJAXStatic("<h1>Control the slider and see the messages on the builtin LED</h1><p>"),
                 new EmbAJAXStatic("</p><p>Slider: "),
                 &slider
                )

void setup() {
  // Example WIFI setup as an access point. Change this to whatever suits you, best.
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig (IPAddress (192, 168, 4, 1), IPAddress (0, 0, 0, 0), IPAddress (255, 255, 255, 0));
  WiFi.softAP("EmbAJAXTest", "12345678");

  // Tell the server to serve our EmbAJAX test page on root
  // installPage() abstracts over the (trivial but not uniform) WebServer-specific instructions to do so
  driver.installPage(&page, "/", updateUI, onConnectionEvent);
  server.begin();

  // pinMode(LEDPIN, OUTPUT);

  pinMode(LEDCONNECTIONPIN, OUTPUT);
  
  last_activity_message = millis();
}

void updateUI() {
  // Here you can add code to handle the slider events
}

void onConnectionEvent(EmbAjaxConnectionEventType event) {
  switch (event)
  {
    case EmbAjaxConnectionEventConnected:
      digitalWrite(LEDCONNECTIONPIN, LED_BRIGHTNESS_OFF);
      break;

    case EmbAjaxConnectionEventDisconnected:
      break;

    case EmbAjaxConnectionEventMessage:
      digitalWrite(LEDCONNECTIONPIN, LED_BRIGHTNESS_ON);
      last_activity_message = millis();
      break;

    default:
      break;
  }
}


void loop() {
  if (millis() > last_activity_message + 1)
  {
    digitalWrite(LEDCONNECTIONPIN, LED_BRIGHTNESS_OFF);
  }

  // handle network. loopHook() simply calls server.handleClient(), in most but not all server implementations.
  driver.loopHook();

  if (!driver.getConnected()) {
    digitalWrite(LEDCONNECTIONPIN, (millis() % 1000) > 500 ? LED_BRIGHTNESS_ON : LED_BRIGHTNESS_OFF);
  }

}
