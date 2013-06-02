char val;
int ledpin=13;
void setup()
{
  Serial.begin(9600);
  pinMode(ledpin,OUTPUT);
}
 
void loop()
{
  val=Serial.read();
  if(val=='q')
  {
    digitalWrite(ledpin,HIGH);
    Serial.println("LED ON!");
  }else if(val=='w'){
  digitalWrite(ledpin,LOW);
Serial.println("LED OFF!");
  }
}
