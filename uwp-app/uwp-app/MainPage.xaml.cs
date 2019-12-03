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

namespace uwp_app
{

    public sealed partial class MainPage : Page
    {
        static DeviceClient deviceClient = null;
        static Sensor sensor;

        private static void CreateClient()
        {
            if (deviceClient == null)
            {
                const string deviceConnectionString = "HostName=iothub18exam.azure-devices.net;DeviceId=AppDevice;SharedAccessKey=RRzx7/FRHlJF5/oJfv3uO8MP8O8fjyyeEKrK/HU197o=";
                deviceClient = DeviceClient.CreateFromConnectionString(deviceConnectionString, TransportType.Mqtt);
            }
        }

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
                    messageX.Text = sensor.X_out.ToString();
                    messageY.Text = sensor.Y_out.ToString();
                    messageZ.Text = sensor.Z_out.ToString();
                    messageId.Text = sensor.messageId.ToString();

                }
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
        }

        public MainPage()
        {
            this.InitializeComponent();
            ReceiveCloudToDeviceMessageAsync();
        }
    }

    public class Sensor
    {
        public string deviceId { get; set; }
        public int messageId { get; set; }
        public float X_out { get; set; }
        public float Y_out { get; set; }
        public float Z_out { get; set; }
    }


}
