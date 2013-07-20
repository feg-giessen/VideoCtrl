namespace VideoCtrl
{
    partial class Form1
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.rbnVideoMuteLiOn = new System.Windows.Forms.RadioButton();
            this.rbnVideoMuteLiOff = new System.Windows.Forms.RadioButton();
            this.txbBeamerLiTemp3 = new System.Windows.Forms.TextBox();
            this.txbBeamerLiTemp2 = new System.Windows.Forms.TextBox();
            this.txbBeamerLiTemp1 = new System.Windows.Forms.TextBox();
            this.txbBeamerLiStatus = new System.Windows.Forms.TextBox();
            this.btnBeamerLiOff = new System.Windows.Forms.Button();
            this.btnBeamerLiOn = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rbnVideoMuteReOn = new System.Windows.Forms.RadioButton();
            this.rbnVideoMuteReOff = new System.Windows.Forms.RadioButton();
            this.txbBeamerReTemp3 = new System.Windows.Forms.TextBox();
            this.txbBeamerReStatus = new System.Windows.Forms.TextBox();
            this.txbBeamerReTemp2 = new System.Windows.Forms.TextBox();
            this.btnBeamerReOff = new System.Windows.Forms.Button();
            this.txbBeamerReTemp1 = new System.Windows.Forms.TextBox();
            this.btnBeamerReOn = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbnVideoMuteLiOn);
            this.groupBox1.Controls.Add(this.rbnVideoMuteLiOff);
            this.groupBox1.Controls.Add(this.txbBeamerLiTemp3);
            this.groupBox1.Controls.Add(this.txbBeamerLiTemp2);
            this.groupBox1.Controls.Add(this.txbBeamerLiTemp1);
            this.groupBox1.Controls.Add(this.txbBeamerLiStatus);
            this.groupBox1.Controls.Add(this.btnBeamerLiOff);
            this.groupBox1.Controls.Add(this.btnBeamerLiOn);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(438, 118);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Projektor LI";
            // 
            // rbnVideoMuteLiOn
            // 
            this.rbnVideoMuteLiOn.Appearance = System.Windows.Forms.Appearance.Button;
            this.rbnVideoMuteLiOn.AutoSize = true;
            this.rbnVideoMuteLiOn.Location = new System.Drawing.Point(155, 64);
            this.rbnVideoMuteLiOn.Name = "rbnVideoMuteLiOn";
            this.rbnVideoMuteLiOn.Size = new System.Drawing.Size(58, 23);
            this.rbnVideoMuteLiOn.TabIndex = 4;
            this.rbnVideoMuteLiOn.TabStop = true;
            this.rbnVideoMuteLiOn.Text = "Schwarz";
            this.rbnVideoMuteLiOn.UseVisualStyleBackColor = true;
            this.rbnVideoMuteLiOn.CheckedChanged += new System.EventHandler(this.rbnVideoMuteLiOn_CheckedChanged);
            // 
            // rbnVideoMuteLiOff
            // 
            this.rbnVideoMuteLiOff.Appearance = System.Windows.Forms.Appearance.Button;
            this.rbnVideoMuteLiOff.AutoSize = true;
            this.rbnVideoMuteLiOff.Checked = true;
            this.rbnVideoMuteLiOff.Location = new System.Drawing.Point(105, 64);
            this.rbnVideoMuteLiOff.Name = "rbnVideoMuteLiOff";
            this.rbnVideoMuteLiOff.Size = new System.Drawing.Size(44, 23);
            this.rbnVideoMuteLiOff.TabIndex = 4;
            this.rbnVideoMuteLiOff.TabStop = true;
            this.rbnVideoMuteLiOff.Text = "Video";
            this.rbnVideoMuteLiOff.UseVisualStyleBackColor = true;
            // 
            // txbBeamerLiTemp3
            // 
            this.txbBeamerLiTemp3.Location = new System.Drawing.Point(340, 90);
            this.txbBeamerLiTemp3.Name = "txbBeamerLiTemp3";
            this.txbBeamerLiTemp3.ReadOnly = true;
            this.txbBeamerLiTemp3.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerLiTemp3.TabIndex = 3;
            this.txbBeamerLiTemp3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // txbBeamerLiTemp2
            // 
            this.txbBeamerLiTemp2.Location = new System.Drawing.Point(340, 64);
            this.txbBeamerLiTemp2.Name = "txbBeamerLiTemp2";
            this.txbBeamerLiTemp2.ReadOnly = true;
            this.txbBeamerLiTemp2.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerLiTemp2.TabIndex = 3;
            this.txbBeamerLiTemp2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // txbBeamerLiTemp1
            // 
            this.txbBeamerLiTemp1.Location = new System.Drawing.Point(340, 38);
            this.txbBeamerLiTemp1.Name = "txbBeamerLiTemp1";
            this.txbBeamerLiTemp1.ReadOnly = true;
            this.txbBeamerLiTemp1.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerLiTemp1.TabIndex = 3;
            this.txbBeamerLiTemp1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // txbBeamerLiStatus
            // 
            this.txbBeamerLiStatus.Location = new System.Drawing.Point(105, 37);
            this.txbBeamerLiStatus.Name = "txbBeamerLiStatus";
            this.txbBeamerLiStatus.ReadOnly = true;
            this.txbBeamerLiStatus.Size = new System.Drawing.Size(229, 20);
            this.txbBeamerLiStatus.TabIndex = 3;
            // 
            // btnBeamerLiOff
            // 
            this.btnBeamerLiOff.Location = new System.Drawing.Point(6, 64);
            this.btnBeamerLiOff.Name = "btnBeamerLiOff";
            this.btnBeamerLiOff.Size = new System.Drawing.Size(75, 23);
            this.btnBeamerLiOff.TabIndex = 2;
            this.btnBeamerLiOff.Text = "OFF";
            this.btnBeamerLiOff.UseVisualStyleBackColor = true;
            this.btnBeamerLiOff.Click += new System.EventHandler(this.btnBeamerLiOff_Click);
            // 
            // btnBeamerLiOn
            // 
            this.btnBeamerLiOn.Location = new System.Drawing.Point(6, 35);
            this.btnBeamerLiOn.Name = "btnBeamerLiOn";
            this.btnBeamerLiOn.Size = new System.Drawing.Size(75, 23);
            this.btnBeamerLiOn.TabIndex = 2;
            this.btnBeamerLiOn.Text = "ON";
            this.btnBeamerLiOn.UseVisualStyleBackColor = true;
            this.btnBeamerLiOn.Click += new System.EventHandler(this.btnBeamerLiOn_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbnVideoMuteReOn);
            this.groupBox2.Controls.Add(this.rbnVideoMuteReOff);
            this.groupBox2.Controls.Add(this.txbBeamerReTemp3);
            this.groupBox2.Controls.Add(this.txbBeamerReStatus);
            this.groupBox2.Controls.Add(this.txbBeamerReTemp2);
            this.groupBox2.Controls.Add(this.btnBeamerReOff);
            this.groupBox2.Controls.Add(this.txbBeamerReTemp1);
            this.groupBox2.Controls.Add(this.btnBeamerReOn);
            this.groupBox2.Location = new System.Drawing.Point(12, 136);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(438, 121);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Projektor RE";
            // 
            // rbnVideoMuteReOn
            // 
            this.rbnVideoMuteReOn.Appearance = System.Windows.Forms.Appearance.Button;
            this.rbnVideoMuteReOn.AutoSize = true;
            this.rbnVideoMuteReOn.Location = new System.Drawing.Point(155, 64);
            this.rbnVideoMuteReOn.Name = "rbnVideoMuteReOn";
            this.rbnVideoMuteReOn.Size = new System.Drawing.Size(58, 23);
            this.rbnVideoMuteReOn.TabIndex = 4;
            this.rbnVideoMuteReOn.Text = "Schwarz";
            this.rbnVideoMuteReOn.UseVisualStyleBackColor = true;
            this.rbnVideoMuteReOn.CheckedChanged += new System.EventHandler(this.rbnVideoMuteReOn_CheckedChanged);
            // 
            // rbnVideoMuteReOff
            // 
            this.rbnVideoMuteReOff.Appearance = System.Windows.Forms.Appearance.Button;
            this.rbnVideoMuteReOff.AutoSize = true;
            this.rbnVideoMuteReOff.Checked = true;
            this.rbnVideoMuteReOff.Location = new System.Drawing.Point(105, 64);
            this.rbnVideoMuteReOff.Name = "rbnVideoMuteReOff";
            this.rbnVideoMuteReOff.Size = new System.Drawing.Size(44, 23);
            this.rbnVideoMuteReOff.TabIndex = 4;
            this.rbnVideoMuteReOff.TabStop = true;
            this.rbnVideoMuteReOff.Text = "Video";
            this.rbnVideoMuteReOff.UseVisualStyleBackColor = true;
            // 
            // txbBeamerReTemp3
            // 
            this.txbBeamerReTemp3.Location = new System.Drawing.Point(340, 90);
            this.txbBeamerReTemp3.Name = "txbBeamerReTemp3";
            this.txbBeamerReTemp3.ReadOnly = true;
            this.txbBeamerReTemp3.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerReTemp3.TabIndex = 3;
            this.txbBeamerReTemp3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // txbBeamerReStatus
            // 
            this.txbBeamerReStatus.Location = new System.Drawing.Point(105, 37);
            this.txbBeamerReStatus.Name = "txbBeamerReStatus";
            this.txbBeamerReStatus.ReadOnly = true;
            this.txbBeamerReStatus.Size = new System.Drawing.Size(229, 20);
            this.txbBeamerReStatus.TabIndex = 3;
            // 
            // txbBeamerReTemp2
            // 
            this.txbBeamerReTemp2.Location = new System.Drawing.Point(340, 64);
            this.txbBeamerReTemp2.Name = "txbBeamerReTemp2";
            this.txbBeamerReTemp2.ReadOnly = true;
            this.txbBeamerReTemp2.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerReTemp2.TabIndex = 3;
            this.txbBeamerReTemp2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // btnBeamerReOff
            // 
            this.btnBeamerReOff.Location = new System.Drawing.Point(6, 64);
            this.btnBeamerReOff.Name = "btnBeamerReOff";
            this.btnBeamerReOff.Size = new System.Drawing.Size(75, 23);
            this.btnBeamerReOff.TabIndex = 2;
            this.btnBeamerReOff.Text = "OFF";
            this.btnBeamerReOff.UseVisualStyleBackColor = true;
            this.btnBeamerReOff.Click += new System.EventHandler(this.btnBeamerReOff_Click);
            // 
            // txbBeamerReTemp1
            // 
            this.txbBeamerReTemp1.Location = new System.Drawing.Point(340, 38);
            this.txbBeamerReTemp1.Name = "txbBeamerReTemp1";
            this.txbBeamerReTemp1.ReadOnly = true;
            this.txbBeamerReTemp1.Size = new System.Drawing.Size(72, 20);
            this.txbBeamerReTemp1.TabIndex = 3;
            this.txbBeamerReTemp1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // btnBeamerReOn
            // 
            this.btnBeamerReOn.Location = new System.Drawing.Point(6, 35);
            this.btnBeamerReOn.Name = "btnBeamerReOn";
            this.btnBeamerReOn.Size = new System.Drawing.Size(75, 23);
            this.btnBeamerReOn.TabIndex = 2;
            this.btnBeamerReOn.Text = "ON";
            this.btnBeamerReOn.UseVisualStyleBackColor = true;
            this.btnBeamerReOn.Click += new System.EventHandler(this.btnBeamerReOn_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(463, 288);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "FeG VideoCtrl";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnBeamerLiOff;
        private System.Windows.Forms.Button btnBeamerLiOn;
        private System.Windows.Forms.Button btnBeamerReOff;
        private System.Windows.Forms.Button btnBeamerReOn;
        private System.Windows.Forms.TextBox txbBeamerLiStatus;
        private System.Windows.Forms.TextBox txbBeamerLiTemp3;
        private System.Windows.Forms.TextBox txbBeamerLiTemp2;
        private System.Windows.Forms.TextBox txbBeamerLiTemp1;
        private System.Windows.Forms.TextBox txbBeamerReTemp3;
        private System.Windows.Forms.TextBox txbBeamerReStatus;
        private System.Windows.Forms.TextBox txbBeamerReTemp2;
        private System.Windows.Forms.TextBox txbBeamerReTemp1;
        private System.Windows.Forms.RadioButton rbnVideoMuteLiOn;
        private System.Windows.Forms.RadioButton rbnVideoMuteLiOff;
        private System.Windows.Forms.RadioButton rbnVideoMuteReOn;
        private System.Windows.Forms.RadioButton rbnVideoMuteReOff;
    }
}

