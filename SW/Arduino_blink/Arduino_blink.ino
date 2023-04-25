void setup() 
{
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() 
{
  digitalWrite(8, HIGH);
  delay(1500);
  digitalWrite(8, LOW);
  delay(500);
  digitalWrite(9, HIGH);
  delay(300);
  digitalWrite(9, LOW);
  delay(700);
}
