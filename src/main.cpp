// Na het indrukken van een nummertoets klinkt er een geluid gedurende 0,2 sec.
// F.02 DONE
// Bij het indrukken van een nummertoets gaat het gele ledje gedurende 0,2 sec 
// aan. DONE
// F.03
// Na het indrukken van de E-toets wordt de ingevoerde code gecontroleerd:
// a)
// als de juiste code ingevoerd is wordt het slot van de kluis gedurende 2    
//           seconden geopend.
// b)
// als de juiste code ingevoerd is klinkt er gedurende 2 seconden een 
// geluid.
// c)
// als de juiste code ingevoerd is brandt de groene led 2 seconden lang.
// d)
// Als een verkeerde code ingevoerd is klinkt er gedurende 1 sec een geluid
// e)
// Als een verkeerde code ingevoerd is brandt de rode led gedurende 1 sec
// F.04
// Na het indrukken van de C-toets wordt de gebruikersinvoer gereset
// F.05
// Na het indrukken van de C-toets klinkt er een geluid gedurende 0,2 sec

#include <Arduino.h>
#include <EEPROM.h>

char getKey();
void checkCode();
void setSolenoid(int value);
void buttonPressed();
void getCode();
void input();
void addInput(char key, int loc);
void resetCode();
void codeError();
void wrongCode();
void kluisOpened();
void setCode();

#define DEBUG true

#define CODE_EEPROM_ADRESS 0
#define CODE_BYTE_SIZE 1

#define ROWS 4 
#define COLS 3 

#define RESET_PIN A1

#define CODE_SIZE 4

#define SOLENOID_PIN A0
#define BUZZER_PIN A2
#define GREEN_LED_PIN A3
#define RED_LED_PIN A4
#define ORANGE_LED_PIN A5

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'C', '0', 'E'}
};

int rowPins[ROWS] = {8, 12, 6, 4};     
int colPins[COLS] = {9, 7, 5};  

char code[4] = {'1', '2', '3', '4'}; 

char codeInput[4] = {0};

int locInput = 0;

void setup() {
	Serial.begin(9600);

	for (int i = 0; i < ROWS; i++) {
		pinMode(rowPins[i], OUTPUT);
		digitalWrite(rowPins[i], HIGH);
	}

	for (int i = 0; i < COLS; i++)
		pinMode(colPins[i], INPUT_PULLUP);

	pinMode(RESET_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(SOLENOID_PIN, OUTPUT);

	pinMode(ORANGE_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);

	getCode();
}

void loop() {
	input();
}

void input() {

	char input = getKey();

	if (input == 'E') {
		checkCode();
	} else if (input == 'C') {
		buttonPressed();
		locInput = 0;
	} else if (digitalRead(RESET_PIN) == LOW) {
		Serial.println("Reset Code!");
		resetCode();
	} else if (input >= '0' && input <= '9') {
		buttonPressed();
		addInput(input, locInput);
		locInput++;
	}

}

void checkCode() {

	if (locInput < 4 || locInput > 4) {
		codeError();
		Serial.println("Code Input Error");
		return;
	}

	for (int i = 0; i < CODE_SIZE; i++) {

		char buffer[20];
		sprintf(buffer, "Code: %c, %c", code[i], codeInput[i]);
		Serial.println(buffer);

		if (codeInput[i] != code[i]) {
			wrongCode();
			locInput = 0;
			Serial.println("Wrong Code");
			return;
		} 
	}

	locInput = 0;

	kluisOpened();

	#if DEBUG

	Serial.println("Open kluis!");

	#endif
}

void addInput(char key, int loc) {

	if (loc > 3) {
		codeError();
		return;
	}

	Serial.println("Input: ");
	Serial.print(key);
	Serial.print(loc);
	codeInput[loc] = key;
}

void buttonPressed() {
	digitalWrite(ORANGE_LED_PIN, HIGH);
	tone(BUZZER_PIN, 500);
	delay(200);
	digitalWrite(ORANGE_LED_PIN, LOW);
	noTone(BUZZER_PIN);
	delay(100);
}

void kluisOpened() {
	setSolenoid(HIGH);
	digitalWrite(GREEN_LED_PIN, HIGH);
	tone(BUZZER_PIN, 800);
	delay(2000);
	setSolenoid(LOW);
	digitalWrite(GREEN_LED_PIN, LOW);
	noTone(BUZZER_PIN);
	delay(100);
}

void wrongCode() {
	digitalWrite(RED_LED_PIN, HIGH);
	tone(BUZZER_PIN, 100);
	delay(1000);
	digitalWrite(RED_LED_PIN, LOW);
	noTone(BUZZER_PIN);
	delay(100);
}

void codeError() {
	for (int i = 0; i < 4; i++) {
		digitalWrite(ORANGE_LED_PIN, HIGH);
		digitalWrite(RED_LED_PIN, HIGH);
		digitalWrite(GREEN_LED_PIN, HIGH);
		delay(100);
		digitalWrite(ORANGE_LED_PIN, LOW);
		digitalWrite(RED_LED_PIN, LOW);
		digitalWrite(GREEN_LED_PIN, LOW);
		delay(100);
	}
}

void setSolenoid(int value) {
	digitalWrite(SOLENOID_PIN, value);
}

void resetCode() {

	char newCode[CODE_SIZE] = {0}; 

	int count = 0;

	while (true) {

		char key = getKey();

		if (key == 'E') {
			Serial.println("New Code!");
			memcpy(code, newCode, sizeof(newCode));
			setCode();
			break;
		} else if (key == 'C') {
			Serial.println("Cancelled New code!");
			break;
		} else if (key >= '0' && key <= '9') {
			buttonPressed();
			newCode[count] = key;
			count++;
		}
	}
}

void setCode() {
	int arrayCount = 0;

	for (int i = CODE_EEPROM_ADRESS; i < CODE_SIZE; i += CODE_BYTE_SIZE) {
		EEPROM.update(i, code[arrayCount]);
		arrayCount++;
	} 

	#if DEBUG

	Serial.println("The code is: ");

	for (int i = 0; i < CODE_SIZE; i++)
		Serial.print(code[i]);

	Serial.println();

	#endif
}

void getCode() {
	int arrayCount = 0;

	for (int i = CODE_EEPROM_ADRESS; i < CODE_SIZE; i += CODE_BYTE_SIZE) {
		EEPROM.get(i, code[arrayCount]);
		arrayCount++;
	} 

	#if DEBUG

	Serial.println("The code is: ");

	for (int i = 0; i < CODE_SIZE; i++)
		Serial.print(code[i]);

	Serial.println();

	#endif
}


char getKey() {
	
	char key = 'N';

	for (int i = 0; i < ROWS; i++) {
		digitalWrite(rowPins[i], LOW);

		for(int j = 0; j < COLS; j++) {
			if (digitalRead(colPins[j]) == LOW) {

				#if DEBUG

				Serial.print("Key Pressed: ");
				Serial.println(keys[i][j]);

				#endif
				
				while (digitalRead(colPins[j]) == LOW);

				delay(50);  

				key = keys[i][j];
			}

		}

		digitalWrite(rowPins[i], HIGH);

	}

	return key;
}