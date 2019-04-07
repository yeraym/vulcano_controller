# VulcanoFX MC6 Midi Controller for HX Stomp

MIDI controller using Adafruit Feather M0 (arduino compatible), 128x64 oled lcd and
six SPST momentary foot switches. Build to send PC and CC MIDI messages to a Line6 HX Stomp. 
Commands on single foot swtich press, two-foot swtich press and long foot swtich press. 

Build for my personal use, shared to help others.

## Usage

Physical layout

```
------------------------
|  Btn4   Btn5   Btn6  |
|                      |
|           Screen     |
|                      |
|  Btn1   Btn2   Btn3  |
------------------------
```

**Page 1 - Utility controls**

```
 btn  |  single press  |  long press
-------------------------------------
Btn1  |  Snapshot 1    |  Stomp mode
-------------------------------------
Btn2  |  Snapshot 2    |  Scroll mode
-------------------------------------
Btn3  |  Snapshot 3    |  Snapshot mode
-------------------------------------
Btn4  |  FS4           | 
-------------------------------------
Btn5  |  FS5           |
-------------------------------------
Btn6  |  Tap Tempo*    |  Tuner

 *delay must be set in note length, not in ms for this to work
```

Btn1+Btn2: Page Up

Btn2+Btn3: Page Down

**Page 2 - Looper controls**

```
 btn  |  single press  |  long press
-------------------------------------
Btn1  |  Record        |     
-------------------------------------
Btn2  |  Play          |
-------------------------------------
Btn3  |  Stop          | Page Down* 
-------------------------------------
Btn4  |  Over dub      |
-------------------------------------
Btn5  |  Undo/Redo     |
-------------------------------------
Btn6  |  Play once     |

 *use this to avoid firing Record or Play commands
```

Btn1+Btn2:
Page Up

Btn2+Btn3:
Page Down

## Code organization

I'm using Bounce2 library with BOUNCE_WITH_PROMPT_DETECTION option to achieve fast command firing. With button.fell event being the fastest option but it sends two command when using long press and two button commands, to avoid this set command code in button.rose detection of ``readButtons()`` function.

## Construction

More details here: https://vulcanofx.blogspot.com/2019/04/vulcanofx-mc6-midi-controller-for-hx.html

### Wiring

### Components and boxing


