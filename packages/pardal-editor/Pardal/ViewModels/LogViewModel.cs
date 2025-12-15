using System;
using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using Pardal.Models;
using Avalonia.Collections;
using Avalonia;
using Avalonia.Controls;
using Pardal.Services;

namespace Pardal.ViewModels;

public partial class LogViewModel : ViewModelBase
{
    public ObservableCollection<LogEntry> Items { get; } = new();

    public DataGridCollectionView View { get; }

    [ObservableProperty]
    private string _filterText = string.Empty;

    public LogViewModel()
    {
        View = new DataGridCollectionView(Items)
        {
            Filter = o => FilterPredicate(o as LogEntry)
        };

        // Sample design data
        if (Design.IsDesignMode)
        {
            Add("System", "Application started", LogType.Info);
            Add("Loader", "Could not find asset: logo.svg", LogType.Warning);
            Add("Renderer", "Unhandled device loss", LogType.Error);
        }
        else
        {
            Logger.Logged += OnLogged;
            Add("System", "Application started", LogType.Info);
            Add("Loader", "Could not find asset: logo.svg", LogType.Warning);
            Add("Renderer", "Unhandled device loss", LogType.Error);
        }
    }

    private void OnLogged(LogEntry entry)
    {
        Items.Add(entry);
    }

    partial void OnFilterTextChanged(string value)
    {
        View.Refresh();
    }

    private bool FilterPredicate(LogEntry? e)
    {
        if (e is null)
            return false;
        if (string.IsNullOrWhiteSpace(FilterText))
            return true;
        var f = FilterText.Trim();
        // simple contains across fields (case-insensitive)
        return e.Category?.Contains(f, StringComparison.OrdinalIgnoreCase) == true
               || e.Body?.Contains(f, StringComparison.OrdinalIgnoreCase) == true
               || e.Type.ToString().Contains(f, StringComparison.OrdinalIgnoreCase)
               || e.Timestamp.ToString("u").Contains(f, StringComparison.OrdinalIgnoreCase);
    }

    public void Add(string category, string body, LogType type)
    {
        Items.Add(new LogEntry
        {
            Timestamp = DateTime.Now,
            Category = category,
            Body = body,
            Type = type
        });
    }
}
