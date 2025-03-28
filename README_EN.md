# Match Counter for OBS

## Overview

Match Counter for OBS is a plugin for OBS Studio designed to support live streaming of competitive games. This plugin allows you to easily manage and display win/loss counts during your stream.

Key features:

* Count and display wins and losses
* Customizable display formats
* Easy win/loss input via a dedicated control panel
* Quick count operations using hotkeys

## Installation

### Windows

1. Download `match-counter-x.x.x-windows-x64.zip` from the [release page](https://github.com/nimiusrd/match-counter-for-obs/releases).
2. Extract the downloaded file.
3. Place the extracted files in the following locations:
   * `match-counter\bin\64bit\match-counter.dll` → `C:\Program Files\obs-studio\obs-plugins\64bit\match-counter.dll`
   * `match-counter\bin\64bit\match-counter.pdb` → `C:\Program Files\obs-studio\obs-plugins\64bit\match-counter.pdb`
   * `match-counter\data\locale\en-US.ini` → `C:\Program Files\obs-studio\data\obs-plugins\match-counter\locale\en-US.ini`
   * `match-counter\data\locale\ja-JP.ini` → `C:\Program Files\obs-studio\data\obs-plugins\match-counter\locale\ja-JP.ini`
4. Restart OBS Studio.

### macOS

1. Download `match-counter-x.x.x-macos-universal.pkg` from the [release page](https://github.com/nimiusrd/match-counter-for-obs/releases).
2. Run the downloaded file and follow the installer instructions.
3. Restart OBS Studio.

## Usage

### Adding the Match Counter

1. Launch OBS Studio.
2. Click the "+" button in the source list.
3. Select "Match Counter."
4. Enter a name and click "OK."
5. Configure the display format in the settings screen.

### Using the Dedicated Control Panel

1. In OBS Studio, go to "Tools" → "Match Counter" from the menu bar.
2. From the displayed panel, you can:
   * Change the display format
   * Add or subtract wins/losses
   * Reset the counter

### Customizing the Display Format

You can customize the display format using the following variables:
* `%w` - Number of wins
* `%l` - Number of losses
* `%t` - Total matches (wins + losses)
* `%r` - Win rate (percentage, e.g., 75.0%)

Examples:
* `%w Wins %l Losses` → "3 Wins 1 Loss"
* `%w-%l` → "3-1"
* `%w/%l (Win Rate: %r)` → "3/1 (Win Rate: 75.0%)"
* `%t Matches %w Wins` → "4 Matches 1 Win"

## Hotkey Configuration

1. Open "Settings" → "Hotkeys" in OBS Studio.
2. In the "Match Counter" section, you can configure the following hotkeys:
   * Add a win
   * Add a loss
   * Reset the counter

## Build Instructions

### Requirements

* CMake 3.28 or later
* C/C++ compiler (e.g., GCC, Clang, MSVC)
* OBS Studio development files
* Qt6 development files (6.0 or later)

### Build Steps

Clone the repository:
```bash
git clone https://github.com/nimiusrd/match-counter-for-obs.git
```
Run CMake:
```bash
mkdir build && cd build
cmake ..
```
Build:
```bash
cmake --build .
```

## License

This plugin is released under the GPLv2 license. See the LICENSE file for details.
