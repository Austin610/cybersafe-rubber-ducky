# How to Turn a CyberSafe into a Rubber Ducky

The CyberSafe device used for this tutorial reports a Waveshare RP2040-Zero
board. It can present itself to a Windows computer as a USB keyboard. This
project uses that ability to open a blank Notepad tab, type an
original poem inspired by the atmosphere of The Weeknd's **“The Abyss”**, and
ask the installed Spotify desktop app to play [the track](https://open.spotify.com/track/5ZDKPFRZC6QlJpf8bCIXTs).
It is a Rubber Ducky-style keyboard demo, not a genuine Hak5 USB Rubber Ducky
or a DuckyScript interpreter.

The demo was built and tested on a **Waveshare RP2040-Zero (RP2040, 2 MB
flash)** with **Windows 11**, **Pico SDK 2.3.0**, and the Spotify desktop app. It
runs once each time the board powers up. No soldering or extra components are
needed.

> Plug the board only into a computer you control. It sends real keyboard
> input to whichever window has focus. Close work you do not want affected
> before testing. This firmware replaces the running Pico FIDO security-key
> firmware until you flash Pico FIDO again.

## What's in this folder

| File | Purpose |
| --- | --- |
| [`firmware/pico_keyboard_demo.uf2`](firmware/pico_keyboard_demo.uf2) | Ready-to-flash image for the Waveshare RP2040-Zero. |
| [`main.c`](main.c) | Keyboard sequence, poem, key mapping, and timing. |
| [`usb_descriptors.c`](usb_descriptors.c) | USB device and HID keyboard descriptors. |
| [`tusb_config.h`](tusb_config.h) | TinyUSB device configuration. |
| [`CMakeLists.txt`](CMakeLists.txt) | Standalone Pico SDK build target. |
| [`pico_sdk_import.cmake`](pico_sdk_import.cmake) | Raspberry Pi's SDK import helper; its BSD license is in the file. |
| [`LICENSE`](LICENSE) | AGPL-3.0 license used by the parent Pico FIDO repository. |

Building needs the Pico SDK and an ARM compiler; flashing the supplied UF2
does not.

## Fast path: flash the supplied UF2

1. On a Windows PC, install and sign in to the [Spotify desktop app](https://www.spotify.com/download/). Select an **English (US)** keyboard layout. The demo uses US key positions.
2. Download this folder from GitHub and locate `firmware/pico_keyboard_demo.uf2`.
3. Unplug the RP2040-Zero. Hold its **BOOT** button while reconnecting its USB cable, then release BOOT. Windows should show a removable drive named `RPI-RP2`. The [Waveshare board guide](https://www.waveshare.com/wiki/RP2040-Zero) describes the same boot mode.
4. Copy `pico_keyboard_demo.uf2` to `RPI-RP2`. The drive disappears when the board reboots. That is expected for a successful UF2 transfer.
5. Wait while the board opens Notepad, types the poem, and opens Spotify. Do not type or switch windows until it finishes.

The prebuilt UF2 is for **RP2040-Zero**. For another RP2040 board, build from
source with its Pico SDK board name instead of flashing this image.

### Expected result

Notepad gets a **new unsaved tab** containing:

```text
on the edge

at the edge of the sleeping city
the streetlights shiver in the rain
i hear your name beneath the traffic
a distant echo in my veins

the night unfolds a black horizon
and every road runs out of ground
i reach for you across the silence
but only hear the river sound

below the bridge the water mirrors
a thousand windows burning blue
each one a life that keeps on moving
while mine is standing still with you

if we are falling through the darkness
let one small light refuse to fade
i leave it shining on the water
for all the words we never said
```

Spotify should show **“The Abyss (feat. Lana Del Rey)”** as the playing track,
including when a different song was already playing. The poem is original; it
does not contain song lyrics. The exact Spotify behavior can change with app
updates, account state, or focus.

## Build from source

Install Git, CMake, Ninja, Python, an ARM `arm-none-eabi` GCC toolchain, and the
[Raspberry Pi Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).
The official Raspberry Pi Pico VS Code extension can install the SDK and
toolchain on Windows. SDK **2.3.0** and GCC **14.2.1** were used for the tested
build. If you install the SDK manually, initialize its submodules.

In PowerShell, open this folder and set paths to **your** SDK and compiler:

```powershell
cd path\to\keyboard_demo
$env:PICO_SDK_PATH = 'C:\path\to\pico-sdk'
$env:PICO_TOOLCHAIN_PATH = 'C:\path\to\arm-none-eabi\bin'
cmake -S . -B build -G Ninja -DPICO_BOARD=waveshare_rp2040_zero
cmake --build build --parallel
```

The resulting file is `build/pico_keyboard_demo.uf2`. Flash it using the same
BOOT and `RPI-RP2` steps above. If `arm-none-eabi-gcc` is already on your
`PATH`, the `PICO_TOOLCHAIN_PATH` setting may be unnecessary. Set
`PICO_BOARD` to the proper board name if you are building for another RP2040
board. On a fresh SDK setup, CMake may also download and build `picotool`
before producing the UF2, so the first build can take longer and may need an
internet connection.

### Verify the downloaded image

The included UF2's SHA-256 is:

```text
1258d2b65138732b2c72f50b410ff2a1b1bc570f1160b8035b67d7915697205c
```

On Windows, compare it with:

```powershell
(Get-FileHash .\firmware\pico_keyboard_demo.uf2 -Algorithm SHA256).Hash.ToLower()
```

A source rebuild may have a different hash because of compiler and SDK
versions. The tested UF2 writes only to the beginning of the RP2040's flash,
through address `0x10004800`; this does not guarantee that data from another
firmware remains usable.

## How it works

1. TinyUSB enumerates one **HID keyboard** interface. Windows treats its
   reports like physical key presses. [TinyUSB's HID guide](https://docs.tinyusb.org/en/latest/reference/class/hid.html) explains the descriptor and report functions used here.
2. `send_key()` sends a press report and a release report. It waits for the
   HID endpoint to be ready, then gives each report a 10 ms settling interval.
   The USB endpoint polls every 5 ms. This was fast enough to type the full
   poem accurately on the tested PC.
3. `type_text()` converts supported ASCII characters into HID key codes.
   Supported input is `a-z`, `A-Z`, `0-9`, space, colon, and newline. Other
   characters currently stop the sequence.
4. The board presses **Win+R**, types `notepad`, and presses Enter. After a
   startup pause, it presses **Ctrl+N** so the poem lands in a new blank tab.
5. It opens `spotify:track:5ZDKPFRZC6QlJpf8bCIXTs` through Windows' Run
   dialog. Spotify's [URI documentation](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) explains the track identifier.
6. A link may navigate to a song without replacing a track already playing.
   The demo waits, sends Space to pause playback, and opens the URI again. On
   the tested PC, that started “The Abyss” from a different playing track.

This is a **one-way keyboard device**. It cannot ask Windows whether Spotify
is installed or confirm which app has focus. The Spotify step works here
because the Windows `spotify:` handler is installed and the app is signed in.
A true conditional check would need a companion program on the PC and a
two-way USB connection.

## Customize it

- Change the poem string inside `type_text(...)` in [`main.c`](main.c). Use
  `\n` for line breaks and keep to the supported characters listed above.
- To use another song, copy its Spotify URI from the Spotify desktop app and
  replace **both** occurrences of `spotify:track:5ZDKPFRZC6QlJpf8bCIXTs`.
- Adjust `wait_ms_with_usb(...)` values if Notepad or Spotify starts more
  slowly on your PC. If letters go missing, increase the 10 ms waits in
  `send_key()`; if everything is reliable, shorten them carefully.
- Rebuild and flash the new UF2 after every source change.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| No `RPI-RP2` drive | Use a data-capable USB cable. Hold BOOT while connecting; the Waveshare board also has a RESET button. |
| Build cannot find the SDK | Point `PICO_SDK_PATH` to the SDK directory containing `pico_sdk_init.cmake`; make sure its submodules are initialized. |
| Build cannot find the compiler | Install `arm-none-eabi` GCC and set `PICO_TOOLCHAIN_PATH` to its `bin` directory. |
| Wrong letters or symbols | Switch Windows to an English (US) keyboard layout before connecting the Pico. |
| Poem goes into another document | Leave the desktop unlocked and let Notepad open. The sequence depends on focus and startup timing; increase the Notepad wait in `main.c` if needed. |
| Missing letters | Increase the press and release settling waits in `send_key()`. |
| Spotify opens but does not play the target | Check that the desktop app is installed, signed in, and foregrounded. Spotify URI navigation and Space-bar playback depend on the app version and focus. |
| Sequence runs again unexpectedly | Every power cycle runs it once. Unplug the board or flash another firmware to stop it. |

## Restore Pico FIDO

Hold BOOT while reconnecting the RP2040-Zero, then copy the correct Pico FIDO
UF2 for that board to `RPI-RP2`. Use the [Pico FIDO release page](https://github.com/polhenarejos/pico-fido/releases) or your own known-good build. The board then reboots as a security key.

Back up credentials you need before switching firmware. The UF2 in this
folder does not deliberately erase the FIDO storage area, but another
firmware's data should never be assumed recoverable without a backup.

