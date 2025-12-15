using System;
using System.Globalization;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using Avalonia.Media;
using Avalonia.VisualTree;

namespace Pardal.Controls;

public partial class EngineView : UserControl
{
    public EngineView()
    {
        InitializeComponent();
    }
    
    public override void Render(DrawingContext drawingContext)
    {
        base.Render(drawingContext);
        
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