namespace Clock_Synchronizer
{
    partial class ClockSynchronizer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ClockSynchronizer));
            this.PortSelectionComboBox = new System.Windows.Forms.ComboBox();
            this.ConnectionButton = new System.Windows.Forms.Button();
            this.PCTimeLabel = new System.Windows.Forms.Label();
            this.ClockTimeLabel = new System.Windows.Forms.Label();
            this.SyncButton = new System.Windows.Forms.Button();
            this.HelpButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // PortSelectionComboBox
            // 
            this.PortSelectionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PortSelectionComboBox.FormattingEnabled = true;
            this.PortSelectionComboBox.Location = new System.Drawing.Point(20, 20);
            this.PortSelectionComboBox.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.PortSelectionComboBox.Name = "PortSelectionComboBox";
            this.PortSelectionComboBox.Size = new System.Drawing.Size(180, 28);
            this.PortSelectionComboBox.TabIndex = 0;
            this.PortSelectionComboBox.UseWaitCursor = true;
            // 
            // ConnectionButton
            // 
            this.ConnectionButton.Location = new System.Drawing.Point(210, 17);
            this.ConnectionButton.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.ConnectionButton.Name = "ConnectionButton";
            this.ConnectionButton.Size = new System.Drawing.Size(112, 35);
            this.ConnectionButton.TabIndex = 1;
            this.ConnectionButton.Text = "Connect";
            this.ConnectionButton.UseVisualStyleBackColor = true;
            this.ConnectionButton.UseWaitCursor = true;
            this.ConnectionButton.Click += new System.EventHandler(this.ConnectionButtonClickEvent);
            // 
            // PCTimeLabel
            // 
            this.PCTimeLabel.AutoSize = true;
            this.PCTimeLabel.Location = new System.Drawing.Point(20, 63);
            this.PCTimeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.PCTimeLabel.Name = "PCTimeLabel";
            this.PCTimeLabel.Size = new System.Drawing.Size(266, 20);
            this.PCTimeLabel.TabIndex = 2;
            this.PCTimeLabel.Text = "PC Time:  MM/DD/YYYY HH:MM:SS";
            this.PCTimeLabel.UseWaitCursor = true;
            // 
            // ClockTimeLabel
            // 
            this.ClockTimeLabel.AutoSize = true;
            this.ClockTimeLabel.Location = new System.Drawing.Point(20, 83);
            this.ClockTimeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.ClockTimeLabel.Name = "ClockTimeLabel";
            this.ClockTimeLabel.Size = new System.Drawing.Size(284, 20);
            this.ClockTimeLabel.TabIndex = 3;
            this.ClockTimeLabel.Text = "Clock Time:  MM/DD/YYYY HH:MM:SS";
            this.ClockTimeLabel.UseWaitCursor = true;
            // 
            // SyncButton
            // 
            this.SyncButton.Enabled = false;
            this.SyncButton.Location = new System.Drawing.Point(332, 17);
            this.SyncButton.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.SyncButton.Name = "SyncButton";
            this.SyncButton.Size = new System.Drawing.Size(112, 35);
            this.SyncButton.TabIndex = 4;
            this.SyncButton.Text = "Sync";
            this.SyncButton.UseVisualStyleBackColor = true;
            this.SyncButton.UseWaitCursor = true;
            this.SyncButton.Click += new System.EventHandler(this.SyncButtonClickEvent);
            // 
            // HelpButton
            // 
            this.HelpButton.Location = new System.Drawing.Point(406, 64);
            this.HelpButton.Name = "HelpButton";
            this.HelpButton.Size = new System.Drawing.Size(40, 40);
            this.HelpButton.TabIndex = 5;
            this.HelpButton.Text = "?";
            this.HelpButton.UseVisualStyleBackColor = true;
            this.HelpButton.Click += new System.EventHandler(this.HelpButtonClickEvent);
            // 
            // ClockSynchronizer
            // 
            this.AcceptButton = this.ConnectionButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(464, 122);
            this.Controls.Add(this.HelpButton);
            this.Controls.Add(this.SyncButton);
            this.Controls.Add(this.ClockTimeLabel);
            this.Controls.Add(this.PCTimeLabel);
            this.Controls.Add(this.ConnectionButton);
            this.Controls.Add(this.PortSelectionComboBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MaximizeBox = false;
            this.Name = "ClockSynchronizer";
            this.Text = "Clock Synchronizer";
            this.UseWaitCursor = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox PortSelectionComboBox;
        private System.Windows.Forms.Button ConnectionButton;
        private System.Windows.Forms.Label PCTimeLabel;
        private System.Windows.Forms.Label ClockTimeLabel;
        private System.Windows.Forms.Button SyncButton;
        private System.Windows.Forms.Button HelpButton;
    }
}

