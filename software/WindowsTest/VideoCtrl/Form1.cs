using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace VideoCtrl
{
    public partial class Form1 : Form
    {
        private System.Threading.Timer timer;

        private BeamerAdapter beamerLi;
        private BeamerAdapter beamerRe;

        public Form1()
        {
            this.InitializeComponent();

            this.beamerLi = new BeamerAdapter("192.168.40.33");
            this.beamerRe = new BeamerAdapter("192.168.40.32");

            this.timer = new System.Threading.Timer(this.DoWork);
            this.timer.Change(0, 2000);
        }

        private delegate void UpdateDelegate();

        private void DoWork(object sender)
        {
            this.beamerLi.DoWork();
            this.beamerRe.DoWork();

            UpdateDelegate del = this.UpdateForm;

            this.Invoke(del);
        }

        private void UpdateForm()
        {
            this.txbBeamerLiStatus.Text = this.beamerLi.Object.Status;
            this.txbBeamerLiTemp1.Text = string.Format("{0:0.##} °C", this.beamerLi.Object.Temperature1);
            this.txbBeamerLiTemp2.Text = string.Format("{0:0.##} °C", this.beamerLi.Object.Temperature2);
            this.txbBeamerLiTemp3.Text = string.Format("{0:0.##} °C", this.beamerLi.Object.Temperature3);

            this.groupBox1.Enabled = this.beamerLi.Online;
            if (!this.beamerLi.Online)
            {
                this.txbBeamerLiStatus.Text = "OFFLINE";
                this.txbBeamerLiStatus.Font = new Font(this.txbBeamerLiStatus.Font, FontStyle.Italic);
                this.txbBeamerLiStatus.ForeColor = Color.Red;
            }
            else
            {
                this.txbBeamerLiStatus.Font = new Font(this.txbBeamerLiStatus.Font, FontStyle.Regular);
                this.txbBeamerLiStatus.ForeColor = Color.Black;
            }

            this.txbBeamerReStatus.Text = this.beamerRe.Object.Status;
            this.txbBeamerReTemp1.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature1);
            this.txbBeamerReTemp2.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature2);
            this.txbBeamerReTemp3.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature3);

            this.groupBox2.Enabled = this.beamerRe.Online;
            if (!this.beamerLi.Online)
            {
                this.txbBeamerReStatus.Text = "OFFLINE";
                this.txbBeamerReStatus.Font = new Font(this.txbBeamerLiStatus.Font, FontStyle.Italic);
                this.txbBeamerReStatus.ForeColor = Color.Red;
            }
            else
            {
                this.txbBeamerReStatus.Font = new Font(this.txbBeamerLiStatus.Font, FontStyle.Regular);
                this.txbBeamerReStatus.ForeColor = Color.Black;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void btnBeamerLiOn_Click(object sender, EventArgs e)
        {
            this.beamerLi.Power(true);
        }

        private void btnBeamerLiOff_Click(object sender, EventArgs e)
        {
            this.beamerLi.Power(false);
        }

        private void btnBeamerReOn_Click(object sender, EventArgs e)
        {
            this.beamerRe.Power(true);
        }

        private void btnBeamerReOff_Click(object sender, EventArgs e)
        {
            this.beamerRe.Power(false);
        }

        private void rbnVideoMuteLiOn_CheckedChanged(object sender, EventArgs e)
        {
            this.beamerLi.VideoMute(rbnVideoMuteLiOn.Checked);
        }

        private void rbnVideoMuteReOn_CheckedChanged(object sender, EventArgs e)
        {
            this.beamerRe.VideoMute(rbnVideoMuteReOn.Checked);
        }

        private void btnResetVideoRouting_Click(object sender, EventArgs e)
        {
            var result = new StringBuilder();

            try
            {
                using (var matrix = new TcpCommunication("192.168.40.31", '\r', 100))
                {
                    matrix.Write("sw i03 o01");
                    result.AppendLine(matrix.Read());

                    matrix.Write("sw i03 o02");
                    result.AppendLine(matrix.Read());

                    matrix.Write("sw i02 o03");
                    result.AppendLine(matrix.Read());

                    matrix.Write("sw i02 o04");
                    result.AppendLine(matrix.Read());
                }
            }
            catch
            {
            }

            try
            {
                using (var stage = new TcpCommunication("192.168.40.31", '\r', 101))
                {
                    stage.Write("sw i03");
                    result.AppendLine(stage.Read());
                }
            }
            catch
            {
            }
        }
    }
}
