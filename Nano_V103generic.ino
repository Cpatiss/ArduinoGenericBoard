
#define withinternet "yes" // commenté ou décommenté pour utilisation du shield ENC28J60/ comment or uncomment for use or not ENC28J60 Shield
#ifdef withinternet 
	#include <UIPEthernet.h>
	#include <SPI.h>
#else
	//#include <UIPEthernet.h> // certains compilateur reclame cette ligne / somme compiltor nedd this line
#endif

#define indebugmode "yes" // mode débuguage / mode debug

#define uchar unsigned char // pour faciliter code / for easier code
#define uint  unsigned int // pour faciliter code / for easier code

/* base sur le circuit imprime Nano V1.03 fritzing avec un arduino nano 
based on nano v1.03 Board see fritzing project with arduino nano chip  */

/* utilisation de deux '74HC595' (registre a decalage 8 bits) montes en serie pour generer 16 sorties (14 driver par dalington ULN2003) avec 3 pins seulement
	to drive 16 outputs (14 drive through darlington, ULN2003) via two 74HC595 (shift register) with only 3 pins */
#define pin_Ser  9   //pin 14 (ser) on the 75HC595 n° 1 data serial
#define pin_RClk  8  //pin 12 on the 75HC595 (RCLK) = data set
#define pin_SrClk  7 //pin 11 on the 75HC595 (SRCLK) = Data shift

uchar FirstOutputValue = '\0'; // valeurs des 8 premieres sorties (74hc595 n°1) / value of the first 74hc595 8 first outputs
uchar SecondOutputValue = '\0'; // valeurs des 8 dernieres sorties (74hc595 n°2) / value of the second 74hc595 8 lest outputs values

#ifdef withinternet 
  #define pin_I13_Sck 13 // pin D13 entree  optocouple (sck) vers moduel ethernet / D13 input sck for shield internet
  #define pin_I12_Miso 12 // pin D12 en entree optocouple  (miso) vers module ethernet / D12 input miso for shield internet
  #define pin_O11_Mosi 11 // pin D11 en sortie optocouple (mosi) vers module ethernet / D11 output mosi for shield internet
  #define pin_O10_Css 10 // pin D10 en sortie module ethernet (css) vers module ethernet / D13 output css for shield internet
#endif
#define pin_I06 6 // pin D6 en entree optocouple (rx manchester filaire ou rf)
#define pin_O05 5 // pin D5 en Sorties directe (out RF ou filaire manchester)
#define pin_IO04 4 // pin D4 en E/S directe
#define pin_IO03 3 // pin D3 en E/S directe
#define pin_IO02 2 // pin D2 en E/S directe

#define pin_IO01 1 // pin D1 tx/rx serial use with usb/serial link not use on board nano v1.03
#define pin_IO00 0 // pin D0 tx/rx serial use with usb/serial link not use on board nano V1.03

#define pin_IA0 A0 // pin A0 en entree optocouple (...)
#define pin_IA1 A1 // pin A1 en entree optocouple (...)
#define pin_IA2 A2 // pin A2 en entree optocouple (...)
#define pin_IOA3 A3 // pin A3 en entree sortie directe (...)
#define pin_IOA4 A4 // pin A4 en entree sortie directe (...)
#define pin_IOA5 A5 // pin A5 en sortie directe (...)
#define pin_IOA6 A6 // pin A6 en entree sortie directe (...)
#define pin_IOA7 A7 // pin A7 en entree sortie directe (...)

//def & init for machester
#define BUFFER_SIZE 4 // 3*8 bits = 24 bits (ou 32 bits passer alors  4)  habituel de manchester code chacon (voir pour diagral) par exemple, changer selon besoin
// when use manchester number of bytes used (3 for 24bits long message , 4 for 32 ..) change depend on define bellow
// Modifier aussi le traitement des codes recus dans LOOP // Change also recieve sub in loop function

uint8_t buffer[BUFFER_SIZE + 1];
#ifdef indebugmode
	// int serialNumModule = 0; // numero du module pour les envois serial (/0 ,/1,/2,/3,/4...)
#endif


#ifdef withinternet
	// L'adresse MAC du shield // mac adress of shield
	byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xA5, 0x7D };
	// L'adresse IP que prendra le shield // fixed ip 
	IPAddress ip(10, 10, 1, 85);

	// Initialise notre serveur / initialize server
	// Ce dernier ecoutera sur le port 80 / server listen on port 80
	EthernetServer serveur(80);

	String url = String("") ; // L'url recu a stocker / to store url received from client
	//char url[100];
	char index = 0; // index indiquant ou l'on est rendu dans la chaine / index of current character of url
#endif


void setup()
{

  // pins 
  #ifdef withinternet 
	  pinMode(pin_I13_Sck, INPUT); // e/s horloge spi web / input/out clock shield web
	  pinMode(pin_I12_Miso , INPUT); // entrée miso spi web / input miso shield web
  #else
    pinMode(pin_I13_Sck, INPUT); // entrée optocouplée / optocoupled input
    pinMode(pin_I12_Miso , INPUT); // entrée optocouplée / optocoupled input
  #endif
	pinMode(pin_I06, INPUT); // input rx (rf ou filaire) manchester
	pinMode(pin_IA0, INPUT);
	pinMode(pin_IA1, INPUT);
	pinMode(pin_IA2, INPUT);
	
  #ifdef withinternet 
	  pinMode(pin_O11_Mosi, OUTPUT); // sortie mosi spi web  / out mosi shield web
	  pinMode(pin_O10_Css, INPUT);// E/sortie chip select shield web / out Css shield web
  #else
    pinMode(pin_O11_Mosi, OUTPUT); // sortie optocouplé  / optocoupled out 
    pinMode(pin_O10_Css, INPUT);// Entrée optocouplée / optocoupled input
  #endif
	pinMode(pin_O05, OUTPUT); // E/S directe / in/out direct
  
	pinMode(pin_Ser, OUTPUT); // ligne data série synchrone registre à décalage / data serial syncrone for shift registers
  pinMode(pin_RClk, OUTPUT); // ligne stockage série synchrone registre à décalage / lacth serial syncrone for shift registers
	pinMode(pin_SrClk, OUTPUT); // ligne horloge série synchrone registre à décalage / clock serial syncrone for shift registers
	
	//pinMode(LED_BUILTIN, OUTPUT);// to test

	// init
	SetOutput495(); // initialise a 0 les sorties des registres 74595 / reset all shift register output

#ifdef indebugmode
	Serial.begin(9600);
#endif
	
	char erreur = 0;
#ifdef withinternet
	// On demarre le shield Ethernet SANS adresse ip (donc donnee via DHCP) // try to start ethernet in DHCP
	//erreur = Ethernet.begin(mac); // décommenté pour démarrer en dhcp /uncomment to start in dhcp
  // ip fixe obligatoire pour le système domotic
	//if (erreur == 0) {// décommenté pour démarrer en dhcp /uncomment to start in dhcp
		#ifdef indebugmode
				Serial.println("IP F");
		#endif
		// si une erreur a eu lieu cela signifie que l'attribution DHCP
		// ne fonctionne pas. On initialise donc en forcant une IP
		Ethernet.begin(mac, ip);
	//}// décommenté pour démarrer en dhcp /uncomment to start in dhcp
	#ifdef indebugmode
		Serial.println("I..");
		Serial.println(Ethernet.localIP());
	#endif
	// Donne une seconde au shield pour s'initialiser
	delay(1000);
	// On lance le serveur
	serveur.begin();
	#ifdef indebugmode
		Serial.println("P!");
	#endif
#endif

	

}// fin setup / end setup 

void loop()
{
	/* set here your loop code
   *  poser ici votre code programme
	 */
#ifdef withinternet
	// traite les requetes web / Web server listen
	// Regarde si un client est connecte et attend une reponse/ look if client connected
	EthernetClient client = serveur.available();
	
	if (client) { // Un client est la ? / a client is here ?
		#ifdef indebugmode
			Serial.println("i!");
		#endif
		url = String("                     "); // on remet a zero notre chaine tampon /  reset string
		index = 0;
		while (client.connected()) { // Tant que le client est connecte / while client connected
			if (client.available()) { // A-t-il des choses a dire ? / and client have someting to say
				
				// traitement des infos du client / operate client string
				char carlu = client.read(); //on lit ce qu'il raconte / read string
				if (carlu != '\n') { // On est en fin de chaine ? / end of client string
					// non ! alors on stocke le caractere / no we continue
					#ifdef indebugmode
						Serial.print(carlu);
					#endif
					url[index] = carlu;
					index++;
					if (index > 25){ index = 0; Serial.println(""); }
				}
				else {
					// on a fini de lire ce qui nous interesse / end of client string
					#ifdef indebugmode
						Serial.println("");
					#endif
					// on marque la fin de l'url (caractere de fin de chaine) / set end of string by chr 0
					url[index] = '\0';
					boolean ok = WebValid(client); // essaie d'interpreter la chaine / try to explot string
					if (ok) {
						// tout s'est bien passe = on met a jour les broches / if all is ok operate
           /* poser ici votre code de fin de traitement web 
            *  set here your end web treatment code 
            */
						//ction();
					}
					break;
				}
			}
		}
		// Donne le temps au client de prendre les donnees / left time client to recieve answer
		delay(10);
		// Ferme la connexion avec le client / close client connection
		client.stop();
		#ifdef indebugmode
				Serial.println("o!");
		#endif
	}
#endif

} // fin loop / end loop

#ifdef withinternet
	 boolean WebValid(EthernetClient client) {
		// renvoi true si la chaine est valide, traite les infos de la chaine
		//Serial.print("receive : ");
		//	Serial.print(url);
		int pos = url.indexOf("o="); // si message commence par o= alors met sorties à jour/ if message begin by 'o=' set output as sended
		//Serial.println(pos);
		if (pos > 0) {
			// renvoi ok 
			enteteClientWebJson(client);
			client.println("OK");
			//puis traite message
			Set3FristBuffer(pos);
			if (buffer[1] != 1) {
				// ordre <>1 met les sorties demandés dans le message / order <>1  set output as value sended 
				char tmp[2] = {'0', url[pos + 9]};
        int numModuleCourant = 0;
				numModuleCourant = atoi(tmp);
				// Serial.println(numModuleCourant);
				FirstOutputValue = (FirstOutputValue | 0x7F) & ( numModuleCourant);
				SetOutput495();
			
			}
			else {
				// ordre de modifier les sorties du nano
				if (( buffer[2] & 0x01 )==1) { 
				SecondOutputValue = buffer[2]>>1;
				SetOutput495();
				}
       
			}
			
			return true;
		}
		else // si le message ne commnce pas par 'o=' / if message not begin by 'o='
      // envoi la page Web des statuts des entrées et sorties / send web page with input/output status
			{
				pos = url.indexOf("S=");
				//Serial.println(pos);
				if (pos > 0) {
					Set3FristBuffer(pos);
					// puis message traite
					enteteClientWebJson(client);
					client.print("{\"ST\":");
					// client.print(lastHeaderGroup); // libre pour une donnée à envoyer de votre programme / free for a data to send of your own program
					client.print(",");
					//client.print(lastHeaderModule); // libre pour une donnée à envoyer de votre programme / free for a data to send of your own program
         /* // libre pour des données à envoyer de votre programme / free for multi datas to send of your own program
					for (int j = 0; j < NbType; j++) {
						client.print(",");
						client.print(statutModule[j]);
					}
					for (int j = 0; j < NbModule; j++) {
						client.print(",");
						client.print(positionModule[j]);
					}
         */
					client.print(",");
					client.print(digitalRead(A3) * 64 + digitalRead(A2) * 32 + digitalRead(A1) * 16 + digitalRead(A0) * 8 + digitalRead(4) * 4 + digitalRead(3) * 2 + digitalRead(2));
					for (int j = A0; j <= A7; j++) {
						client.print(",");
						client.print(analogRead(j));
					}
					client.print(",");
					client.print(FirstOutputValue);
					client.print(",");
					client.print(SecondOutputValue);
          
					client.println("}");
					return true;
				}
				else {
				// puis message traite
				enteteClientWebJson(client);
				client.println("ERR No Order");
				#ifdef indebugmode
					Serial.println("no tra");
				#endif
				return false;
				}
			
			}
		} //  find WebValide / end web valid
#endif
#ifdef withinternet
	 void Set3FristBuffer(int pos) {
		 // met les trois premières valeurs à partir de pos de l'url dans les buffers 1 et 2 avec 0 = 4 / treat 3 first characters of string in char array
		 buffer[0] = 4;
		 char ord[3] = { url[pos + 2], url[pos + 3], url[pos + 4] };
		 buffer[1] = atoi(ord);
		 ord[0] = url[pos + 5];
		 ord[1] = url[pos + 6];
		 ord[2] = url[pos + 7];
		 buffer[2] = atoi(ord);
	 }
	void enteteClientWebJson(EthernetClient client) {
		// envoi l'entete de reponse pour le client web en json / set headtext of jason message
		// Tout d'abord le code de reponse 200 = reussite
		client.println("HTTP/1.1 200 OK");
		// Puis le type mime du contenu renvoye, du json
		client.println("Content-Type: application/json");
		// Autorise le cross origin
		client.println("Access-Control-Allow-Origin: *");
		// Et c'est tout !
		// On envoi une ligne vide pour signaler la fin du header
		client.println();
	}
#endif
void SetOutput495()
{
	/* envoi les valeurs des sorties n°1  et 2 sur les 74Hc595 en effectuant des decalages a gauche
	send serial datas to two 74hc595 via shift integer*/
		

	/* lacth pin a la masse / ground latchPin and hold low for as long as you are transmitting */
	digitalWrite(pin_RClk, LOW);
	// envoi les deux bytes par decalage /send two datas bytes by shift //
	shiftOut(pin_Ser, pin_SrClk, MSBFIRST, SecondOutputValue);
	shiftOut(pin_Ser, pin_SrClk, MSBFIRST, FirstOutputValue);
   //Serial.println(FirstOutputValue);
  //Serial.println(SecondOutputValue);
	//lacth pin a +5 v / return the latch pin high to signal chip that it 
	//no longer needs to listen for information
	digitalWrite(pin_RClk, HIGH);
	// retour a l'etat initial de latch pin
	digitalWrite(pin_RClk, LOW);
}


