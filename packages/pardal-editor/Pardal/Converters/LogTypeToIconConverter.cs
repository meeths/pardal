using System;
using Avalonia.Data.Converters;
using Pardal.Models;

namespace Pardal.Converters;

public class LogTypeToIconConverter : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, System.Globalization.CultureInfo culture)
    {
        var type = value as LogType? ?? (value is string s && Enum.TryParse<LogType>(s, out var lt) ? lt : LogType.Info);

        // Unicode glyphs that work cross-platform reasonably well
        return type switch
        {
            LogType.Info => "ℹ",
            LogType.Warning => "⚠",
            LogType.Error => "⛔",
            _ => string.Empty
        };
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, System.Globalization.CultureInfo culture)
        => throw new NotSupportedException();
}
