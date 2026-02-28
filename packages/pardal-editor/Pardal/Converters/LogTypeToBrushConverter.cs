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
                _ => Brushes.Transparent
            },
            _ => Brushes.Transparent
        };
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, System.Globalization.CultureInfo culture)
        => throw new NotSupportedException();
}
