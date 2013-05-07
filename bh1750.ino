/*
 BH1750光照度传感器程序代码
 Version 1.0
 作者:王俊锋泽
 博客：http://www.xn--7tqt52e1pef42b.cn/
 接法:
 VCC-5v
 GND-GND
 SCL-SCL(A5)
 SDA-SDA(A4)
 ADD-GND
 */

/*
Sample code for the BH1750 Light sensor
Version 0.1
website:www.DFRobot.com

Connection:

VCC-5v
GND-GND
SCL-SCL(analog pin 5)
SDA-SDA(analog pin 4)
ADD-NC
*/
#include <Wire.h>
#include <math.h>

int BH1750address = 0x23;//BH1750 I2C地址
byte buff[2];
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
}
 
void loop()
{
    Serial.print( BH1750() );
    Serial.println("[lx]");
}
 
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
