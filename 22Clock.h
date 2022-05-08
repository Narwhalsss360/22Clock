#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "NarsLibraries.h"
#include <RTClib.h>
#include <EEPROM.h>

#pragma region Defenitions
#define SECONDS_MINIMUM ZERO
#define SECONDS_MAXIMUM 59
#define MINUTES_MINIMUM ZERO
#define MINUTES_MAXIMUM 59
#define HOURS_MINIMUM ZERO
#define HOURS_MAXIMUM 23
#define DAYS_MINIMUM 1
#define DAYS_MAXIMUM 31
#define MONTHS_MINIMUM 1
#define MONTHS_MAXIMUM 12
#define YEARS_MINIMUM 1970
#define YEARS_MAXIMUM 2038
#define SECONDS_IN_HOUR 3600
#define TIMEZONE_MINIMUM -12
#define TIMEZONE_MAXIMUM 14
#define TIME_RECV_ADDR 1
#define TIME_SEND_ADDR 1
#define TIME_SEND_INTERVAL 500

#define BUZZER_INTERVAL 500

#define BACKLIGHT_TOGGLE_INTERVAL 500
#define RESET_INTERVAL 7000
#define SAVE_INTVERVAL 120000

#define ROTARY_DEBOUNCE 100
#define ROTARY_PUSH_DEBOUNCE 55
#define BUTTON_DEBOUNCE 30

#define LCD_FRAMERATE 10
#define LCD_ADDR 0x27
#define LCD_ROWS 4
#define LCD_COLS 20
#define LCD_DEFAULT_BRIGHTNESS 191
#define LCD_LINES 8
#define LCD_BLINK_INTERVAL 1000

#define USER "USER"
#define START_WAIT_TIME 2000

#define isBetweenInclude(low, val, high) low <= val && val <= high
#define isBetween(low, val, high) low < val && val < high
#pragma endregion

#pragma region Change DateTime
void changeSecond(DateTime* pDt, byte newSec)
{
	if (isBetweenInclude(SECONDS_MINIMUM, newSec, SECONDS_MAXIMUM))
	    *pDt = DateTime(pDt->year(), pDt->month(), pDt->day(), pDt->hour(), pDt->minute(), newSec);
}

void changeMinute(DateTime* pDt, byte newMin)
{
	if (isBetweenInclude(MINUTES_MINIMUM, newMin, MINUTES_MAXIMUM))
	    *pDt = DateTime(pDt->year(), pDt->month(), pDt->day(), pDt->hour(), newMin, pDt->second());
}

void changeHour(DateTime* pDt, byte newHr)
{
	if (isBetweenInclude(HOURS_MINIMUM, newHr, HOURS_MAXIMUM))
        *pDt = DateTime(pDt->year(), pDt->month(), pDt->day(), newHr, pDt->minute(), pDt->second());
}

void changeDay(DateTime* pDt, byte newDay)
{
	if(isBetweenInclude(DAYS_MINIMUM, newDay, DAYS_MAXIMUM))
	    *pDt = DateTime(pDt->year(), pDt->month(), newDay, pDt->hour(), pDt->minute(), pDt->second());
}

void changeMonth(DateTime* pDt, byte newMon)
{
	if (isBetweenInclude(MONTHS_MINIMUM, newMon, MONTHS_MAXIMUM))
	    *pDt = DateTime(pDt->year(), newMon, pDt->day(), pDt->hour(), pDt->minute(), pDt->second());
}

void changeYear(DateTime* pDt, unsigned short newYr)
{
	if (isBetweenInclude(YEARS_MINIMUM, newYr, YEARS_MAXIMUM))
	    *pDt = DateTime(newYr, pDt->month(), pDt->day(), pDt->hour(), pDt->minute(), pDt->second());
}
#pragma endregion

#pragma region Globals
char daysOfTheWeek[7][10] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
char months[12][10] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
void reset();
void rotaryServiceRoutine();
unsigned long uptime = ZERO;
unsigned long lastTimeSaved = ZERO;
unsigned long lastTimeSent = ZERO;

enum LINE_NUMS
{
    LINE_1,
    LINE_2,
    LINE_3,
    LINE_4,
    LINE_5,
    LINE_6,
    LINE_7,
    LINE_8
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
    A1_MINUTE_ADDRESS,
    A1_ENABLE_ADDRESS
};
#pragma endregion

#pragma region PINS
struct ROTARYPINS
{
    const byte a = 2,
        b = 3,
        sw = 4;
};

struct PINS
{
    const byte button = 5,
        displayBacklight = 7,
        buzzer = 8;
    const ROTARYPINS rotary;
};

const PROGMEM PINS pins;
#pragma endregion

#pragma region DISPLAY
struct DISP
{
    enum MENUS
    {
        CLOCKFACE,
        MAIN_SETTINGS,
            TIME_SETTINGS,
                SET_TIME,
            ALARM_SETTINGS,
        NOTIFICATION
    };

    LiquidCrystal_I2C display = LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
    String nextLines[LCD_LINES];
    byte brightness;
    byte savedBrightness;
    unsigned long lastBlink = ZERO;
    unsigned long lastFrameTime;
    unsigned int frameInterval;
    MENUS menu = CLOCKFACE;
    byte pointer = ZERO;
    byte scroll = ZERO;
    bool editing = false;
    bool cursor;
    byte cursorCol;

    void setup()
    {
        pinMode(pins.displayBacklight, OUTPUT);
        this->display.init();
        this->display.setBacklight(BYTE_MAX);
        this->display.setCursor(ZERO, ZERO);
        this->frameInterval = 1000/LCD_FRAMERATE;
        this->getSaved();
    }

    void send()
    {
        if (uptime - this->lastFrameTime >= this->frameInterval)
        {
            this->lastFrameTime = uptime;
            analogWrite(pins.displayBacklight, this->brightness);

            if (cursor)
                    this->cursorCol = this->addPointer(this->pointer);

            if (this->newLines())
            {
                this->saveOldLines();
                this->display.clear();

                this->display.setCursor(ZERO, LINE_1);
                this->display.print(this->nextLines[LINE_1 + this->scroll]);

                this->display.setCursor(ZERO, LINE_2);
                this->display.print(this->nextLines[LINE_2 + this->scroll]);

                this->display.setCursor(ZERO, LINE_3);
                this->display.print(this->nextLines[LINE_3 + this->scroll]);

                this->display.setCursor(ZERO, LINE_4);
                this->display.print(this->nextLines[LINE_4 + this->scroll]);
                
                if (this->cursor)
                    display.setCursor(this->cursorCol, this->pointer);
            }
        }
    }

    void goToMenu(MENUS go)
    {
        this->menu = go;
        this->resetSubSettings();
    }

    void getSaved()
    {
    this->savedBrightness = EEPROM.read(BRIGHTNESS_ADDRESS);
    if (this->savedBrightness)
        this->brightness = this->savedBrightness;
    else
        this->brightness = LCD_DEFAULT_BRIGHTNESS;
    }

    void save()
    {
        EEPROM.update(BRIGHTNESS_ADDRESS, savedBrightness);
    }

    void setLine(String str, byte row)
    {
        this->nextLines[row] = str;
    }

    void addToLine(String str, byte row)
    {
        this->setLine(this->nextLines[row] + str, row);
    }

    void quickSetLines(String str1, String str2, String str3, String str4)
    {
        this->clearLines();
        this->setLine(str1, LINE_1);
        this->setLine(str2, LINE_2);
        this->setLine(str3, LINE_3);
        this->setLine(str4, LINE_4);
        this->setLine("", LINE_5);
        this->setLine("", LINE_6);
        this->setLine("", LINE_7);
        this->setLine("", LINE_8);
    }

    void setLineFromRight(String str, byte row)
    {
        byte count = LCD_COLS - this->nextLines[row].length() - str.length();
        for (byte spaces = ZERO; spaces < count; spaces++)
        {
            this->nextLines[row] += ' ';
        }
        this->nextLines[row] += str;
    }

    void clearLines()
    {
        this->nextLines[LINE_1] = "";
        this->nextLines[LINE_2] = "";
        this->nextLines[LINE_3] = "";
        this->nextLines[LINE_4] = "";
        this->nextLines[LINE_5] = "";
        this->nextLines[LINE_6] = "";
        this->nextLines[LINE_7] = "";
        this->nextLines[LINE_8] = "";
    }

    void edit()
    {
        this->editing = !this->editing;
        (this->editing) ? this->display.blink() : this->display.noBlink();
    }

    void increaseBrightness()
    {
        if (this->brightness != BYTE_MAX)
        {
            this->brightness++;
        }
    }

    void decreaseBrightness()
    {
        if (this->brightness > ZERO)
        {
            this->brightness--;
        }
    }

    void movePointerUp()
    {
        if (this->pointer > LINE_1)
        {
            this->pointer--;
            if (this->pointer < LINE_4)
            {
                this->scroll = 0;
            }
        }
        else
        {
            this->pointer = this->getLastLine();
            if (this->pointer > LINE_4)
                this->scroll = 4;
        }
    }

    void movePointerDown()
    {
        byte last = this->getLastLine();
        if (this->pointer < last )
        {
            this->pointer++;
            if (pointer > LINE_4)
            {
                this->scroll = 4;
            }
        }
        else
        {
            this->pointer = LINE_1;
            this->scroll = ZERO;
        }
    }
private:
    void resetSubSettings()
    {
        this->pointer = LINE_1;
        this->scroll = LINE_1;
        this->cursor = false;
        this->editing = false;
    }

    void saveOldLines()
    {
        this->oldLines[LINE_1] = this->nextLines[LINE_1];
        this->oldLines[LINE_2] = this->nextLines[LINE_2];
        this->oldLines[LINE_3] = this->nextLines[LINE_3];
        this->oldLines[LINE_4] = this->nextLines[LINE_4];
        this->oldLines[LINE_5] = this->nextLines[LINE_5];
        this->oldLines[LINE_6] = this->nextLines[LINE_6];
        this->oldLines[LINE_7] = this->nextLines[LINE_7];
        this->oldLines[LINE_8] = this->nextLines[LINE_8];
    }

    bool newLines()
    {
        if (this->nextLines[LINE_1] != this->oldLines[LINE_1] || this->nextLines[LINE_2] != this->oldLines[LINE_2] || this->nextLines[LINE_3] != this->oldLines[LINE_3] || this->nextLines[LINE_4] != this->oldLines[LINE_4]
         || this->nextLines[LINE_5] != this->oldLines[LINE_5] || this->nextLines[LINE_6] != this->oldLines[LINE_6] || this->nextLines[LINE_7] != this->oldLines[LINE_7] || this->nextLines[LINE_8] != this->oldLines[LINE_8]
        )
            return true;
        else
            return false;
    }
    
    byte getLastLine()
    {
        for (byte line = LINE_8; line != LINE_1; line--)
        {
            if (this->nextLines[line] != "")
            {
                return line;
            }
        }
    }

    byte addPointer(byte row)
    {
        if (this->nextLines[row].length() == 20)
        {
            for (byte col = LCD_COLS - 1; col > ZERO; col--)
            {
                if (col != 0)
                {
                    if (this->nextLines[row][col] == ' ')
                    {
                        this->nextLines[row][col] = '>';
                        return col;
                    }
                }
            }
        }
        else
        {
            this->nextLines[row] += '<';
        }
    }

    String oldLines[LCD_LINES];
};

DISP display;
#pragma endregion

#pragma region INPUT
struct _INPUT
{
    Rotary rotary = Rotary(pins.rotary.a, pins.rotary.b, true, INPUT_PULLUP, ROTARY_DEBOUNCE);
    Push rotaryPush = Push(pins.rotary.sw, INPUT_PULLUP, ROTARY_PUSH_DEBOUNCE);
    Push button = Push(pins.button, INPUT_PULLUP, BUTTON_DEBOUNCE);
    ROTARYSTATES rotaryState;
    unsigned long lastRotaryCheck = ZERO;
    
    void setup()
    {
        addInterrupt(pins.rotary.a, rotaryServiceRoutine, this->rotary.mode);
    }

    void update()
    {
        this->rotaryPush.update();
        this->button.update();
        this->rotaryState = (ROTARYSTATES)this->rotary.getState();
        if (this->button.released())
        {
            unsigned int holdTime = this->button.getReleasedHoldTime();
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
                reset();
            }
        }
    }
};

_INPUT input;
#pragma endregion

#pragma region TIME
struct TIME
{
    RTC_DS3231 rtc;
    DateTime localTime;
    DateTime GMT;
    byte alarmHour = ZERO;
    byte alarmMinute = ZERO;
    bool alarm = false;
    bool fired = false;
    int timeZone = ZERO;
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
    
        if (this->localTime.hour() == this->alarmHour)
        {
            if (this->localTime.minute() == this->alarmMinute)
            {
                if (isBetweenInclude(ZERO, this->localTime.second(), 5))
                {
                    this->fired = true;
                }
            }
        }

        if (this->fired)
        {
            display.quickSetLines
            (
                "ALARM",
                "PRESS BUTTON",
                (String(this->alarmHour) + ':' + String(this->alarmMinute)),
                ""
            );
            display.goToMenu(display.NOTIFICATION);
            this->buzz();
            if (input.button.current()) 
            {
                this->fired = false;
                digitalWrite(pins.buzzer, LOW);
            }
        }
    }

    void decreaseTimeZone()
    {
        if (this->timeZone != TIMEZONE_MINIMUM) this->timeZone--;
    }

    void increaseTimeZone()
    {
        if (this->timeZone != TIMEZONE_MAXIMUM) this->timeZone++;
    }

    void getSaved()
    {
        this->timeZone = EEPROM.read(TIME_ZONE_ADDRESS) + TIMEZONE_MINIMUM;
        this->useGMT = (EEPROM.read(USE_GMT_ADDRESS) > 0) ? true : false;
        this->use24Hour = (EEPROM.read(USE_24H_ADDRESS) > 0) ? true : false;
        this->useShortDate = (EEPROM.read(USE_SHORT_DATE_ADDRESS) > 0) ? true : false;
        this->alarm = (EEPROM.read(A1_ENABLE_ADDRESS) > 0) ? true : false;
        this->alarmHour = EEPROM.read(A1_HOUR_ADDRESS);
        this->alarmMinute = EEPROM.read(A1_MINUTE_ADDRESS);
    }

    void save()
    {
        EEPROM.update(TIME_ZONE_ADDRESS, (this->timeZone - TIMEZONE_MINIMUM));
        EEPROM.update(USE_GMT_ADDRESS, (this->useGMT) ? 1 : 0);
        EEPROM.update(USE_24H_ADDRESS, (this->use24Hour) ? 1 : 0);
        EEPROM.update(USE_SHORT_DATE_ADDRESS, (this->useShortDate) ? 1 : 0);
        EEPROM.update(A1_HOUR_ADDRESS, this->alarmHour);
        EEPROM.update(A1_MINUTE_ADDRESS, this->alarmMinute);
        EEPROM.update(A1_ENABLE_ADDRESS, (this->alarm) ? 1 : 0 );
    }

    void decreaseAlarmHour()
    {
        if (this->alarmHour > HOURS_MINIMUM)
            this->alarmHour--;

    }

    void increaseAlarmHour()
    {
        if (this->alarmHour < HOURS_MAXIMUM)
            this->alarmHour++;
    }

    void decreaseAlarmMinute()
    {
        if (this->alarmMinute > MINUTES_MINIMUM)
            this->alarmMinute--;
    }

    void increaseAlarmMinute()
    {
        if (this->alarmMinute < MINUTES_MAXIMUM)
            this->alarmMinute++;
    }
private:
    unsigned long lastBuzz;
    bool currentBuzz = false;

    void buzz()
    {
        if (uptime - this->lastBuzz >= BUZZER_INTERVAL)
        {
            this->lastBuzz = uptime;
            this->currentBuzz = !this->currentBuzz;
            digitalWrite(pins.buzzer, this->currentBuzz);
        }
    }
};

TIME time;
#pragma endregion