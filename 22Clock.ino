#include "22Clock.h"

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
    //time.setup();
    //display.setup();

    if (wasReset)
    {

    }
    else
    {

    }

    Serial.begin(1000000);

    while (true)
    {
        uptime = millis();
        //input.update();
        Serial.println(input.rotary.getState());
    }
    
    delay(WAIT_TIME);
}

#pragma region Menu Methods
void menuSwitch()
{

}

void clockface()
{
    display.clearLines();
    display.setLine(RECIPIENT, LINE_1);

    display.setLine(String((time.use24Hour) ? ((time.useGMT) ? time.GMT.hour() : time.GMT.twelveHour() ) : ((time.useGMT ? time.localTime.hour(): time.localTime.twelveHour()))), LINE_2);
    display.add(":", LINE_2);
    display.add(String(time.localTime.minute()), LINE_2);
    display.add(":", LINE_2);
    display.add(String(time.localTime.second()), LINE_2);

    display.setLine(daysOfTheWeek[(time.useGMT) ? time.GMT.dayOfTheWeek() : time.localTime.dayOfTheWeek()], LINE_3);

    if (time.useShortDate)
    {
        if (time.useGMT)
        {
            display.setLine(String(time.GMT.month()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(time.localTime.month()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }
    else
    {
        if (time.useGMT)
        {
            display.setLine(String(months[time.GMT.month() - 1]), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(months[time.localTime.month() - 1]), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }

    if (input.backPress()) display.next();

    display.send();
}

void settings()
{
    display.clearLines();
    display.quickSet
    (
        "TIME SETTINGS",
        "ALARM SETTINGS",
        "BRIGHTNESS",
        ""
    );

    display.setLineFromRight(String(display.brightness), LINE_3);

    if (input.rotaryPush.pressed())
    {
        switch (display.pointer)
        {
        case LINE_1: display.goTo(display.TIME_SETS);
            break;
        case LINE_2: display.goTo(display.ALARM_SETS);
            break;
        case LINE_3: display.edit();
            break;
        default:
            break;
        }
    }
    
    switch (input.rotaryState)
    {
    case ROTARYSTATES::CLOCKWISE:
        if (display.editing && display.pointer == LINE_3) display.increaseBrightness();
        if (!display.editing) display.movePointerDown();
        break;
    case ROTARYSTATES::COUNTER_CLOCKWISE:
        if (display.editing && display.pointer == LINE_3) display.decreaseBrightness();
        if (!display.editing) display.movePointerUp();
        break;
    default:
        break;
    }

    if (input.backPress()) display.goTo(display.CLOCKFACE);

    display.send();
}

void timeSettings()
{
    display.clearLines();
    display.setLine("SET TIME", LINE_1);
    display.setLine("USE 24 HOUR", LINE_2);
    display.setLineFromRight(boolToString(time.use24Hour), LINE_2);
    display.setLine("USE GMT", LINE_3);
    display.setLineFromRight(boolToString(time.useGMT), LINE_3);
    display.setLine("TIMEZONE", LINE_4);
    display.setLineFromRight(String(time.timeZone), LINE_4);

    if (input.rotaryPush.pressed())
    {
        switch (display.pointer)
        {
        case LINE_1:
            display.goTo(display.SET_TIME);
            break;
        case LINE_2:
        case LINE_3:
        case LINE_4:
            display.edit();
            break;
        default:
            break;
        }
    }
    
    switch (input.rotaryState)
    {
    case COUNTER_CLOCKWISE:
        if (display.editing)
        {
            switch (display.pointer)
            {
            case LINE_2:
                time.use24Hour = false;
                break;
            case LINE_3:
                time.useGMT = false;
                break;
            case LINE_4:
                time.decreaseTimeZone();
                break;
            default:
                break;
            }
        }
        else
        {
            display.movePointerUp();
        }
        break;
    case CLOCKWISE:
        if (display.editing)
        {
            switch (display.pointer)
            {
            case LINE_2:
                time.use24Hour = true;
                break;
            case LINE_3:
                time.useGMT = true;
                break;
            case LINE_4:
                time.increaseTimeZone();
                break;
            default:
                break;
            }
        }
        else
        {
            display.movePointerDown();
        }
        break;   
    default:
        break;
    }

    if (input.backPress()) display.goTo(display.SETTINGS);

    display.send();
}

void setTime()
{
    DateTime newTime = time.localTime;
    display.clearLines();
    display.setLine("SET HOUR", LINE_1);
	display.setLineFromRight(String(newTime.hour()), LINE_1);
	display.setLine("SET MINUTE", LINE_2);
	display.setLineFromRight(String(newTime.minute()), LINE_2);
	display.setLine("SET SECOND", LINE_3);
	display.setLineFromRight(String(newTime.second), LINE_3);
	display.setLine("SET MONTH", LINE_4);
	display.setLineFromRight(String(newTime.month()), LINE_4);
	display.setLine("SET DAY", LINE_5);
	diaplay.setLineFromRight(String(newTime.day()), LINE_5);
	display.setLine("SET YEAR", LINE_6);
	display.setLineFromRight(String(newTime.year()), LINE_6);
	
	switch(input.rotaryState)
	{
		case COUNTER_CLOCKWISE:
			if (display.editing)
			{
				switch(display.pointer)
				{
					case LINE_1:
						changeHour(&newTime, newTime.second() + 1);
						break;
					case LINE_2:
						break;
					case LINE_3:
						break;
					case LINE_4:
						break;
					case LINE_5:
						break;
					case LINE_6:
						break;
					default:
						break;
				}
			}
			else
			{
				display.movePointerUp();
			}
			break;
		case CLOCKWISE:
			if (display.editing)
			{
				switch(display.pointer)
				{
					case LINE_1:
						break;
					case LINE_2:
						break;
					case LINE_3:
						break;
					case LINE_4:
						break;
					case LINE_5:
						break;
					case LINE_6:
						break;
					default:
						break;
				}
			}
			else
			{
				display.movePointerDown();
			}
			break;
		default:
			break;
	}
	
	if(newTime.isValid())
		if (newTime != time.localTime)
			time.rtc.adjust(newTime);
		
    display.send();
}

void notification()
{
    display.clearLines();
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
#pragma endregion >= RESET_INTERVAL)
            {
                startReset();
            }
        }
    }

    bool backPress()
    {
        if (this->button.released() && this->button.getReleasedHoldTime() < BACKLIGHT_TOGGLE_INTERVAL)
            return true;
        else
            return false;
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
    }

    void decreaseTimeZone()
    {
        if (this->timeZone != MIN_TIMEZONE) this->timeZone--;
    }

    void increaseTimeZone()
    {
        if (this->timeZone != MAX_TIMEZONE) this->timeZone++;
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
    //time.setup();
    //display.setup();

    if (wasReset)
    {

    }
    else
    {

    }

    Serial.begin(1000000);

    while (true)
    {
        uptime = millis();
        //input.update();
        Serial.println(input.rotary.getState());
    }
    
    delay(WAIT_TIME);
}

#pragma region Menu Methods
void menuSwitch()
{

}

void clockface()
{
    display.clearLines();
    display.setLine(RECIPIENT, LINE_1);

    display.setLine(String((time.use24Hour) ? ((time.useGMT) ? time.GMT.hour() : time.GMT.twelveHour() ) : ((time.useGMT ? time.localTime.hour(): time.localTime.twelveHour()))), LINE_2);
    display.add(":", LINE_2);
    display.add(String(time.localTime.minute()), LINE_2);
    display.add(":", LINE_2);
    display.add(String(time.localTime.second()), LINE_2);

    display.setLine(daysOfTheWeek[(time.useGMT) ? time.GMT.dayOfTheWeek() : time.localTime.dayOfTheWeek()], LINE_3);

    if (time.useShortDate)
    {
        if (time.useGMT)
        {
            display.setLine(String(time.GMT.month()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(time.localTime.month()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(":", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }
    else
    {
        if (time.useGMT)
        {
            display.setLine(String(months[time.GMT.month() - 1]), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(months[time.localTime.month() - 1]), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }

    if (input.backPress()) display.next();

    display.send();
}

void settings()
{
    display.clearLines();
    display.quickSet
    (
        "TIME SETTINGS",
        "ALARM SETTINGS",
        "BRIGHTNESS",
        ""
    );

    display.setLineFromRight(String(display.brightness), LINE_3);

    if (input.rotaryPush.pressed())
    {
        switch (display.pointer)
        {
        case LINE_1: display.goTo(display.TIME_SETS);
            break;
        case LINE_2: display.goTo(display.ALARM_SETS);
            break;
        case LINE_3: display.edit();
            break;
        default:
            break;
        }
    }
    
    switch (input.rotaryState)
    {
    case ROTARYSTATES::CLOCKWISE:
        if (display.editing && display.pointer == LINE_3) display.increaseBrightness();
        if (!display.editing) display.movePointerDown();
        break;
    case ROTARYSTATES::COUNTER_CLOCKWISE:
        if (display.editing && display.pointer == LINE_3) display.decreaseBrightness();
        if (!display.editing) display.movePointerUp();
        break;
    default:
        break;
    }

    if (input.backPress()) display.goTo(display.CLOCKFACE);

    display.send();
}

void timeSettings()
{
    display.clearLines();
    display.setLine("SET TIME", LINE_1);
    display.setLine("USE 24 HOUR", LINE_2);
    display.setLineFromRight(boolToString(time.use24Hour), LINE_2);
    display.setLine("USE GMT", LINE_3);
    display.setLineFromRight(boolToString(time.useGMT), LINE_3);
    display.setLine("TIMEZONE", LINE_4);
    display.setLineFromRight(String(time.timeZone), LINE_4);

    if (input.rotaryPush.pressed())
    {
        switch (display.pointer)
        {
        case LINE_1:
            display.goTo(display.SET_TIME);
            break;
        case LINE_2:
        case LINE_3:
        case LINE_4:
            display.edit();
            break;
        default:
            break;
        }
    }
    
    switch (input.rotaryState)
    {
    case COUNTER_CLOCKWISE:
        if (display.editing)
        {
            switch (display.pointer)
            {
            case LINE_2:
                time.use24Hour = false;
                break;
            case LINE_3:
                time.useGMT = false;
                break;
            case LINE_4:
                time.decreaseTimeZone();
                break;
            default:
                break;
            }
        }
        else
        {
            display.movePointerUp();
        }
        break;
    case CLOCKWISE:
        if (display.editing)
        {
            switch (display.pointer)
            {
            case LINE_2:
                time.use24Hour = true;
                break;
            case LINE_3:
                time.useGMT = true;
                break;
            case LINE_4:
                time.increaseTimeZone();
                break;
            default:
                break;
            }
        }
        else
        {
            display.movePointerDown();
        }
        break;   
    default:
        break;
    }

    if (input.backPress()) display.goTo(display.SETTINGS);

    display.send();
}

void setTime()
{
    DateTime newTime = time.localTime;
    display.clearLines();
    display.setLine("SET HOUR", LINE_1);
	display.setLineFromRight(String(newTime.hour()), LINE_1);
	display.setLine("SET MINUTE", LINE_2);
	display.setLineFromRight(String(newTime.minute()), LINE_2);
	display.setLine("SET SECOND", LINE_3);
	display.setLineFromRight(String(newTime.second), LINE_3);
	display.setLine("SET MONTH", LINE_4);
	display.setLineFromRight(String(newTime.month()), LINE_4);
	display.setLine("SET DAY", LINE_5);
	diaplay.setLineFromRight(String(newTime.day()), LINE_5);
	display.setLine("SET YEAR", LINE_6);
	display.setLineFromRight(String(newTime.year()), LINE_6);
	
	switch(input.rotaryState)
	{
		case COUNTER_CLOCKWISE:
			if (display.editing)
			{
				switch(display.pointer)
				{
					case LINE_1:
						break;
					case LINE_2:
						break;
					case LINE_3:
						break;
					case LINE_4:
						break;
					case LINE_5:
						break;
					case LINE_6:
						break;
					default:
						break;
				}
			}
			else
			{
				display.movePointerUp();
			}
			break;
		case CLOCKWISE:
			if (display.editing)
			{
				switch(display.pointer)
				{
					case LINE_1:
						break;
					case LINE_2:
						break;
					case LINE_3:
						break;
					case LINE_4:
						break;
					case LINE_5:
						break;
					case LINE_6:
						break;
					default:
						break;
				}
			}
			else
			{
				display.movePointerDown();
			}
			break;
		default:
			break;
	}
	
	if(newTime.isValid())
		if (newTime != time.localTime)
			time.rtc.adjust(newTime);
		
    display.send();
}

void notification()
{
    display.clearLines();
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
#pragma endregion	}
			}
			else
			{
				display.movePointerDown();
			}
			break;
		default:
			break;
	}
	
	if(newTime.isValid())
		if (newTime != time.localTime)
			time.rtc.adjust(newTime);
		
    display.send();
}

void notification()
{
    display.clearLines();
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
#pragma endregionased()) display.menu = display.CLOCKFACE;
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