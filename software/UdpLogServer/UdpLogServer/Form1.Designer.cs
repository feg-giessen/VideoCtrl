namespace UdpLogServer
{
    partial class frmMain
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
            this.label1 = new System.Windows.Forms.Label();
            this.nud_Port = new System.Windows.Forms.NumericUpDown();
            this.rbn_Connect = new System.Windows.Forms.RadioButton();
            this.lv_Main = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btnClear = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.nud_Port)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Port:";
            // 
            // nud_Port
            // 
            this.nud_Port.Location = new System.Drawing.Point(54, 11);
            this.nud_Port.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.nud_Port.Name = "nud_Port";
            this.nud_Port.Size = new System.Drawing.Size(87, 20);
            this.nud_Port.TabIndex = 1;
            this.nud_Port.Value = new decimal(new int[] {
            64781,
            0,
            0,
            0});
            // 
            // rbn_Connect
            // 
            this.rbn_Connect.Appearance = System.Windows.Forms.Appearance.Button;
            this.rbn_Connect.AutoSize = true;
            this.rbn_Connect.Location = new System.Drawing.Point(147, 8);
            this.rbn_Connect.Name = "rbn_Connect";
            this.rbn_Connect.Size = new System.Drawing.Size(57, 23);
            this.rbn_Connect.TabIndex = 2;
            this.rbn_Connect.TabStop = true;
            this.rbn_Connect.Text = "Connect";
            this.rbn_Connect.UseVisualStyleBackColor = true;
            this.rbn_Connect.CheckedChanged += new System.EventHandler(this.RbnConnectCheckedChanged);
            // 
            // lv_Main
            // 
            this.lv_Main.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lv_Main.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader4});
            this.lv_Main.Location = new System.Drawing.Point(16, 42);
            this.lv_Main.Name = "lv_Main";
            this.lv_Main.Size = new System.Drawing.Size(801, 531);
            this.lv_Main.TabIndex = 3;
            this.lv_Main.UseCompatibleStateImageBehavior = false;
            this.lv_Main.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "#";
            this.columnHeader1.Width = 35;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Time";
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Cat";
            this.columnHeader3.Width = 57;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Message";
            this.columnHeader4.Width = 622;
            // 
            // btnClear
            // 
            this.btnClear.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClear.Location = new System.Drawing.Point(742, 8);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 4;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.BtnClearClick);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(829, 585);
            this.Controls.Add(this.btnClear);
            this.Controls.Add(this.lv_Main);
            this.Controls.Add(this.rbn_Connect);
            this.Controls.Add(this.nud_Port);
            this.Controls.Add(this.label1);
            this.MinimumSize = new System.Drawing.Size(335, 300);
            this.Name = "frmMain";
            this.Text = "UDP Debug Server";
            ((System.ComponentModel.ISupportInitialize)(this.nud_Port)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown nud_Port;
        private System.Windows.Forms.RadioButton rbn_Connect;
        private System.Windows.Forms.ListView lv_Main;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.Button btnClear;
    }
}

