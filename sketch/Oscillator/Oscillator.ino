void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
}

void loop() {

    //1200rpm
    Serial.println("1200");
    for (long i = 0; i < 50; i++) {
        digitalWrite(13, LOW);
        delay(10);
        digitalWrite(13, HIGH);
        delay(90);
    }

    //1500-10000rpm
    for (long rpm = 15; rpm <= 95; rpm += 5) {
        long t_low = 10;
        long t_high = ((10 * 60 * 2) / rpm) - t_low;
        long n = 3000 / (t_low + t_high);

        Serial.print(rpm);
        Serial.print(",");

        Serial.print(t_low);
        Serial.print(",");
        Serial.print(t_high);
        Serial.print(",");
        Serial.print(n);
        Serial.println(",");

        for (int i = 0; i < n; i++) {
            digitalWrite(13, LOW);
            delay(t_low);
            digitalWrite(13, HIGH);
            delay(t_high);
        }
    }
}