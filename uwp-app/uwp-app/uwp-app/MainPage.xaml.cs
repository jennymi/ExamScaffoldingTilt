using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Microsoft.Azure.Devices.Client;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

using LiveCharts;
using LiveCharts.Uwp;

namespace uwp_app
{

    public sealed partial class MainPage : Page
    {
        List<double> roll = new List<double> { 3, 4 };
        List<double> pitch = new List<double> { 1, 2 };

        public MainPage()
        {
            this.InitializeComponent();
            ReceiveCloudToDeviceMessageAsync();
            CreateChart();
        }

        static DeviceClient deviceClient = null;
        static Sensor sensor;

        private static void CreateClient()
        {
            if (deviceClient == null)
            {
                const string deviceConnectionString = "HostName=iothub18exam.azure-devices.net;DeviceId=AppDeviceTest;SharedAccessKey=Ixeg2Y38G7y1KXdy9vhS3zzANxa7NGGZgwg4ysPoK1c=";
                deviceClient = DeviceClient.CreateFromConnectionString(deviceConnectionString, TransportType.Mqtt);
            }
        }

        public void CreateChart()
        {
            SeriesCollection = new SeriesCollection
            {
                new LineSeries
                {
                    Title = "Series 1",
                    Values = new ChartValues<double>(),
                    PointGeometry = null

                },
                new LineSeries
                {
                    Title = "Series 2",
                    Values = new ChartValues<double>(),
                    PointGeometry = null
                }
            };

            Labels = new List<string> { "10 dec" };

            YFormatter = value => value.ToString();

            DataContext = this;
        }

        public async Task AddValuesAsync(double rollValue, double pitchValue, string dateTime)
        {

            roll.Add(rollValue);
            pitch.Add(pitchValue);
            foreach (var item in roll)
            {
                SeriesCollection[0].Values.Add(item);
            }

            foreach (var item in pitch)
            {
                SeriesCollection[1].Values.Add(item);
            }

            foreach (var item in pitch)
            {
                Labels.Add(dateTime);
            }
        }

        public SeriesCollection SeriesCollection { get; set; }
        public List<string> Labels { get; set; }
        public Func<double, string> YFormatter { get; set; } 

        public async void ReceiveCloudToDeviceMessageAsync()
        {
            CreateClient();

            while (true)
            {
                var receiveMessage = await deviceClient.ReceiveAsync();
                if (receiveMessage != null)
                {
                    var messageData = Encoding.ASCII.GetString(receiveMessage.GetBytes());
                    await deviceClient.CompleteAsync(receiveMessage);
                    sensor = JsonConvert.DeserializeObject<Sensor>(messageData);
                    messageRoll.Text = sensor.rollF.ToString();
                    messagePitch.Text = sensor.pitchF.ToString();
                    messageTime.Text = sensor.dateTime;
                    //messageZ.Text = sensor.Z_out.ToString();
                    deviceId.Text = sensor.deviceId.ToString();


                    await AddValuesAsync(sensor.rollF, sensor.pitchF, sensor.dateTime);
                    

                }
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
        }
    }

    public class Sensor
    {
        public string deviceId { get; set; }
        //public int messageId { get; set; }
        public float rollF { get; set; }
        public float pitchF { get; set; }
        public string dateTime { get; set; }

    }


}
