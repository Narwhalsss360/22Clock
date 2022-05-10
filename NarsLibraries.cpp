#include "NarsLibraries.h"

/// <summary>
/// Connect to serialCom Protocol.
/// </summary>
bool NarsSerialCom::connected;

/// <summary>
/// SerialCom data
/// </summary>
unsigned long NarsSerialCom::data[256];

/// <summary>
/// Set if ready to recieve.
/// </summary>
bool NarsSerialCom::ready;

#pragma region x2i

/// <summary>
/// Outputs unsinged long from Hex Char array.
/// </summary>
/// <param name="s">: Char Array</param>
/// <returns>Number From Hex</returns>
unsigned long x2i(char* s)
{
	unsigned long x = 0;
	for (;;)
	{
		char c = *s;
		if (c >= '0' && c <= '9')
		{
			x *= 16;
			x += c - '0';
		}
		else if (c >= 'A' && c <= 'F')
		{
			x *= 16;
			x += (c - 'A') + 10;
		}
		else break;
		s++;
	}
	return x;
}
#pragma endregion

#pragma region toHex
/// <summary>
/// Outputs string from unsigned long input and string length, replaces empty spots with 0's.
/// </summary>
/// <param name="input">: Long number</param>
/// <param name="stringLength">: End Length</param>
/// <returns>End string result</returns>
String toHex(unsigned long input, byte stringLength)
{
	String toggled = String(input, HEX);
	toggled.toUpperCase();
	if (toggled.length() != stringLength)
	{
		byte outLength = toggled.length();
		String original = toggled;
		toggled = "";
		for (int i = 0; i < stringLength - outLength; i++)
		{
			toggled += "0";
		}
		toggled += original;
	}
	return toggled;
}
#pragma endregion

#pragma region octalToDecimal
/// <summary>
/// Returns base-10 from octal input.
/// </summary>
/// <param name="n">: Octal Number</param>
/// <returns>int (Base-10)</returns>
static int octalToDecimal(int n)
{
	int num = n;
	int dec_value = 0;
	int base = 1;
	int temp = num;
	while (temp > 0) {
		int last_digit = temp % 10;
		temp = temp / 10;
		dec_value += last_digit * base;
		base = base * 8;
	}
	return dec_value;
}
#pragma endregion

#pragma region bitCast
/// <summary>
/// Cast ulong to long with correct signage
/// </summary>
/// <param name="in"></param>
/// <returns>value</returns>
long bitCast(unsigned long in)
{
	return (in - LONG_MAX);
}
#pragma endregion

#pragma region bitCast
/// <summary>
/// Cast long to ulong with correct signage
/// </summary>
/// <param name="in"></param>
/// <returns>value</returns>
unsigned long bitCast(long in)
{
	return (in + LONG_MAX);
}
#pragma endregion

#pragma region mapf
/// <summary>
/// Map values double
/// </summary>
/// <param name="x">Value to map</param>
/// <param name="in_min">Input min</param>
/// <param name="in_max">Input max</param>
/// <param name="out_min">Output min</param>
/// <param name="out_max">Output max</param>
/// <returns>mapped values</returns>
double mapf(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#pragma endregion

#pragma region HornerScheme
/// <summary>
/// The Horner Scheme
/// </summary>
/// <param name="Num"></param>
/// <param name="Divider"></param>
/// <param name="Factor"></param>
/// <returns></returns>
unsigned long hornerScheme(unsigned long Num, unsigned long Divider, unsigned long Factor)
{
	unsigned long Remainder = 0, Quotient = 0, Result = 0;
	Remainder = Num % Divider;
	Quotient = Num / Divider;
	if (!(Quotient == 0 && Remainder == 0))
		Result += hornerScheme(Quotient, Divider, Factor) * Factor + Remainder;
	return Result;
}
#pragma endregion

#pragma region intToFreq
/// <summary>
/// 4-Digit to xxx.xxx
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
double intToFreq(double input)
{
	input += 10000;
	input /= 100;
	return input;
}
#pragma endregion

#pragma region boolToString
/// <summary>
/// Converts bool to string "True"/"False"
/// </summary>
/// <param name="input">true/false input</param>
/// <returns>true/false string</returns>
String boolToString(int input)
{
	if (input > 0)
	{
		return "True";
	}
	else
	{
		return "False";
	}
}
#pragma endregion

#pragma region boolToString
/// <summary>
/// Converts bool to string "True"/"False"
/// </summary>
/// <param name="input">true/false input</param>
/// <returns>true/false string</returns>
String boolToString(bool input)
{
	if (input)
	{
		return "True";
	}
	else
	{
		return "False";
	}
}
#pragma endregion

#pragma region byteWrite
void byteWrite(byte pin, byte byteOut) {
	for (byte i = 0; i < 8; i++) 
	{
		digitalWrite(pin, LOW);
		if (0x80 & byteOut) digitalWrite(pin, HIGH);
		else digitalWrite(pin, LOW);
		digitalWrite(pin, HIGH);
		digitalWrite(pin, LOW);
		byteOut <<= 1;
	}
}
#pragma endregion

#pragma region printRight
#ifdef LiquidCrystal_I2C_h
void printRight(LiquidCrystal_I2C* ptr, String str, byte row, byte cols)
{
	ptr->setCursor(cols - str.length(), row);
	ptr->print(str);
}
#endif // LiquidCrystal_I2C_h
#pragma endregion

#pragma region RGBA
/// <summary>
/// Default Constructor
/// </summary>
RGBA::RGBA()
	:red(0), green(0), blue(0), alpha(0)
{

}

/// <summary>
/// RGBA input constructor
/// </summary>
/// <param name="_red">Red</param>
/// <param name="_green">Green</param>
/// <param name="_blue">Blue</param>
/// <param name="_alpha">Alpha</param>
RGBA::RGBA(byte _red, byte _green, byte _blue, byte _alpha)
	:red(_red), green(_green), blue(_blue), alpha(_alpha)
{

}
#pragma endregion

#pragma region RGB
/// <summary>
/// Default Constructor
/// </summary>
RGB::RGB()
	:red(0), green(0), blue(0)
{

}

/// <summary>
/// Input Constructor
/// </summary>
/// <param name="_red">Red</param>
/// <param name="_green">Green</param>
/// <param name="_blue">Blue</param>
RGB::RGB(byte _red, byte _green, byte _blue)
	:red(_red), green(_green), blue(_blue)
{

}
#pragma endregion

#pragma region HSV
/// <summary>
/// Default Constructor
/// </summary>
HSV::HSV()
	:hue(0), saturation(0), value(0)
{

}

/// <summary>
/// Input Constructor
/// </summary>
/// <param name="h">Hue</param>
/// <param name="s">Saturation</param>
/// <param name="v">Value</param>
HSV::HSV(float h, float s, float v)
{
	this->hue = (0 <= h && h <= 360) ? h : 0;
	this->saturation = (0 <= s && s <= 100) ? s : 0;
	this->value = (0 <= v && v <= 100) ? v : 0;
}
#pragma endregion

#pragma region UnitConverter
double UnitConverter::convert(UOM unitType, double input, const byte inputUnit, const byte outputUnit)
{	
	this->original = input;
	switch (unitType)
	{

	case UnitConverter::UOM::LENGTH:
	{
		
		switch ((LENGTH)inputUnit)
		{

		case UnitConverter::LENGTH::picometer:
		{	

			switch ((LENGTH)outputUnit)
			{
			case UnitConverter::LENGTH::picometer:
				this->result = result;
				return this->result;
				break;
			case UnitConverter::LENGTH::nanometer:
				this->result = input * 1000;
				return this->result;
				break;
			case UnitConverter::LENGTH::micrometer:
				this->result = input * 1000000;
				return this->result;
				break;
			case UnitConverter::LENGTH::millimeter:
				this->result = input * 1000000000;
				break;
			case UnitConverter::LENGTH::centimeter:
				this->result = input * 10000000000;
				return this->result;
				break;
			case UnitConverter::LENGTH::decimeter:
				this->result = input * 100000000000;
				return this->result;
				break;
			case UnitConverter::LENGTH::meter:
				this->result = input * 1000000000000;
				return this->result;
				break;
			case UnitConverter::LENGTH::dekameter:
				this->result = input * 10000000000000;
				return this->result;
				break;
			case UnitConverter::LENGTH::hectometer:
				break;
			case UnitConverter::LENGTH::kilometer:
				break;
			case UnitConverter::LENGTH::megameter:
				break;
			case UnitConverter::LENGTH::gigameter:
				break;
			case UnitConverter::LENGTH::terameter:
				break;
			case UnitConverter::LENGTH::petameter:
				break;
			case UnitConverter::LENGTH::inch:
				break;
			case UnitConverter::LENGTH::foot:
				break;
			case UnitConverter::LENGTH::yard:
				break;
			case UnitConverter::LENGTH::mile:
				break;
			case UnitConverter::LENGTH::lightyear:
				break;
			default:
				break;
			}

			break; 
		}

		case UnitConverter::LENGTH::nanometer:
		{
			break; 
		}

		case UnitConverter::LENGTH::micrometer:
		{
			break; 
		}

		case UnitConverter::LENGTH::millimeter:
		{
			break; 
		}

		case UnitConverter::LENGTH::centimeter:
		{
			break; 
		}

		case UnitConverter::LENGTH::decimeter:
		{
			break; 
		}

		case UnitConverter::LENGTH::meter:
		{
			break; 
		}

		case UnitConverter::LENGTH::dekameter:
		{
			break; 
		}

		case UnitConverter::LENGTH::hectometer:
		{
			break; 
		}

		case UnitConverter::LENGTH::kilometer:
		{
			break; 
		}

		case UnitConverter::LENGTH::megameter:
		{
			break; 
		}

		case UnitConverter::LENGTH::gigameter:
		{
			break; 
		}

		case UnitConverter::LENGTH::terameter:
		{
			break; 
		}

		case UnitConverter::LENGTH::petameter:
		{
			break; 
		}

		case UnitConverter::LENGTH::inch:
		{
			break; 
		}

		case UnitConverter::LENGTH::foot:
		{
			break; 
		}

		case UnitConverter::LENGTH::yard:
		{
			break; 
		}

		case UnitConverter::LENGTH::mile:
		{
			break; 
		}

		case UnitConverter::LENGTH::lightyear:
		{
			break; 
		}

		default:
			break;
		}

		break;
	}

	case UnitConverter::UOM::VOLUME:
	{
		const VOLUME PinputUnit = (VOLUME)inputUnit;
		const VOLUME PoutputUnit = (VOLUME)outputUnit;
		break; 
	}

	case UnitConverter::UOM::TIME:
	{
		const TIME PinputUnit = (TIME)inputUnit;
		const TIME PoutputUnit = (TIME)outputUnit;
		break; 
	}

	case UnitConverter::UOM::TEMPERATURE:
	{
		const TEMPERATURE PinputUnit = (TEMPERATURE)inputUnit;
		const TEMPERATURE PoutputUnit = (TEMPERATURE)outputUnit;
		break; 
	}

	case UnitConverter::UOM::MASS:
	{
		const MASS PinputUnit = (MASS)inputUnit;
		const MASS PoutputUnit = (MASS)outputUnit;
		break; 
	}

	case UnitConverter::UOM::LUMOSITY:
	{
		const LUMOSITY PinputUnit = (LUMOSITY)inputUnit;
		const LUMOSITY PoutputUnit = (LUMOSITY)outputUnit;
		break; 
	}

	default:
		break;
	}
}
#pragma endregion

#pragma region COLOR
/// <summary>
/// Default constructor.
/// </summary>
COLOR::COLOR()
	:hexCode(0), rgb(), hsv()
{

}

/// <summary>
/// Hex input constructor.
/// </summary>
/// <param name="_hexCode">Hex code</param>
COLOR::COLOR(unsigned long _hexCode)
	:hexCode(_hexCode), rgb(), hsv()
{
	this->convertHexToRGB();
	this->convertRGBToHSV();
}

/// <summary>
/// HSV Constructor
/// </summary>
/// <param name="_hsv">HSV input</param>
COLOR::COLOR(HSV _hsv)
	:hexCode(0), rgb(), hsv()
{
	this->convertHSVToRGB();
	this->convertRGBToHex();
}

/// <summary>
/// RGB Constructor
/// </summary>
/// <param name="_rgb">RGB input</param>
COLOR::COLOR(RGB _rgb)
	:hexCode(0), rgb(_rgb), hsv()
{
	this->convertRGBToHSV();
	this->convertRGBToHex();
}

void COLOR::convertHexToRGB()
{
	this->rgb.red = this->hexCode >> 16;
	this->rgb.green = (this->hexCode & 0x00ff00) >> 8;
	this->rgb.blue = (this->hexCode & 0x0000ff);
}

void COLOR::convertHSVToRGB()
{
	float s = this->hsv.saturation / 100;
	float v = this->hsv.value / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(this->hsv.hue / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (this->hsv.hue >= 0 && this->hsv.hue < 60) {
		r = C, g = X, b = 0;
	}
	else if (this->hsv.hue >= 60 && this->hsv.hue < 120) {
		r = X, g = C, b = 0;
	}
	else if (this->hsv.hue >= 120 && this->hsv.hue < 180) {
		r = 0, g = C, b = X;
	}
	else if (this->hsv.hue >= 180 && this->hsv.hue < 240) {
		r = 0, g = X, b = C;
	}
	else if (this->hsv.hue >= 240 && this->hsv.hue < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	this->rgb.red = (r + m) * 255;
	this->rgb.green = (g + m) * 255;
	this->rgb.blue = (b + m) * 255;
}

void COLOR::convertRGBToHex()
{
	this->hexCode |= this->rgb.red << 16;
	this->hexCode |= this->rgb.blue << 8;
	this->hexCode |= this->rgb.green;
}

void COLOR::convertRGBToHSV()
{
	float fCMax = max(max(this->rgb.red, this->rgb.green), this->rgb.blue);
	float fCMin = min(min(this->rgb.red, this->rgb.green), this->rgb.blue);
	float fDelta = fCMax - fCMin;

	if (fDelta > 0) {
		if (fCMax == this->rgb.red) {
			this->hsv.hue = 60 * (fmod(((this->rgb.green - this->rgb.blue) / fDelta), 6));
		}
		else if (fCMax == this->rgb.green) {
			this->hsv.hue = 60 * (((this->rgb.blue - this->rgb.red) / fDelta) + 2);
		}
		else if (fCMax == this->rgb.blue) {
			this->hsv.hue = 60 * (((this->rgb.red - this->rgb.green) / fDelta) + 4);
		}

		if (fCMax > 0) {
			this->hsv.saturation = fDelta / fCMax;
		}
		else {
			this->hsv.saturation = 0;
		}

		this->hsv.value = fCMax;
	}
	else {
		this->hsv.hue = 0;
		this->hsv.saturation = 0;
		this->hsv.value = fCMax;
	}

	if (this->hsv.hue < 0) {
		this->hsv.hue = 360 + this->hsv.hue;
	}
}

#pragma endregion

#pragma region NarsSerialCom
/// <summary>
/// Invoke method on serialEvent. After parse, done/special method invokes.
/// </summary>
/// <param name="done">Done function pointer</param>
/// <param name="special">Special function pointer</param>
void NarsSerialCom::onSerialEvent(void (*done) (unsigned int _register, unsigned long data), void (*special) (unsigned int _register, String data))
{
	String completeString;
	String command;

	while (Serial.available())
	{
		char inChar = (char)Serial.read();
		completeString += inChar;
	}

	command = completeString.substring(0, 2);

	if (command == "*B")
	{
		connected = true;
	}

	if (command == "*D")
	{
		unsetReady();
		String registerString = completeString.substring(2, 6);;
		String dataString = completeString.substring(6, 14);;
		unsigned int _register;
		unsigned long _data;
		char _registerBuffer[4];
		char dataBuffer[8];

		strcpy(_registerBuffer, registerString.c_str());
		strcpy(dataBuffer, dataString.c_str());

		_register = x2i(_registerBuffer);
		_data = x2i(dataBuffer);

		if (_register <= 256)
		{
			data[_register] = _data;
		}
		done(_register, _data);
	}

	if (command == "*S")
	{
		unsetReady();
		String registerString = completeString.substring(2, 6);
		unsigned int _register;
		char _registerBuffer[4];
		strcpy(_registerBuffer, registerString.c_str());
		_register = x2i(_registerBuffer);

		String specialString;
		String temp = completeString;
		temp.remove(0, 6);
		for (int i = 0; i < temp.length() - 1; i++)
		{
			char tempChar = (char)temp[i];
			if (tempChar != '-')
			{
				specialString += tempChar;
			}
		}
		specialString.remove(specialString.length() - 1);
		special(_register, specialString);
	}

	if (command == "*E")
	{
		connected = false;
	}

}			

/// <summary>
/// Sends data
/// </summary>
/// <param name="_register">register</param>
/// <param name="data">data</param>
void NarsSerialCom::send(unsigned int _register, unsigned long data)
{
	if (connected)
	{
		String completeString = "*D";
		if (_register <= 65535 || data <= 4294967295)
		{
			String registerString = toHex(_register, 4);
			String dataString = toHex(data, 8);
			completeString += registerString + dataString + '-';
			Serial.println(completeString);
		}
	}
}

/// <summary>
/// Sends special data
/// </summary>
/// <param name="_register">register</param>
/// <param name="data">data</param>
void NarsSerialCom::sendSpecial(unsigned int _register, String data)
{
	if (connected)
	{
		String completeString = "*S";
		if (_register <= 65535)
		{
			String registerString = toHex(_register, 4);
			completeString += registerString + data + '-';
			Serial.println(completeString);
		}
	}
}

/// <summary>
/// Send range of data
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
void NarsSerialCom::sendDataRange(uint8_t start, uint8_t end)
{
	for (uint8_t i = start; i <= end; i++)
	{
		send(i, data[i]);
	}
}

/// <summary>
/// Set Ready
/// </summary>
void NarsSerialCom::setReady()
{
	ready = true;
	send(0, 1);
}

/// <summary>
/// Unset Ready
/// </summary>
void NarsSerialCom::unsetReady()
{
	ready = false;
	send(0, 0);
}
#pragma endregion

#pragma region PushToggle
/// <summary>
/// Push Toggle class
/// </summary>
/// <param name="_pin"></param>
/// <param name="_invert"></param>
/// <param name="_debounceDelay"></param>
PushToggle::PushToggle(byte _pin, bool _invert, byte _debounceDelay)
{
	this->pin = _pin;
	this->invertedInput = _invert;
	this->debounceDelay = _debounceDelay;
}

/// <summary>
/// Set a callback to run when pin state changes.
/// </summary>
/// <param name="_callBack">function pointer</param>
void PushToggle::setCallBack(void (*_callBack) (void))
{
	this->callBack = _callBack;
}

/// <summary>
/// Check button with debounce and toggle
/// </summary>.
void PushToggle::read()
{
	bool pinState;

	if (this->invertedInput)
	{
		pinState = !digitalRead(this->pin);
	}
	else
	{
		pinState = digitalRead(this->pin);
	}
	
	if (pinState != this->lastButtonState)
	{
		this->lastDebounceTime = millis();
	}

	if ((millis() - this->lastDebounceTime) > this->debounceDelay)
	{
		if (pinState != this->endState)
		{
			this->endState = pinState;

			if (endState)
			{
				this->toggle = !this->toggle;
			}
		}
	}

	this->lastButtonState = pinState;
	if (this->callBack != NULL)
		this->callBack();
}

/// <summary>
/// Check of toggled
/// </summary>
/// <returns></returns>
bool PushToggle::toggled()
{
	return this->toggle;
}
#pragma endregion

#pragma region Push
/// <summary>
/// Push Class.
/// </summary>
/// <param name="_pin">Button pin.</param>
/// <param name="_inverted">invert the input.</param>
/// <param name="_debounceDelay">refresh time for debouncing.</param>
Push::Push(byte _pin, bool _inverted, int _debounceDelay)
	:pin(_pin), inverted(_inverted), debounceDelay(_debounceDelay), onRelease(NULL), onPress(NULL), onPressTime(0), releasedHoldTime(0), lastDebounceTime(0), pressedHoldTime(0)
{
	pinMode(_pin, (inverted) ? INPUT_PULLUP : INPUT);
}

/// <summary>
/// Get state, and check for if currently pressing, pressed, or released.
/// </summary>
void Push::update()
{
	if (millis() - this->lastDebounceTime >= this->debounceDelay)
	{
		this->called[PRESSEDMEMBER] = false;
		this->called[RELEASEDMEMBER] = false;
		this->lastDebounceTime = millis();
		this->state[CURRENT] = (inverted) ? !(bool)digitalRead(this->pin) : digitalRead(this->pin);

		if (!this->state[CURRENT] && this->state[PREVIOUS])
		{
			this->state[RELEASE] = true;
			this->state[PREVIOUS] = false;
			this->state[PRESS] = false;
			this->releasedHoldTime = millis() - this->onPressTime;
			this->pressedHoldTime = 0;
			if (this->onRelease != NULL) this->onRelease(this->releasedHoldTime);
		}
		else
		{
			this->state[RELEASE] = false;
		}

		if (this->state[CURRENT] && !this->state[PREVIOUS])
		{
			this->state[PREVIOUS] = true;
			this->state[PRESS] = true;
			this->releasedHoldTime = 0;
			this->pressedHoldTime = millis();
			this->onPressTime = millis();
			if (this->onPress != NULL) this->onPress();
		}
		else
		{
			this->state[PRESS] = false;
		}
	}
}

/// <summary>
/// Check the current state of the button.
/// </summary>
/// <returns>Current button state</returns>
bool Push::current()
{
	return this->state[CURRENT];
}

/// <summary>
/// Check if the button was just pressed(not the current state).
/// </summary>
/// <returns>Pressed bool</returns>
bool Push::pressed()
{
	if (this->called[PRESSEDMEMBER])
	{
		return false;
	}
	else
	{
		this->called[PRESSEDMEMBER] = true;
		return this->state[PRESS];
	}
}

/// <summary>
/// Check if the button was released.
/// </summary>
/// <returns>Released bool</returns>
bool Push::released()
{
	if (this->called[RELEASEDMEMBER])
	{
		return false;
	}
	else
	{
		this->called[RELEASEDMEMBER] = true;
		return this->state[RELEASE];
	}
}

/// <summary>
/// If just released, get the time the button was held for.
/// </summary>
unsigned int Push::getReleasedHoldTime()
{
	return this->releasedHoldTime;
}

unsigned int Push::getPushedHoldTime()
{
	return this->pressedHoldTime;
}
#pragma endregion

#pragma region Rotary
Rotary::Rotary()
	:pinA(NULL), pinB(NULL), pinS(NULL), inverted(NULL), useInterrupt(NULL), debouce(NULL), lastRead(ZERO)
{

}

/// <summary>
/// Rotary Class.
/// </summary>
/// <param name="_pinA">Rotary pin A</param>
/// <param name="_pinB">Rotary pin B</param>
Rotary::Rotary(byte _pinA, byte _pinB, bool useI, bool _inverted, unsigned int _debouce)
	:pinA(_pinA), pinB(_pinB), pinS(NULL), inverted(_inverted), useInterrupt(useI), debouce(_debouce), lastRead(ZERO)
{
	this->init();
}

/// <summary>
/// Rotary Class
/// </summary>
/// <param name="_pinA">Rotary pin A</param>
/// <param name="_pinB">Rotary pin B</param>
/// <param name="_pinS">Rotary pin Switch</param>
Rotary::Rotary(byte _pinA, byte _pinB, byte _pinS, bool useI, bool _inverted, unsigned int _debouce)
	:pinA(_pinA), pinB(_pinB), pinS(_pinS), inverted(_inverted), useInterrupt(useI), debouce(_debouce), lastRead(ZERO)
{
	this->init();
}

void Rotary::init()
{
	pinMode(this->pinA, (this->inverted) ? INPUT_PULLUP: INPUT);
	pinMode(this->pinB, (this->inverted) ? INPUT_PULLUP : INPUT);
	pinMode(this->pinS, (this->inverted) ? INPUT_PULLUP : INPUT);
	this->defineMode();
}

void Rotary::serviceRoutine()
{
	if (!(this->inverted) ? !digitalRead(this->pinA) : digitalRead(this->pinA))
		return;
	if (this->useInterrupt)
	{
		if (millis() - this->lastRead >= this->debouce)
		{
			this->lastRead = millis();
			if ((this->inverted) ? !digitalRead(this->pinB) : digitalRead(this->pinB))
			{
				this->state = COUNTER_CLOCKWISE;
			}
			else
			{
				this->state = CLOCKWISE;
			}
		}
	}
}

bool Rotary::getSwitch()
{
	if (this->pinS != NULL)
	{
		return (this->inverted) ? !digitalRead(this->pinS) : digitalRead(this->pinS);
	}
	else
	{
		return false;
	}
}

int Rotary::getState()
{
	if (!this->useInterrupt)
	{
		bool pinA = (this->inverted) ? !digitalRead(this->pinA) : digitalRead(this->pinA);
		bool pinB = (this->inverted) ? !digitalRead(this->pinB) : digitalRead(this->pinB);

		if (pinA != pinB)
		{
			if (pinB)
			{
				return COUNTER_CLOCKWISE;
			}
			else
			{
				return CLOCKWISE;
			}
		}
		else
		{
			return IDLE;
		}
	}
	else
	{
		int returnState = state;
		this->state = IDLE;
		return returnState;
	}
}

void Rotary::defineMode()
{
	this->mode = (this->inverted) ? FALLING : RISING;
}
#pragma endregion

#ifdef TwoWire_h
#pragma region WireSearch
/// <summary>
/// Search for wire devices.
/// </summary>
/// <returns>Device Addresses</returns>
void WireSearch::search()
{
	for (byte address = 0; address < 127; address++)
	{
		this->availableWireAddresses[address] = 0;
	}

	for (byte address = 1; address < 127; address++)
	{
		Wire.beginTransmission(address); 
		byte status = Wire.endTransmission();
		if (status == 0)
		{
			this->availableWireAddresses[address] = true;
		}
	}
}
#pragma endregion

#pragma region WireHost
/// <summary>
/// Check if client is available.
/// </summary>
void WireHost::check()
{
	Wire.beginTransmission(this->deviceProperties.address);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(this->deviceProperties.address, (byte)1);
	if (1 <= Wire.available())
	{
		notFoundCounter = 0;
		this->deviceProperties.id = (byte)Wire.read();
		this->deviceProperties.data[0] = this->deviceProperties.id;
		this->deviceProperties.state = this->deviceProperties.states.CONNECTED;
	}
	else
	{
		if (notFoundCounter == 255)
		{
			this->deviceProperties.state = this->deviceProperties.states.DISCONNECTED;
		}
		else
		{
			notFoundCounter++;
		}
	}
}

/// <summary>
/// Get data from all registers.
/// </summary>
void WireHost::getData()
{
	if (this->deviceProperties.state == this->deviceProperties.states.CONNECTED)
	{
		for (byte _register = 1; _register < 64; _register++)
		{
			Wire.beginTransmission(this->deviceProperties.address);
			Wire.write(_register);
			Wire.endTransmission();
			Wire.requestFrom(this->deviceProperties.address, (byte)1);
			if (1 <= Wire.available())
			{
				this->deviceProperties.data[_register] = Wire.read();
			}
		}
	}
}

/// <summary>
/// Send data from all registers.
/// </summary>
void WireHost::sendData()
{
	if (this->deviceProperties.state == this->deviceProperties.states.CONNECTED)
	{
		for (byte _register = 65; _register < 255; _register++)
		{
			Wire.beginTransmission(this->deviceProperties.address);
			Wire.write(_register);
			Wire.endTransmission();
			Wire.beginTransmission(this->deviceProperties.address);
			Wire.write(this->deviceProperties.data[_register]);
			Wire.endTransmission();
		}
	}
}

/// <summary>
/// Send data from specific register.
/// </summary>
/// <param name="addr">Specific register</param>
void WireHost::sendData(byte addr)
{
	if (this->deviceProperties.state == this->deviceProperties.states.CONNECTED)
	{
		Wire.beginTransmission(this->deviceProperties.address);
		Wire.write(addr);
		Wire.endTransmission();
		Wire.beginTransmission(this->deviceProperties.address);
		Wire.write(this->deviceProperties.data[addr]);
		Wire.endTransmission();
	}
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="_address"></param>
WireHostLite::WireHostLite(byte _address)
{
	this->address = _address;
}
#pragma endregion

#pragma region WireHostLite
/// <summary>
/// Check if client is available
/// </summary>
void WireHostLite::check()
{
	Wire.beginTransmission(this->address);
	Wire.write(0x00);
	Wire.endTransmission();
	Wire.requestFrom(this->address, (byte)1);
	if (1 <= Wire.available())
	{
		notFoundCounter = 0;
		this->id = (byte)Wire.read();
		this->connected = true;
	}
	else
	{
		if (notFoundCounter == 255)
		{
			this->connected = false;
		}
		else
		{
			notFoundCounter++;
		}
	}
}

/// <summary>
/// Send data to client on a register
/// </summary>
/// <param name="dataRegister">register select</param>
/// <param name="data">data</param>
void WireHostLite::sendData(byte dataRegister, byte data)
{
	if (this->connected)
	{
		if (dataRegister >= 65)
		{
			Wire.beginTransmission(this->address);
			Wire.write(dataRegister);
			Wire.endTransmission();
			Wire.beginTransmission(this->address);
			Wire.write(data);
			Wire.endTransmission();
		}
	}
}

/// <summary>
/// Get data from a specific register
/// </summary>
/// <param name="dataRegister">Register to get data from</param>
/// <returns>data from selected register</returns>
byte WireHostLite::getData(byte dataRegister)
{
	if (this->connected)
	{
		if (dataRegister <= 64)
		{
			Wire.beginTransmission(this->address);
			Wire.write(dataRegister);
			Wire.endTransmission();
			Wire.requestFrom(this->address, (byte)1);
			if (1 <= Wire.available())
			{
				return Wire.read();
			}
		}
	}
}
#pragma endregion

#pragma region WireClient
/// <summary>
/// Invoke in Wire.OnReceive
/// </summary>
/// <param name="bytes">On Receive</param>
void WireClient::onReceive(int bytes)
{
	this->recvData = Wire.read();

	if (recv)
	{
		this->deviceProperties.data[this->registerSelect] = recvData; 
		this->recv = false;
	}
	else
	{
		this->registerSelect = this->recvData;
		if (this->recvData >= 65)
		{
			this->recv = true;
		}
	}	
}

/// <summary>
/// Invoke on Wire.onRequest
/// </summary>
void WireClient::onRequest()
{
	if (!recv)
	{
		Wire.write(this->deviceProperties.data[this->registerSelect]);
	}
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="address"></param>
/// <param name="id"></param>
WireClient::WireClient(byte address, byte id)
{
	this->deviceProperties.address = address;
	this->deviceProperties.id = id;
	this->deviceProperties.data[0] = this->deviceProperties.id;
}
#pragma endregion
#endif // TwoWire_h