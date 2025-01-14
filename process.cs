using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            SerialPort mySP = new SerialPort("33");
            var sw = new System.IO.StreamWriter("w37.csv", false, System.Text.Encoding.GetEncoding("shift_jis"));
            mySP.BaudRate = 250000;
            mySP.Open();
            while (true)
            {
                string myData = mySP.ReadLine();
                sw.WriteLine(myData);
                Console.WriteLine(myData);

            }
        }
    }
}

