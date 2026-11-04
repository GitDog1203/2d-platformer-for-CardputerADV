# 2D Platformer for Cardputer ADV

I made a 2D platformer game for the M5Stack Cardputer ADV!

I wanted to see if I could make a fully playable platformer on such a small device. I ended up making 10 levels, enemies, blocks, spikes, a level select menu, and a Speedrun Mode!

I built and tested the game on my actual Cardputer ADV. Here's what I made!

## Features

- 10 playable levels
- Enemies
- Blocks and spikes
- Movable player character
- Level select menu
- Speedrun Mode

## Quick Start (Precompiled Firmware)

You don't need Arduino IDE to install the game! You can download the precompiled firmware from the Releases page.

### Prerequisites (Windows)

**1. Install Python**
- Download Python from https://www.python.org/downloads/
- During installation, check **Add Python to PATH**.
- Open Command Prompt and verify:

```bash
python --version
```

**2. Install esptool**

```bash
python -m pip install esptool
python -m esptool version
```

**3. Connect your Cardputer ADV**
- Connect your Cardputer ADV to your computer using USB.
- Find its COM port in Windows Device Manager under Ports (COM & LPT).

### Installing the Game!

1. Go to the [Releases page](https://github.com/GitDog1203/2d-platformer-for-CardputerADV/releases).
2. Download `CardputerPlatformer.merged.bin`.
3. Open Command Prompt.

**4. Check your connection**

Replace `YOUR_COM_PORT` with your Cardputer's actual COM port (for example, COM5).

```bash
python -m esptool --port YOUR_COM_PORT chip-id
```

**5. Flash the firmware**

Replace `PATH_TO_FIRMWARE.bin` with the full path to the downloaded firmware file. Keep the quotation marks.

```bash
python -m esptool --chip esp32s3 --port YOUR_COM_PORT --baud 460800 write-flash --flash-size 8MB 0x0 "PATH_TO_FIRMWARE.bin"
```

6. Wait for flashing to finish. Your Cardputer should restart.
7. Enjoy the game!

**Important:** Flashing replaces the existing firmware on your Cardputer ADV.

## Arduino IDE Setup

If you want to compile the project yourself:

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Go to File → Preferences.
3. Find Additional Boards Manager URLs.
4. Add this URL:

   `https://m5stack.github.io/arduino-index/package_m5stack_index.json`

5. Go to Tools → Board → Boards Manager.
6. Search for M5Stack and install it.
7. Open Library Manager and search for `M5Cardputer`.
8. Install M5Cardputer by M5Stack. Install any additional libraries it requests.
9. Open the project in Arduino IDE and select the M5Cardputer board.
10. Compile and upload!

## Demo

[Watch the project demo](https://youtu.be/bkrS34Pa7zU?si=QN0ffAb8y5FdqVYk)

## Credits

- Hardware: M5Stack Cardputer ADV
- Arduino libraries: M5Stack
- Original project idea and development: Me

Have fun! (=
