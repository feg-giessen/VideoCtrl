using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VideoCtrl
{
    class Beamer
    {
        private static Dictionary<string, string> statusList = new Dictionary<string, string>
        {
            { "00", "Power On" },
            { "80", "Betriebsbereitschaft (Stand-By)" },
            { "40", "Countdown" },
            { "20", "Abkühlen" },
            { "10", "Stromversorgungsfehler" },
            { "28", "Unregelmäßigkeit beim Abkühlen" },
            { "24", "Abkühlen zum Modus Automatische Lampenabschaltung" },
            { "04", "Modus Automatische Lampenabschaltung nach dem Abkühlen" },
            { "21", "Abkühlen nachdem der Projektor ausgeschaltet wurde wenn die Lampen aus sind." },
            { "81", "Betriebsbereitschaft nach dem Abkühlen wenn die Lampen ausgeschaltet sind." },
            { "88", "Betriebsbereitschaft nach dem Abkühlen wenn Unregelmäßigkeiten mit der Temperatur auftreten." },
            { "2C", "Abkühlen nachdem der Projektor durch die Shutter-Steuerung ausgeschaltet wurde." },
            { "8C", "Betriebsbereitschaft nach dem Abkühlen durch die Shutter-Steuerung" }
        };

        public string Status
        {
            get;
            private set;
        }

        public decimal Temperature1 { get; private set; }

        public decimal Temperature2 { get; private set; }

        public decimal Temperature3 { get; private set; }

        public void SetStatus(string value)
        {
            if (string.IsNullOrEmpty(value))
                return;

            if (statusList.ContainsKey(value))
            {
                this.Status = statusList[value];
            }
            else
            {
                this.Status = value;
            }
        }
        
        public void SetTemperatures(string value)
        {
            if (string.IsNullOrEmpty(value))
                return;

            string[] values = value.Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

            if (values.Length != 3)
                return;

            decimal parsed;

            if (decimal.TryParse(values[0], NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out parsed))
            {
                this.Temperature1 = parsed;
            }

            if (decimal.TryParse(values[1], NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out parsed))
            {
                this.Temperature2 = parsed;
            }

            if (decimal.TryParse(values[2], NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out parsed))
            {
                this.Temperature3 = parsed;
            }
        }
    }
}
