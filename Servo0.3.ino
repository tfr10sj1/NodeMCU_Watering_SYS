#include <Servo.h>
#include <ESP8266WiFi.h>;
#include <ESP8266HTTPClient.h>;
#include <ESP8266WebServer.h>

String oldYear = "";
String oldMonth = "";
String oldDay = "";
String oldHour = "";
String oldMinute = "";

String Hour = "06";
String Minute = "30";

String Direction = "v";
String Delayv = "1500";
String Delayh = "1500";
String Countlimit = "7";
String sequence = "vhvhvhvhvhvhvh";
String nbrpertime = "2";
String nexttime = "6";
String savedHour = "";
int currentsequenselement = 0;
ESP8266WebServer server(80); // Create a webserver object listens HTTP request on port 80

const char* ssid = "ComHemF17542";
const char* password = "ytkwsqqu";

Servo servo;
int count = 0;
int tag = 0;
int ENA = 4;
int IN1 = 0;
void watering(void)
{
   if(tag == 0)
  {
    count ++;
    if(count <= Countlimit.toInt())
    {
      servo.attach(5);
      servo.write(-180);
      delay(1000);
      servo.detach();
      digitalWrite(IN1, HIGH);
      digitalWrite(ENA, HIGH);
      delay(Delayh.toInt());
      analogWrite(ENA, 0);
      delay(3000);
    }
    else
    {
      servo.attach(5);
      tag = 1;
      servo.write(180);
      delay(1000);
      count = 0;
      tag = 0;
      servo.detach();
      digitalWrite(IN1, HIGH);
      digitalWrite(ENA, HIGH);
      delay(Delayv.toInt());
      analogWrite(ENA, 0);
      delay(3000);
    }
  }
}
void setup() {
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
    // set all the motor control pins to outputs
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.println("Connecting..");
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP
  
    server.on("/", HTTP_GET, handleRoot);        // Call 'handleRoot' function
    server.on("/login", HTTP_POST, handleLogin); // Call 'handleLogin' function when a POST request made to "/login"
    server.onNotFound(handleNotFound);           // call function "handleNotFound" when unknown URI requested
  
    server.begin();                            // start the server
    //Serial.println("HTTP server started");
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin("http://worldclockapi.com/api/jsonp/cet/now?callback=mycallback");  //Specify request destination
    int httpCode = http.GET();  //Send the request
    if (httpCode > 0) //Check the returning code
    { 
      String payload = http.getString();   //Get the request response payload
      for(int i = 41; i< 60; i++)
      { 
        if(i > 40 && i < 45)
          {
            oldYear = oldYear + payload[i];
          }
        if(i > 45 && i < 48 )
          {
            oldMonth  = oldMonth + payload[i];
          }
        if(i > 48 && i < 51 )
          {
            oldDay  = oldDay + payload[i];
          }
        if(i > 51 && i < 54)
          {
            oldHour = oldHour + payload[i];
          }
        if (i> 54 && i < 57)
          {
            oldMinute = oldMinute + payload[i];
          } 
       }
    }
    http.end();   //Close connection
}

void loop() 
{
  server.handleClient();                     // Listen for HTTP requests
  String currentYear = "";
  String currentMonth = "";
  String currentDay = "";
  String currentHour = "";
  String currentMinute = "";
  if (WiFi.status() == WL_CONNECTED) 
  { //Check WiFi connection status
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin("http://worldclockapi.com/api/jsonp/cet/now?callback=mycallback");  //Specify request destination
    int httpCode = http.GET();  //Send the request
    if (httpCode > 0) //Check the returning code
    { 
      String payload = http.getString();   //Get the request response payload
      for(int i = 41; i< 60; i++)
      { 
        if(i > 40 && i < 45)
          {
            currentYear = currentYear + payload[i];
          }
        if(i > 45 && i < 48 )
          {
            currentMonth  = currentMonth + payload[i];
          }
        if(i > 48 && i < 51 )
          {
            currentDay  = currentDay + payload[i];
          }
        if(i > 51 && i < 54)
          {
            currentHour = currentHour + payload[i];
          }
        if (i> 54 && i < 57)
          {
            currentMinute = currentMinute + payload[i];
          } 
       }
       if(currentYear != oldYear){
          oldYear = currentYear;
       }
       if(currentMonth != oldMonth){
          oldMonth = currentMonth;
       }
        if(currentDay != oldDay){
          oldDay = currentDay;
        }
        if(currentHour != oldHour){
          oldHour = currentHour;
        }
        if(currentMinute != oldMinute){
          oldMinute = currentMinute;
        }
    }
    http.end();   //Close connection
  }
  
  if(currentHour == Hour && currentMinute == Minute)
  {
    watering();
    servo.detach();
    delay(60000);    //Send a request every 30 seconds
  }
}
void handleRoot() 
{                          // When URI / is requested, make login Webpage
  server.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"uname\" placeholder=\"Username\"></br><input type=\"password\" name=\"pass\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form>");
}

void handleLogin() 
{                         //Handle POST Request
  if( ! server.hasArg("uname") || ! server.hasArg("pass") || server.arg("uname") == NULL || server.arg("pass") == NULL) // Request without data
  { 
    server.send(400, "text/plain", "400: Invalid Request");// Print Data on screen
    return;
  }
  if(server.arg("uname") == "Sam_simsim" && server.arg("pass") == "tfr10sj1") 
  { // If username and the password are correct
    handleserver();
  } 
  else { // Username and password don't match
    server.send(401, "text/plain", "401: Invalid Credentials");
  }
}
void handleNotFound(){
  if(server.arg("action")== "v" || server.arg("action")== "V")
    {
      Direction = "v";
      servo.attach(5);
      servo.write(180);
      delay(1500);
      servo.detach();
      
    }
    if(server.arg("action")== "h" || server.arg("action")== "H")
    {
      Direction = "h";
      servo.attach(5);
      servo.write(-180);
      delay(1500);
      servo.detach();
    }
    if(server.arg("action") == "0" && Direction == "v")
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(ENA, HIGH);
      delay(Delayv.toInt());
      analogWrite(ENA, 0);
      delay(3000);
    }
    else if(server.arg("action") == "0" && Direction == "h")
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(ENA, HIGH);
      delay(Delayh.toInt());
      analogWrite(ENA, 0);
      delay(3000);
    }
     if(!server.hasArg("AutomaticWatering")&& server.arg("hour") != "" && server.arg("minute")!= "")
    {
       Hour = server.arg("hour");
       Minute = server.arg("minute");
    }
      if(!server.hasArg("Counter") && server.arg("counter")!= "" && server.arg("Delayv")!= "" && server.arg("Delayh")!= "")
      {
        Countlimit = server.arg("counter");
        Delayv = server.arg("Delayv");
        Delayh = server.arg("Delayh");
      }
      else if(!server.hasArg("Counter") && server.arg("counter")!= "")
      {
        Countlimit = server.arg("counter");
      }
       else if(!server.hasArg("Counter") && server.arg("Delayv")!= "")
      {
        Delayv = server.arg("Delayv");
      }
        else if(!server.hasArg("Counter") && server.arg("Delayh")!= "")
      {
        Delayh = server.arg("Delayh");
      }
      handleserver();
      Program(sequence, nbrpertime, nexttime);
     }
void handleAction()
{
  server.send(200, "text/html", "<h1>Action = " +server.arg("action")+"</h1>");
}
void handleserver()
{
  server.send(200, "text/html", "<h1><form action=\"/Action\" method=\"POST\"><input type=\"text\" name=\"action\" placeholder=\"Action\"><br><input type=\"submit\" value=\"Send Command\"></form></h1><p>Date: "+ oldYear +"-"+ oldMonth + "-" + oldDay + " Time: " + oldHour + ":" + oldMinute+ "</p><h1><form action=\"/AutomaticWatering\" method=\"POST\"><input type=\"text\" name=\"hour\" placeholder=\"Which hour?\"></br><input type=\"text\" name=\"minute\" placeholder=\"Which minute?\"></br><input type=\"submit\" value=\"Submit\"></form></h1><p>Watering Time is: "+Hour+ ":"+Minute+"</p><h1><form action=\"/Counter\" method=\"POST\"><input type=\"text\" name=\"counter\" placeholder=\"Add count_limit 1 - 7 \"><input type=\"text\" name=\"Delayv\" placeholder=\"VDir_Defualt = 1500 ms \"><input type=\"text\" name=\"Delayh\" placeholder=\"HDir_Defualt = 1500 ms \"><br><input type=\"submit\" value=\"Send Command\"></form></h1><p>Counter is: "+count+"</p><p>CounterLimit is: "+Countlimit+"</p><p>Delayv is: "+Delayv+"</p><p>Delayh is: "+Delayh+"</p><p><form action=\"/Program\" method=\"POST\"><input type=\"text\" name=\"program\" placeholder=\"Program e.g. vhvhvhvh\"><input type=\"text\" name=\"Nbrpertime\" placeholder=\" elements will execute/time? \"><input type=\"text\" name=\"Nexttime\" placeholder=\" Next time will continue? \"><br><input type=\"submit\" value=\"Send Program\"></form></p>");
}

void Program(String sequence, String nbrpertime, String nexttime)
{
  if(nexttime.toInt()+savedHour.toInt() == oldHour.toInt())
  {
    for(currentsequenselement; currentsequenselement < sequence.length(); currentsequenselement++)
    {
      if(currentsequenselement < nbrpertime.toInt() - 1)
      {
        Serial.println(sequence[currentsequenselement]);
      }
    }
  }
}
