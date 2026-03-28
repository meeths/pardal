using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using Avalonia;
using Avalonia.Reactive;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media;
using Avalonia.Platform;
using Avalonia.VisualTree;
using Avalonia.Threading;
using Pardal.Models;
using Pardal.Services;

namespace Pardal.Controls;

public partial class EngineView : UserControl
{
    private Process? _engineProcess;
    private IntPtr _engineHandle = IntPtr.Zero;

    [DllImport("user32.dll", SetLastError = true)]
    private static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

    public EngineView()
    {
        InitializeComponent();
        UpdateVisuals();
        this.GetObservable(BoundsProperty).Subscribe(new AnonymousObserver<Rect>(_ => OnBoundsChanged()));
        this.EffectiveViewportChanged += (s, e) => OnBoundsChanged();
    }

    [DllImport("user32.dll")]
    private static extern bool EnumChildWindows(IntPtr hWndParent, EnumWindowsProc lpEnumFunc, IntPtr lParam);

    private delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    private void OnBoundsChanged()
    {
        if (_engineProcess != null && !_engineProcess.HasExited)
        {
            var topLevel = TopLevel.GetTopLevel(this);
            if (topLevel == null) return;

            var platformHandle = topLevel.TryGetPlatformHandle();
            if (platformHandle == null) return;

            var point = this.TranslatePoint(new Point(0, 0), topLevel);
            if (point == null) return;

            var bounds = new Rect(point.Value, this.Bounds.Size);
            double scaling = topLevel.RenderScaling;

            // Find the window belonging to our process that is a child of the top level window
            if (_engineHandle == IntPtr.Zero)
            {
                EnumChildWindows(platformHandle.Handle, (hWnd, lParam) =>
                {
                    GetWindowThreadProcessId(hWnd, out uint processId);
                    if (processId == (uint)_engineProcess.Id)
                    {
                        _engineHandle = hWnd;
                        return false; // Stop enumeration
                    }
                    return true;
                }, IntPtr.Zero);
            }

            if (_engineHandle != IntPtr.Zero)
            {
                MoveWindow(_engineHandle, (int)(bounds.X * scaling), (int)(bounds.Y * scaling), (int)(bounds.Width * scaling), (int)(bounds.Height * scaling), true);
            }
        }
    }

    private void OnStartEngineClick(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        // For now, we use a default path or let the user provide it.
        // Assuming pardal-test-app.exe is in the same directory as the editor or a known location.
        // In a real scenario, this might come from a configuration.
        RunEngine("C:\\Users\\Sisco\\Projects\\pardal\\bin\\pardal-engine-app\\Debug\\pardal-engine-app.exe");
        //RunEngine("C:\\Users\\Sisco\\Projects\\pardal\\bin\\pardal-test-app\\Debug\\pardal-test-app.exe");
    }

    private void UpdateVisuals()
    {
        var button = this.FindControl<Button>("StartEngineButton");
        if (button != null)
        {
            button.IsVisible = _engineProcess == null || _engineProcess.HasExited;
        }
        InvalidateVisual();
    }

    public void RunEngine(string exePath)
    {
        if (_engineProcess != null && !_engineProcess.HasExited)
            return;

        var topLevel = TopLevel.GetTopLevel(this);
        if (topLevel == null) return;

        var platformHandle = topLevel.TryGetPlatformHandle();
        if (platformHandle == null) return;

        var handle = platformHandle.Handle;

        var point = this.TranslatePoint(new Point(0, 0), topLevel);
        var bounds = point != null ? new Rect(point.Value, this.Bounds.Size) : new Rect(0, 0, 1280, 720);
        double scaling = topLevel.RenderScaling;

        ProcessStartInfo startInfo = new ProcessStartInfo(exePath);
        startInfo.Arguments = $"--parent_window {handle} --window_rect ({(int)(bounds.X * scaling)}, {(int)(bounds.Y * scaling)}, {(int)(bounds.Width * scaling)}, {(int)(bounds.Height * scaling)})";
        startInfo.UseShellExecute = false;
        startInfo.RedirectStandardOutput = true;
        startInfo.RedirectStandardError = true;
        startInfo.WorkingDirectory = Path.GetDirectoryName(exePath);
        startInfo.CreateNoWindow = true;
        try
        {
            _engineProcess = new Process();
            _engineProcess.StartInfo = startInfo;
            _engineProcess.EnableRaisingEvents = true;
            _engineProcess.Exited += (s, e) =>
            {
                _engineHandle = IntPtr.Zero;
                Dispatcher.UIThread.Post(UpdateVisuals);
            };

            _engineProcess.OutputDataReceived += (s, e) => ProcessLogLine(e.Data, false);
            _engineProcess.ErrorDataReceived += (s, e) => ProcessLogLine(e.Data, true);
            
            _engineProcess.Start();
            _engineProcess.BeginOutputReadLine();
            _engineProcess.BeginErrorReadLine();
            UpdateVisuals();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Failed to start engine: {ex.Message}");
            _engineProcess = null;
            UpdateVisuals();
        }
    }

    public void StopEngine()
    {
        if (_engineProcess != null && !_engineProcess.HasExited)
        {
            try
            {
                _engineProcess.Kill();
                _engineProcess.WaitForExit(1000);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error killing engine: {ex.Message}");
            }
            finally
            {
                _engineProcess = null;
                _engineHandle = IntPtr.Zero;
            }
        }
    }

    private void ProcessLogLine(string? line, bool isErrorStream)
    {
        if (string.IsNullOrEmpty(line)) return;

        // Strip ANSI codes and determine log level
        LogType type = isErrorStream ? LogType.Error : LogType.Info;
        
        // Priority 1: ANSI codes
        if (line.Contains("\u001b[1m\u001b[31m") || line.Contains("\u001b[31m"))
        {
            type = LogType.Error;
        }
        else if (line.Contains("\u001b[1m\u001b[33m") || line.Contains("\u001b[33m"))
        {
            type = LogType.Warning;
        }
        else if (line.Contains("\u001b[1m\u001b[32m") || line.Contains("\u001b[32m"))
        {
            type = LogType.Info;
        }

        // Remove ANSI codes
        string cleanLine = Regex.Replace(line, @"\u001b\[[0-9;]*m", "").Trim();
        if (string.IsNullOrEmpty(cleanLine)) return;

        // Priority 2: Keywords at start of line or in brackets
        if (type == LogType.Info || (isErrorStream && type == LogType.Error))
        {
            var upperLine = cleanLine.ToUpperInvariant();
            if (upperLine.StartsWith("ERROR:") || upperLine.StartsWith("[ERROR]") || upperLine.Contains(" FATAL ") || upperLine.Contains("EXCEPTION:"))
            {
                type = LogType.Error;
            }
            else if (upperLine.StartsWith("WARNING:") || upperLine.StartsWith("[WARNING]") || upperLine.StartsWith("WARN:") || upperLine.StartsWith("[WARN]"))
            {
                type = LogType.Warning;
            }
        }

        // Determine category
        string category = "Engine";
        
        // Try to find [Category]
        var categoryMatch = Regex.Match(cleanLine, @"^\[([^\]]+)\]");
        if (categoryMatch.Success)
        {
            string candidate = categoryMatch.Groups[1].Value;
            // If it's a level name, it might not be the category, but we'll accept it for now or look further
            category = candidate;
            cleanLine = cleanLine.Substring(categoryMatch.Length).TrimStart();
            
            // Check if there's another bracketed term or a colon (e.g. "[Category] [Level] Message" or "[Category] Level: Message")
            var secondMatch = Regex.Match(cleanLine, @"^\[([^\]]+)\]");
            if (secondMatch.Success)
            {
                // We found another bracket. If the first one was a level, swap them.
                string firstUpper = category.ToUpperInvariant();
                if (firstUpper == "INFO" || firstUpper == "WARNING" || firstUpper == "ERROR" || firstUpper == "DEBUG")
                {
                    category = secondMatch.Groups[1].Value;
                    cleanLine = cleanLine.Substring(secondMatch.Length).TrimStart();
                }
            }
        }
        else
        {
            // Try to find "Category: "
            categoryMatch = Regex.Match(cleanLine, @"^([A-Z][a-zA-Z0-9_]{1,20}):\s");
            if (categoryMatch.Success)
            {
                string candidate = categoryMatch.Groups[1].Value;
                string upperCandidate = candidate.ToUpperInvariant();
                if (upperCandidate != "ERROR" && upperCandidate != "WARNING" && upperCandidate != "INFO" && upperCandidate != "DEBUG" && upperCandidate != "WARN")
                {
                    category = candidate;
                    cleanLine = cleanLine.Substring(categoryMatch.Length).TrimStart();
                }
            }
        }

        Dispatcher.UIThread.Post(() =>
        {
            Logger.Publish(new LogEntry
            {
                Timestamp = DateTime.Now,
                Category = category,
                Body = cleanLine,
                Type = type
            });
        });
    }

    public override void Render(DrawingContext drawingContext)
    {
        base.Render(drawingContext);
        
        if (_engineProcess != null && !_engineProcess.HasExited)
            return;

        var tb = this.GetTransformedBounds()!.Value;
        var bounds = tb.Bounds;
        bounds = bounds.Deflate(5);

        var width = bounds.Width;
        var height = bounds.Height;
        
        if (width <= 0 || height <= 0)
            return;

        // Border and clip
        RoundedRect roundedRect = new(bounds, 5, 5);

        var penBorder = new Pen(Brushes.DarkGray, 1, lineCap: PenLineCap.Square);
        drawingContext.DrawRectangle(Brushes.Transparent, penBorder, roundedRect);


        // Diagonal lines
        var pen = new Pen(Brushes.DarkGray, 50, lineCap: PenLineCap.Square);
        const double spacing = 150;

        drawingContext.PushClip(roundedRect);

        for (double k = -width; k <= width; k += spacing)
        {
            var p1 = new Point(bounds.X + k, bounds.Y + height);
            var p2 = new Point(bounds.X + k + height, bounds.Y);
            drawingContext.DrawLine(pen, p1, p2);
        }


        // Engine status
        var brush = new SolidColorBrush(Colors.DarkRed, 0.5);

        drawingContext.DrawRectangle(brush, penBorder, new Rect(bounds.X, bounds.Y, bounds.Width, 40));

        var ft = new FormattedText("Engine is not available", CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
            Typeface.Default, 12, new SolidColorBrush(Colors.White));
        drawingContext.DrawText(ft, new Point(bounds.X + 10, bounds.Y + 10));

    }
}