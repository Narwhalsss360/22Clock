#pragma region Includes
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NarsLibraries.h>
#include <RTClib.h>
#include <EEPROM.h>
#pragma endregion

#pragma region Defenitions
#define SECONDS_IN_HOUR 3600
#define MIN_TIMEZONE -12
#define MAX_TIMEZONE 14

#define BACKLIGHT_TOGGLE_INTERVAL 500
#define RESET_INTERVAL 7000
#define SAVE_INTVERVAL 120000

#define ROTARY_PUSH_DEBOUNCE 55
#define BUTTON_DEBOUNCE 30

#define FRAMERATE 20
#define LCD_ADDR 0x27
#define LCD_ROWS 4
#define LCD_COLS 20
#define DEFAULT_BRIGHTNESS 191

#define RECIPIENT "USER"
#define WAIT_TIME 2000
#pragma endregion 

#pragma region Globals
void (*reset)(void) = NULL;
void rotaryServiceRoutine();
unsigned long uptime;
char daysOfTheWeek[7][10] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
char months[12][10] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
unsigned long lastSaveTime;

enum LINES
{
    LINE_1,
    LINE_2,
    LINE_3,
    LINE_4
};

enum MEM_ADDRESSES
{
    WAS_RESET_ADDRESS,
    BRIGHTNESS_ADDRESS,
    TIME_ZONE_ADDRESS,
    USE_GMT_ADDRESS,
    USE_SHORT_DATE_ADDRESS,
    USE_24H_ADDRESS,
    A1_HOUR_ADDRESS,
    A1_MINUTE_ADDRESS
};
#pragma endregion

#pragma region PINS
struct ROTARYPINS
{
    const byte a = 2,
        b = 2,
        sw = 2;
};

struct PINS
{
    const byte button = 2,
        displayBacklight = 2,
        buzzer = 2;
    const ROTARYPINS rotary;
};

const PROGMEM PINS pins;
#pragma endregion

#pragma region INPUT
struct _INPUT
{
    Rotary rotary = Rotary(true, INPUT_PULLUP, pins.rotary.a, pins.rotary.b);
    Push rotaryPush = Push(pins.rotary.sw, INPUT_PULLUP, ROTARY_PUSH_DEBOUNCE);
    Push button = Push(pins.button, INPUT_PULLUP, BUTTON_DEBOUNCE);

    ROTARYSTATES rotaryState;

    void setup()
    {
        addInterrupt(pins.rotary.a, rotaryServiceRoutine, this->rotary.mode);
    }

    void update()
    {
        this->rotaryPush.update();
        this->button.update();
        rotaryState = this->rotary.getState();
        if (input.button.released())
        {
            unsigned int holdTime = input.button.getReleasedHoldTime();
            if (holdTime >= BACKLIGHT_TOGGLE_INTERVAL)
            {
                if (!display.brightness)
                {
                    display.brightness = display.savedBrightness;
                }
                else
                {
                    display.savedBrightness = display.brightness;
                    display.brightness = ZERO;
                }
            }

            if (holdTime >= RESET_INTERVAL)
            {
                startReset();
            }
        }
    }
};

_INPUT input;
#pragma endregion

#pragma region DISPLAY
struct DISP
{
    enum MENUS
    {
        CLOCKFACE,
        NOTIFICATION
    };

    LiquidCrystal_I2C display = LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
    String nextLines[LCD_ROWS];
    byte brightness;
    byte savedBrightness;
    unsigned long lastFrameTime;
    unsigned int frameInterval;
    MENUS menu = CLOCKFACE;
    byte pointer;
    byte scroll;
    bool cursor;

    void setup()
    {
        pinMode(pins.displayBacklight, OUTPUT);
        this->display.init();
        this->frameInterval = FRAMERATE/1000;
        this->getSaved();
    }

    void send()
    {
        if (uptime - lastFrameTime >= frameInterval)
        {
            analogWrite(pins.displayBacklight, this->brightness);

            if (this->newLines())
            {
                this->saveOldLines();
                this->display.clear();

                if (cursor)
                    this->nextLines[pointer] = '>' + this->nextLines[pointer];

                this->display.setCursor(ZERO, LINE_1);
                this->display.print(this->nextLines[LINE_1]);

                this->display.setCursor(ZERO, LINE_2);
                this->display.print(this->nextLines[LINE_2]);

                this->display.setCursor(ZERO, LINE_3);
                this->display.print(this->nextLines[LINE_3]);

                this->display.setCursor(ZERO, LINE_4);
                this->display.print(this->nextLines[LINE_4]);
                this->lastFrameTime = uptime;
            }
        }
    }

    void next()
    {
        this->lastMenu = this->menu;
        byte temp = (byte)this->menu;
        if (temp == NOTIFICATION)
            temp = CLOCKFACE;
        else
            temp++;
        this->menu = (MENUS)temp;
        this->resetSubSettings();
    }

    void goTo(MENUS go)
    {
        this->lastMenu = this->menu;
        this->menu = go;
        this->resetSubSettings();
    }

    void getSaved()
    {
    this->savedBrightness = EEPROM.read(BRIGHTNESS_ADDRESS);
    if (this->savedBrightness)
        this->brightness = this->savedBrightness;
    else
        this->brightness = DEFAULT_BRIGHTNESS;
    }

    void save()
    {
        EEPROM.update(BRIGHTNESS_ADDRESS, savedBrightness);
    }

    void add(String str, byte row)
    {
        this->nextLines[row] += str;
    }

    void fromRight(String str, byte row)
    {
        for (int spaces = 0; spaces < abs(this->nextLines[row].length() - str.length()); spaces++)
        {
            this->nextLines[row] += ' ';
        }
        this->nextLines[row] += str;
    }

    void clear()
    {
        this->nextLines[LINE_1];
        this->nextLines[LINE_2];
        this->nextLines[LINE_3];
        this->nextLines[LINE_4];
    }
private:
    void resetSubSettings()2
    {
        this->pointer = LINE_1;
        this->scroll = LINE_1;
    }

    void saveOldLines()
    {
        this->oldLines[LINE_1] = this->nextLines[LINE_1];
        this->oldLines[LINE_2] = this->nextLines[LINE_2];
        this->oldLines[LINE_3] = this->nextLines[LINE_3];
        this->oldLines[LINE_4] = this->nextLines[LINE_4];
    }

    bool newLines()
    {
        if (this->nextLines[LINE_1] != this->oldLines[LINE_1] | this->nextLines[LINE_2] != this->oldLines[LINE_2] | this->nextLines[LINE_3] != this->oldLines[LINE_3] | this->nextLines[LINE_4] != this->oldLines[LINE_4])
            return true;
        else
            return false;
    }
    String oldLines[LCD_ROWS];
    MENUS lastMenu;
};

DISP display;
#pragma endregion

#pragma region TIME
struct TIME
{
    RTC_DS3231 rtc;
    DateTime localTime;
    DateTime GMT;
    byte alarmHour = 0;
    byte alarmMinute = 0;
    bool alarm = false;
    int timeZone = 0;
    bool autoOff = true;
    bool useGMT = false;
    bool use24Hour = false;
    bool useShortDate = false;

    void setup()
    {
        pinMode(pins.buzzer, OUTPUT);
        this->rtc.begin();
        this->rtc.disable32K();

        if(rtc.lostPower())
        {
            this->rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }

        this->rtc.clearAlarm(1);
        this->rtc.clearAlarm(2);
        this->rtc.writeSqwPinMode(DS3231_OFF);
    }

    void update()
    {
        this->localTime = this->rtc.now();
        this->GMT = DateTime(this->localTime.unixtime() + (this->timeZone*SECONDS_IN_HOUR));
    }

    void getSaved()
    {
        this->timeZone = EEPROM.read(TIME_ZONE_ADDRESS) - MIN_TIMEZONE;
        this->useGMT = (EEPROM.read(USE_GMT_ADDRESS) > 0) ? true : false;
        this->use24Hour = (EEPROM.read(USE_24H_ADDRESS) > 0) ? true : false;
        this->useShortDate = (EEPROM.read(USE_SHORT_DATE_ADDRESS) > 0) ? true : false;
        this->alarmHour = EEPROM.read(A1_HOUR_ADDRESS);
        this->alarm = EEPROM.read(A1_MINUTE_ADDRESS);
    }

    void save()
    {
        EEPROM.update(TIME_ZONE_ADDRESS, (this->timeZone - MIN_TIMEZONE));
        EEPROM.update(USE_GMT_ADDRESS, (this->useGMT) ? 1 : 0);
        EEPROM.update(USE_24H_ADDRESS, (this->use24Hour) ? 1 : 0);
        EEPROM.update(USE_SHORT_DATE_ADDRESS, (this->useShortDate) ? 1 : 0);
        EEPROM.update(A1_HOUR_ADDRESS, this->alarmHour);
        EEPROM.update(A1_MINUTE_ADDRESS, this->alarmMinute);
    }
};

TIME time;
#pragma endregion

#pragma region Main Methods
void startReset()
{
    save(true);
    display.nextLines[LINE_1] = "Resseting...";
    display.send();
    delay(500);
    reset();
}

void rotaryServiceRoutine()
{
    input.rotary.serviceRoutine();
}

void save(bool wasReset)
{
    if (wasReset)
    {
        EEPROM.write(WAS_RESET_ADDRESS, 1);
    }
    else
    {
        EEPROM.update(WAS_RESET_ADDRESS, 0);
    }

    display.save();
    time.save();
}

bool getSaved()
{

}

void setup()
{
    bool wasReset = getSaved();
    input.setup();
    time.setup();
    display.setup();

    if (wasReset)
    {

    }
    else
    {

    }

    delay(WAIT_TIME);
}

#pragma region Menu Methods
void menuSwitch()
{

}

void clockface()
{
    display.clear();
    display.add(RECIPIENT, LINE_1);

    display.add((time.use24Hour) ? ((time.useGMT) ? time.GMT.hour() : time.GMT.twelveHour() ) : ((time.useGMT ? time.localTime.hour(): time.localTime.twelveHour())), LINE_2);
    display.add(' ', LINE_2);
    display.add(time.localTime.minute(), LINE_2);
    display.add(' ', LINE_2);
    display.add(time.localTime.second(), LINE_2);

    display.add(daysOfTheWeek[(time.useGMT) ? time.GMT.dayOfTheWeek() : time.localTime.dayOfTheWeek()], LINE_3);

    if (time.useShortDate)
    {
        if (time.useGMT)
        {
            display.nextLines[LINE_4] = time.GMT.month();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.GMT.day();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.GMT.year();
        }
        else
        {
            display.nextLines[LINE_4] = time.localTime.month();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.localTime.day();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.localTime.year(); 
        }
    }
    else
    {
        if (time.useGMT)
        {
            display.nextLines[LINE_4] = months[time.GMT.month() - 1];
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.GMT.day();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.GMT.year();
        }
        else
        {
            display.nextLines[LINE_4] = months[time.localTime.month() - 1];
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.localTime.day();
            display.nextLines[LINE_4] += ' ';
            display.nextLines[LINE_4] += time.localTime.year();
        }
    }

    if (input.rotaryPush.released()) display.next();

    display.send();
}

void settings()
{
    display.nextLines[LINE_1] = 
}

void notification()
{
    display.send();

    if (input.button.released()) display.menu = display.CLOCKFACE;
}
#pragma endregion

void loop()
{
    uptime = millis();
    time.update();
    input.update();
    menuSwitch();
    if (uptime - lastSaveTime >= SAVE_INTVERVAL)
    {
        save(false);
    }
}
#pragma endregion