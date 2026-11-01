// Hellbound (PC): starts the bundled mGBA (portable, in .\mgba) on the game's ROM (.\game) and exits.
// Built by gba\tools\package-pc.ps1 with the .NET Framework's csc.exe.
using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;

static class Launcher
{
    [STAThread]
    static void Main()
    {
        string root = AppDomain.CurrentDomain.BaseDirectory;
        string emu = Path.Combine(root, "mgba", "mGBA.exe");
        string rom = Path.Combine(root, "game", "sm-hellbound.gba");
        if (!File.Exists(emu) || !File.Exists(rom))
        {
            MessageBox.Show("Some of the game's files are missing. Unzip the whole folder and run Hellbound.exe from it.",
                "Hellbound", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return;
        }
        var start = new ProcessStartInfo(emu, "\"" + rom + "\"");
        start.WorkingDirectory = Path.Combine(root, "mgba");
        start.UseShellExecute = false;
        Process.Start(start);
    }
}
