using System;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;
using Nars_Libraries_Framework48;

namespace Clock_Synchronizer
{
    public partial class ClockSynchronizer : Form
    {
        const int TimeRefreshInterval = 100;
        Thread ExternalsThread;
        private volatile bool KillThreads = false;
        System.Windows.Forms.Timer TimeRefreshTimer = new System.Windows.Forms.Timer();

        public ClockSynchronizer()
        {
            InitializeComponent();
            Load += LoadedEvent;
            FormClosing += ClosingEvent;
            PortSelectionComboBox.DropDown += RefreshPorts;
            TimeRefreshTimer.Interval = TimeRefreshInterval;
            TimeRefreshTimer.Tick += RefreshTime;
            TimeRefreshTimer.Enabled = true;
        }

        private void RefreshPorts(object Sender, EventArgs Event)
        {
            UseWaitCursor = true;
            PortSelectionComboBox.Items.Clear();
            foreach (string PortName in System.IO.Ports.SerialPort.GetPortNames())
            {
                PortSelectionComboBox.Items.Add(PortName);
            }
            UseWaitCursor = false;
        }

        private void RefreshTime(object Sender, EventArgs Event)
        {
            PCTimeLabel.Text = $"PC Time: { Externals.PCTime }";
            
            if (Externals.Port.state == Nars_Libraries_Framework48.Serial.State.Connected)
                ClockTimeLabel.Text = $"Clock Time: { Externals.ClockTime }";
            else   
                ClockTimeLabel.Text = $"Clock Time: N/A";
        }

        private void ConnectionButtonClickEvent(object sender, EventArgs e)
        {
            UseWaitCursor = true;
            if (Externals.Port.state != Nars_Libraries_Framework48.Serial.State.Connected)
            {
                Result ConnectResult = Externals.Port.connect(PortSelectionComboBox.Text);
                if (ConnectResult.success)
                {
                    ConnectionButton.Text = "Done";
                    PortSelectionComboBox.Enabled = false;
                    SyncButton.Enabled = true;
                }
                else
                {
                    throw new Exception("Could not connect to port, Select a different port.");
                }
                UseWaitCursor = false;
            }
            else
            {
                Externals.Port.disconnect();
                ConnectionButton.Text = "Connect";
                PortSelectionComboBox.Enabled = true;
                SyncButton.Enabled = false;
                Close();
            }
        }

        private void SyncButtonClickEvent(object sender, EventArgs e)
        {
            UseWaitCursor = true;
            Externals.SendPCTime();
            UseWaitCursor = false;
        }

        private void HelpButtonClickEvent(object sender, EventArgs e)
        {
            Process Browser = new Process();
            Browser.StartInfo.FileName = "chrome.exe";
            Browser.StartInfo.Arguments = "https://github.com/Narwhalsss360/22Clock";
            Browser.Start();
        }

        private void ExternalsLoop()
        {
            while (true)
            {
                MethodInvoker Loop = delegate () { Externals.ExternalsLoop(); Application.DoEvents(); };
                if (!KillThreads) Invoke(Loop);
            }
        }

        private void LoadedEvent(object Sender, EventArgs Event)
        {
            ExternalsThread = new Thread(ExternalsLoop);
            ExternalsThread.Name = "Externals Thread";
            ExternalsThread.Start();
            TimeRefreshTimer.Start();
            UseWaitCursor = false;
        }

        private void ClosingEvent(object Sender, FormClosingEventArgs Event)
        {
            UseWaitCursor = true;
            Text = "Closing";
            Externals.Port.serialPort.Close();
            Externals.Port.serialPort.Dispose();

            if (!ExternalsThread.Join(0))
            {
                Event.Cancel = true;
                KillThreads = true;
                while (!ExternalsThread.Join(0))
                {
                    ExternalsThread.Abort();
                }
            }

            Event.Cancel = false;
        }
    }
}