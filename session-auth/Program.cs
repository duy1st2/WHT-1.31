using MongoDB.Bson;
using MongoDB.Driver;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Security.Cryptography;

internal static class Program
{
    internal static JsonDocument ReadConfiguration()
    {
        using var stream = typeof(Program).Assembly.GetManifestResourceStream("WHT.MongoConfiguration") ?? throw new InvalidDataException();
        using var buffer = new MemoryStream(); stream.CopyTo(buffer);
        var sealedBytes = buffer.ToArray();
        using var keyStream = typeof(Program).Assembly.GetManifestResourceStream("WHT.MongoKey") ?? throw new InvalidDataException();
        byte[] key = new byte[32]; keyStream.ReadExactly(key);
        if (keyStream.ReadByte() != -1) throw new InvalidDataException();
        byte[] clear = [];
        try {
            clear = ConfigurationCipher.Decrypt(sealedBytes, key);
            using var plaintext = new MemoryStream(clear, false); return JsonDocument.Parse(plaintext);
        } finally { CryptographicOperations.ZeroMemory(clear); CryptographicOperations.ZeroMemory(key); }
    }
    // P/Invoke cannot marshal a generic Func delegate as WNDENUMPROC.
    [UnmanagedFunctionPointer(CallingConvention.Winapi)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private delegate bool WindowCallback(IntPtr window, IntPtr data);
    [DllImport("user32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool EnumWindows(WindowCallback callback, IntPtr data);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] private static extern int GetWindowText(IntPtr window, StringBuilder text, int count);
    [DllImport("user32.dll")] private static extern uint GetWindowThreadProcessId(IntPtr window, out uint process);
    internal static bool ValidSession(string value) => value.Length == 32 && value.All(Uri.IsHexDigit);
    internal static bool MatchesTitle(string title, string username) => title == "Main - " + username || title.StartsWith("Main - " + username + " - ", StringComparison.Ordinal);
    private static bool MainRunning(string username)
    {
        bool found = false;
        EnumWindows((window, _) => {
            var title = new StringBuilder(512);
            GetWindowText(window, title, title.Capacity);
            if (!MatchesTitle(title.ToString(), username)) return true;
            GetWindowThreadProcessId(window, out uint pid);
            try { using var process = Process.GetProcessById((int)pid); found = process.ProcessName.Equals("LoadFileMapWar3", StringComparison.OrdinalIgnoreCase); }
            catch { }
            return !found;
        }, IntPtr.Zero);
        return found;
    }
    private static async Task<int> Main(string[] args)
    {
        if(args.Length==2&&args[0]=="--test-update-progress") {
            using var progress=UpdateProgress.TryOpen(args[1]);
            if(progress==null)return 2;
            progress.Download(75,100);return 0;
        }
        if(args.SequenceEqual(new[]{"--check-update"}))return await ToolUpdater.CheckAsync();
        if(args.SequenceEqual(new[]{"--verify-update-download"}))return await ToolUpdater.VerifyDownloadAsync();
        if(args.Length==3&&args[0]=="--validate-update-file") {
            try{ToolUpdater.ValidateFile(args[1],ToolUpdater.Normalize(args[2]),null);Console.WriteLine("PASS update package targets Warcraft 1.29");return 0;}
            catch(InvalidDataException){Console.Error.WriteLine("Rejected update package: version, target or format mismatch");return 1;}
        }
        if(args.Length==4&&args[0]=="--watch-update"&&int.TryParse(args[2],out var parent)) {
            try {
                using var ready=EventWaitHandle.OpenExisting(args[3]);
                using var progress=UpdateProgress.TryOpen(args[3]);
                try{return await ToolUpdater.WatchAsync(args[1],parent,progress);}
                finally{ready.Set();}
            }catch{return 2;}
        }
        if(args.SequenceEqual(new[] { "--inspect-toolinfo" })) {
            try {
                using var config=ReadConfiguration();
                var settings=MongoClientSettings.FromConnectionString(config.RootElement.GetProperty("connectionString").GetString()!);
                settings.ServerSelectionTimeout=TimeSpan.FromSeconds(3);
                var db=new MongoClient(settings).GetDatabase(config.RootElement.GetProperty("database").GetString()!);
                using var timeout=new CancellationTokenSource(TimeSpan.FromSeconds(5));
                var docs=await db.GetCollection<BsonDocument>(ToolUpdater.CollectionName).Find(FilterDefinition<BsonDocument>.Empty).Limit(3).ToListAsync(timeout.Token);
                foreach(var doc in docs){Console.WriteLine("Fields="+string.Join(",",doc.Names));if(doc.TryGetValue("version",out var v))Console.WriteLine("Version="+v.ToString());}
                Console.WriteLine("Documents="+docs.Count);return 0;
            }catch{Console.Error.WriteLine("toolinfo-1.29 inspection failed");return 2;}
        }
        bool diagnose = args.SequenceEqual(new[] { "--diagnose" });
        string stage = "session-file";
        int Deny(string reason, int code = 1) { if (diagnose) Console.Error.WriteLine(reason); return code; }
        if (args.SequenceEqual(new[] { "--self-test" })) {
            try {
                ToolUpdater.SelfTest();
                UpdateProgress.SelfTest();
                ConfigurationCipher.SelfTest();
                using var config = ReadConfiguration();
                _ = MongoClientSettings.FromConnectionString(config.RootElement.GetProperty("connectionString").GetString()!);
                if (string.IsNullOrWhiteSpace(config.RootElement.GetProperty("database").GetString())) { Console.Error.WriteLine("FAIL: empty database"); return 1; }
                if (!ValidSession(new string('a',32)) || ValidSession("bad") || !MatchesTitle("Main - user - 10 ngày","user") || MatchesTitle("Main - username","user") || MatchesTitle("Main - other","user")) { Console.Error.WriteLine("FAIL: session or title validation"); return 1; }
                // Exercise the real native callback boundary, even without the app.
                if (!EnumWindows((_, _) => true, IntPtr.Zero) && Marshal.GetLastWin32Error() != 0) return 1;
                Console.WriteLine("PASS session format, username matching and native window enumeration"); return 0;
            } catch (Exception ex) {
                Console.Error.WriteLine("FAIL self-test exception: " + ex);
                return 1;
            }
        }
        try {
            var file = Path.Combine(Path.GetTempPath(), "LoadFileMapWar3-session.txt");
            string session = File.ReadAllText(file).Trim();
            if (!ValidSession(session)) return Deny("invalid-session-format");
            // Embedded encrypted configuration; never emitted or written as plaintext.
            stage = "mongo-configuration";
            using var configuration = ReadConfiguration();
            var settings = MongoClientSettings.FromConnectionString(configuration.RootElement.GetProperty("connectionString").GetString()!);
            settings.ServerSelectionTimeout = TimeSpan.FromSeconds(3);
            settings.ConnectTimeout = TimeSpan.FromSeconds(3);
            settings.SocketTimeout = TimeSpan.FromSeconds(3);
            using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(5));
            var users = new MongoClient(settings).GetDatabase(configuration.RootElement.GetProperty("database").GetString()!).GetCollection<BsonDocument>("users");
            stage = "mongo-query";
            var matches = await users.Find(Builders<BsonDocument>.Filter.Eq("sessionId",session)).Project(Builders<BsonDocument>.Projection.Include("username").Exclude("_id")).Limit(2).ToListAsync(timeout.Token);
            if (matches.Count != 1 || !matches[0].TryGetValue("username",out var value) || !value.IsString) return Deny("session-user-not-found-or-ambiguous", 3);
            string username = value.AsString;
            if (string.IsNullOrWhiteSpace(username) || username.Contains('\n') || username.Contains('\r')) return Deny("invalid-username");
            stage = "main-window";
            if (!MainRunning(username)) return Deny("matching-main-window-not-found");
            if (File.ReadAllText(file).Trim() != session) return Deny("session-changed-during-check");
            if (diagnose) { Console.WriteLine("PASS MongoDB session and running Main window"); return 0; }
            Console.OutputEncoding = new UTF8Encoding(false);
            Console.WriteLine(session);
            Console.WriteLine(username);
            return 0;
        } catch (Exception error) { return Deny(stage + ": " + error.GetType().Name, 2); }
    }
}
