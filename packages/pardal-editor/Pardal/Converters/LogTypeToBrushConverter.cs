using System;
using Avalonia.Data.Converters;
using Avalonia.Media;
using Pardal.Models;

namespace Pardal.Converters;

public class LogTypeToBrushConverter : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, System.Globalization.CultureInfo culture)
    {
        var mode = parameter as string ?? string.Empty;
        var type = value as LogType? ?? (value is string s && Enum.TryParse<LogType>(s, out var lt) ? lt : LogType.Info);

        return mode switch
        {
            "Foreground" => type switch
            {
                LogType.Info => Brushes.ForestGreen,
                LogType.Warning => Brushes.DarkOrange,
                LogType.Error => Brushes.IndianRed,
                _ => Brushes.Black
            },
            "RowBackground" => type switch
            {
                LogType.Info => new SolidColorBrush(Color.FromArgb(0x10, 0x22, 0x8B, 0x22)), // light green
                LogType.Warning => new SolidColorBrush(Color.FromArgb(0x20, 0xFF, 0xA5, 0x00)), // light orange
                LogType.Error => new SolidColorBrush(Color.FromArgb(0x20, 0xFF, 0x00, 0x00)), // light red
                _ => Brushes.Transparent
            },
            _ => Brushes.Transparent
        };
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, System.Globalization.CultureInfo culture)
        => throw new NotSupportedException();
}
