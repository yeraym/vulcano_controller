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

Btn1: 
Single press -> Snapshot 1
Long press -> Stomp mode
      
Btn2: 
Single press -> Snapshot 2
Long press -> Scroll mode

Btn3: 
Single press -> Snapshot 3
Long press -> Snapshot mode

Btn4: 
Single press -> FS4

Btn5: 
Single press -> FS5

Btn6: 
Single press -> Tap Tempo (delay must be set in note length, not in ms for this to work)
Long press -> Tuner

Btn1+Btn2:
Page Up

Btn2+Btn3:
Page Down

**Page 2 - Looper controls**

Btn1: 
Single press -> Record
      
Btn2: 
Single press -> Play

Btn3: 
Single press -> Stop
Long press -> Page Down (use this to avoid firing Record or Play commands)

Btn4: 
Single press -> Over dub

Btn5: 
Single press -> Undo/Redo

Btn6: 
Single press -> Play once

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


