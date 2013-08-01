const int TrigPin = 3;
const int EchoPin = 4;  
// 定义变色灯LED的三个输出接口，这里也可以选用三个led
int ledpin1 = 5;  // R
int ledpin2 = 6;  // G
int ledpin3 = 7;  // B
int beeppin = 8;  // 蜂鸣器的pin
int i = 0;
 
 
void setup ()
{
  Serial.begin(9600);     //测量结果将通过此串口输出至 PC 上的串口监视器
  pinMode(EchoPin, INPUT);      //设置EchoPin 为输入模式
  pinMode(TrigPin, OUTPUT);     //设置超声波数字IO脚模式，OUTPUT为输出 
  pinMode(ledpin1,OUTPUT);
  pinMode(ledpin2,OUTPUT);
  pinMode(ledpin3,OUTPUT);
  pinMode(beeppin,OUTPUT);      //设置蜂鸣器引脚输出模式
}
 
 
void loop()
{
  digitalWrite(TrigPin, LOW);     // 通过Trig/Pin 发送脉冲，触发 HC-SR04 测距，使发出发出超声波信号接口低电平2μs
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);    // 使发出发出超声波信号接口高电平10μs，这里是至少10μs
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);     // 保持发出超声波信号接口低电平
  int distance = pulseIn(EchoPin, HIGH);   // 读出脉冲时间
  distance= distance/58;           // 将脉冲时间转化为距离（单位：厘米）
  Serial.println(distance);        //输出距离值
 
  if(distance<20)            //当距离<20cm（这个数据可以在超声波测量的范围内随意改，3cm~450cm ）
  {
    setColor(255, 0, 0);//三色LED的设置颜色，如果是用三个led则这里删去
    buzzer1();
  }
  else if (distance<=120)    //当距离<120cm（这个数据可以在超声波测量的范围内随意改，3cm~450cm ）
  {
    setColor(0, 0, 255);
    buzzer2();
  }
  else
  {
    setColor(0, 255, 0);
    buzzer3();
  }
}
 
//*****************************************************************
void buzzer1()
{
  tone(beeppin,900);
  delay(300);  //响声间隔300ms 听上去更急促
  noTone(beeppin);
}
 
void buzzer2()
{
  tone(beeppin,600);
  delay(500);  //响声间隔500ms 听上去普通
  noTone(beeppin);
}
 
void buzzer3()  //这里我用了除余的方法，目的是如果处在安全距离，在不用delay的情况下蜂鸣器响声不要那么频繁
{
  if(i%100 == 1){
    tone(beeppin,500);
  }
  else{
    noTone(beeppin);
  }
  i++;
}
 
void setColor(int red, int green, int blue)  //三色LED的设置颜色函数，如果是用三个led则这里删去。然后把上面也改一下
{
  analogWrite(ledpin3, red);
  analogWrite(ledpin2, green);
  analogWrite(ledpin1, blue);
}
