#ifndef NarsLibraries
#define NarsLibraries

#if __has_include ("Wire.h")
#include "Wire.h"
#endif // __has_include ("Wire.h")

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Defenitions
#define ZERO 0
#define NEWLINE '\n'
#define CARRIAGERETURN '\r'
#define NULLTERMINATOR '\0'

#ifdef ARDUINO_ARCH_AVR
#define ULONG_MAX 4294967295 
#define LONG_MAX 2147483647
#define LONG_MIN -2147483648

#define UINT_MAX 65535
#define INT_MAX 32767
#define INT_MIN -32768

#define BYTE_MAX 255

#define ADC_MAX 1023
#define PWN_MAX 255
#endif // ARDUINO_ARCH_AVR

#define SERIALCOM_BAUD 1000000
#define LOG_MSG "LOG: ";
#define VERBOSE_MSG "VERBOSE: "
#define NOTICE_MSG "NOTICE: "
#define WARNING_MSG "WARNING: "
#define ERROR_MSG "ERROR: "

#define addInterrupt(pin, ISR, mode) attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);
#define removeInterrupt(pin) detachInterrupt(digitalPinToInterrupt(pin));
#define BCD2DEC(num) hornerScheme(num, 0x10, 10)  
#pragma endregion

unsigned long x2i(char*);

String toHex(unsigned long, byte);

int octalToDecimal(int);

long bitCast(unsigned long);

unsigned long bitCast(long);

double mapf(double, double, double, double, double);

unsigned long hornerScheme(unsigned long, unsigned long, unsigned long);

double intToFreq(double);

String boolToString(bool);

void byteWrite(byte, byte);

String boolToString(int);

#ifdef LiquidCrystal_I2C_h
void printRight(LiquidCrystal_I2C* ptr, String str, byte row, byte cols);
#endif // LiquidCrystal_I2C_h

struct RGBA
{
	byte red,
		green,
		blue,
		alpha;
	RGBA();
	RGBA(byte, byte, byte, byte);
};

struct RGB
{
	byte red,
		green,
		blue;
	RGB();
	RGB(byte, byte, byte);
};

struct HSV
{
	float hue,
		saturation,
		value;
	HSV();
	HSV(float, float, float);
};

struct UnitConverter
{
	enum class UOM : byte
	{
		LENGTH,
		VOLUME,
		TIME,
		TEMPERATURE,
		MASS,
		LUMOSITY
	};

	enum class LENGTH : byte
	{
		picometer,
		nanometer,
		micrometer,
		millimeter,
		centimeter,
		decimeter,
		meter,
		dekameter,
		hectometer,
		kilometer,
		megameter,
		gigameter,
		terameter,
		petameter,
		inch,
		foot,
		yard,
		mile,
		lightyear
	};

	enum class VOLUME : byte
	{
		picoliter,
		nanoliter,
		microliter,
		milliliter,
		centiliter,
		deciliter,
		liter,
		dekaliter,
		hectoliter,
		kiloliter,
		megaliter,
		gigaliter,
		teraliter,
		petaliter,
		ounce,
		pint,
		gallon
	};

	enum class TIME : byte
	{
		nanosecond,
		millisecond,
		second,
		minute,
		hour,
		day,
		week,
		month,
		year,
		decade,
		century
	};

	enum class TEMPERATURE : byte
	{
		kelvin,
		Celcius,
		Fahrenheit
	};

	enum class MASS : byte
	{
		picogram,
		nanogram,
		microgram,
		milligram,
		centigram,
		decigram,
		gram,
		dekagram,
		hectogram,
		kilogram,
		tonne,
		ounce,
		pound,
		ton
	};

	enum class LUMOSITY : byte
	{
		candela,
		lumens
	};

	double result;
	double original;
	UOM originalUnit;
	UOM resultUnit;

	double convert(UOM, double, const byte, const byte);
};

struct COLOR
{
public:
	RGB rgb;
	HSV hsv;
	unsigned long hexCode;
private:
	void convertHexToRGB();
	void convertHSVToRGB();
	void convertRGBToHex();
	void convertRGBToHSV();
public:
	COLOR();
	COLOR(unsigned long);
	COLOR(HSV);
	COLOR(RGB);
};

enum class TYPES : byte
{
	UND,
	BL,
	S8,
	S16,
	S32,
	U8,
	U16,
	U32,
	F32,
	D32
};

enum class LOGLEVEL : byte
{
	VERBOSE,
	NOTICE,
	WARNING,
	ERROR
};

enum ROTARYSTATES
{
	IDLE,
	COUNTER_CLOCKWISE,
	CLOCKWISE
};

class NarsSerialCom
{
public:
	static unsigned long data[256];
	static bool connected;
	static void onSerialEvent(void (*) (unsigned int, unsigned long), void (*) (unsigned int, String));
	static void send(unsigned int, unsigned long);
	static void sendSpecial(unsigned int, String);
	static void sendDataRange(uint8_t, uint8_t);
	static void setReady();
	static void unsetReady();
private:
	static bool ready;
};

class PushToggle
{
public:
	PushToggle(byte, bool, byte);
	void read();
	bool toggled();
	void setCallBack(void (*) (void));
private:
	byte pin;
	bool invertedInput;
	byte debounceDelay;
	unsigned long lastDebounceTime;
	bool lastButtonState;
	bool endState;
	bool toggle;
	void (*callBack) (void);
};

class Push
{
public:
	Push(byte, bool, int);
	void update();
	bool current();
	bool pressed();
	bool released();
	unsigned int getReleasedHoldTime();
	unsigned int getPushedHoldTime();
	void (*onRelease) (unsigned int);
	void (*onPress) (void);
private:
	enum BUTTONSTATESINDEX
	{
		CURRENT,
		PRESS,
		RELEASE,
		PREVIOUS
	};
	enum member
	{
		PRESSEDMEMBER,
		RELEASEDMEMBER
	};
	const byte pin;
	const bool inverted;
	const unsigned short debounceDelay;
	bool state[4];
	unsigned long onPressTime;
	unsigned long lastDebounceTime;
	unsigned int releasedHoldTime;
	unsigned int pressedHoldTime;
	bool called[2];
};

class Rotary
{
public:
	Rotary();
	Rotary(byte, byte, bool, bool, unsigned int);
	Rotary(byte, byte, byte, bool, bool, unsigned int);
	void serviceRoutine();
	int getState();
	bool getSwitch();
	mutable int mode;
private:
	void init();
	void defineMode();
	const byte pinA;
	const byte pinB;
	const byte pinS;
	const unsigned int debouce;
	const bool inverted;
	const bool useInterrupt;
	int state;
	unsigned long lastRead;
};

class Logger
{
public:
	Logger();
	void verbose(const char* mes);
	void notice(const char* mes);
	void warning(const char* mes);
	void error(const char* mes);
	void verbose(const char* module, const char* mes);
	void notice(const char* module, const char* mes);
	void warning(const char* module, const char* mes);
	void error(const char* module, const char* mes);
	void verbose(String mes);
	void notice(String mes);
	void warning(String mes);
	void error(String mes);
	void verbose(String module, String mes);
	void notice(String module, String mes);
	void warning(String module, String mes);
	void error(String module, String mes);
	void addLocal(void (*_local)(Logger));
	void logAll();
	void directLog(TYPES, word, const char*);
	void addToLogger(TYPES, word, const char*);
	void removeFromLogger(word);
	void addOutput();
	void enableSerialOutput(bool);
	void setShowModule(bool);
	void setShowLevel(bool);
private:
	void (*local)(Logger) = 0;
	void (*output)(const char*) = 0 ;
	word registeredValues[16] = { 0 };
	TYPES types[16] = { TYPES::UND };
	char names[10][16];
	bool serialOutput = false;
	bool showModule;
	bool showLevel;
};

#if defined(TwoWire_h)

struct WireSearch
{
	bool availableWireAddresses[127];
	void search();
};

struct DevicePropertiesTemplateSlave
{
	byte id;
	byte address;
	byte data[256];
};

struct DevicePropertiesTemplateMaster
{
	struct StatesTemplate
	{
		enum DeviceStatus
		{
			CONNECTED,
			NOT_FOUND,
			DISCONNECTED
		};
	} states;
	byte id;
	byte address;
	byte data[256];
	int state;
};

class WireHost
{
public:
	DevicePropertiesTemplateMaster deviceProperties;
	void check();
	void getData();
	void sendData();
	void sendData(byte);
private:
	byte notFoundCounter;
};

class WireHostLite
{
public:
	bool connected;
	byte id;
	byte address;
	void check();
	byte getData(byte);
	void sendData(byte, byte);
	WireHostLite(byte);
private:
	byte notFoundCounter;
};

class WireClient
{
public:
	DevicePropertiesTemplateSlave deviceProperties;
	void onReceive(int);
	void onRequest();
	byte registerSelect;
	WireClient(byte, byte);
private:
	bool recv;
	int recvData;
};

#define LIB_SIZE (sizeof(NarsSerialCom) + sizeof(PushToggle) + sizeof(UnitConverter) + sizeof(WireSearch) + sizeof(DevicePropertiesTemplateSlave) + sizeof(DevicePropertiesTemplateMaster) + sizeof(WireHost) + sizeof(WireHostLite) + sizeof(WireClient) + sizeof(Logger) + sizeof(RGBA) + sizeof(RGB) + sizeof(HSV) + sizeof(COLOR) + sizeof(TYPES) + sizeof(LOGLEVEL))

#endif // TwoWire_h

#ifndef TwoWire_h
#define LIB_SIZE (sizeof(NarsSerialCom) + sizeof(PushToggle) + sizeof(UnitConverter) + sizeof(Logger) + sizeof(RGBA) + sizeof(RGB) + sizeof(HSV) + sizeof(COLOR) + sizeof(TYPES) + sizeof(LOGLEVEL))
#endif // !TwoWire_h

extern NarsSerialCom SerialCom;
#endif