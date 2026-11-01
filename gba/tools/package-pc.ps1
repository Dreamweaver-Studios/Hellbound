# Packages the game for PC (user request, 2026-10-23): the built ROM plus a portable copy of mGBA and a small launcher,
# as gba\dist\Hellbound (PC)\ and a zip of it next to it.
#   Hellbound.exe               launcher (tools\pc\Launcher.cs, compiled with the .NET Framework's csc.exe)
#   game\sm-hellbound.gba       the ROM (the game saves next to it, as sm-hellbound.sav)
#   mgba\                       mGBA.exe from the installed mGBA, in portable mode (portable.ini: its settings stay in
#                               this folder and never touch the player's own mGBA), with its licence files
#   README.txt                  controls, credits, mGBA's licence and source
# Rebuild the ROM first (gba\build.ps1); this only copies it.
param([string]$Mgba = 'C:\Program Files\mGBA')
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$gba = Split-Path $PSScriptRoot
$rom = Join-Path $gba 'sm-hellbound.gba'
$name = 'Hellbound (PC)'
$dist = Join-Path $gba 'dist'
$out = Join-Path $dist $name
$csc = 'C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe'
foreach ($f in @($rom, "$Mgba\mGBA.exe", $csc)) { if (-not (Test-Path $f)) { throw "Missing $f" } }

if (Test-Path $out) { Remove-Item $out -Recurse -Force }
New-Item -ItemType Directory -Force "$out\game", "$out\mgba" | Out-Null
$utf8 = New-Object Text.UTF8Encoding($false)

# The game
Copy-Item $rom "$out\game\sm-hellbound.gba"

# mGBA: the self-contained Qt build, its licences, portable mode and our settings (a 4x window: mGBA opens at its saved
# width/height, videoScale only applies when picked from its menu; no FPS counter or update check; saves next to the ROM)
Copy-Item "$Mgba\mGBA.exe" "$out\mgba\"
Copy-Item "$Mgba\LICENSE.txt" "$out\mgba\"
Copy-Item "$Mgba\licenses" "$out\mgba\licenses" -Recurse
[IO.File]::WriteAllText("$out\mgba\portable.ini", '', $utf8)
[IO.File]::WriteAllText("$out\mgba\config.ini", @"
[ports.qt]
videoScale=4
width=960
height=640
fullscreen=0
lockAspectRatio=1
lockIntegerScaling=1
showFps=0
showOSD=1
showLibrary=0
updateAutoCheck=0
pauseOnFocusLost=0
pauseOnMinimize=1
muteOnMinimize=1
autoload=1
volume=256
"@.Replace("`n", "`r`n"), $utf8)

# The launcher, with the hero (the user's soldier, "idle (with rifle).png", since 2026-10-29; Mario's sprites\mario1.png
# before) as its icon: square, nearest-neighbour, as a PNG-compressed .ico at 256/48/32/16
$src = [Drawing.Image]::FromFile((Join-Path (Split-Path (Split-Path $gba)) 'Super Mario Run and Gun\Non-Mario Build\new sprites\idle (with rifle).png'))
$sizes = 256, 48, 32, 16
$pngs = foreach ($s in $sizes) {
  $bmp = New-Object Drawing.Bitmap $s, $s
  $g = [Drawing.Graphics]::FromImage($bmp)
  $g.InterpolationMode = 'NearestNeighbor'; $g.PixelOffsetMode = 'Half'
  $side = [Math]::Max($src.Width, $src.Height)
  $k = $s / $side
  $g.DrawImage($src, [int](($side - $src.Width) / 2 * $k), [int](($side - $src.Height) / 2 * $k), [int]($src.Width * $k), [int]($src.Height * $k))
  $g.Dispose()
  $ms = New-Object IO.MemoryStream; $bmp.Save($ms, [Drawing.Imaging.ImageFormat]::Png); $bmp.Dispose()
  , $ms.ToArray()
}
$src.Dispose()
$ico = Join-Path $env:TEMP 'smhb-launcher.ico'
$fs = [IO.File]::Create($ico); $w = New-Object IO.BinaryWriter $fs
$w.Write([uint16]0); $w.Write([uint16]1); $w.Write([uint16]$sizes.Count)
$offset = 6 + 16 * $sizes.Count
for ($i = 0; $i -lt $sizes.Count; $i++) {
  $s = $sizes[$i]
  $w.Write([byte]($s % 256)); $w.Write([byte]($s % 256)); $w.Write([byte]0); $w.Write([byte]0)
  $w.Write([uint16]1); $w.Write([uint16]32); $w.Write([uint32]$pngs[$i].Length); $w.Write([uint32]$offset)
  $offset += $pngs[$i].Length
}
foreach ($p in $pngs) { $w.Write($p) }
$w.Close()
& $csc /nologo /target:winexe /optimize+ "/win32icon:$ico" "/out:$out\Hellbound.exe" /r:System.Windows.Forms.dll "$PSScriptRoot\pc\Launcher.cs"
if ($LASTEXITCODE -ne 0) { throw 'The launcher did not compile' }
Remove-Item $ico

[IO.File]::WriteAllText("$out\README.txt", @"
HELLBOUND (PC)
by Dreamweaver Studios

To play: unzip the whole folder anywhere you can write to (not Program Files), then run
"Hellbound.exe". Your progress and high scores are saved in the game folder.

KEYBOARD
  Arrow keys   move / aim (UP aims up, UP+LEFT/RIGHT diagonally), DOWN crouches
  X            jump (A)
  Z            shoot (B)
  S            toss grenades (R)
  Enter        START: menus, pause
  Backspace    SELECT
  Alt+Enter    full screen on/off

GAMEPADS
  Most controllers work straight away. To change any button, use Tools > Settings > Controllers
  (keyboard: Tools > Settings > Keyboard).

CREDITS
  Dreamweaver Studios
  Sprites: ansimuz, Jesse Munguia, Anokolisa
  Music: Bert Cole, Heatley Bros, Pixel 8 Beats, The Grump Sounds

ABOUT THE EMULATOR
  The game is a Game Boy Advance game. It runs in mGBA (https://mgba.io), included in the mgba folder
  in portable mode, so it keeps its own settings and doesn't change any mGBA you have installed.
  mGBA is (c) Jeffrey Pfau and contributors, distributed under the Mozilla Public License 2.0
  (mgba\LICENSE.txt; the licences of the libraries it uses are in mgba\licenses).
  Its source code is available at https://github.com/mgba-emu/mgba.
"@.Replace("`r`n", "`n").Replace("`n", "`r`n"), $utf8)

# The zip
$zip = Join-Path $dist "$name.zip"
if (Test-Path $zip) { Remove-Item $zip }
Compress-Archive -Path $out -DestinationPath $zip -CompressionLevel Optimal
"Packaged $out"
"Zip: $zip ({0:N1} MB)" -f ((Get-Item $zip).Length / 1MB)
