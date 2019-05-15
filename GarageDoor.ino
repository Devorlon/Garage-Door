#include <ESP8266WiFi.h>

const char* ssid     = "18CLARINDA";
const char* password = "mickeymousedonaldduck";

WiFiServer server(80);

String header;

int garageDoorState = 0;

const int output1 = 5;


void setup() 
{
  Serial.begin(115200);
  pinMode(output1, OUTPUT);
  digitalWrite(output1, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  delay(80);
  digitalWrite(output1, HIGH);
  delay(10);
  digitalWrite(output1, LOW); 
}

void loop()
{
  WiFiClient client = server.available();   

  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Somthing went terribly wrong. At least I caught it.");
    setup();
  }

  if(buttonPress()==1)
  {
    delay(70);
    if(garageDoorState == 1)
    {
      garageDoorState = 0;
    }
    else
    {
      garageDoorState = 1;
    }
  }
  
  if (client) 
  {                      
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) 
    {            
      if (client.available()) 
      {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') 
        {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            toggle();

            if(garageDoorState==0) 
            {
              Serial.println("Garage Door - Open");            
            } 
              else if (garageDoorState==1) 
            {
              Serial.println("Garage Door - Closed");
            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            client.println("<body><h1>Garage Door Web Server</h1>");
            
            

            if(garageDoorState==1)
            {
              client.println("<p>Garage Door - Open</p>");
            }
            else 
            {
              client.println("<p>Garage Door - Closed</p>");
            }
            
            client.println("<p><a href=\"/open\"><button class=\"button\">Open / Close</button></a></p>");

            if (header.indexOf("GET /open") >= 0) 
            {
              delay(5);
              client.println("<meta http-equiv=\"refresh\" content=\"0; URL='http://192.168.0.8/'\" />");
            }
            
            client.println("</body></html>");
            
            client.println();
            break;
          } 
          else 
          { 
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {  
          currentLine += c;    
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void toggle()
{
  if ((header.indexOf("GET /open") >= 0) & (garageDoorState==0)) 
  {
    garageDoorState = 1;
    digitalWrite(output1, HIGH);
    delay(10);
    digitalWrite(output1, LOW);            
  } 
    else if ((header.indexOf("GET /open") >= 0) & (garageDoorState==1)) 
  {
    garageDoorState = 0;
    digitalWrite(output1, HIGH);
    delay(10);
    digitalWrite(output1, LOW);
  }
}

int buttonPress()
{  
  delay(10);
  if(analogRead(A0) > 100)
  {
    return 1;
  }
  return 0;
}
