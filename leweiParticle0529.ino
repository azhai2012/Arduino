/*
   open.lewei50.com  sensor  client
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <math.h>
#include <dht11.h>

#define USERKEY         "dce04945f5044a15a8cfbcd5555fd8bc"//lewei userkey

dht11 DHT11;
#define DHT11PIN 2

int BH1750address = 0x23;//BH1750 I2C地址
byte buff[2];

// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(216,52,233,121);      // numeric IP for api.cosm.com
char server[] = "open.lewei50.com";   // name address for cosm API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 30*1000; //delay between updates to cosm.com


int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
  Serial.begin(9600);
  pinMode(8,INPUT);
  // start the Ethernet connection with DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for(;;)
      ;
  }
  else{
    Serial.println("Ethernet configuration OK");
  }
  Wire.begin();
  starttime = millis();
}

int x=0; //simulated sensor output
int sampling=1;
int transfering=0;
void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if(1==sampling)
  {
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;

    if ((millis()-starttime) > sampletime_ms)
    {
      Serial.print("before filter:");
      Serial.println(lowpulseoccupancy);
      lowpulseoccupancy=Filter1(lowpulseoccupancy);
      Serial.print("behind filter:");
      Serial.println(lowpulseoccupancy);
      ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
      Serial.print(lowpulseoccupancy);
      Serial.print(",");
      Serial.print(ratio);
      Serial.print(",");
      Serial.println(concentration);
      lowpulseoccupancy = 0;
      //initiate the http post
      sampling=0;
      transfering=1;
    }
  }
  // http post begin 
  if(1==transfering)
  {
    if (client.available())
    {
      char c = client.read();
      Serial.print(c);
    }

    // if there's no net connection, but there was one last time
    // through the loop, then stop the client:
    if (!client.connected() && lastConnected) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
      //initiate the PPDS testing
      transfering=0;
      sampling=1;
      starttime=millis();

    }

    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
      Serial.print("http post:");
      Serial.println(concentration);

      //lm35
      int lm35temp = 30;
      //int lm35n = analogRead(A0);//读取A0口的电压值
      //int lm35temp = lm35n * (5 / 1023);//温度数据，温度数据由电压值换算得到
      //Serial.print("lm35temp:");
      //Serial.println(lm35temp);

      // DHT11传感器数据
      DHT11.read(DHT11PIN);
      int dht11hum = DHT11.humidity;
      Serial.print("shidu:");
      Serial.println(dht11hum);
      int dht11temp = DHT11.temperature;
      Serial.print("wendu:");
      Serial.println(dht11temp);

      // BH1750
      int light = BH1750()*100;
      Serial.print("light:");
      Serial.println(light);
      sendData(lm35temp,dht11hum,dht11temp,light,concentration);
    }
    // store the state of the connection for next time through
    // the loop:
    lastConnected = client.connected();
    //Serial.print("lastConnected:");
    //Serial.println(lastConnected);

  }

}

// this method makes a HTTP connection to the server:
void sendData(int lm35temp,int dht11hum,int dht11temp,int light,int thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("POST /api/V1/gateway/Updatesensors/01 "); // 01代表01网关，如果是02网关这里换成02
    client.println("HTTP/1.1");
    client.print("userkey: ");
    client.println(USERKEY);
    client.println("Host: open.lewei50.com ");
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 116 + getLength(lm35temp) + getLength(dht11hum) + getLength(dht11temp) + getLength(light) + getLength(thisData);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    //client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    // 这里的用p1，是因为用户在系统里面已经添加了一个传感器缩写叫p1的传感器 (在01网关下面)

    client.print("[{\"Name\":\"t1\",\"Value\":");     
    client.print(lm35temp);
    client.print("},{\"Name\":\"t2\",\"Value\":");
    client.print(dht11hum);
    client.print("},{\"Name\":\"t3\",\"Value\":");
    client.print(dht11temp);
    client.print("},{\"Name\":\"t4\",\"Value\":");
    client.print(light);
    client.print("},{\"Name\":\"t5\",\"Value\":");
    client.print(thisData);
    client.println("}]");

    Serial.print("[{\"Name\":\"t1\",\"Value\":");     
    Serial.print(lm35temp);
    Serial.print("},{\"Name\":\"t2\",\"Value\":");
    Serial.print(dht11hum);
    Serial.print("},{\"Name\":\"t3\",\"Value\":");
    Serial.print(dht11temp);
    Serial.print("},{\"Name\":\"t4\",\"Value\":");
    Serial.print(light);
    Serial.print("},{\"Name\":\"t5\",\"Value\":");
    Serial.print(thisData);
    Serial.println("}]");
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}


// This method calculates the number of digits in the
// sensor reading.  Since each digit of the ASCII decimal
// representation is a byte, the number of digits equals
// the number of bytes:

int getLength(int someValue) {
  // there's at least one byte:
  int digits = 1;
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0) {
    dividend = dividend /10;
    digits++;
  }
  // return the number of digits:
  return digits;
}

/* filter program : 20130521 */
#define FILTERLEN 10 

unsigned long Array_Average( unsigned long* Array,int length)
{
  int x;
  unsigned long returnVal;
  unsigned long result=0;
  for(x=0;x<length;x++)
  {
    result+=Array[x];
    Serial.print("result=");
    Serial.println(result);
  }
  returnVal=result/length;
  return returnVal;
}

unsigned long Filter1(unsigned long lowpulse)
{
  static unsigned long sfiterArray[FILTERLEN];
  static int sindex=0;
  int x;
  Serial.println("filter1 begin:");
  if(FILTERLEN>sindex)
  {
    sindex++;
    Serial.println(sindex);
    sfiterArray[sindex]=lowpulse;
    Serial.println("filter1 END");
    return lowpulse;
  }
  else
  {
    for(x=0;x<FILTERLEN-1;x++)
    {
      sfiterArray[x]=sfiterArray[x+1];
    }
    sfiterArray[FILTERLEN-1]=lowpulse;
    for(x=0;x<FILTERLEN;x++)
    {
      Serial.println(sfiterArray[x]);
    }
    Serial.println("Aver:");
    Serial.println(Array_Average(sfiterArray,FILTERLEN));
    Serial.println("filter1 END");
    return(Array_Average(sfiterArray,FILTERLEN));

  }


}
/*END: filter program : 20130521 */

double BH1750() //BH1750设备操作
{
  int i=0;
  double  val=0;
  //开始I2C读写操作
  Wire.beginTransmission(BH1750address);
  Wire.write(0x10);//1lx reolution 120ms//发送命令
  Wire.endTransmission();  
  delay(200);
  //读取数据
  Wire.beginTransmission(BH1750address);
  Wire.requestFrom(BH1750address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  if(2==i)
  {
    val=((buff[0]<<8)|buff[1])/1.2;
  }
  return val;
}
