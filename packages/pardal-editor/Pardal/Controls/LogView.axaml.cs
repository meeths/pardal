using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Pardal.ViewModels;

namespace Pardal.Controls;

public partial class LogView : UserControl
{
    public LogView()
    {
        InitializeComponent();
        if (Design.IsDesignMode)
        {
            DataContext = new LogViewModel();
        }
        else if (DataContext is null)
        {
            DataContext = new LogViewModel();
        }
    }

    private void InitializeComponent()
    {
        AvaloniaXamlLoader.Load(this);
    }
}
