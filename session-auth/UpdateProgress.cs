using System.IO.MemoryMappedFiles;

internal sealed class UpdateProgress : IDisposable
{
    private readonly MemoryMappedFile mapping;
    private readonly MemoryMappedViewAccessor view;
    private int phase;
    private UpdateProgress(MemoryMappedFile mapping)
    {
        this.mapping = mapping;
        view = mapping.CreateViewAccessor(0, 8, MemoryMappedFileAccess.ReadWrite);
    }
    internal static UpdateProgress? TryOpen(string eventName)
    {
        MemoryMappedFile? mapping = null;
        try {
            mapping = MemoryMappedFile.OpenExisting(eventName + ".Progress", MemoryMappedFileRights.ReadWrite);
            return new UpdateProgress(mapping);
        } catch { mapping?.Dispose(); return null; }
    }
    internal static int DownloadPercent(long received, long? length) =>
        length is > 0 ? (int)Math.Clamp(received * 100 / length.Value, 0, 99) : -1;
    internal void Publish(int state, int percent)
    {
        view.Write(4, percent);
        Thread.MemoryBarrier();
        view.Write(0, state);
        phase = state;
    }
    internal void Download(long received, long? length) => Publish(2, DownloadPercent(received, length));
    internal void Failed() { if (phase != 0) Publish(5, 0); }
    public void Dispose() { view.Dispose(); mapping.Dispose(); }
    internal static void SelfTest()
    {
        if (DownloadPercent(0, 100) != 0 || DownloadPercent(45, 100) != 45 ||
            DownloadPercent(100, 100) != 99 || DownloadPercent(101, 100) != 99 ||
            DownloadPercent(40, null) != -1 || DownloadPercent(40, 0) != -1)
            throw new InvalidDataException("Invalid download progress");
        string name = "Local\\WHT.Progress.Test." + Guid.NewGuid().ToString("N");
        using var mapping = MemoryMappedFile.CreateNew(name + ".Progress", 8);
        using var reader = mapping.CreateViewAccessor(0, 8, MemoryMappedFileAccess.Read);
        using var writer = TryOpen(name) ?? throw new InvalidDataException("Progress IPC unavailable");
        writer.Download(45, 100);
        if (reader.ReadInt32(0) != 2 || reader.ReadInt32(4) != 45) throw new InvalidDataException("Progress IPC mismatch");
        writer.Publish(4, 100);
        if (reader.ReadInt32(0) != 4 || reader.ReadInt32(4) != 100) throw new InvalidDataException("Verified progress mismatch");
    }
}
