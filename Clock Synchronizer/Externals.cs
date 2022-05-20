using System;
using System.ComponentModel;
using Nars_Libraries_Framework48;
using Nars_Libraries_Framework48.Serial;

namespace Clock_Synchronizer
{
    public static class Externals
    {
        const int UnixTimeAddress = 0x0001;
        const int NameAddress = 0x0002;
        const int ExternalInterval = 50;

        public static DateTime PCTime;
        public static DateTime ClockTime;
        public static string Name = "";
        public static DateTimeConverter DateTimeConverter = new DateTimeConverter();
        public static System.Timers.Timer ExternalsTimer = new System.Timers.Timer(ExternalInterval);
        public static NarsSerialCom Port = new NarsSerialCom();

        public static void GetClockTime()
        {
            Receive NewReceive = Port.getData((ushort)UnixTimeAddress);
            if (!NewReceive.success) return;
            ClockTime = UnixTimeToDateTime((uint)NewReceive.data);
        }

        public static Result SendPCTime()
        {
            return Port.send(UnixTimeAddress, DateTimeToUnix(DateTime.Now));
        }

        public static void GetClockName()
        {
            Receive NewReceive = Port.getData(NameAddress);
            if (!NewReceive.success) return;
            Name = (string)NewReceive.data;
        }

        public static Result SendName(string NewName)
        {
            return Port.send((ushort)NameAddress, NewName);
        }

        public static void ExternalsLoop()
        {
            PCTime = DateTime.Now;
            if (Port.state == State.Connected)
            {
                Port.checkQueue();
                GetClockTime();
                GetClockName();
            }
            
        }

        public static DateTime UnixTimeToDateTime(double UnixTimeStamp)
        {
            return new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Local).AddSeconds(UnixTimeStamp).ToLocalTime();
        }

        public static uint DateTimeToUnix(DateTime _DateTime)
        {
            return (uint)(_DateTime - new DateTime(1970, 1, 1, 0, 0, 0)).TotalSeconds;
        }
    }
}
