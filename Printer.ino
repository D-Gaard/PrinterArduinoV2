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
//
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
//
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

const byte s1x = 5; //6
const byte s2x = 4; //5
const byte s3x = 7; //8
const byte s4x = 6; //7

//scanning
const int shLimit = 648; //Grænse for hvornår farven er sort/hvid | over -> sort - under ->hvid

//størrelse af scannerArray
const int stepsX = 32;//skal kunne divideres med 8
const int stepsY = 40;

//papir kant
int papirX = 70; //total #steps x
int papirY = 105; //total #steps y
int papirScanX = 32; //#steps for x ved scan
int papirScanY = 40; //#steps for y ved scan

//GLOBALE VARIABLE
//Array
byte scanArray[(stepsX / 8) - 1][stepsY - 1]; //(der trækkes en fra da 0 tæller med og x divideres med 8 pga. der er 8 1/0 i en byte) | Der anvendes ikke boolean, da den fylder en byte i kompileren 
//Serial
String Storage;
//-----------------------------------------------------------------

//TEST VÆRDIER SOM SKAL FJERNES VED ENDELIGE VERSION



int a4y = 80; //#steps for y-aksen på papir
int a4x = 30; //#steps fo y-aksen på papiret

//overvej at teste stepper arrayet, og se om man kan steppe et step ad gangen

//*************************************
//Navn: steup
//Formål: Initialiser porte og klargør system
//Af: Mads Daugaard | April-Maj - 2019
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
	Serial.begin(9600);

	//nulstilling / nødvendig opsætning
	digitalWrite(portD, HIGH); //tænder 4051 diasble port for at forhindre alle outputs i at modtage strøm
	//analogWrite(portEM, 255); //trækker tuschen op

	//test setups
	analogWrite(A1, 255);
	digitalWrite(6, HIGH);
	digitalWrite(7, HIGH);
	int xpapir = 0;
}

//*************************************
//Navn: loop
//Formål: Styrer hele programmet og looper
//Af: Mads Daugaard | April-Maj - 2019
//*************************************

void loop() {

	if (Serial.available() > 0) {
		Storage = Serial.readStringUntil(';');
		// Serial.println('1');

		//tjekker om komandoen modtaget matcher nogle kontroltermer
		if (Storage == "print") {
			Serial.println("print");
			printEM(120);
		}
	
		else if (Storage == "printPK") {
Serial.println("Tegner Papirsets kant");
tegnPapirKant();
Serial.println("Papirets kant er tegnet");
		}
		else if (Storage == "printSK") {
		Serial.println("Tegner Scaningens kant");
		tegnScaningKant(papirScanX, papirScanY);
		Serial.println("Kanten er tegnet");
		}
		else if (Storage == "scan") {
		Serial.println("Scanner Papir");
		scanFelt();
		Serial.println("Scanning færdig");
		}
		else if (Storage == "printScan") {
		Serial.println("printerScanning");
		printFelt();
		Serial.println("Print Færdig");
		}
		else if (Storage == "fx1") {
		Serial.println("step 1x frem");
		runMotorFrem('x', 20);
		}
		else if (Storage == "fx10") {
		Serial.println("step 10x frem");
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		runMotorFrem('x', 20);
		}
		else if (Storage == "fy1") {
		Serial.println("step y1 frem");
		runMotorFrem('y', 40);
		}
		else if (Storage == "fy10") {
		Serial.println("step 10y frem");
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		runMotorFrem('y', 40);
		}
		else if (Storage == "scan1") {
		Serial.println("Scan");
		Serial.println(analogRead(A0));
		}
		else if (Storage == "scan1L") {
			Serial.println("Scan");
			for (int n = 0; n < 70; n++) {
				runMotorFrem('x', 20);
				Serial.println(analogRead(A0));
			}
			resetMotor(70, 0);
		}
		else if (Storage == "stop") {
		Serial.println("stop EM");
		analogWrite(portEM, 0);
		}
		else if (Storage == "start") {
		Serial.println("start EM");
		analogWrite(portEM, 255);
		}
		else if (Storage == "visScan") {
			Serial.println("viser scanning: ");
			printAlt();
		}
		else {
		Serial.println("Ukendt komando");
		}

		//Lukker serial, for at fjerne unødvidigt resterende output | .flush() havde ikke den ønskede effekt
		Serial.end();
		//åbner seriel, så beskeder igen kan modtages
		Serial.begin(9600);
	}



	/* for (int n = 0; n < 6; n++) {
	   //reset xakse motor
	   runMotorBagud('y', 50);
	 }
	   //resetMotor(stepsX, stepsY); //sender motoren tilbage til start
	   delay(12000); //12 sekunder til at udskifte papir
	   printArray();  //printer den tidligere scanning
	   printAlt();   //printer arrayets værdier, burde være [0000-0000]
	   printDone = 1;
	   delay(2000);


	 /*
	   for (int a=10;a<1000;a=a+10){
	   analogWrite(A1,255);
	   delay(a);
	   analogWrite(A1,0);
	   delay(1000);
	   Serial.println(a);
	   }
	 */
}
void scanFelt(){
	papirSetup(19-14, 30); //kører motoren til det højre nederste hjørne
	tagScanning(); //scanner papiret
	resetMotor(19-14, 70); //resetter motor til start pos
}
void printFelt() {
	papirSetup(19, 30); //kører motoren til det højre nederste hjørne
	printArray();
	resetMotor(19, 70); //resetter motor til start pos
}

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
					scanArray[x][y] = farveScan();
				}
				else {
					//der står mere i arrayet, derfor skal alt rykkes mod venstre 1 og indsættes 1/0 på den højre mest plads
					byte a = scanArray[x][y];
					byte b = (a << 1);
					scanArray[x][y] = (farveScan() | b);

				}
				//stepper næste step med motorn på x-aske
				runMotorFrem('x', 40);
			}
		}
		//stepper næste stepå på y-aksen
		runMotorFrem('y', 60);

		//run x tilbage til "start"
		for (int n = 0; n < stepsX; n++) {
			runMotorBagud('x', 25);
		}

	}
}

void printEM(int delays) {
	//laver et print
	//dropper tuschen
	analogWrite(portEM, 0);
	delay(delays);
	//trækker den op igen
	analogWrite(portEM, 255);
	delay(delays);
}

void printArray() {
	//looper gennem hele arrayet
	for (int y = 0; y < stepsY; y++) {
		//yaksen
		for (int x = 0; x < stepsX / 8; x++) {
			//xaksen
			for (byte counter = 0; counter <= 7; counter++) {
				//alle 8 pladser i en byte (8 steps på x-aksen)
				if (scanArray[x][y] % 2 != 0) {
					//tallet er ikke lige og derfor står der 1 på den sidste plads i arrayey
					printEM(120);
				}
				//shifter mod venstre for at få den næste værdi frem
				byte a = scanArray[x][y];
				scanArray[x][y] = a >> 1;

				//stepper næste step med motorn
				runMotorFrem('x', 40);
			}
		}
		//stepper næste stepå på y-aksen
		runMotorFrem('y', 60);

		//run x tilbage til "start"
		for (int n = 0; n < stepsX; n++) {
			runMotorBagud('x', 25);
		}
	}
}

void tegnScaningKant(int stepsX, int stepsY) {

	//tegner en kant for det område på papiret som vil blive scannet
	papirSetup(19, 30); //kører motoren til det højre nederste hjørne

	//tegner kanten
	for (int n = 0; n < stepsX; n++) {
		//stepper x-aksen
		runMotorFrem('x', 20);
		//printer en gang efter hvert step
		printEM(220);
	}

	for (int n = 0; n < stepsY; n++) {
		//stepper y-aksen
		runMotorFrem('y', 40);
		//printer en gang efter hvert step
		printEM(220);
	}
	for (int n = 0; n < stepsX; n++) {
		//stepper x-aksen
		runMotorBagud('x', 20);
		//printer en gang efter hvert step
		printEM(220);
	}

	for (int n = 0; n < stepsY; n++) {
		//stepper y-aksen
		runMotorBagud('y', 40);
		//printer en gang efter hvert step
		printEM(220);
	}
}
void tegnPapirKant() {
	//stepper lang hele papirets kant og printer en gang pr. sted
	for (int n = 0; n < papirX; n++) {
		//stepper x-aksen
		runMotorFrem('x', 20);
		//printer en gang efter hvert step
		printEM(220);
	}

	for (int n = 0; n < papirY; n++) {
		//stepper y-aksen
		runMotorFrem('y', 40);
		//printer en gang efter hvert step
		printEM(220);
	}
	for (int n = 0; n < papirX; n++) {
		//stepper x-aksen
		runMotorBagud('x', 20);
		//printer en gang efter hvert step
		printEM(220);
	}

	for (int n = 0; n < papirY; n++) {
		//stepper y-aksen
		runMotorBagud('y', 40);
		//printer en gang efter hvert step
		printEM(220);
	}
}

void runMotorFrem(char akse, int delays) {
	//tænder for 4051
	digitalWrite(portD, LOW);
	delay(delays);

	//kører den givne stepper ud fra input "akse"
	if (akse == 'x') {
		//run motor fremad x aksen
		sandKonv(s1x);
		//Serial.println("1");
		delay(delays);

		sandKonv(s2x);
		//Serial.println("2");
		delay(delays);

		sandKonv(s3x);
		//Serial.println("3");
		delay(delays);

		sandKonv(s4x);
		//Serial.println("4");
		delay(delays);

	}

	if (akse == 'y') {
		//run motor fremad y aksen
		sandKonv(s1y);
		//Serial.println("1");
		delay(delays);

		sandKonv(s2y);
		//Serial.println("2");
		delay(delays);

		sandKonv(s3y);
		//Serial.println("3");
		delay(delays);

		sandKonv(s4y);
		//Serial.println("4");
		delay(delays);
	}

	//slukker for 4051
	digitalWrite(portD, HIGH);
}

void runMotorBagud(char akse, int delays) {
	//tænder for 4051
	digitalWrite(portD, LOW);
	delay(delays);

	//kører den givne stepper ud fra input "akse"
	if (akse == 'x') {
		sandKonv(s4x);
		//  Serial.println("4");
		delay(delays);

		sandKonv(s3x);
		//Serial.println("3");
		delay(delays);

		sandKonv(s2x);
		//Serial.println("2");
		delay(delays);

		sandKonv(s1x);
		//Serial.println("1");
		delay(delays);

	}
	else if (akse == 'y') {
		sandKonv(s4y);
		//Serial.println("4");
		delay(delays);

		sandKonv(s3y);
		//Serial.println("3");
		delay(delays);

		sandKonv(s2y);
		//Serial.println("2");
		delay(delays);

		sandKonv(s1y);
		//  Serial.println("1");
		delay(delays);
	}

	//slukker for 4051
	digitalWrite(portD, HIGH);
}

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

//***************************************************************TEST / KONTROL***************************************************************
void printAlt() {
	for (int y = 0; y < stepsY; y++) {
		//yaksen
		for (int x = 0; x < stepsX / 8; x++) {
			//xaksen
			if (scanArray[x][y] == 0) {
				//print 0 nuller for at vise værdien binært
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
				Serial.print("0");
			}
			else			{
				Serial.print(scanArray[x][y], BIN);
				Serial.print(" ");
			}
		
					}
		Serial.println("");
	}
}
