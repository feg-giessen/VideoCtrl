using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace VideoCtrl
{
    internal class TcpCommunication
    {
        private TcpClient client;

        private char delimiter;

        private Stream stream;

        public TcpCommunication(string ipAddress, char messageDelimiter)
        {
            this.client = new TcpClient(ipAddress, 5000);
            this.delimiter = messageDelimiter;
            this.stream = this.client.GetStream();
        }

        public void Write(string message)
        {
            var writer = new StreamWriter(stream);
            writer.Write(message + this.delimiter); // Encoding.ASCII.GetBytes(
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
            while (data[index - 1] != delimiter);

            return new string(data, 0, index - 1); // do not return delimiter
        }
    }
}
