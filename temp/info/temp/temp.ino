//Inclusion des bibliothèques
#include <Wire.h> //bibliothèque pour la communication avec les capteurs
#include <rgb_lcd.h>//bibliothèque pour l'écran LCD
#include <DS1307.h>//bibliothèque pour l'horloge RTC
#include <Seeed_MCP9808.h>//bibliothèque pour le capteur de température
#include <Adafruit_NeoPixel.h>//bibliothèque pour les LED

//constante definissant le numéro de la pin où sont branchées les LED
const int PIN_LED = 3;
//objet permettant de controller les LED
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(9, PIN_LED, NEO_GRB + NEO_KHZ800);
//objet permettant de controller le themomètre
MCP9808  sensor;
//objet permettant de controller l'horloge RTC
DS1307 clock;
//objet permettant de controller l'écran LCD
rgb_lcd lcd;

//constante definissant le numéro de la pin où est branché le capteur infrarouge
const int PIN_IR = 2;


//Focntion executée au démarage de la carte (mise sous tension)
void setup() {
  //Initialise la liaison série
  Serial.begin(9600);
  //Initialise le capteur de température
  //Si cela échou on affiche un message
  if(sensor.init())
  {
      Serial.println("Initialisation capteur raté!!");
  }
  else
  {
    Serial.println("capteur initialisé!!");
  }
  //Initialise l'écran LCD
  lcd.begin(16, 2);
  //Affichage d'un message sur l'écran
  lcd.print("hello, world!");
  //Initialise l'horloge
  clock.begin();
  //Pour changer l'heure de l'horloge décoummenter les lignes suivant (retirer le /*)
  //Modifier la date
  //Televerser le programme
  //Recommenter les lignes
  //Televerser le programme a nouveau (sinon l'heure sera toujours la même au démarage)
  /*clock.fillByYMD(2019,6,12);//Jan 19,2013
  clock.fillByHMS(11,15,30);//15:28 30"
  clock.fillDayOfWeek(WED);//Saturday
  clock.setTime();//write time to the RTC chip*/

  //Initialise la pin du capteur infrarouge en tant qu'entré
  pinMode(PIN_IR, INPUT);

  //Initialise les LED
  pixels.begin();
  pixels.show();

  //Attend 5 secondes
  delay(5000);
  //Efface ce qu'il se trouve sur le LCD
  lcd.clear();

    
}

//Fonction affichant l'heure sur l'écran LCD
void printHour()
{
  clock.getTime();//Récupère l'heure dans le capteur horloge
  lcd.setCursor(4, 0);// on se déplace sur la colone 4 et la ligne 0 sur l'ecran LCD
  if(clock.hour<10)lcd.print(0); //Si entre 0 et 9 on affiche un 0 avant
  lcd.print(clock.hour, DEC); //affichage de l'heure
  lcd.print(":");
  if(clock.minute<10)lcd.print(0);
  lcd.print(clock.minute, DEC);//affichage des minutes
  lcd.print(":");
  if(clock.second<10)lcd.print(0);
  lcd.print(clock.second, DEC);//affichage des secondes
}

//Fonction étant exécutée en boucle
void loop() {
  //on déclare une variable flotante (nombre à virgule) de température
  float temp=0;
  //on récupère la température et on la met dans la variable temp
  sensor.get_temp(&temp);
  if(temp<=15)//Si temp inférieur ou égal à 15 degrés
  {
    pixels.fill(pixels.Color(0,0,100));//On indique à toutes les leds de passer en bleu
    pixels.show();//on affiche(applique) la modification
  }
  else if(temp>16 && temp<30)//Si temp supérieur à 16 et inférieur à 30
  {
    pixels.fill(pixels.Color(0,100,0));
    pixels.show();
  }
  else //Sinon (spérieur ou égale à 30)
  {
    pixels.fill(pixels.Color(100,0,0));
    pixels.show();
  }
  
  if(digitalRead(PIN_IR)==LOW) //Si la pin du capteur infrarouge est LOW (0V), une présence est détecté, on affiche la température
  {
    lcd.clear(); //On efface l'écran LCD
    lcd.setCursor(2, 0);// on se déplace sur la colone 2 et la ligne 0 sur l'ecran LCD
    lcd.print("Temperature");// on affiche Temperature
    lcd.setCursor(5, 1);// on se déplace sur la colone 5 et la ligne 1 sur l'ecran LCD
    lcd.print(temp);// on affiche la température
    delay(5000);// on attend 5 seconde
    lcd.clear();// on efface l'écran
  }
  else //Si on ne detecte rien on affiche l'heure
  {
    printHour();
  }
  delay(100);//on attend 100 milisecondes
}
