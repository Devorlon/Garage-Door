#include <ESP8266WiFi.h>

const char* ssid     = "SSID";
const char* password = "PASSWORD";

WiFiServer server(80);

String header;

int garageDoorState = 0;
const int output1 = 5;
WiFiClient client;

void setup() 
{
  Serial.begin(115200);
  pinMode(output1, OUTPUT);
  digitalWrite(output1, LOW);

  delay(8000);
  digitalWrite(output1, HIGH);
  delay(10);
  digitalWrite(output1, LOW);

  IPAddress ip(192,168,0,210);   
  IPAddress gateway(192,168,0,1);   
  IPAddress subnet(255,255,255,0); 

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
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
}

void testing()
{  
  file.open("test.txt","w+");
  file.write("testy test");
  file.close();
}

void loop()
{
  client = server.available();   

  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Somthing went terribly wrong. At least I caught it.");
    setup();
  }

  testing();
  
  buttonPress();
  
  GUI();
}

void toggle()
{
  if (garageDoorState==0) 
  {
    garageDoorState = 1;
    digitalWrite(output1, HIGH);
    delay(100);
    digitalWrite(output1, LOW);            
  } 
    else
  {
    garageDoorState = 0;
    digitalWrite(output1, HIGH);
    delay(100);
    digitalWrite(output1, LOW);
  }
}

void buttonPress()
{  
  delay(100);
  if(analogRead(A0) > 100)
  {
    toggle();

    do
    {
      delay(100);
    }
    while (analogRead(A0) > 100);
  }
}

void flipState()
{
  
  if(garageDoorState==1)
    {
      garageDoorState==0;
    }
    else
    {
      garageDoorState==1;
    }
}

void GUI()
{
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
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            client.println("<body><h1>Garage Door Web Server</h1>");
            
            client.println("<p><a href=\"/open\"><button class=\"button\">Open</button></a></p>");
            client.println("<p><a href=\"/close\"><button class=\"button\">Close</button></a></p>");
            
            if(garageDoorState==1)
            {
              client.println("<p>Garage Door - Open</p>");
            }
            else
            {
              client.println("<p>Garage Door - Closed</p>");
            }
            
            if (header.indexOf("GET /flip") >= 0) 
            {
              flipState();
            }
            else if (header.indexOf("GET /open") >= 0) 
            {
              client.println("<meta http-equiv=\"refresh\" content=\"0; URL='http://192.168.0.210/'\" />");
              toggle();
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
