using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace VideoCtrl
{
    internal class TcpCommunication : IDisposable
    {
        private TcpClient client;

        private char delimiter;

        private Stream stream;

        public TcpCommunication(string ipAddress, char messageDelimiter, int port = 5000)
        {
            this.client = new TcpClient(ipAddress, port);
            this.delimiter = messageDelimiter;
            this.stream = this.client.GetStream();
        }

        public void Write(string message)
        {
            var writer = new StreamWriter(stream);
            writer.Write(message + this.delimiter);
            writer.Flush();
        }

        public string Read()
        {
            var data = new char[20000];

            int index = 0;
            var reader = new StreamReader(stream);
            do
            {
                reader.Read(data, index, 1);
                index++;
            }
            while (data[index - 1] != delimiter && index < data.Length);

            return new string(data, 0, index - 1); // do not return delimiter
        }

        /// <summary>
        /// Führt anwendungsspezifische Aufgaben aus, die mit dem Freigeben, Zurückgeben oder Zurücksetzen von nicht verwalteten Ressourcen zusammenhängen.
        /// </summary>
        public void Dispose()
        {
            if (this.client != null)
                ((IDisposable)this.client).Dispose();

            if (this.stream != null)
                this.stream.Dispose();
        }
    }
}
