#include "22Clock.h"

void reset()
{
    saveAll(true);
    display.quickSetLines
    (
        "Resseting...",
        "",
        "",
        ""
    );
    display.goToMenu(display.NOTIFICATION);
    delay(500);
    void (*pReset) (void) = NULL;
    pReset();
}

void rotaryServiceRoutine()
{
    input.rotary.serviceRoutine();
}

void saveAll(bool wasReset)
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

bool getAllSaved()
{
    display.getSaved();
    time.getSaved();
    return (EEPROM.read(WAS_RESET_ADDRESS) > 0) ? true : false;
}

void comRecv(unsigned int addr, unsigned long data)
{
    if (addr == TIME_RECV_ADDR)
    {
        time.localTime = DateTime(data);
        display.clearLines();
        display.quickSetLines
        (
            "TIME RECEIVED",
            "UNIX",
            String(data),
            ""
        );
        display.goToMenu(display.NOTIFICATION);
    }
}

void serialEvent()
{
    SerialCom.onSerialEvent(&comRecv, NULL);
}

void comSendTime()
{
    SerialCom.send(TIME_SEND_ADDR, time.localTime.unixtime());
}

void setup()
{
    bool wasReset = getAllSaved();
    input.setup();
    time.setup();
    display.setup();

    Serial.begin(SERIALCOM_BAUD);

    display.clearLines();
    if (wasReset)
    {
        display.quickSetLines
        (
            "Welcome",
            USER,
            "Was reset.",
            ""
        );
    }
    else
    {
        display.quickSetLines
        (
            "Welcome",
            USER,
            "",
            ""
        );
    }
    display.goToMenu(display.NOTIFICATION);
    //delay(WAIT_TIME);
    display.goToMenu(display.CLOCKFACE);
}

#pragma region Menu Methods
void menuSwitching()
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
    case display.MAIN_SETTINGS:
        settings();
        break;
    case display.TIME_SETTINGS:
        timeSettings();
        break;
    case display.SET_TIME:
        setTime();
        break;
    case display.ALARM_SETTINGS:
        setAlarm();
        break;
    case display.NOTIFICATION:
        notification();
        break;
    default:
    
        break;
    }
}

void clockface()
{
    display.clearLines();
    display.setLine(USER, LINE_1);

    display.setLine(String((time.use24Hour) ? ((time.useGMT) ? time.GMT.hour() : time.localTime.hour()) : ((time.useGMT) ? time.GMT.twelveHour() : time.localTime.twelveHour())) , LINE_2);
    display.addToLine(":", LINE_2);
    display.addToLine(String(time.localTime.minute()), LINE_2);
    display.addToLine(":", LINE_2);
    display.addToLine(String(time.localTime.second()), LINE_2);

    if (!time.use24Hour)
        if (time.useGMT)
            display.addToLine((time.GMT.isPM()) ? " PM" : " AM", LINE_2);
        else
            display.addToLine((time.localTime.isPM()) ? " PM" : " AM", LINE_2);

    display.setLine(daysOfTheWeek[(time.useGMT) ? time.GMT.dayOfTheWeek() : time.localTime.dayOfTheWeek()], LINE_3);

    if (input.rotaryPush.pressed())
        display.goToMenu(display.MAIN_SETTINGS);

    if (time.useShortDate)
    {
        if (time.useGMT)
        {
            display.setLine(String(time.GMT.month()), LINE_4);
            display.addToLine(", ", LINE_4);
            display.addToLine(String(time.GMT.day()), LINE_4);
            display.addToLine(" ", LINE_4);
            display.addToLine(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(time.localTime.month()), LINE_4);
            display.addToLine(", ", LINE_4);
            display.addToLine(String(time.localTime.day()), LINE_4);
            display.addToLine(" ", LINE_4);
            display.addToLine(String(time.localTime.year()), LINE_4);
        }
    }
    else
    {
        if (time.useGMT)
        {
            display.setLine(String(months[time.GMT.month() - 1]), LINE_4);
            display.addToLine(", ", LINE_4);
            display.addToLine(String(time.GMT.day()), LINE_4);
            display.addToLine(" ", LINE_4);
            display.addToLine(String(time.GMT.year()), LINE_4);
        }
        else
        {
            display.setLine(String(months[time.localTime.month() - 1]), LINE_4);
            display.addToLine(", ", LINE_4);
            display.addToLine(String(time.localTime.day()), LINE_4);
            display.addToLine(" ", LINE_4);
            display.addToLine(String(time.localTime.year()), LINE_4);
        }
    }

    display.send();
}

void settings()
{
    display.clearLines();
    display.quickSetLines
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
        case LINE_1: display.goToMenu(display.TIME_SETTINGS);
            break;
        case LINE_2: display.goToMenu(display.ALARM_SETTINGS);
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
        display.goToMenu(display.CLOCKFACE);

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
            display.goToMenu(display.SET_TIME);
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
        display.goToMenu(display.MAIN_SETTINGS);

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
        display.goToMenu(display.TIME_SETTINGS);

    display.send();
}

void setAlarm()
{
    display.clearLines();
    display.quickSetLines
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

    switch (input.rotaryState)
    {
    case COUNTER_CLOCKWISE:
        if (display.editing)
        {
            switch (display.pointer)
            {
            case LINE_1:
                time.alarm = false;
                break;
            case LINE_2:
                time.decreaseAlarmHour();
                break;
            case LINE_3:
                time.decreaseAlarmMinute();
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
            case LINE_1:
                time.alarm = true;
                break;
            case LINE_2:
                time.increaseAlarmHour();
                break;
            case LINE_3:
                time.increaseAlarmMinute();
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
        display.goToMenu(display.MAIN_SETTINGS);

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
    menuSwitching();
    if (uptime - lastTimeSent >= TIME_SEND_INTERVAL)
    {
        lastTimeSent = uptime;
        comSendTime();
    }
    if (uptime - lastTimeSaved >= SAVE_INTVERVAL)
    {
        saveAll(false);
    }
}