#include "ESP8266WiFi.h" //bibliothèque pour couper le wifi de la carte
#include <Servo.h> //bibliothèque pour gérer les servomoteurs (moteur des roues)

//Les objets servos permette de faire tourné les roues.
// servo.write(90) moteur à l'arret
// servo.write(10) moteur va dans un sens
// servo.write(170) moteur va dans l'autre sens
//objet gérant la roue gauche
Servo myservoL; 
//objet gérant la roue droite
Servo myservoR; 

//variable contenant la valeur de de capteur de lumière 1
int sensor1=0;
//variable contenant la valeur de de capteur de lumière 2
int sensor2=0;

//fanction mettant à jour les variables des capteurs de lumière
void readSensor(){

  //La pin D4 gère le multiplexeur analogique
  //On demande au multiplexeur donné la valeur du capteur 1
  digitalWrite(D4,LOW);
  //On lit la valeur du capteur 1
  sensor1 = analogRead(A0);
  //on attend 10 millisecondes
  delay(10);
  //On demande au multiplexeur donné la valeur du capteur 2
  digitalWrite(D4,HIGH);
  //On lit la valeur du capteur 2
  sensor2 = analogRead(A0);
  //on attend 10 millisecondes
  delay(10);
}

//Fonction pour avancer
void foward()
{
  Serial.println("F");
  myservoR.write(10);
  myservoL.write(170);
}
void backward()
{
  Serial.println("B");
  myservoR.write(170);
  myservoL.write(10);
}
void left()
{
  Serial.println("L");
  myservoR.write(10);
  myservoL.write(90);
}
void right()
{
  Serial.println("R");
  myservoR.write(90);
  myservoL.write(170);
}
void stop()
{
  Serial.println("S");
  myservoR.write(90);
  myservoL.write(90);
}


//Focntion executée au démarage de la carte (mise sous tension)
void setup() {
  //Initialise la liaison série
  Serial.begin(9600);
  //Eteint le wifi
  WiFi.forceSleepBegin();
  //on attent 1 milliseconde
  delay(1);              

  //On initialise les objets servos en indiquant sur quelle pin les servos sont connectés
  myservoL.attach(D1);; 
  myservoR.attach(D2);*
  //on arrête les servos
  myservoL.write(90); 
  myservoR.write(90); 
  //on initialise la pin controllant le multiplexeur
  pinMode(D4, OUTPUT);
}

//Fonction étant exécutée en boucle
void loop() {

  //on appelle la focntion pour lire la valeur des capteurs de lumière
  readSensor();

  //Si la valeur est élever on arrete les moteurs (on est dans le noir)
  if(sensor1>500 && sensor2 >500)
  {
    stop();
  }
  else
  {
    int diffsensor= sensor1-sensor2;
    //Si la valeur absolue de la différence entre leur deux capteur est inférieur à 150, on avance
    if(abs(diffsensor)<150)
    {
      foward();
    }
    else //Sinon on tourne selon le capteur avec la valeur la plus élevée
    {
      if(sensor1<sensor2)
      {
        right();
      }
      else
      {
        left();
      }
    }
  }
  //On affiche de la valeur des capteur sur la liaison série (terminal série)
  Serial.print(sensor1);
  Serial.print("\t");
  Serial.println(sensor2);

  //on attent 50 millisecondes
  delay(50);              

  //on réinitialise le watchdog de la carte
  //nécessaire si on ne veut pas que la carte s'arrête toute seule
  wdt_reset();
}
