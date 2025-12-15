using System;

namespace Pardal.Models;

public class LogEntry
{
    public DateTime Timestamp { get; set; }
    public string Category { get; set; } = string.Empty;
    public string Body { get; set; } = string.Empty;
    public LogType Type { get; set; } = LogType.Info;
}
