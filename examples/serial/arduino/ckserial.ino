char bytes[1024];

void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
}

void loop()
{
  int nRead = Serial.readBytesUntil('\n', bytes, 1024);
  if(nRead > 0)
    Serial.println("bar");
}
