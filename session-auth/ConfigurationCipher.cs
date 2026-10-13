using System.Security.Cryptography;

internal static class ConfigurationCipher
{
    private static ReadOnlySpan<byte> Header => "WHTCFG02"u8;
    internal static byte[] Encrypt(ReadOnlySpan<byte> clear, ReadOnlySpan<byte> key)
    {
        byte[] payload = new byte[36 + clear.Length];
        Header.CopyTo(payload);
        RandomNumberGenerator.Fill(payload.AsSpan(8, 12));
        using var aes = new AesGcm(key, 16);
        aes.Encrypt(payload.AsSpan(8, 12), clear, payload.AsSpan(36), payload.AsSpan(20, 16), Header);
        return payload;
    }
    internal static byte[] Decrypt(ReadOnlySpan<byte> payload, ReadOnlySpan<byte> key)
    {
        if (key.Length != 32 || payload.Length < 36 || !payload[..8].SequenceEqual(Header)) throw new InvalidDataException();
        byte[] clear = new byte[payload.Length - 36];
        try {
            using var aes = new AesGcm(key, 16);
            aes.Decrypt(payload.Slice(8, 12), payload[36..], payload.Slice(20, 16), clear, Header);
            return clear;
        } catch { CryptographicOperations.ZeroMemory(clear); throw; }
    }
    internal static void SelfTest()
    {
        byte[] key = RandomNumberGenerator.GetBytes(32);
        var payload = Encrypt("configuration-test"u8, key);
        byte[] clear = Decrypt(payload, key);
        if (!clear.AsSpan().SequenceEqual("configuration-test"u8)) throw new InvalidDataException();
        CryptographicOperations.ZeroMemory(clear);
        foreach (int offset in new[] { 8, 20, 36 }) {
            payload[offset] ^= 1;
            try { _ = Decrypt(payload, key); throw new InvalidDataException("Tampering accepted"); }
            catch (AuthenticationTagMismatchException) { }
            payload[offset] ^= 1;
        }
        key[0] ^= 1;
        try { _ = Decrypt(payload, key); throw new InvalidDataException("Wrong key accepted"); }
        catch (AuthenticationTagMismatchException) { }
        CryptographicOperations.ZeroMemory(key);
    }
}
