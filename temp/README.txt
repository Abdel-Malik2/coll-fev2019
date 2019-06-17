<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/fr/"><img alt="Licence Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/3.0/fr/88x31.png" /></a><br />Les différentes partie de ce projet sont mise à disposition selon les termes de la <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/fr/">Licence Creative Commons Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France</a>.

---

Horloge avec thermomètre intégré.
Lorque l'on passe la main au dessus la température s'affiche.
La couleur de l'horloge change en fonction de la température. 



# Programmation
- Installer le logiciel arduino [lien](https://www.arduino.cc/en/Main/Software)
- Installer les bibliothèques suivantes dans l'IDE Arduino (Croquis>Inclure une  bibliotèque> Gerer les bibliothèques )
	- Rtc by Makuna
	- Grove - LCD RGB Backlight
	- Grove - RTC DS1307
	- Adafruit NeoPixel
- Suivre le tutoriel suivant pour installer la dernière bibliothèque http://wiki.seeedstudio.com/Grove-I2C_High_Accuracy_Temperature_Sensor-MCP9808/#software
- Dans le menu Outils->type carte choisir "Arduino Uno"
- Dans le menu Outils->port choisir le port de la liaison serie (si il y en a plusieurs vous devez choisir celui qui apparait quand vous branchez la carte)
- Televerser le programme sur la carte

# Liste du matériel:
- Arduino Uno
- Grove - Base Shield V2
- Grove - 16 x 2 LCD
- Grove - RTC
- Grove - I2C High Accuracy Temperature Sensor(MCP9808)
- Grove - Branch Cable for Servo
- Capteur infrarouge E18-D80NK
- Ruban LED RGB  WS2812B (densité 60led/m)
- Contreplaqué peuplier 5mm 
- Plexiglass semi opaque blanc 3mm

# Outils:
- Découpeuse laser
- Pistolet à colle
- Fer à souder


# Electronique:
- Mettre l'intérupteur du Base Shield V2 sur 5V
- Connecter le Grove - 16 x 2 LCD sur un port I2C du Base Shield V2
- Connecter le Grove - RTC sur un port I2C du Base Shield V2
- Connecter le Grove - Temperature Sensor sur un port I2C du Base Shield V2
- Connecter le Grove - Branch Cable for Servo sur le port D2 du Base Shield V2
- Connecter le Capteur infrarouge E18-D80NK sur le fil jaune du Grove - Branch Cable for Servo (rouge/rouge, vert/noir, jaune/jaune)
- Decouper un ruban de 9 LED WS2812B et le cabler pour le connecter au second cable du Grove - Branch Cable for Servo

# Mecanique
- Couper le boitier dans du Contreplaqué peuplier 5mm
- Couper les lames en plexiglass semi-opaque 3mm
- Mettre à plat les faces du boitier comme indiqué dans le fichier svg
- Positionner les lames dans le boitier et coller les au extrémitées
- Coller le ruban de led par dessus les lames
- Intaller le capteur Capteur infrarouge E18-D80NK en le vissant sur la face du dessus
- Placer l'écran Grove - 16 x 2 LCD sur la face. Mettre un point de colle pour le fixer.
- Inserer le capteur Grove - Temperature Sensor dans la fente sur la face arrière.
- Replier les faces pour fermer le boîtier en positionnant bien la carte arduino.