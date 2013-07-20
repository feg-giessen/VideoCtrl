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

            this.txbBeamerReStatus.Text = this.beamerRe.Object.Status;
            this.txbBeamerReTemp1.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature1);
            this.txbBeamerReTemp2.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature2);
            this.txbBeamerReTemp3.Text = string.Format("{0:0.##} °C", this.beamerRe.Object.Temperature3);
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
    }
}
