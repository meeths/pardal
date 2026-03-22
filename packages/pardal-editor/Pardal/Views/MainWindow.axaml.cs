using System;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Pardal.Controls;

namespace Pardal.Views;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        Closing += (s, e) =>
        {
            var engineView = this.FindControl<EngineView>("MainEngineView");
            engineView?.StopEngine();
        };
    }

    private void HandleExitClick(object? sender, RoutedEventArgs e)
    {
        Close();
    }
}