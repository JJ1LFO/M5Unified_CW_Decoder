# M5Unified_CW_Decoder

## Description:
 Ported Android CW Decoder made by OZ1JMH to M5Unified (M5Core2).
M5Core2 has a built-in microphone and a 2” LCD, making it perfect for CW decoder by itself.

When the parameters are adjusted for built-in microphne, it works very comfortably.
 

* Supports M5Unified (M5Core2).
* Convert Goertzel algorithm to fixed-point arithmetic operations.
* Vsualize detection magnitude.
* Morse tape printing function.
* Automatic Gain Control for microphone.

* Checked with Arduino IDE 2.3.2, M5Unified 0.1.17 and esp32 3.0.5

* Since it is coded in M5Unified, it should be easy to port to other models in M5Stack.

* Video [![Video Title](https://img.youtube.com/vi/B3Z8Gvv3AQ0/0.jpg)](https://www.youtube.com/watch?v=B3Z8Gvv3AQ0)

This program is a modified version of "OZ1JHM CW Decoder VER 1.01",
originally developed by Hjalmar Skovholm Hansen OZ1JHM.

## ToDo

* Automatic tone frequency tracking function.
* Microphone AGC maximum gain adjustment feature.
* Support for Japanese Morse code
