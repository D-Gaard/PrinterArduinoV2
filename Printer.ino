/*
  Program: Printer | med scanner og printer funktion

  Krav: Ses i rapportens afsnit: "Kravspecifikationer"

  Af: Mads Daugaard | Marts - Maj 2019
*/

//Informationer:
//PRINTER MAKSIMUM STØRRELSE:
//  70-72STEPS
// -----------
//|           |1
//|           |0
//|           |5
//|           |S
//|           |T
//|           |E
//|           |P
//|           |S
// -----------
//
//EM DEALAY 100 milisekunder
//Distance mellem tusch og scanner: 14-15steps med x
//
// Den del af papiret som vil blive tegnet på
// 19   32   19
// ------------
//|   |    |   |
//|   |    |   | 35
//|---|----|---|
//|   |    |   |
//|   |    |   | 40
//|---|----|---|
//|   |    |   |
//|   |    |   | 30
// ------------
//

//GLOBALE KONSTANTER
//Digital porte 0-13
const byte portA = 2; //4051 A
const byte portB = 3; //4051 B
const byte portC = 4; //4051 C
const byte portD = 5; //4051 disable in/out
const byte LED1 = 6;  //LED1
const byte LED2 = 7;  //LED2

//Analog porte A0-A5
const char portSensor = A0; //sensor port som læser lysstyrken (spænding 0-1023)
const char portEM = A1; //Elektromagnet port bruges til at tænde og slukke for EM (0-255)

//motor til port 0-7 på 4051
const byte s1y = 0; //1
const byte s2y = 1; //2
const byte s3y = 2; //3
const byte s4y = 3; //4

const byte s1x = 5; //2
const byte s2x = 4; //1
const byte s3x = 7; //4
const byte s4x = 6; //3

//print
const int printDelay = 120;

//størrelse af scannerArray
const int stepsX = 32;//skal kunne divideres med 8
const int stepsY = 40;

//papir kant
int papirX = 70; //total #steps x
int papirY = 105; //total #steps y
int papirScanX = 32; //#steps for x ved scan
int papirScanY = 40; //#steps for y ved scan

//GLOBALE VARIABLE
//scanning
int shLimit = 751; //Grænse for hvornår farven er sort/hvid | over -> sort - under ->hvid

//Array
//Scanner array - (x divideres med 8 pga. der er 8 1/0 i en byte) | Der anvendes ikke boolean, da den fylder en byte i kompileren | Arrayet fyldes med en figur til at starte med
byte scanArray[stepsY][(stepsX / 8)]{
  {0, 0, 0, 0},
  {255, 255, 255, 255},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {191, 0, 0, 253},
  {191, 0, 0, 253},
  {191, 0, 0, 253},
  {191, 0, 0, 253},
  {191, 0, 0, 253},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {143, 0, 0, 241},
  {143, 0, 0, 241},
  {143, 0, 0, 241},
  {143, 255, 255, 241},
  {143, 255, 255, 241},
  {143, 255, 255, 241},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {128, 0, 0, 1},
  {255, 255, 255, 255},
  {0, 0, 0, 0},
  {128, 65, 65, 1},
  {128, 99, 99, 1},
  {128, 85, 85, 1},
  {128, 73, 73, 1},
  {128, 65, 65, 1}
};

//Serial monitor
String Storage;

//--------------Kode-------------------


//*************************************
//Navn: steup
//Formål: Initialiser porte og klargør system
//Af: Mads Daugaard | April - 2019
//*************************************
void setup() {
	//pinModes digital 0-13
	pinMode(portA, OUTPUT); //2
	pinMode(portB, OUTPUT); //3
	pinMode(portC, OUTPUT); //4
	pinMode(portD, OUTPUT); //5
	pinMode(LED1, OUTPUT);  //6
	pinMode(LED2, OUTPUT);  //7

	//pinModes analog A0-A5
	pinMode(portSensor, INPUT); //A0
	pinMode(A1, OUTPUT); //A1

	//serial
	Serial.begin(9600); //starter seriel monitor

	//nulstilling / nødvendig opsætning
	digitalWrite(portD, HIGH); //tænder 4051 diasble port for at forhindre outputs i at modtage strøm
	analogWrite(portEM, 255); //trækker tuschen op
	digitalWrite(LED1, HIGH); //tænder LED 1
	digitalWrite(LED2, HIGH); //tænder LED 2

}

//*************************************
//Navn: loop
//Formål: Styrer hele programmet og looper
//Af: Mads Daugaard | April - 2019
//*************************************

void loop() {
	//tjekker om der sendes noget til seriel
	if (Serial.available() > 0) {
		//der er inkomende information (seriel er aktiv)
		Storage = Serial.readStringUntil(';'); //læser data indtil ; fremkommer

		//tjekker om komandoen modtaget matcher nogle kontroltermer
		if (Storage == "testPrint") {
			//tester print funktion
			Serial.println("print");
			printEM(printDelay);
		}
		else if (Storage == "printPK") {
			//Printer en kant rundt på papiret hvor der kan printes og scannes
			Serial.println("Tegner Papirsets kant");
			tegnPapirKant();
			Serial.println("Papirets kant er tegnet");
		}
		else if (Storage == "printSK") {
			//printer kanten af den indre firkant som er størrelse der bruges  
			Serial.println("Tegner Scaningens kant");
			tegnScaningKant(papirScanX, papirScanY);
			Serial.println("Kanten er tegnet");
		}
		else if (Storage == "scan") {
			//Scanner et område
			Serial.println("Scanner Papir");
			scanFelt();
			Serial.println("Scanning færdig");
		}
		else if (Storage == "printScan") {
			//printer det scannede område
			Serial.println("printerScanning");
			printFelt();
			Serial.println("Print Færdig");
		}
		else if (Storage == "limitL") {
			//indstiller scanner niveau til Lav
			Serial.println("Limit 745");
			shLimit = 745;
		}
		else if (Storage == "limitM") {
			//indstiller scanner niveau til Middel
			Serial.println("Limit 745");
			shLimit = 751;
		}
		else if (Storage == "limitH") {
			//indstiller scanner niveau til Høj
			Serial.println("Limit 745");
			shLimit = 755;
		}
		else if (Storage == "lysOn") {
			//tænder LED'er
			Serial.println("Tænder lys");
			digitalWrite(LED1, HIGH);
			digitalWrite(LED2, HIGH);
		}
		else if (Storage == "lysOff") {
			//slukker LED'er
			Serial.println("Slukker lys");
			digitalWrite(LED1, LOW);
			digitalWrite(LED2, LOW);
		}
		else if (Storage == "fx1") {
			//stepper 1 step med x
			Serial.println("step 1x frem");
			runMotorFrem('x', 20);
		}
		else if (Storage == "fy1") {
			//stepper 1 step med y
			Serial.println("step y1 frem");
			runMotorFrem('y', 40);
		}
		else if (Storage == "scan1") {
			//viser en scan måling
			Serial.println("Scan");
			Serial.println(analogRead(A0));
		}
		else if (Storage == "scan1L") {
			//scanner en linje
			Serial.println("Scan");
			for (int n = 0; n < 70; n++) {
				runMotorFrem('x', 20);
				Serial.println(analogRead(A0));
			}
			resetMotor(70, 0);
		}
		else if (Storage == "stopEM") {
			//slukker for tusch
			Serial.println("stop EM");
			analogWrite(portEM, 0);
		}
		else if (Storage == "startEM") {
			//tænder for tusch
			Serial.println("start EM");
			analogWrite(portEM, 255);
		}
		else if (Storage == "visScan") {
			//viser scannet i seriel monitoren
			Serial.println("viser scanning: ");
			printAlt();
		}
		else {
			//fortæller brugeren at der er skrevet forkert
			Serial.println("Ukendt komando");
		}

		//Lukker serial, for at fjerne unødvidigt resterende output | .flush() havde ikke den ønskede effekt
		Serial.end();
		//åbner seriel, så beskeder igen kan modtages
		Serial.begin(9600);
	}

}
//*************************************
//Navn: scanFelt
//Formål: Scanner et område og gemmer det
//Af: Mads Daugaard | April - 2019
//*************************************
void scanFelt() {
	papirSetup(19 - 14, 30); //kører motoren til det højre nederste hjørne
	Serial.println("start");
	tagScanning(); //scanner papiret
	Serial.println("retur");
	resetMotor(19 - 14, 70 - 5); //resetter motor til start pos
	Serial.println("done");
}
//*************************************
//Navn: printFelt
//Formål: printer det sidste scan på papiret
//Af: Mads Daugaard | April - 2019
//*************************************
void printFelt() {
	papirSetup(19, 30); //kører motoren til det højre nederste hjørne
	printArray();       //printer motiv
	resetMotor(19, 70); //resetter motor til start pos
}

//*************************************
//Navn: papirSetup
//Formål: Kører motoren i startposition
//Af: Mads Daugaard | April - 2019
//*************************************
void papirSetup(int motorXStartPos, int motorYStartPos) {
	//kør motoren på x-aksen i position
	for (int n = 0; n < motorXStartPos; n++) {
		runMotorFrem('x', 25);
	}
	//kør motoren på y-aksen i position
	for (int n = 0; n < motorYStartPos; n++) {
		runMotorFrem('y', 45);
	}
}

//*************************************
//Navn: farveScan
//Formål: Scaner A0 og returnere om det er sort eller hvid 
//Af: Mads Daugaard | April - 2019
//*************************************
byte farveScan() {
	//undersøger fraven ud fra en scanning på en analogport
	if (analogRead(portSensor) > shLimit) {
		//den er sort (1)
		return 1;
	}
	else {
		//den er hvid (0)
		return 0;
	}
}

//*************************************
//Navn: tagScanning
//Formål: Scanner et område på papiret
//Af: Mads Daugaard | April - 2019
//*************************************
void tagScanning() {
	//looper gennem hele arrayet
	for (int y = 0; y < stepsY; y++) {
		//yaksen
		for (int x = 0; x < stepsX / 8; x++) {
			//xaksen
			for (byte counter = 0; counter <= 7; counter++) {
				//alle 8 pladser i en byte
				if (counter == 0) {
					//der står intet på pladsen, så den er bare ligmed 0/1 | dette sørger samtidigt for at initialisere pladsen
					scanArray[y][x] = farveScan();
				}
				else {
					//der står mere i arrayet, derfor skal alt rykkes 1 mod venstre og indsættes 1/0 på den højre mest plads
					byte a = scanArray[y][x]; //gemmer værdi fra array

					byte b = (a << 1); //shifter værdi og gemmer denne
					scanArray[y][x] = (farveScan() | b); //sammenligner med "or" og gemmer den nye streng

				}
				//stepper næste step med motorn på x-aske
				runMotorFrem('x', 20);
				delay(60); //delay for mere tid mellem hert scan -> bedre kvalitet
			}
		}
		//stepper næste stepå på y-aksen
		runMotorFrem('y', 40);

		//run x tilbage til "start"
		for (int n = 0; n < stepsX; n++) {
			runMotorBagud('x', 20); //køre motoren tilbage på x-aksen
		}

	}
}

//*************************************
//Navn: printEM
//Formål: laver en prik på papiret
//Af: Mads Daugaard | April - 2019
//*************************************
void printEM(int delays) {
	//dropper tuschen
	analogWrite(portEM, 0);
	delay(delays * 2);
	//trækker den op igen
	analogWrite(portEM, 255);
	delay(delays);
}
//*************************************
//Navn: printArray
//Formål: printer hvad der er blevet scannet
//Af: Mads Daugaard | April - 2019
//*************************************
void printArray() {
	//looper gennem hele arrayet
	for (int y = 0; y < stepsY; y++) {
		//yaksen
		for (int x = 0; x < stepsX / 8; x++) {
			//xaksen
			for (byte counter = 0; counter <= 7; counter++) {
				//alle 8 pladser i en byte (8 steps på x-aksen)
				if (scanArray[y][x] > 127) {
					//tallet er større end 127 ergo står der 1 på den første plads i byten, og der skal printes
					printEM(printDelay);
				}
				//shifter mod venstre for at få den næste værdi frem
				byte a = scanArray[y][x];
				scanArray[y][x] = a << 1;

				//stepper næste step med motorn
				runMotorFrem('x', 25);
			}
		}
		//stepper næste stepå på y-aksen
		runMotorFrem('y', 40);

		//run x tilbage til "start"
		for (int n = 0; n < stepsX; n++) {
			runMotorBagud('x', 25);
		}
	}
}

//*************************************
//Navn: tegnScaningKant
//Formål: Tegner en kant hvor der vil blive scannet
//Af: Mads Daugaard | April - 2019
//*************************************
void tegnScaningKant(int stepsX, int stepsY) {

	//tegner en kant for det område på papiret som vil blive scannet
	papirSetup(19, 30); //kører motoren til det højre nederste hjørne

	//tegner kanten
	for (int n = 0; n < stepsX; n++) {
		//stepper x-aksen
		runMotorFrem('x', 20);
		//printer en gang efter hvert step
		printEM(printDelay);
	}

	for (int n = 0; n < stepsY; n++) {
		//stepper y-aksen
		runMotorFrem('y', 40);
		//printer en gang efter hvert step
		printEM(printDelay);
	}
	for (int n = 0; n < stepsX; n++) {
		//stepper x-aksen
		runMotorBagud('x', 20);
		//printer en gang efter hvert step
		printEM(printDelay);
	}

	for (int n = 0; n < stepsY; n++) {
		//stepper y-aksen
		runMotorBagud('y', 40);
		//printer en gang efter hvert step
		printEM(printDelay);
	}
}

//*************************************
//Navn: tegnPapirKant
//Formål: Tegner en kant langs papirets ydre
//Af: Mads Daugaard | April - 2019
//*************************************
void tegnPapirKant() {
	//stepper lang hele papirets kant og printer en gang pr. sted
	for (int n = 0; n < papirX; n++) {
		//stepper x-aksen
		runMotorFrem('x', 20);
		//printer en gang efter hvert step
		printEM(printDelay);
	}

	for (int n = 0; n < papirY; n++) {
		//stepper y-aksen
		runMotorFrem('y', 40);
		//printer en gang efter hvert step
		printEM(printDelay);
	}
	for (int n = 0; n < papirX; n++) {
		//stepper x-aksen
		runMotorBagud('x', 20);
		//printer en gang efter hvert step
		printEM(printDelay);
	}

	for (int n = 0; n < papirY; n++) {
		//stepper y-aksen
		runMotorBagud('y', 40);
		//printer en gang efter hvert step
		printEM(printDelay);
	}
}
//*************************************
//Navn: runMotorFrem
//Formål: Kører en mortor 1 step fremad
//Af: Mads Daugaard | April - 2019
//*************************************
void runMotorFrem(char akse, int delays) {
	//tænder for 4051
	digitalWrite(portD, LOW);
	delay(delays);

	//kører den givne stepper ud fra input "akse"
	if (akse == 'x') {
		//run motor fremad x aksen
		sandKonv(s1x);
		delay(delays);

		sandKonv(s2x);
		delay(delays);

		sandKonv(s3x);
		delay(delays);

		sandKonv(s4x);
		delay(delays);

	}

	if (akse == 'y') {
		//run motor fremad y aksen
		sandKonv(s1y);
		delay(delays);

		sandKonv(s2y);
		delay(delays);

		sandKonv(s3y);
		delay(delays);

		sandKonv(s4y);
		delay(delays);
	}

	//slukker for 4051
	digitalWrite(portD, HIGH);
}

//*************************************
//Navn: runMotorBagud
//Formål: Kører en mortor 1 step bagud
//Af: Mads Daugaard | April - 2019
//*************************************
void runMotorBagud(char akse, int delays) {
	//tænder for 4051
	digitalWrite(portD, LOW);
	delay(delays);

	//kører den givne stepper ud fra input "akse"
	if (akse == 'x') {
		sandKonv(s4x);
		delay(delays);

		sandKonv(s3x);
		delay(delays);

		sandKonv(s2x);
		delay(delays);

		sandKonv(s1x);
		delay(delays);

	}
	else if (akse == 'y') {
		sandKonv(s4y);
		delay(delays);

		sandKonv(s3y);
		delay(delays);

		sandKonv(s2y);
		delay(delays);

		sandKonv(s1y);
		delay(delays);
	}

	//slukker for 4051
	digitalWrite(portD, HIGH);
}

//*************************************
//Navn: resetMotor
//Formål: Kører printerhovedet tilbage til en position
//Af: Mads Daugaard | April - 2019
//*************************************
void resetMotor(int xsteps, int ysteps) {
	//reset motor
	for (int n = 0; n < xsteps; n++) {
		//reset xakse motor
		runMotorBagud('x', 40);
	}

	for (int n = 0; n < ysteps; n++) {
		//reset yakse motor
		runMotorBagud('y', 40);
	}
}
//*************************************
//Navn: sandKonv
//Formål: Konverter en værdi fra 0-7 til den tilsvarende ben på 4051
//Af: Mads Daugaard | April - 2019
//*************************************
void sandKonv(int value) {
	//opsætter variable
	byte valA = 0;
	byte valB = 0;
	byte valC = 0;

	//Undersøg hvilken motor der skal tændes
	/*NR |A|B|C|
	  0  |0|0|0|
	  1  |1|0|0|
	  2  |0|1|0|
	  3  |1|1|0|
	  4  |0|0|1|
	  5  |1|0|1|
	  6  |0|1|1|
	  7  |1|1|1|
	*/
	if (value == 0) {
		//tænd nr 1
		valA = 0;
		valB = 0;
		valC = 0;
	}
	if (value == 1) {
		//tænd nr 2
		valA = 1;
		valB = 0;
		valC = 0;
	}

	if (value == 2) {
		//tænd nr 3
		valA = 0;
		valB = 1;
		valC = 0;
	}

	if (value == 3) {
		//tænd nr 4
		valA = 1;
		valB = 1;
		valC = 0;
	}

	if (value == 4) {
		//tænd nr 5
		valA = 0;
		valB = 0;
		valC = 1;
	}

	if (value == 5) {
		//tænd nr 6
		valA = 1;
		valB = 0;
		valC = 1;
	}

	if (value == 6) {
		//tænd nr 7
		valA = 0;
		valB = 1;
		valC = 1;
	}

	if (value == 7) {
		//tænd nr 8
		valA = 1;
		valB = 1;
		valC = 1;
	}

	//tænder for portene således at der vælges en port fra 0-7
	digitalWrite(portA, valA);
	digitalWrite(portB, valB);
	digitalWrite(portC, valC);
}

//*************************************
//Navn: printAlt
//Formål: printer hele arayet til seriel monitor
//Af: Mads Daugaard | April - 2019
//*************************************
void printAlt() {
	for (int y = 0; y < stepsY; y++) {
		//yaksen
		for (int x = 0; x < stepsX / 8; x++) {
			//xaksen

			byte val = scanArray[y][x]; //gemmer en byte fra arrayet
			for (int i = 7; i >= 0; i--)
			{
				bool b = bitRead(val, i); //læser hver enkelt værdi fra en byte
				Serial.print(b); //printer hver bit
			}

			Serial.print(" "); //laver et mellemrum mellem hver byte

		}
		Serial.println(""); //laver et linje skift mellem hver række
	}
}