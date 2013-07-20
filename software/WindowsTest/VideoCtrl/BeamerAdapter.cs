using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VideoCtrl
{
    internal class BeamerAdapter
    {
        private TcpCommunication client;

        public BeamerAdapter(string ipAddress)
        {
            this.Object = new Beamer();

            this.client = new TcpCommunication(ipAddress, '\r');
        }

        public Beamer Object
        {
            get;
            private set;
        }

        public void Power(bool value)
        {
            this.client.Write(value ? "C00" : "C01");
        }

        public void VideoMute(bool value)
        {
            this.client.Write(value ? "C0D" : "C0E");
        }

        public void DoWork()
        {
            this.client.Write("CR0");
            this.Object.SetStatus(this.client.Read());

            this.client.Write("CR6");
            this.Object.SetTemperatures(this.client.Read());
        }
    }
}
