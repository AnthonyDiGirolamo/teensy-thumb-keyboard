#ifndef LAYOUTAPPLEM0110A_H
#define LAYOUTAPPLEM0110A_H

#include <Arduino.h>
#include "LayoutCommon.h"

#define NUM_ROWS 5
#define NUM_COLS 17

// Apple M0110a modern pcb
// Row Pins
#define ROW0 0
#define ROW1 1
#define ROW2 2
#define ROW3 3
#define ROW4 4
// Column Pins
#define COL0 20
#define COL1 23
#define COL2 22
#define COL3 21
#define COL4 9
#define COL5 10
#define COL6 11
#define COL7 12
#define COL8 15
#define COL9 14
#define COL10 13
#define COL11 16
#define COL12 17
#define COL13 5
#define COL14 6
#define COL15 7
#define COL16 8

const uint8_t row_pins[] =
  {
   ROW0,
   ROW1,
   ROW2,
   ROW3,
   ROW4,
  };

const uint8_t col_pins[] =
  {
   COL0,
   COL1,
   COL2,
   COL3,
   COL4,
   COL5,
   COL6,
   COL7,
   COL8,
   COL9,
   COL10,
   COL11,
   COL12,
   COL13,
   COL14,
   COL15,
   COL16,
  };

const char ascii_key_matrix[3][NUM_ROWS][NUM_COLS] =
  {
   // Base Layer
   {
    //0           1            2          3    4    5    6    7          8            9         10          11    12           13   14   15   16
    {'`',         '1',         '2',       '3', '4', '5', '6', '7',       '8',         '9',      '0',        '-',  '=',         'N', '/', '*', '-'},
    //18          19           20         21   22   23   24   25         26           27        28          29    30           31   32   33   34
    {'\t',        'q',         'w',       'e', 'r', 't', 'y', 'u',       'i',         'o',      'p',        '[',  ']',         '7', '8', '9', '+'},
    //35          36           37         38   39   40   41   42         43           44        45          46    47           48   49   50   51
    {ASCII_CTRL,  'a',         's',       'd', 'f', 'g', 'h', 'j',       'k',         'l',      ';',        '\'', '\n',        '4', '5', '6', '+'},
    //52          53           54         55   56   57   58   59         60           61        62          -     63           64   65   66   67
    {ASCII_SHIFT, 'z',         'x',       'c', 'v', 'b', 'n', 'm',       ',',         '.',      '/',        0,    ASCII_SHIFT, '1', '2', '3', '\n'},
    //68          69           70         -    -    71   -    72         73           74        75          76    77           78   79   80   81
    {ASCII_CTRL,  ASCII_SUPER, ASCII_ALT, 0,   0,   ' ', 0,   ASCII_ALT, ASCII_SUPER, ASCII_FN, ASCII_CTRL, '\\', '\b',        '0', '0', '.', '\n'},
   },
   // Shift Layer
   {
    //0   1    2    3    4    5    6    7    8    9    10   11    12   13 14 15 16
    {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',  '+', 0, 0, 0, 0,},
    //18  19   20   21   22   23   24   25   26   27   28   29    30   31 32 33 34
    {0,   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  '}', 0, 0, 0, 0},
    //35  36   37   38   39   40   41   42   43   44   45   46    47   48 49 50 51
    {0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0,   0, 0, 0, 0},
    //52  53   54   55   56   57   58   59   60   61   62   -     63   64 65 66 67
    {0,   'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,    0,   0, 0, 0, 0},
    //68  69   70   -    -    71   -    72   73   74   75   76    77   78 79 80 81
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0, 0, 0, 0},
   },
   // Fn Layer
   {
    //0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
    //18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //52 53 54 55 56 57 58 59 60 61 62 -  63 64 65 66 67
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //68 69 70 -  -  71 -  72 73 74 75 76 77 78 79 80 81
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   },
  };


// for all keycodes see:
// ~/apps/arduino-1.8.5/hardware/teensy/avr/cores/teensy3/keylayouts.h
// https://www.pjrc.com/teensy/td_keyboard.html
const uint16_t usb_key_matrix[NUM_ROWS][NUM_COLS] =
  // Base Layer
  {
   //0                 1                2                3      4      5          6      7                8                9           10                11              12                 13        14            15              16
   {KEY_ESC,           KEY_1,           KEY_2,           KEY_3, KEY_4, KEY_5,     KEY_6, KEY_7,           KEY_8,           KEY_9,      KEY_0,            KEY_MINUS,      KEY_EQUAL,         KEY_N,    KEYPAD_SLASH, KEYPAD_ASTERIX, KEYPAD_MINUS},
   //18                19               20               21     22     23         24     25               26               27          28                29              30                 31        32            33              34
   {KEY_TAB,           KEY_Q,           KEY_W,           KEY_E, KEY_R, KEY_T,     KEY_Y, KEY_U,           KEY_I,           KEY_O,      KEY_P,            KEY_LEFT_BRACE, KEY_RIGHT_BRACE,   KEYPAD_7, KEYPAD_8,     KEYPAD_9,       KEYPAD_PLUS},
   //35                36               37               38     39     40         41     42               43               44          45                46              47                 48        49            50              51
   {MODIFIERKEY_CTRL,  KEY_A,           KEY_S,           KEY_D, KEY_F, KEY_G,     KEY_H, KEY_J,           KEY_K,           KEY_L,      KEY_SEMICOLON,    KEY_QUOTE,      KEY_ENTER,         KEYPAD_4, KEYPAD_5,     KEYPAD_6,       KEYPAD_PLUS},
   //52                53               54               55     56     57         58     59               60               61          62                -               63                 64        65            66              67
   {MODIFIERKEY_SHIFT, KEY_Z,           KEY_X,           KEY_C, KEY_V, KEY_B,     KEY_N, KEY_M,           KEY_COMMA,       KEY_PERIOD, KEY_SLASH,        0,              MODIFIERKEY_SHIFT, KEYPAD_1, KEYPAD_2,     KEYPAD_3,       KEYPAD_ENTER},
   //68                69               70               -      -      71         -      72               73               74          75                76              77                 78        79            80              81
   {MODIFIERKEY_CTRL,  MODIFIERKEY_GUI, MODIFIERKEY_ALT, 0,     0,     KEY_SPACE, 0,     MODIFIERKEY_ALT, MODIFIERKEY_GUI, ASCII_FN,   MODIFIERKEY_CTRL, KEY_BACKSLASH,  KEY_BACKSPACE,     KEYPAD_0, KEYPAD_0,     KEYPAD_PERIOD,  KEYPAD_ENTER},
  };

#endif
