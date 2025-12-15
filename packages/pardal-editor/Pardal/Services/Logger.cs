using System;
using Pardal.Models;

namespace Pardal.Services;

public static class Logger
{
    public static event Action<LogEntry>? Logged;

    public static void Publish(LogEntry entry)
    {
        Logged?.Invoke(entry);
    }

    public static void Info(string category, string message) => Publish(new LogEntry
    {
        Timestamp = DateTime.Now,
        Category = category,
        Body = message,
        Type = LogType.Info
    });

    public static void Warning(string category, string message) => Publish(new LogEntry
    {
        Timestamp = DateTime.Now,
        Category = category,
        Body = message,
        Type = LogType.Warning
    });

    public static void Error(string category, string message) => Publish(new LogEntry
    {
        Timestamp = DateTime.Now,
        Category = category,
        Body = message,
        Type = LogType.Error
    });
}
