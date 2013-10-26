using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TcpServer
{
    class Program
    {
        static void Main(string[] args)
        {
            int port;

            string input;
            do
            {
                Console.Write("Port: ");
                input = Console.ReadLine();
            }
            while (!int.TryParse(input, out port));

            var listener = new TcpListener(new IPEndPoint(IPAddress.Any, port));

            listener.Start();

            byte[] rcvBuffer = new byte[32];

            for (; ; )
            {
                Socket client = null;

                try
                {
                    client = listener.AcceptSocket();

                    Console.Write("Handling client at " + client.RemoteEndPoint + " - ");

                    // Empfange bis der client die Verbindung schließt, das geschieht indem 0
                    // zurückgegeben wird
                    int totalbytesEchoed = 0;
                    int bytesRcvd;

                    while ((bytesRcvd = client.Receive(rcvBuffer, 0, rcvBuffer.Length, SocketFlags.None)) > 0)
                    {
                        byte[] send = Encoding.ASCII.GetBytes("Command OK\r\n");
                        client.Send(send, 0, send.Length, SocketFlags.None);
                        totalbytesEchoed += bytesRcvd;
                    }
                    Console.WriteLine("echoed {0} bytes.", totalbytesEchoed);

                    client.Shutdown(SocketShutdown.Both);
                    client.Close();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                    client.Close();
                }
            }
        }
    }
}
