using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VideoCtrl
{
    internal class BeamerAdapter
    {
        private Func<TcpCommunication> client;

        public BeamerAdapter(string ipAddress)
        {
            this.Object = new Beamer();

            this.client = () => new TcpCommunication(ipAddress, '\r');
        }

        public Beamer Object { get; private set; }

        public void Power(bool value)
        {
            try
            {
                using (var c = this.client.Invoke())
                {
                    c.Write(value ? "C00" : "C01");

                    this.Online = true;
                }
            }
            catch (Exception)
            {
                this.Online = false;
            }
        }

        public void VideoMute(bool value)
        {
            try
            {
                using (var c = this.client.Invoke())
                {
                    c.Write(value ? "C0D" : "C0E");

                    this.Online = true;
                }
            }
            catch (Exception)
            {
                this.Online = false;
            }
        }

        public bool Online { get; private set; }

        public void DoWork()
        {
            try
            {
                using (var c = this.client.Invoke())
                {
                    c.Write("CR0");
                    this.Object.SetStatus(c.Read());

                    c.Write("CR6");
                    this.Object.SetTemperatures(c.Read());

                    this.Online = true;
                }
            }
            catch (Exception)
            {
                this.Online = false;
            }
        }
    }
}
