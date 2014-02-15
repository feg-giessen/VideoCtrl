using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Globalization;

namespace UdpLogServer
{
    public partial class frmMain : Form
    {
        private Socket socket = null;

        private IPEndPoint endpoint;

        private int counter = 0;

        private byte[] buffer = new byte[1500];

        public frmMain()
        {
            this.InitializeComponent();
        }

        private void BtnClearClick(object sender, EventArgs e)
        {
            this.lv_Main.Items.Clear();
            this.counter = 0;
        }

        private void RbnConnectCheckedChanged(object sender, EventArgs e)
        {
            if (this.rbn_Connect.Checked)
            {
                if (this.socket != null)
                {
                    this.socket.Dispose();
                    this.socket = null;
                }

                this.socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                this.socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                this.socket.EnableBroadcast = true;

                IPAddress ip;
                ip = IPAddress.Any;
                ip = IPAddress.Parse("192.168.40.11");

                this.endpoint = new IPEndPoint(ip, (int)this.nud_Port.Value);
                this.socket.Bind(this.endpoint);

                EndPoint remote = new IPEndPoint(IPAddress.Parse("192.168.40.22"), 0);
                ////this.socket.BeginReceiveFrom(this.buffer, 0, 1500, SocketFlags.None, ref remote, this.DataReveive, this.socket);
                this.socket.BeginReceive(this.buffer, 0, 1500, SocketFlags.None, this.DataReveive, this.socket);

            }
            else if (this.socket != null)
            {
            }
        }

        private void DataReveive(IAsyncResult ar)
        {
            var s1 = (Socket)ar.AsyncState;

            int length = s1.EndReceive(ar);

            if (length > 4)
            {
                string cat = Encoding.ASCII.GetString(this.buffer, 0, 4);
                string message = Encoding.ASCII.GetString(this.buffer, 4, length - 4);

                Action action = () => this.lv_Main.Items.Add(new ListViewItem(new[]
                                                                                  {
                                                                                      (this.counter++).ToString(CultureInfo.CurrentCulture), 
                                                                                      DateTime.Now.ToLongTimeString(), 
                                                                                      cat, 
                                                                                      message
                                                                                  }));

                this.Invoke((Delegate)action);
            }

            this.buffer = new byte[1500];

            this.socket.BeginReceive(this.buffer, 0, 1500, SocketFlags.None, this.DataReveive, s1);
        }
    }
}
