/*
  Cliente Web
  Ezequiel França
*/
#include <Ethernet.h>

char server[] = "lampada-fiap.herokuapp.com";
byte mac[] = { 0x98, 0x4F, 0xEE, 0x05, 0x44, 0x9B };
byte ip[] = { 10, 1, 102, 33 };
byte gateway[] = { 10,1,102,254 };
byte subnet[] = { 255,255,255,0 };
byte dns1[] = {192,168,60,81};
EthernetClient client;
unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in millisecondstku
String result;
String estado;
int porta_rele1 = 12;

void setup() {
  pinMode(12, OUTPUT);
  // Open serial communications and wait for port to open:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
//  system("telnetd -l /bin/sh");
//  system("ifup eth0");
  system("telnetd -l /bin/sh"); //Start the telnet server on Galileo
  system("ifconfig eth0 10.1.102.33 netmask 255.255.255.0 up");
  system("route add default gw 10.1.102.254 eth0");
  system("echo 'nameserver 192.168.60.81' > /etc/resolv.conf");
  while (!Serial) {
    ; 
  }
  delay(3000);
  Serial.println("connecting");
  Serial.println(client);
  delay(3000);
  
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET / HTTP/1.0");
    //client.println();
    httpRequest();
  }
  else {
    // if you didn’t get a connection to the server:
    Serial.println("connection failed");
  }
}
void loop()
{
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

  if (client.available()) {
    char c = client.read();
    result.concat(c);
  }
  Serial.println(estado);
  if (result.endsWith("false")) {
    digitalWrite(porta_rele1, LOW);
    estado = "desligada";
  } else if (result.endsWith("true")) {
    digitalWrite(porta_rele1, HIGH);
    estado = "ligado";
  }
  // Se o server desconectar
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting");
    client.stop();
    // do nothing forevermore:
    //for(;;)
    //;
  }
}

void httpRequest() {
  // Encerra todas conexoes para um novo request
  client.stop();

  // Se conectou com sucesso
  if (client.connect(server, 80)) {
    //Serial.println("conectandoooo...");
    // mandando aquele GET 
    client.println("GET / HTTP/1.1");
    client.println("Host: lampada-fiap.herokuapp.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // Pra saber quanto tempo levou
    lastConnectionTime = millis();
  } else {
    // Se der merda
    Serial.println("connection failed");
  }
}

void acionaLampada(int state) {
  Serial.println(state);
  digitalWrite(porta_rele1, state);

}
