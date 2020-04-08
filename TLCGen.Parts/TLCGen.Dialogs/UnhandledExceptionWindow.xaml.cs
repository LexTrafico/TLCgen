using System;
using System.Diagnostics;
using System.Reflection;
using System.Windows;

namespace TLCGen.Dialogs
{
    /// <summary>
    /// Interaction logic for EnterNameWindow.xaml
    /// </summary>
    public partial class UnhandledExceptionWindow : Window
    {
        public string DialogTitle
        {
            get => this.Title;
            set => this.Title = value;
        }

        public string DialogMessage
        {
            get => (string)MessageLabel.Content;
            set => MessageLabel.Content = value;
        }

        public string DialogExpceptionText
        {
            get => (string)ExceptionTextTb.Text;
            set => ExceptionTextTb.Text = value;
        }

        public UnhandledExceptionWindow()
        {
            InitializeComponent();
        }

        private void OKButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void SendButton_Click(object sender, RoutedEventArgs e)
        {
            var subject = $"Fout opgetreden in TLCGen (versie {Assembly.GetEntryAssembly()?.GetName().Version})";
            var body = ("Bij het werken met de applicatie TLCGen is er een fout opgetreden. " +
                        "De fout tradt op na de volgende handelingen en onder de volgende omstandigheden:" + Environment.NewLine + Environment.NewLine +
                        ">>> OMSCHRIJF HIER DE DETAILS <<<" + Environment.NewLine + Environment.NewLine + DialogExpceptionText).Replace(Environment.NewLine, "%0D%0A");
            Process.Start($"mailto:tlcgen@codingconnected.eu?subject={subject}&body={body}");
            this.Close();
        }

    }
}