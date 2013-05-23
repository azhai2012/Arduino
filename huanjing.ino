1
/*
Arduino+乐联网平台搭建室内环境监测系统

BH1750光照强度传感器接法:
VCC-5v
GND-GND
SCL-SCL(analog pin 5)
SDA-SDA(analog pin 4)

DHT11接法：
VCC-->5V
GND-->GND
DATA-->digital pin 2


PPD42NS接法：
针脚冲下，从右至左为1-5接口
Pin 1  => GND
Pin 3  => 5VDC
Pin 4  => Digital Pin 8
*/

#include <Wire.h>
#include <math.h>
#include <dht11.h>

dht11 DHT11;
#define DHT11PIN 2
int BH1750address = 0x23;//BH1750 I2C地址
byte buff[2];

int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 10000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
Wire.begin();
Serial.begin(9600);
pinMode(8,INPUT);
//starttime = millis();
}

void loop() {
// LM35温度传感器数据
int n = analogRead(A0);//读取A0口的电压值
float vol = n * (5.0 / 1023.0*100);//使用浮点数存储温度数据，温度数据由电压值换算得到
//Serial.print("LM35 Temperature (oC):");
Serial.print(vol);//串口输出温度数据

// DHT11传感器数据
DHT11.read(DHT11PIN);
Serial.print(",");
Serial.print((float)DHT11.humidity, 2);
Serial.print(",");
Serial.print((float)DHT11.temperature, 2);

//输出一个换行
//Serial.println("\n");

//光照传感器数据
Serial.print(",");
Serial.print(BH1750());

duration = pulseIn(pin, LOW);
lowpulseoccupancy = lowpulseoccupancy+duration;


ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
Serial.print(",");
Serial.println(concentration);
//Serial.println("---");
lowpulseoccupancy = 0;

delay(30000);
}

double BH1750() //BH1750设备操作
{
int i=0;
double val=0;
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
buff[i] = Wire.read(); // receive one byte
i++;
}
Wire.endTransmission();
if(2==i)
{
val=((buff[0]<<8)|buff[1])/1.2;
}
return val;
}
