#include <WiFi.h>
#include <WebServer.h> 
#include <webhtml.h>    //Your Html Web 

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// gotta create a server
WebServer server(80);

// definitions of your desired intranet created by the ESP32
IPAddress PageIP( 192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet( 255, 255, 255, 0);

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[2048];
// just some buffer holder for char operations
char buf[32];

void setup() { 
  Serial.begin(115200);
  // disableCore0WDT();

  Serial.println("starting server");
  WiFi.softAP(ssid,password);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP address: "); Serial.println(ip);

  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);
  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page  || For sending the sensor value to web server
  server.on("/xml", SendXML);
  
  // To get responds from  web server
  server.on("/b1Press", b1Press);server.on("/b2Press", b2Press);server.on("/b3Press", b3Press);server.on("/b4Press", b4Press);server.on("/b5Press", b5Press);server.on("/b6Press", b6Press);server.on("/zufPress", zufPress);server.on("/testPress", testPress);
  server.on("/volinc",  volinc); server.on("/voldec",  voldec);
  server.on("/inctime", inctime);server.on("/dectime", dectime);
  // finally begin the server
  server.begin();
}

void loop(){
  server.handleClient();
}

int photoelecB= 5;
int motdetectB= 10;

void SendXML() {
  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  // // send photo electric barrieer
  sprintf(buf, "<B0>%d</B0>\n", photoelecB);
  strcat(XML, buf);
  
  // // send motion detect value
  sprintf(buf, "<V0>%d</V0>\n", motdetectB);
  strcat(XML, buf);

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  Serial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);
}

void b1Press() {
  Serial.println("b1");
}
void b2Press() {
  Serial.println("b2");
}
void b3Press() {
  Serial.println("b3");
}
void b4Press() {
  Serial.println("b4");
}
void b5Press() {
  Serial.println("b5");
}
void b6Press() {
  Serial.println("b6");
}
void zufPress() {
  Serial.println("zuf");
}
void testPress() {
  Serial.println("test");
}
void volinc() {
  Serial.println("sound++");
}
void voldec() {
  Serial.println("sound--");
}
void inctime() {
  Serial.println("inctime--");
}
void dectime() {
  Serial.println("dectime--");
}
// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite() {
  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}
