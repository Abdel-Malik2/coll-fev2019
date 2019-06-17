
#include <Wire.h> //bibliothèque pour la communication avec les capteurs (I2C)
#include <RtcDS3231.h> //bibliothèque pour le capteur horloge
#include <rgb_lcd.h>//bibliothèque pour l'écran LCD
#include <SoftwareSerial.h>//bibliothèque pour la cimmonucation avec le lecteur mp3
#include <DFRobotDFPlayerMini.h>//bibliothèque pour le lecteur mp3
#include <Adafruit_NeoPixel.h>//bibliothèque pour les LED
#include <EEPROM.h>//bibliothèque pour sauvgarder des données 
//configuration de la bibliothèque encodeur
#define ENCODER_USE_INTERRUPTS
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>//bibliothèque pour le bouton rotatif
#include <Bounce2.h>//bibliothèque pour gérer les evenements du bouton

//constante definissant le numéro de la pin où sont branchées les LED
#define PIN_LED 8
//objet permettant de controller les LED
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(20, PIN_LED, NEO_GRB + NEO_KHZ800);
//objet permettant de controller l'écran LCD
rgb_lcd lcd;
//objet permettant de controller l'horloge RTC
RtcDS3231<TwoWire> Rtc(Wire);
//objet permettant d'utiliser la communication avec le lecteur mp3
SoftwareSerial mySoftwareSerial(5, 6);
//objet permettant de controller le lecteur mp3
DFRobotDFPlayerMini myDFPlayer;
//objet permettant de contrôller le bouton rotatif
Encoder myEnc(2, 3);


//constante definissant le numéro de la pin où est branché le bouton
#define PIN_BUT 7
//objet permettant de gerer les evenements du bouton
Bounce bouton = Bounce();

//constante definissant le numéro de la pin où est branché le capteur de mouvement
#define PIN_PIR 4
//objet permettant de gerer les evenements du capteur de mouvement
Bounce pir = Bounce();

//constantes indiquant les adresses où sont stokées les valeurs des couleurs des leds
#define EEPROMCR 10
#define EEPROMCV 11
#define EEPROMCB 12

//declaration des variables des couleurs des LED
unsigned char colorR;
unsigned char colorV;
unsigned char colorB;

//Constantes gerant les durées utilisées pour le changement d'intensité de LED lors de la détection d'un mouvement
//temps d'allumage
#define TIMEON 5000
//temps a pleine puissance
#define TIMESTARTOFF 10000
//temps d'extintion
#define TIMEENDOFF 20000
//variable contenant le temps au moment du démarage du pattern
long startpatterntime=0;
//varaible contenant le temps actuel du pattern relativement au début du pattern
long patterntime=0;


//constantes indiquant les adresses où sont stokées les valeurs de l'alarme
#define EEPROMAHEURE 0
#define EEPROMAMINUTE 1
#define EEPROMAACTIVE 2

//variable contenant l'heure de l'alarme
unsigned char aheure = 9;
//variable contenant les minutes de l'alarme
unsigned char aminute = 0;
//variable booléen indiquant si l'alarme est active où non
bool alarmeon=false;

//objet permettant de récupérer l'heure et la date
RtcDateTime now;
//variable contenant la seconde actuelle
int seconde =0;
//variable contenant la minute actuelle
int minute =0;

//variable contenant l'état du sequenceur gerant le menu et l'alarme
int mode = 0;

//Focntion executée au démarage de la carte (mise sous tension)
void setup() {
  //Initialise la liaison série
  Serial.begin(9600);
  
  //Initialise les LED
  pixels.begin();
  pixels.show();
  
  //Initialise l'écran LCD
  lcd.begin(16, 2);
  //Affichage d'un message sur l'écran
  lcd.print("hello, world!");

  //initialise le bouton principal
  bouton.attach(PIN_BUT,INPUT_PULLUP);
  //defini le temps de réaction du bouton en milliseconde
  bouton.interval(25);
  //initialise le capteur de mouvement
  pir.attach(PIN_PIR,INPUT);
  pir.interval(25);

  //initialise la communication avec le lecteur mp3
  mySoftwareSerial.begin(9600);
  //Initialise le lecteur mp3, si cela echou on affiche un message. 
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  else //message affiché si l'initialisation c'est bien passé
  {
    Serial.println(F("DFPlayer Mini online."));
  }

  //on le volume au maximum
  myDFPlayer.volume(30);

  //initialisation du capteur de l'horloge
  Rtc.Begin();
  //on récupère l'heure à laquelle le code est compilé
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //On vérifie la comminication avec l'horloge
  if (!Rtc.IsDateTimeValid()) 
  {
      if (Rtc.LastError() != 0)
      {
          // we have a communications error
          // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
          // what the number means
          Serial.print("RTC communications error = ");
          Serial.println(Rtc.LastError());
      }
      else
      {
          // Common Cuases:
          //    1) first time you ran and the device wasn't running yet
          //    2) the battery on the device is low or even missing
          Serial.println("RTC lost confidence in the DateTime!");
          // following line sets the RTC to the date & time this sketch was compiled
          // it will also reset the valid flag internally unless the Rtc device is
          // having an issue
          Rtc.SetDateTime(compiled);
      }
  }
  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }
  //On copare la date avec l'heure de compilation et on la met à jour si nécessaire. 
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
  //configuration de l'horloge
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

  //on efface l'écran
  lcd.clear();

  //On récupère les données en mémoire et on initialise les variables
  setTempsAlarme(EEPROM.read(EEPROMAHEURE), EEPROM.read(EEPROMAMINUTE));
  setColor(EEPROM.read(EEPROMCR),EEPROM.read(EEPROMCV),EEPROM.read(EEPROMCB));
  alarmeon=EEPROM.read(EEPROMAACTIVE);

  
}

//fonction modifiant l'heure a laquelle l'alarme doit démarer
void setTempsAlarme(unsigned char heure, unsigned char minute){
  if(heure>23) heure=0;
  if(minute>59) minute=0;
  aheure=heure;
  aminute=minute;
  EEPROM.write(EEPROMAHEURE, aheure);
  EEPROM.write(EEPROMAMINUTE, aminute);
}

//on déclenche ou on arette l'alarme
void setAlarme(bool on)
{
  if(on)//si on déclenche
  {
    mode=10;//on passe en mode 10
    myDFPlayer.play(1); // on déclenche la musique
    // ecrit un message sur l'ecran en le faisant clignoté
    lcd.blink();
    lcd.print("C'est l'heure!!!");
  }
  else
  {
      mode=0;//on repasse en mode 0 (affichage de l'heure)
      myDFPlayer.pause(); //on arette la musique
      lcd.noBlink(); //on arette le clignotement de l'écran
  }
}

//fonction modifiant enregistrant la couleur des LED
void setColor(unsigned char rouge, unsigned char vert, unsigned char bleu){
  colorR=rouge;
  colorV=vert;
  colorB=bleu;
  EEPROM.write(EEPROMCR, colorR);
  EEPROM.write(EEPROMCV, colorV);
  EEPROM.write(EEPROMCB, colorB);
}



///////////////////////////////////////
//Variables et fonction permettant de claculer le nombre de pas tourné sur le bouton rotatif
//la fonction renvoie le nombre de pas effectué durant les 200 dernières miliseconde
long lastupdate = 0;
long endupdate = 0;
long startupdate = 0;
long oldPosition  = -999;
long storePosition  = -999;

int calculEncDiff()
{
  
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if(startupdate==0)startupdate=millis();
    endupdate=millis();
    oldPosition = newPosition;
  }
  if((endupdate!=0 && millis()-endupdate>100) || ((millis()-startupdate>200) &&storePosition!=oldPosition))
  {
    int diff =(storePosition-oldPosition)/4;
    storePosition=oldPosition;
    endupdate=0;
    startupdate=0;
    return diff;
  }
  return 0;
}
//////////////////////////


//Fonction affichant l'heure sur l'écran LCD
void printTime()
{
  if(seconde!=now.Second()) //Si on a changer de seconde on met à jour l'affichage
  {
    //on met à jour la seconde actuelle
    seconde=now.Second();
    // on se déplace sur la colone 4 et la ligne 0 sur l'ecran LCD
    // répétition pour évité les erreurs d'affichage
    lcd.setCursor(4, 0);
    lcd.setCursor(4, 0);
    lcd.setCursor(4, 0);
    if(now.Hour()<10)lcd.print(0);//Si entre 0 et 9 on affiche un 0 avant
    lcd.print(now.Hour(), DEC);//affichage de l'heure
    lcd.print(":");
    if(now.Minute()<10)lcd.print(0);
    lcd.print(now.Minute(), DEC);
    lcd.print(":");
    if(now.Second()<10)lcd.print(0);
    lcd.print(now.Second(), DEC);
  }
}
//Fonction affichant le menu de l'alarme sur l'écran LCD
void printAlarme()
{
    lcd.setCursor(4, 0);
    lcd.print("Alarme");
    lcd.setCursor(1, 1);
    if(alarmeon)
    {
      lcd.print("On");
    }
    else
    {
      lcd.print("Off");
    }
    lcd.setCursor(5, 1);
    
    if(aheure<10)lcd.print("0");
    lcd.print(aheure, DEC);
    lcd.print(":");
    if(aminute<10)lcd.print(0);
    lcd.print(aminute, DEC);
}


//Fonction affichant le menu des couleur sur l'écran LCD
void printCouleur()
{
    lcd.setCursor(4, 0);
    lcd.print("Couleur");
    lcd.setCursor(1, 1);
    if(colorR<100)lcd.print("0");
    if(colorR<10)lcd.print("0");
    lcd.print(colorR,DEC);
    
    lcd.setCursor(6, 1);
    if(colorV<100)lcd.print("0");
    if(colorV<10)lcd.print("0");
    lcd.print(colorV,DEC);
    
    lcd.setCursor(11, 1);
    if(colorB<100)lcd.print("0");
    if(colorB<10)lcd.print("0");
    lcd.print(colorB,DEC);
}

//Fonction contenant le sequenceur gérant le menu et l'alarme
//enc est le nombre de pas tourné sur le bouton rotatif (0 si immobile)
void menu(int enc){
  //booleen indiquant s'il faut mettre à jour l'affichage
  //bouton appuyé ou bouton rotatif tournée
  bool change = false;
  if (bouton.fell()) //Si on appuis sur le bouton
  {
    change=true; //on mettera a jour l'affichage
    if(mode <10)//Si on est en mode menu (mode inférieur à 10)
    {
      mode++;
      if(mode>6) mode = 0;
    } else if(mode==10) //Si mode 10 (alarme allumé) on l'éteint
    {
      setAlarme(false);
    }
    Serial.print("mode ");
    Serial.println(mode);
  }


  //Gestion du menu selon les valeurs du bouton rotatif
  if(enc!=0)//Si le bouton rotatif a été tourné, on mettera à jour l'affichage 
  {
    change=true;
  }
  if(mode == 1)//Mode 1 on change l'activation de l'alarme
  {
    if(enc!=0) alarmeon = ! alarmeon;
    EEPROM.write(EEPROMAACTIVE,alarmeon);
  }
  else if(mode == 2)//Mode 2 on change l'heure de l'alarme
  {
    if(enc!=0)
    {
      setTempsAlarme(aheure+enc,aminute);
    }
  }
  else if(mode == 3)//Mode 3 on change les minutes de l'alarme
  {
    if(enc!=0)
    {
      setTempsAlarme(aheure,aminute+enc);
    }
  }
  else if(mode == 4)//Mode 4 on change la couleur rouge des leds
  {
    if(enc!=0)
    {
      setColor(colorR+enc,colorV,colorB);
    }
  }
  else if(mode == 5)//Mode 4 on change la couleur verte des leds
  {
    if(enc!=0)
    {
      setColor(colorR,colorV+enc,colorB);
    }
  }
  else if(mode == 6)//Mode 4 on change la couleur bleu des leds
  {
    if(enc!=0)
    {
      setColor(colorR,colorV,colorB+enc);
    }
  }

  //Si il faut mettre à jour l'affichage, on efface l'écran, on eteint les pixels
  if(change)
  {
    Serial.println("Clear");
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.clear();
    lcd.clear();
    pixels.clear();
    pixels.show();
  }

  //Gestion de l'affichage
  if(mode == 0)//Mode 0 affichage de l'heure
  {
    printTime();
  }
  else if(1 <= mode  && mode <=3 && change )//Mode 1 à 3 affichage du menu de l'alarme
  {
    Serial.println("Menu Alarme");
    printAlarme();
    //on affiche de le chevron sur le paramètre à modifié
    if(mode == 1)
    {
      lcd.setCursor(0,1);
      lcd.print(">");
    } else if(mode == 2)
    {
      lcd.setCursor(4,1);
      lcd.print(">");
    } else if (mode == 3)
    {
      lcd.setCursor(10,1);
      lcd.print("<");
    }
  }
  else if(4 <= mode  && mode <=6 && change )//Mode 4 à 6 affichage du menu des couleurs
  {
    Serial.println("Menu Couleur");
    printCouleur();
    //on affiche de le chevron sur le paramètre à modifié
    if(mode == 4)
    {
      lcd.setCursor(0,1);
      lcd.print(">");
    } else if(mode == 5)
    {
      lcd.setCursor(5,1);
      lcd.print(">");
    } else if (mode == 6)
    {
      lcd.setCursor(10,1);
      lcd.print(">");
    }
    //On affiche la couleur réglée
    pixels.fill(pixels.Color(colorR,colorV,colorB));
    pixels.show();
  }
}

//Fonction gérant l'affichage du pattern des leds lors de la détection d'un mouvement
void looppattern()
{
  patterntime=millis()-startpatterntime; //On calcule le temps relativement au temps de départ du pattern

  if(patterntime<TIMEON) //Si on est dans la premier phase, on augemente la luminausité proportionnelement
  {
    pixels.fill(pixels.Color(map(patterntime,0,TIMEON,0,colorR),map(patterntime,0,TIMEON,0,colorV),map(patterntime,0,TIMEON,0,colorB)));
  }
  else if(patterntime<TIMESTARTOFF)//Si on est dans la deuxième phase on est à pleine luminausité
  {
    pixels.fill(pixels.Color(colorR,colorV,colorB));
  }
  else if(patterntime<TIMEENDOFF)//Si on est dans la troisième phase on est à réduit la luminausité proportionnelement
  {
    pixels.fill(pixels.Color(map(patterntime,TIMESTARTOFF,TIMEENDOFF,colorR,0),map(patterntime,TIMESTARTOFF,TIMEENDOFF,colorV,0),map(patterntime,TIMESTARTOFF,TIMEENDOFF,colorB,0)));
  }
  else //Sinon on est à la fin du pattern
  {
    //on remet à 0 le temps de départ
    startpatterntime=0;
    //on éteint les leds 
    pixels.fill(pixels.Color(0,0,0));
  }
  //On applique les changements au niveau des leds
  pixels.show();
}

//Fonction étant exécutée en boucle
void loop() {

  //on met a jour les objets en focntion du bouton principale et du capteur de mouvement
  bouton.update();
  pir.update();
  //on récupère la valeur du bouton rotatif
  int encDiff= calculEncDiff();
  //S'il a bougé on affiche le nombre de pas
  if(encDiff!=0)Serial.println(encDiff);
  //on gère le menu et l'alarme
  menu(encDiff);

  //On récupère la nouvelle heure depuis le capteur
  if (!Rtc.IsDateTimeValid()) 
  {
      if (Rtc.LastError() != 0)
      {
          Serial.println(Rtc.LastError());
      }
      else
      {
          Serial.println("RTC lost confidence in the DateTime!");
      }
  }
  now = Rtc.GetDateTime();
  //Si la minute acteulle a changée on vérifie l'alarme
  if(minute != now.Minute())
  {
    minute=now.Minute();
    //Si l'heure et les minutes correspondent on déclenche l'alarme
    if(now.Minute()==aminute && now.Hour()==aheure && mode==0 && alarmeon)
    {
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.clear();
      setAlarme(true);
    }
  }
  //Si il y a une détection de mouvement on déclenche le pattern
  if ( pir.rose() && startpatterntime==0) {
    startpatterntime=millis();//on enregistre le temps de démarage du pattern (en milliseconde)
    Serial.println("Detect");
  }
  //Si le temps de démarage est différent de 0, un détection est en cours, donc on affiche le pattern lumineux
  if(startpatterntime!=0)
    looppattern();
  //on met en pause 1 milliseconde, ce qui économise de l'énergie
  delay(1);
}
