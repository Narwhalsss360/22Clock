#include "22Clock.h"

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
    uptime = millis();
    if (uptime - input.lastRotaryCheck >= ROTARY_IGNORE_TIME)
    {
        input.lastRotaryCheck = uptime;
        input.rotary.serviceRoutine();
    }
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

    //Serial.begin(SERIALCOM_BAUD);

    //delay(WAIT_TIME);
}

#pragma region Menu Methods
void menuSwitch()
{
    if (display.menu == display.CLOCKFACE || display.menu == display.NOTIFICATION)
        display.cursor = false;
    else
        display.cursor = true;

    switch (display.menu)
    {
    case display.CLOCKFACE:
        clockface();
        break;
    case display.SETTINGS:
        settings();
        break;
    case display.TIME_SETS:
        timeSettings();
        break;
    case display.SET_TIME:
        setTime();
        break;
    case display.ALARM_SETS:
        setAlarm();
        break;
    default:
        break;
    }
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

    if (input.rotaryPush.pressed())
        display.goTo(display.SETTINGS);

    if (time.useShortDate)
    {
        if (time.useGMT)
        {
            display.setLine(String(time.GMT.month()), LINE_4);
            display.add(", ", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(time.localTime.month()), LINE_4);
            display.add(", ", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }
    else
    {
        if (time.useGMT)
        {
            display.setLine(String(months[time.GMT.month() - 1]), LINE_4);
            display.add(", ", LINE_4);
            display.add(String(time.GMT.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(months[time.localTime.month() - 1]), LINE_4);
            display.add(", ", LINE_4);
            display.add(String(time.localTime.day()), LINE_4);
            display.add(" ", LINE_4);
            display.add(String(time.localTime.year()), LINE_4);
        }
    }

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

    if (input.button.pressed())
        display.goTo(display.CLOCKFACE);

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

    if (input.button.pressed()) 
        display.goTo(display.SETTINGS);

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
	display.setLineFromRight(String(newTime.second()), LINE_3);
	display.setLine("SET MONTH", LINE_4);
	display.setLineFromRight(String(newTime.month()), LINE_4);
	display.setLine("SET DAY", LINE_5);
	display.setLineFromRight(String(newTime.day()), LINE_5);
	display.setLine("SET YEAR", LINE_6);
	display.setLineFromRight(String(newTime.year()), LINE_6);
	
    if (input.rotaryPush.pressed()) 
        display.edit();

	switch(input.rotaryState)
	{
		case COUNTER_CLOCKWISE:
			if (display.editing)
			{
				switch(display.pointer)
				{
                case LINE_1:
                    changeHour(&newTime, newTime.hour() - 1);
                    break;
                case LINE_2:
                    changeMinute(&newTime, newTime.minute() - 1);
                    break;
                case LINE_3:
                    changeSecond(&newTime, newTime.second() - 1);
                    break;
                case LINE_4:
                    changeMonth(&newTime, newTime.month() - 1);
                    break;
                case LINE_5:
                    changeDay(&newTime, newTime.day() - 1);
                    break;
                case LINE_6:
                    changeYear(&newTime, newTime.year() - 1);
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
                    changeHour(&newTime, newTime.hour() + 1);
                    break;
                case LINE_2:
                    changeMinute(&newTime, newTime.minute() + 1);
                    break;
                case LINE_3:
                    changeSecond(&newTime, newTime.second() + 1);
                    break;
                case LINE_4:
                    changeMonth(&newTime, newTime.month() + 1);
                    break;
                case LINE_5:
                    changeDay(&newTime, newTime.day() + 1);
                    break;
                case LINE_6:
                    changeYear(&newTime, newTime.year() + 1);
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

    if (input.button.pressed()) 
        display.goTo(display.TIME_SETS);

    display.send();
}

void setAlarm()
{
    display.clearLines();
    display.quickSet
    (
        "ENABLED",
        "HOUR",
        "MINUTE",
        ""
    );

    display.setLineFromRight(boolToString(time.alarm), LINE_1);
    display.setLineFromRight(String(time.alarmHour), LINE_2);
    display.setLineFromRight(String(time.alarmMinute), LINE_3);

    if (input.rotaryPush.pressed())
        display.edit();

    if (display.editing)
    {
        switch (input.rotaryState)
        {
        case COUNTER_CLOCKWISE:
            switch (display.pointer)
            {
            case LINE_1:
                time.alarm = false;
                break;
            case LINE_2:
                if (time.alarmHour > HR_MIN)
                    time.alarmHour--;
                break;
            case LINE_3:
                if (time.alarmMinute > MIN_MIN)
                    time.alarmMinute--;
                break;
            default:
                break;
            }
            break;
        case CLOCKWISE:
            switch (display.pointer)
            {
            case LINE_1:
                time.alarm = true;
                break;
            case LINE_2:
                if (time.alarmHour < HR_MIN)
                    time.alarmHour++;
                break;
            case LINE_3:
                if (time.alarmMinute < MIN_MIN)
                    time.alarmMinute++;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (input.rotaryState)
        {
        case COUNTER_CLOCKWISE:
            display.movePointerUp();
            break;
        case CLOCKWISE:
            display.movePointerDown();
            break;
        default:
            break;
        }
    }
    
    if (input.button.pressed())
        display.goTo(display.CLOCKFACE);

    display.send();
}

void notification()
{
    display.send();

    if (input.button.pressed()) display.menu = display.CLOCKFACE;
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