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
    }

    private void HandleExitClick(object? sender, RoutedEventArgs e)
    {
        Environment.Exit(0);
    }
}