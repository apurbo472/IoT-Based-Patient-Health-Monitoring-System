// Virtual Heartbeat Generator for Proteus
// Output pin simulates pulse sensor signal

const int pulsePin = 3;   // output pin
int bpm = 72;             // simulated heart rate

unsigned long beatInterval;
unsigned long lastBeat = 0;

void setup()
{
  pinMode(pulsePin, OUTPUT);
  beatInterval = 60000 / bpm;   // ms per beat
}

void loop()
{
  unsigned long currentTime = millis();

  if (currentTime - lastBeat >= beatInterval)
  {
    // generate heartbeat spike
    digitalWrite(pulsePin, HIGH);
    delay(40);                // pulse width
    digitalWrite(pulsePin, LOW);

    lastBeat = currentTime;
  }
}