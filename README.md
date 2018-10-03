# ArduinoGenericBoard
Generic board for Arduino Nano and ENC28J60 Shield (optional)

PCB layout, fritzing included, gerber files in zip also included for Arduino Nano with printer 3D Case
Case is able to set on DIN rail
+ With optional shield RJ45 ENC28J60 - drive by UIPEthernet.h via SPI 10-11-12-13 pins. 
+ With 2 shift registers (SN74HC595) and 2 drivers (ULN 2003) witch provide 16 extra OUTs (14 drived by ULN provide high current outputs, one connected to reed relay) (only use D7, D8, D9 for synchrone serial)
+ With 7 optocouplers 4N25 or equ. (6 for input ,1 for output).All pul-up resistors could be individual or resistor type B array
+ With Voltage regulator (7805).It can be added a cooling radiator (on the bottom side if shield ENC28J60 plugs as picture show)

All the input/output are on screw terminal (protected by case) arround the case and both (almost) on a 2.54 SIL connector.

A sample of arduino program (to be continued) is also included.


Boitier (sur rail DIN) pour une carte arduino Nano (Tracé de circuit Frtizing inclus, fichiers  des gerber en zip aussi) incluant :
+ La place pour une carte ENC28J60 shield internet pour Nano (utilisant la liaison SPI via les E/S D10,11,12 et 13).
Cette carte est optionnelle ces E/S sont sinon disponibles sur les borniers.
+ 2 registres à décalage (SN74HC595) accolés à deux drivers (ULN2003) procurant 14 sorties fort courant plus une connectée à un relais reed.
Ces sorties n'utilisent que 3 E/S de l'arduino (D7,8 et9).
+ 6 Entrées optocouplées et une sortie optocouplée via des 4N25 (ou équivalents). Toutes les résistances de tirages peuvent être individuelles ou des réseaux de resistances de type B.
+ Un régulateur de tension (7805) avec radiateur (celui-ci doit être monté coté soudure en cas d'utilisation du shield internet , comme sur les photos jointes).

Toutes les entrées/ sorties sont disponibles sur des bornes à vis (d'accès protégé par le boitier) disposées tout autour de celui-ci et pour la plupart disponibles sur des connecteurs SIL au pas de 2.54.

Un petit exemple de programme arduino (à compléter) est également disponible.







