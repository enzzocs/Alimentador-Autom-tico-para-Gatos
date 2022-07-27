
/*
PWM
controla a luminosidade de um led conforme o valor do potenciometro
*/
 
 int motor_pwm = 6; // pino do led
 
 void setup()
 { 
    pinMode(motor_pwm, OUTPUT); // configura pino como saída
 }
 
 void loop()
 {
  
    analogWrite(motor_pwm, 252); 
}
