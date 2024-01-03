using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using NrrdCommon;

namespace NrrdTest
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var filePath = @"C:\Users\tsvi\ltu.se\Johannes Huber - CITA-LTU Pipeline\D1_D1.nrrd";

            var nrrd = Nrrd.Load(filePath);

            Console.WriteLine("Num dimensions:  {0}", nrrd.Dimensions);
            Console.WriteLine("Content:         {0}", nrrd.Content);
            Console.WriteLine("Type:            {0}", nrrd.Type);

            Console.WriteLine("Axes:");
            foreach (var axis in nrrd.Axes)
            {
                Console.WriteLine("    label: {0}", axis.Label);
                Console.WriteLine("         size:   {0}", axis.Size);
                Console.WriteLine("         kind:   {0}", axis.Kind);
                Console.WriteLine("         centre: {0}", axis.Centre);
            }

            Console.WriteLine("Reading some data...");
            var data = nrrd.Data<UInt16>();

            for (int i = 0; i < 150; ++i)
                Console.WriteLine("{0}", data[i]);

            Console.ReadLine();

        }
    }
}
