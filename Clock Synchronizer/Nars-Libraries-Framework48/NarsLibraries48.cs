namespace Nars_Libraries_Framework48
{
    /// <summary>
    /// Return type containg information on function
    /// </summary>
    public class Result
    {
        readonly public bool success;
        
        readonly public string message;
        public Result(bool _success, string _message)
        {
            this.success = _success;
            this.message = _message;
        }
    }

    namespace Serial
    {
        /// <summary>
        /// Serial connection states
        /// </summary>
        public enum State
        {
            Disconnected,
            Connected,
            Lost
        }

        /// <summary>
        /// Return type containing received message information : Derived from Result
        /// </summary>
        public class Receive : Result
        {
            readonly public bool special;
            readonly public ushort register;
            readonly public object data;

            public Receive(bool _success, string _message, bool _special, ushort _register, object _data):base(_success, _message)
            {
                this.special = _special;
                this.register = _register;
                this.data = _data;
            }
        }

        /// <summary>
        /// Class for communication with an arduino through a serial port. Must use with Nars Arduino library.
        /// </summary>
        public class NarsSerialCom
        {
            /// <summary>
            /// Received Data Storage
            /// </summary>
            public object[] receivedData = new object[65535];

            /// <summary>
            /// Connection state
            /// </summary>
            public State state = State.Disconnected;

            /// <summary>
            /// Send queue
            /// </summary>
            public System.Collections.Generic.Queue<string> sendQueue = new System.Collections.Generic.Queue<string>();

            /// <summary>
            /// on Receive user handler
            /// </summary>
            private System.Action<Receive> onReceiveHandler = null;

            /// <summary>
            /// Bool property if client device is ready to receive.
            /// </summary>
            public bool ready { get; private set; }

            /// <summary>
            /// SerialPort object
            /// </summary>
            public System.IO.Ports.SerialPort serialPort = new System.IO.Ports.SerialPort();

            /// <summary>
            /// Temporary port, for onRecv
            /// </summary>
            private System.IO.Ports.SerialPort tempPort = new System.IO.Ports.SerialPort();

            /// <summary>
            /// Last Receive return.
            /// </summary>
            public Receive lastReceive = null;

            /// <summary>
            /// Last Result return
            /// </summary>
            public Result lastResult = null;

            /// <summary>
            /// Max queue size
            /// </summary>
            public byte queueSize = 10;

            /// <summary>
            /// Constructor
            /// </summary>
            public NarsSerialCom()
            {
                this.serialPort.BaudRate = 1000000;
                this.serialPort.Parity = System.IO.Ports.Parity.None;
                this.serialPort.StopBits = System.IO.Ports.StopBits.One;
                this.serialPort.DataBits = 8;
                this.serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(DataReceiveHandler);
                this.lastResult = new Result(true, "Instantiated");
            }

            /// <summary>
            /// Connect to client on selected port.
            /// </summary>
            /// <param name="port">Selected port</param>
            /// <returns></returns>
            public Result connect(string port)
            {
                if (this.state == State.Disconnected)
                {
                    this.serialPort.PortName = port;
                    this.serialPort.Open();
                    this.serialPort.WriteLine("*B-");
                    this.lastResult = new Result(true, "Connected");
                    this.state = State.Connected;
                    return this.lastResult;
                }
                else
                {
                    this.lastResult = new Result(false, "Already connected");
                    return this.lastResult;
                }
            }

            /// <summary>
            /// Checks queue if not empty, this will send the queued item.
            /// </summary>
            /// <returns></returns>
            public Result checkQueue()
            {
                if (this.sendQueue.Count > 0)
                {
                    string sent = this.sendQueue.Dequeue();
                    this.serialPort.WriteLine(sent);
                    this.lastResult = new Result(true, sent);
                    return this.lastResult;
                }
                else
                {
                    this.lastResult = new Result(true, "Queue empty");
                    return this.lastResult;
                }
            }

            /// <summary>
            /// Send data to client device
            /// </summary>
            /// <param name="register">Register</param>
            /// <param name="inData">Data</param>
            /// <returns></returns>
            public Result send(ushort register, object inData)
            {
                uint data;
                if (inData is int || inData is uint || inData is long || inData is ulong)
                {
                    data = (uint)inData;
                    string sent = "*D" + Usefuls.NarsMethods.fixedLengthHex(register, 4) + Usefuls.NarsMethods.fixedLengthHex(data, 8) + "-";
                    if (this.ready)
                    {
                        this.serialPort.WriteLine(sent);
                        this.lastResult = new Result(true, sent);
                        return this.lastResult;
                    }
                    else
                    {
                        if (this.sendQueue.Count <= queueSize)
                        {
                            this.sendQueue.Enqueue(sent);
                            this.lastResult = new Result(true, "Queued " + sent);
                            return this.lastResult;
                        }
                        else
                        {
                            this.lastResult = new Result(false, "Queue size limit reached");
                            return this.lastResult;
                        }
                    }
                }
                else if (inData is string)
                {
                    string sent = "*S" + Usefuls.NarsMethods.fixedLengthHex(register, 4) + (string)inData + "-";
                    if (this.ready)
                    {
                        this.serialPort.WriteLine(sent);
                        this.lastResult = new Result(true, sent);
                        return this.lastResult;
                    }
                    else
                    {
                        if (this.sendQueue.Count <= queueSize)
                        {
                            this.sendQueue.Enqueue(sent);
                            this.lastResult = new Result(true, "Queued " + sent);
                            return this.lastResult;
                        }
                        else
                        {
                            this.lastResult = new Result(false, "Queue size limit reached");
                            return this.lastResult;
                        }
                    }
                }
                else
                {
                    this.lastResult = new Result(false, "Invalid Type");
                    return this.lastResult;
                }
            }

            /// <summary>
            /// Get data from a register
            /// </summary>
            /// <param name="register">Register</param>
            /// <returns></returns>
            public Receive getData(ushort register)
            {
                if (this.receivedData[register] is string)
                {
                    return this.lastReceive = new Receive(true, "Returned", true, register, this.receivedData[register]);
                }
                else if (this.receivedData[register] is uint)
                {
                    return this.lastReceive = new Receive(true, "Returned", false, register, this.receivedData[register]);
                }
                else
                {
                    return this.lastReceive = new Receive(false, "Could not get data type", false, register, this.receivedData[register]);
                }
            }

            /// <summary>
            /// Disconnect from client
            /// </summary>
            /// <returns></returns>
            public Result disconnect()
            {
                if (this.state == State.Connected)
                {
                    this.serialPort.Close();
                    return this.lastResult = new Result(true, "Disconnected");
                }
                else
                {
                    return this.lastResult = new Result(false, "Already disconnected");
                }
            }

            /// <summary>
            /// Add a handler that has a recieve argument.
            /// </summary>
            /// <param name="Handler">void handler(Receive)</param>
            public void addOnReceiveHandler(System.Action<Receive> Handler)
            {
                this.onReceiveHandler = Handler;
            }

            /// <summary>
            /// After data received, parse data.
            /// </summary>
            /// <param name="data"></param>
            private void parseData(string data)
            {
                if (data.Length > 6)
                {
                    if (data.Substring(0, 2) == "*D")
                    {
                        if (data.Length == 16)
                        {
                            ushort tempRegister = ushort.Parse(data.Substring(2, 4), System.Globalization.NumberStyles.HexNumber);
                            uint tempData = uint.Parse(data.Substring(6, 8), System.Globalization.NumberStyles.HexNumber);
                            this.receivedData[tempRegister] = (uint)tempData;
                            if (this.onReceiveHandler != null)
                            {
                                this.lastReceive = new Receive(true, data, false, tempRegister, tempData);
                                onReceiveHandler(this.lastReceive);
                            }
                            if (tempRegister == 0)
                            {
                                this.ready = System.Convert.ToBoolean(tempData);
                            }
                        }
                        else
                        {
                            if (this.onReceiveHandler != null)
                            {
                                this.lastReceive = new Receive(false, "Possible Data Loss: length is not 16", false, 0, null);
                                onReceiveHandler(this.lastReceive);
                            }
                        }
                    }
                    else if (data.Substring(0, 2) == "*S")
                    {
                        ushort tempRegister = ushort.Parse(data.Substring(2, 4), System.Globalization.NumberStyles.HexNumber);
                        string tempData = data.Substring(6, data.Length - 8);
                        this.receivedData[tempRegister] = (string)tempData;
                        if (this.onReceiveHandler != null)
                        {
                            this.lastReceive = new Receive(true, data, true, tempRegister, tempData);
                            onReceiveHandler(this.lastReceive);
                        }
                    }
                    else
                    {
                        if (this.onReceiveHandler != null)
                        {
                            this.lastReceive = new Receive(false, "Possible Data Loss: No Command", false, 0, null);
                            onReceiveHandler(this.lastReceive);
                        }
                    }
                }
                else
                {
                    if (this.onReceiveHandler != null)
                    {
                        this.lastReceive = new Receive(false, "Possible Data Loss: length less than minumum", false, 0, null);
                        onReceiveHandler(this.lastReceive);
                    }
                }
            }

            /// <summary>
            /// On receive
            /// </summary>
            /// <param name="sender"></param>
            /// <param name="eventArgs"></param>
            private void DataReceiveHandler(object sender, System.IO.Ports.SerialDataReceivedEventArgs eventArgs)
            {
                this.tempPort = (System.IO.Ports.SerialPort)sender;
                foreach (string line in this.tempPort.ReadExisting().Split('\n'))
                {
                    parseData(line);
                }
            }
        }

        /// <summary>
        /// Class for communication with an arduino through a serial port. Must use with Nars Arduino library.
        /// </summary>
        public class NarsSerialComOpenNET
        {

            public object[] receivedData = new object[65535];

            public State state = State.Disconnected;

            public System.Collections.Generic.Queue<string> sendQueue = new System.Collections.Generic.Queue<string>();

            private System.Action<Receive> onReceiveHandler = null;

            public bool ready = false;

            private System.Timers.Timer timer = new System.Timers.Timer();

            public OpenNETCF.IO.Ports.SerialPort serialPort = new OpenNETCF.IO.Ports.SerialPort();

            public OpenNETCF.IO.Ports.SerialPort tempPort = new OpenNETCF.IO.Ports.SerialPort();

            public Receive lastRecieve;

            public Result lastResult;

            public int timeout = 700;

            private bool timedOut = false;

            public byte queueSize = 10;

            public NarsSerialComOpenNET()
            {
                this.serialPort.BaudRate = 1000000;
                this.serialPort.DataBits = 8;
                this.serialPort.Parity = OpenNETCF.IO.Ports.Parity.None;
                this.serialPort.StopBits = OpenNETCF.IO.Ports.StopBits.One;
                this.serialPort.ReceivedEvent += DataRecievedHandler;
                this.timer = new System.Timers.Timer();
                this.timer.AutoReset = false;
                this.timer.Enabled = true;
                this.timer.Elapsed += onTimeOut;
                this.lastResult = new Result(true, "Instantiated");
            }

            public void addOnReceiveHandler(System.Action<Receive> _onReceiveHandler)
            {
                this.onReceiveHandler = _onReceiveHandler;
            }

            public Result connect(string port)
            {
                if (this.state == State.Disconnected)
                {
                    this.serialPort.PortName = port;
                    this.serialPort.Open();
                    this.state = State.Connected;
                    return lastResult = new Result(true, "Connected");
                }
                else
                {
                    return lastResult = new Result(false, "Already connected");
                }
            }

            public Result checkQueue()
            {
                if (this.sendQueue.Count > 0)
                {
                    if (this.ready)
                    {
                        string sent = sendQueue.Dequeue();
                        this.serialPort.WriteLine(sent);
                        return this.lastResult = new Result(true, $"Sent: {sent}");
                    }
                    else
                    {
                        return this.lastResult = new Result(false, "Not Ready");
                    }
                }
                else
                {
                    return this.lastResult = new Result(false, "Queue Empty");
                }
            }

            public Result send(ushort register, object data)
            {
                if (register <= 65535)
                {
                    if (data is int)
                    {
                        if ((int)data >= 0)
                        {
                            string sent = "*D" + Usefuls.NarsMethods.fixedLengthHex(register, 4) + Usefuls.NarsMethods.fixedLengthHex((long)data, 8) + "-";
                            if (this.ready)
                            {
                                this.serialPort.WriteLine(sent);
                                return this.lastResult = new Result(true, $"Sent: {sent}");
                            }
                            else
                            {
                                if (sendQueue.Count <= queueSize)
                                {
                                    this.sendQueue.Enqueue(sent);
                                    return this.lastResult = new Result(true, $"Queued: {sent}");
                                }
                                else
                                {
                                    return this.lastResult = new Result(false, "Max queue size limit reached");
                                }
                            }
                        }
                        else
                        {
                            return this.lastResult = new Result(false, "Out of range");
                        }
                    } 
                    else if(data is uint)
                    {
                        string sent = "*D" + Usefuls.NarsMethods.fixedLengthHex(register, 4) + Usefuls.NarsMethods.fixedLengthHex((uint)data, 8) + "-";
                        if (this.ready)
                        {
                            this.serialPort.WriteLine(sent);
                            return this.lastResult = new Result(true, $"Sent: {sent}");
                        }
                        else
                        {
                            if (sendQueue.Count <= queueSize)
                            {
                                this.sendQueue.Enqueue(sent);
                                return this.lastResult = new Result(true, $"Queued: {sent}");
                            }
                            else
                            {
                                return this.lastResult = new Result(false, "Max queue size limit reached");
                            }
                        }
                    }
                    else if (data is string)
                    {
                        string sent = "*D" + Usefuls.NarsMethods.fixedLengthHex(register, 4) + (string)data + "-";
                        if (this.ready)
                        {
                            this.serialPort.WriteLine(sent);
                            return this.lastResult = new Result(true, $"Sent: {sent}");
                        }
                        else
                        {
                            if (sendQueue.Count <= queueSize)
                            {
                                this.sendQueue.Enqueue(sent);
                                return this.lastResult = new Result(true, $"Queued: {sent}");
                            }
                            else
                            {
                                return this.lastResult = new Result(false, "Max queue size limit reached");
                            }
                        }
                    }
                    else
                    {
                        return this.lastResult = new Result(false, $"Invalid type: {data.GetType().ToString()}");
                    }
                }
                else
                {
                    return this.lastResult = new Result(false, "Out of range");
                }
            }

            public Result disconnect()
            {
                if (this.state == State.Connected)
                {
                    serialPort.WriteLine("*E-");
                    serialPort.Close();
                    return this.lastResult = new Result(true, "Disconneced");
                }
                else
                {
                    return this.lastResult = new Result(false, "Already disconnected");
                }
            }

            private void onTimeOut(object source, System.Timers.ElapsedEventArgs e)
            {
                this.timedOut = true;
            }

            private void DataRecievedHandler(object sender, OpenNETCF.IO.Ports.SerialReceivedEventArgs e)
            {
                tempPort = (OpenNETCF.IO.Ports.SerialPort)sender;
                string temp = tempPort.ReadExisting();

                foreach (string line in temp.Split('\n'))
                {
                    if (line.Length >= 6)
                    {
                        if (line.Substring(0, 2) == "*D")
                        {
                            if (line.Length == 16)
                            {
                                
                                ushort tempRegister = ushort.Parse(line.Substring(2, 4), System.Globalization.NumberStyles.HexNumber);
                                uint tempData = uint.Parse(line.Substring(6, 8), System.Globalization.NumberStyles.HexNumber);
                                this.receivedData[tempRegister] = tempData;
                                if (tempRegister == 0)
                                {
                                    this.ready = System.Convert.ToBoolean(tempData);
                                    this.lastRecieve = new Receive(true, "Ready set: " + line, false, 0, tempData);
                                    if (this.onReceiveHandler != null) onReceiveHandler(lastRecieve);
                                }
                                else
                                {
                                    this.lastRecieve = new Receive(true, "Received: " + line, false, tempRegister, tempData);
                                    if (this.onReceiveHandler != null) onReceiveHandler(lastRecieve);
                                }
                            }
                        }
                        

                        if (line.Substring(0, 2) == "*S")
                        {
                            ushort tempRegister = ushort.Parse(line.Substring(2, 4), System.Globalization.NumberStyles.HexNumber);
                            string tempData = line.Substring(6, line.Length - 8);
                            this.receivedData[tempRegister] = tempData;
                            this.lastRecieve = new Receive(true, "Received: " + line, true, tempRegister, tempData);
                            if (this.onReceiveHandler != null) onReceiveHandler(lastRecieve);
                        }
                    }
                    else
                    {
                        this.lastRecieve = new Receive(false, "Possible Data Loss Line (less than 6): " + line, false, 0, 0);
                        if (this.onReceiveHandler != null) onReceiveHandler(lastRecieve);
                    }
                }
            }    
        }
    }

    namespace Usefuls
    {
        /// <summary>
        /// Useful methods
        /// </summary>
        public static class NarsMethods
        {
            /// <summary>
            /// Returns fixed length hex string from number.
            /// </summary>
            /// <param name="input">Number</param>
            /// <param name="length">Length</param>
            public static string fixedLengthHex(long input, byte length)
            {
                string output = input.ToString("X");
                if (output.Length != length)
                {
                    string original = output;
                    int originalLength = output.Length;
                    output = "";
                    for (int i = 0; i < length - originalLength; i++)
                    {
                        output += "0";
                    }
                    output += original;
                }
                return output;
            }

            /// <summary>
            /// Map a value to another
            /// </summary>
            /// <param name="x">Value to read</param>
            /// <param name="in_min">Value to read minimum</param>
            /// <param name="in_max">Value to read maximum</param>
            /// <param name="out_min">Out minimum</param>
            /// <param name="out_max">Out maximum</param>
            /// <returns>Mapped value</returns>
            public static double mapValue(double x, double in_min, double in_max, double out_min, double out_max)
            {
                return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
            }
        }

        /// <summary>
        /// Byte bit operations extension
        /// </summary>
        public static class ByteExtenstions
        {
            /// <summary>
            /// Toggle bit.
            /// </summary>
            /// <param name="b"></param>
            /// <param name="pos">bit</param>
            /// <returns></returns>
            /// <exception cref="System.ArgumentOutOfRangeException"></exception>
            public static byte ToggleBit(this byte b, int pos)
            {
                if (pos < 0 || pos > 7)
                    throw new System.ArgumentOutOfRangeException("pos", "Index must be in the range of 0-7.");

                return (byte)(b ^ (1 << pos));
            }

            /// <summary>
            /// Check if bit is set.
            /// </summary>
            /// <param name="b"></param>
            /// <param name="pos">bit</param>
            /// <returns></returns>
            /// <exception cref="System.ArgumentOutOfRangeException"></exception>
            public static bool IsBitSet(this byte b, int pos)
            {
                if (pos < 0 || pos > 7)
                    throw new System.ArgumentOutOfRangeException("pos", "Index must be in the range of 0-7.");

                return (b & (1 << pos)) != 0;
            }

            /// <summary>
            /// Set bit.
            /// </summary>
            /// <param name="b"></param>
            /// <param name="pos">bit</param>
            /// <returns></returns>
            /// <exception cref="System.ArgumentOutOfRangeException"></exception>
            public static byte SetBit(this byte b, int pos)
            {
                if (pos < 0 || pos > 7)
                    throw new System.ArgumentOutOfRangeException("pos", "Index must be in the range of 0-7.");

                return (byte)(b | (1 << pos));
            }

            /// <summary>
            /// Unset bit.
            /// </summary>
            /// <param name="b"></param>
            /// <param name="pos">bit</param>
            /// <returns></returns>
            /// <exception cref="System.ArgumentOutOfRangeException"></exception>
            public static byte UnsetBit(this byte b, int pos)
            {
                if (pos < 0 || pos > 7)
                    throw new System.ArgumentOutOfRangeException("pos", "Index must be in the range of 0-7.");

                return (byte)(b & ~(1 << pos));
            }

            /// <summary>
            /// To string in binary
            /// </summary>
            /// <param name="b"></param>
            /// <returns></returns>
            public static string ToBinaryString(this byte b)
            {
                return System.Convert.ToString(b, 2).PadLeft(8, '0');
            }
        }
    }
}