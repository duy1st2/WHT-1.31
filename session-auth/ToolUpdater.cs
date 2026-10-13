using MongoDB.Bson;
using MongoDB.Driver;
using System.Diagnostics;
using System.Net;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text.RegularExpressions;
using System.Text;
using Microsoft.Win32.SafeHandles;

internal static class ToolUpdater
{
    internal const string CollectionName = "toolinfo-1.31";
    private const long MaximumBytes = 128 * 1024 * 1024;
    private static readonly Version Current = Normalize(typeof(Program).Assembly.GetName().Version!.ToString());
    private sealed record Release(Version Version, string FolderId, string? Sha256);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int MessageBoxW(IntPtr owner, string text, string title, uint flags);
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    private static extern uint GetFinalPathNameByHandleW(SafeFileHandle handle,StringBuilder path,uint size,uint flags);
    private static string CanonicalDirectory(string file)
    {
        using var handle=File.OpenHandle(file,FileMode.Open,FileAccess.Read,FileShare.ReadWrite|FileShare.Delete);
        var path=new StringBuilder(32768);uint count=GetFinalPathNameByHandleW(handle,path,(uint)path.Capacity,0);
        if(count==0||count>=path.Capacity)throw new IOException("Cannot resolve client directory");
        return Path.GetDirectoryName(path.ToString())!;
    }

    internal static Version Normalize(string value)
    {
        if(value.Length > 32 || !Version.TryParse(value.Trim(), out var parsed)) throw new InvalidDataException("Invalid version");
        return new Version(parsed.Major, parsed.Minor, Math.Max(0, parsed.Build), Math.Max(0, parsed.Revision));
    }
    private static async Task<Release> ReadReleaseAsync(CancellationToken token)
    {
        using var config = Program.ReadConfiguration();
        var settings = MongoClientSettings.FromConnectionString(config.RootElement.GetProperty("connectionString").GetString()!);
        settings.ServerSelectionTimeout = TimeSpan.FromSeconds(3);
        settings.ConnectTimeout = TimeSpan.FromSeconds(3);
        settings.SocketTimeout = TimeSpan.FromSeconds(3);
        var db = new MongoClient(settings).GetDatabase(config.RootElement.GetProperty("database").GetString()!);
        var projection = Builders<BsonDocument>.Projection.Include("version").Include("folderId").Include("sha256").Exclude("_id");
        var docs = await db.GetCollection<BsonDocument>(CollectionName).Find(FilterDefinition<BsonDocument>.Empty).Project(projection).Limit(2).ToListAsync(token);
        if(docs.Count != 1) throw new InvalidDataException("Expected one toolinfo-1.31 document");
        return ParseRelease(docs[0]);
    }
    private static string ValidateFolderId(string value)
    {
        string id = value.Trim();
        if(!Regex.IsMatch(id, @"\A[A-Za-z0-9_-]{10,100}\z")) throw new InvalidDataException("Invalid toolinfo folderId");
        return id;
    }
    private static Release ParseRelease(BsonDocument doc)
    {
        if(!doc.TryGetValue("version", out var version) || !version.IsString) throw new InvalidDataException("Expected toolinfo version");
        if(!doc.TryGetValue("folderId", out var folderId) || !folderId.IsString) throw new InvalidDataException("Expected toolinfo folderId");
        string? hash = null;
        if(doc.TryGetValue("sha256", out var checksum)) {
            if(!checksum.IsString || !Regex.IsMatch(checksum.AsString, "\\A[0-9a-fA-F]{64}\\z")) throw new InvalidDataException("Invalid update checksum");
            hash = checksum.AsString;
        }
        return new Release(Normalize(version.AsString), ValidateFolderId(folderId.AsString), hash);
    }
    internal static string FindFileId(string html, string folderId)
    {
        folderId = ValidateFolderId(folderId);
        // Drive's public folder data escapes JSON as JavaScript hex characters.
        string decoded = Regex.Replace(html, @"\\x([0-9a-fA-F]{2})", m => ((char)Convert.ToByte(m.Groups[1].Value,16)).ToString());
        var matches = Regex.Matches(decoded, @"\[""(?<id>[A-Za-z0-9_-]{10,100})""\s*,\s*\[(?<parents>[^\]]*)\]\s*,\s*""(?<name>WHT-1\.31\.mix|WH-1\.31\.mix|WHT\.mix|WH\.mix)""");
        var files = matches.Cast<Match>().Where(m => m.Groups["parents"].Value.Contains('"'+folderId+'"',StringComparison.Ordinal))
            .Select(m => (Id:m.Groups["id"].Value, Name:m.Groups["name"].Value)).Distinct().ToList();
        var preferred = files.Where(f => f.Name == "WHT-1.31.mix").ToList();
        if(preferred.Count == 0) preferred = files.Where(f => f.Name == "WH-1.31.mix").ToList();
        if(preferred.Count == 0) preferred = files.Where(f => f.Name == "WHT.mix").ToList();
        if(preferred.Count == 0) preferred = files.Where(f => f.Name == "WH.mix").ToList();
        if(preferred.Count != 1) throw new InvalidDataException("Expected one public WHT.mix or WH.mix in update folder");
        return preferred[0].Id;
    }
    private static HttpClient CreateHttpClient() => new(new HttpClientHandler { AutomaticDecompression = DecompressionMethods.All }) { Timeout = TimeSpan.FromMinutes(3) };
    private static void CheckResponse(HttpResponseMessage response)
    {
        response.EnsureSuccessStatusCode();
        var uri = response.RequestMessage?.RequestUri ?? throw new InvalidDataException();
        if(uri.Scheme != "https" || !(uri.Host == "drive.google.com" || uri.Host == "drive.usercontent.google.com" || uri.Host.EndsWith(".googleusercontent.com",StringComparison.OrdinalIgnoreCase))) throw new InvalidDataException("Unexpected update download host");
    }
    private static async Task<string> DownloadAsync(string folder, string folderId, CancellationToken token, UpdateProgress? progress=null)
    {
        folderId = ValidateFolderId(folderId);
        using var client = CreateHttpClient();
        using var listing = await client.GetAsync("https://drive.google.com/drive/folders/"+folderId, token);
        CheckResponse(listing);
        string id = FindFileId(await listing.Content.ReadAsStringAsync(token), folderId);
        using var response = await client.GetAsync("https://drive.usercontent.google.com/download?id="+id+"&export=download&confirm=t", HttpCompletionOption.ResponseHeadersRead, token);
        CheckResponse(response);
        if(response.Content.Headers.ContentLength > MaximumBytes) throw new InvalidDataException("Update too large");
        var length=response.Content.Headers.ContentLength;
        progress?.Download(0,length);
        string path = Path.Combine(folder,"WHT.mix");
        await using var input = await response.Content.ReadAsStreamAsync(token);
        await using var output = new FileStream(path,FileMode.CreateNew,FileAccess.Write,FileShare.None);
        byte[] buffer = new byte[65536]; long total = 0; int count;
        while((count = await input.ReadAsync(buffer,token)) != 0) {
            total += count; if(total > MaximumBytes) throw new InvalidDataException("Update too large");
            await output.WriteAsync(buffer.AsMemory(0,count),token);
            progress?.Download(total,length);
        }
        return path;
    }
    internal static void ValidateFile(string path, Version expected, string? expectedHash)
    {
        var file = new FileInfo(path);
        if(file.Length < 65536 || file.Length > MaximumBytes) throw new InvalidDataException("Invalid update size");
        using var stream = File.OpenRead(path);
        using var pe = new PEReader(stream,PEStreamOptions.LeaveOpen);
        if(pe.PEHeaders.CoffHeader.Machine != Machine.I386 || (pe.PEHeaders.CoffHeader.Characteristics & Characteristics.Dll)==0) throw new InvalidDataException("Update must be an x86 DLL");
        var info = FileVersionInfo.GetVersionInfo(path);
        if(Normalize(info.ProductVersion ?? "") != expected || info.ProductName != "WHT 1.31") throw new InvalidDataException("Downloaded version or Warcraft target does not match");
        bool mpq = false; byte[] header = new byte[4];
        for(long offset=512;offset+4<=stream.Length;offset+=512){stream.Position=offset;stream.ReadExactly(header);if(header.AsSpan().SequenceEqual("MPQ\u001a"u8)){mpq=true;break;}}
        if(!mpq) throw new InvalidDataException("Missing bundled UI archive");
        if(expectedHash != null && !Hash(path).Equals(expectedHash,StringComparison.OrdinalIgnoreCase)) throw new InvalidDataException("Update checksum mismatch");
    }
    private static string Hash(string path){using var stream=File.OpenRead(path);return Convert.ToHexString(SHA256.HashData(stream));}
    private static void Log(string directory,string text)
    {
        try{File.AppendAllText(Path.Combine(directory,"WHT.update.log"),DateTime.Now.ToString("s")+" "+text+Environment.NewLine);}catch{}
    }
    internal static void Install(string staged,string target,string? originalHash)
    {
        if((File.Exists(target)?Hash(target):null)!=originalHash) throw new IOException("Tool changed while update was staged");
        string expectedHash=Hash(staged);
        string backupDirectory=Path.Combine(Path.GetDirectoryName(target)!,"backup-wht-update-"+DateTime.Now.ToString("yyyyMMdd-HHmmss")+"-"+Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(backupDirectory);
        string backup=Path.Combine(backupDirectory,"WHT.mix");
        if(File.Exists(target))File.Replace(staged,target,backup);else File.Move(staged,target);
        if(Hash(target)!=expectedHash){if(File.Exists(backup))File.Replace(backup,target,null);throw new IOException("Installed update verification failed");}
    }
    internal static async Task<int> WatchAsync(string directory,int parentId,UpdateProgress? progress=null)
    {
        directory=Path.GetFullPath(directory);
        if(!Directory.Exists(directory))return 2;
        string? staging=null;
        try {
            using var parent=Process.GetProcessById(parentId);
            _=parent.SafeHandle; // Pin this process instance, not a future reused PID.
            if(!new[]{"war3","Warcraft III","Frozen Throne"}.Contains(parent.ProcessName,StringComparer.OrdinalIgnoreCase))return 2;
            string target=Path.Combine(directory,"WHT.mix");
            string clientDirectory=CanonicalDirectory(target);
            if(!parent.HasExited) {
                string phase="query";
                try {
                    using var queryTimeout=new CancellationTokenSource(TimeSpan.FromSeconds(5));
                    var release=await ReadReleaseAsync(queryTimeout.Token);
                    if(release.Version!=Current) {
                        progress?.Publish(1,0);
                        string? originalHash=File.Exists(target)?Hash(target):null;
                        staging=Path.Combine(directory,".WHT-update-"+Guid.NewGuid().ToString("N"));Directory.CreateDirectory(staging);
                        using var downloadTimeout=new CancellationTokenSource(TimeSpan.FromMinutes(3));
                        phase="download";
                        string downloaded=await DownloadAsync(staging,release.FolderId,downloadTimeout.Token,progress);
                        phase="validate";
                        progress?.Publish(3,99);
                        ValidateFile(downloaded,release.Version,release.Sha256);
                        progress?.Publish(4,100);
                        Log(directory,"Downloaded verified update "+release.Version+"; waiting for Warcraft exit");
                        if(!parent.HasExited)MessageBoxW(IntPtr.Zero,"Đã tải WHT phiên bản "+release.Version.ToString(3)+". Hãy đóng Warcraft III để cài bản cập nhật. Thiết lập INI sẽ được giữ nguyên.","WHT Update",0x10040);
                        await parent.WaitForExitAsync();
                        foreach(var running in Process.GetProcesses()) {
                            using(running) {
                                string name;
                                try{name=running.ProcessName;}catch(InvalidOperationException){continue;}
                                if(!new[]{"war3","Warcraft III","Frozen Throne"}.Contains(name,StringComparer.OrdinalIgnoreCase))continue;
                                if(running.HasExited)continue;
                                string? image=running.MainModule?.FileName;
                                if(image==null||CanonicalDirectory(image).Equals(clientDirectory,StringComparison.OrdinalIgnoreCase))throw new IOException("Another Warcraft client still running");
                            }
                        }
                        phase="install";Install(downloaded,target,originalHash);
                        Log(directory,"Installed update "+release.Version+"; reopen Warcraft III");
                        MessageBoxW(IntPtr.Zero,"Đã cập nhật WHT lên phiên bản "+release.Version.ToString(3)+". Bạn có thể mở lại Warcraft III.","WHT Update",0x10040);
                        return 0;
                    }
                }catch(Exception error){progress?.Failed();Log(directory,"Update "+phase+" failed: "+error.GetType().Name);return 2;}
            }
            return 0;
        }catch(Exception error){progress?.Failed();Log(directory,"Updater failed: "+error.GetType().Name);return 2;}
        finally{if(staging!=null)CleanStaging(staging);}
    }
    private static void CleanStaging(string staging)
    {
        // Only remove the exact file/directory created by this updater.
        try{File.Delete(Path.Combine(staging,"WHT.mix"));Directory.Delete(staging);}catch{}
    }
    internal static async Task<int> CheckAsync()
    {
        try{
            using var timeout=new CancellationTokenSource(TimeSpan.FromSeconds(5));var release=await ReadReleaseAsync(timeout.Token);
            Console.WriteLine("Current="+Current+" Remote="+release.Version+" Update="+(release.Version!=Current));
            using var client=CreateHttpClient();using var response=await client.GetAsync("https://drive.google.com/drive/folders/"+release.FolderId);
            CheckResponse(response);_=FindFileId(await response.Content.ReadAsStringAsync(),release.FolderId);
            Console.WriteLine("PASS public update file found");return 0;
        }catch(Exception error){Console.Error.WriteLine("Update check failed: "+error.GetType().Name);return 2;}
    }
    internal static async Task<int> VerifyDownloadAsync()
    {
        string staging=Path.Combine(Path.GetTempPath(),"WHT-download-test-"+Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(staging);
        try {
            using var timeout=new CancellationTokenSource(TimeSpan.FromMinutes(3));
            var release=await ReadReleaseAsync(timeout.Token);
            string file=await DownloadAsync(staging,release.FolderId,timeout.Token);
            ValidateFile(file,release.Version,release.Sha256);
            Console.WriteLine("PASS public Drive download, x86 mix and MongoDB version "+release.Version+"; no installation performed");return 0;
        }catch(Exception error){Console.Error.WriteLine("Download verification failed: "+error.GetType().Name);return 2;}
        finally{CleanStaging(staging);}
    }
    internal static void SelfTest()
    {
        if(Normalize("1.0.0")!=Normalize("1.0.0.0") || Normalize("1.0.1")==Normalize("1.0.0"))throw new InvalidDataException();
        const string folderId="folderid123456789", otherFolderId="folderid987654321";
        var release=ParseRelease(new BsonDocument{{"version","1.0.1"},{"folderId"," "+folderId+" "}});
        if(release.Version!=Normalize("1.0.1")||release.FolderId!=folderId||release.Sha256!=null)throw new InvalidDataException();
        foreach(var invalid in new[]{new BsonDocument{{"version","1.0.1"}},new BsonDocument{{"version","1.0.1"},{"folderId",42}},new BsonDocument{{"version","1.0.1"},{"folderId",""}},new BsonDocument{{"version","1.0.1"},{"folderId","https://drive.google.com/drive/folders/"+folderId}}}) {
            try{_=ParseRelease(invalid);throw new Exception("Invalid folderId accepted");}catch(InvalidDataException){}
        }
        string listing="[\"fileid123456789\",[\""+folderId+"\"],\"WHT.mix\"]";
        if(FindFileId(listing,release.FolderId)!="fileid123456789")throw new InvalidDataException();
        string targetListing=listing.Replace("fileid123456789","targetid123456789").Replace("WHT.mix","WHT-1.31.mix");
        if(FindFileId(listing+targetListing,folderId)!="targetid123456789")throw new InvalidDataException("Wrong Warcraft target selected");
        var targetRelease=ParseRelease(new BsonDocument{{"version","1.0.5"},{"folderId",otherFolderId},{"sha256",new string('a',64)}});
        if(targetRelease.Version!=Normalize("1.0.5")||targetRelease.FolderId!=otherFolderId||targetRelease.Sha256!=new string('a',64))throw new InvalidDataException("Wrong target metadata");
        try{_=ParseRelease(new BsonDocument{{"version","1.0.5"},{"folderId",folderId},{"sha256","invalid"}});throw new Exception("Invalid checksum accepted");}catch(InvalidDataException){}
        if(FindFileId(listing.Replace("\"",@"\x22").Replace("[",@"\x5b").Replace("]",@"\x5d"),folderId)!="fileid123456789")throw new InvalidDataException();
        var otherRelease=ParseRelease(new BsonDocument{{"version","1.0.1"},{"folderId",otherFolderId}});
        if(FindFileId(listing.Replace(folderId,otherFolderId),otherRelease.FolderId)!="fileid123456789")throw new InvalidDataException();
        try{_=FindFileId(listing,otherRelease.FolderId);throw new Exception("Wrong folder accepted");}catch(InvalidDataException){}
        try{_=FindFileId(listing+listing.Replace("fileid123456789","fileid987654321"),folderId);throw new Exception("Ambiguous update accepted");}catch(InvalidDataException){}
        try{_=Normalize("not-a-version");throw new Exception("Invalid version accepted");}catch(InvalidDataException){}
        string temp=Path.Combine(Path.GetTempPath(),"WHT-update-test-"+Guid.NewGuid().ToString("N"));Directory.CreateDirectory(temp);
        try{
            string target=Path.Combine(temp,"WHT.mix"),staged=Path.Combine(temp,"staged.mix"),ini=Path.Combine(temp,"WHT.ini");
            File.WriteAllText(target,"old");File.WriteAllText(staged,"new");File.WriteAllText(ini,"settings");
            if(CanonicalDirectory(target)!=CanonicalDirectory(ini))throw new InvalidDataException("Native path resolution failed");
            Install(staged,target,Hash(target));
            if(File.ReadAllText(target)!="new"||File.ReadAllText(ini)!="settings"||File.ReadAllText(Directory.GetFiles(temp,"WHT.mix",SearchOption.AllDirectories).Single(p=>p!=target))!="old")throw new InvalidDataException();
            File.WriteAllText(staged,"second");
            try{Install(staged,target,"changed");throw new Exception("Changed target accepted");}catch(IOException){}
            if(File.ReadAllText(target)!="new"||File.ReadAllText(staged)!="second")throw new InvalidDataException("Failed update changed target");
            try{ValidateFile(staged,Current,null);throw new Exception("Non-PE update accepted");}catch(InvalidDataException){}
        }finally{if(!Path.GetFullPath(temp).StartsWith(Path.GetFullPath(Path.GetTempPath()),StringComparison.OrdinalIgnoreCase))throw new IOException();Directory.Delete(temp,true);}
    }
}
