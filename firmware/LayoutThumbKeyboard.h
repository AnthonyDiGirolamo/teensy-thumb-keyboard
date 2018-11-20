#ifndef LAYOUTTHUMBKEYBOARD_H
#define LAYOUTTHUMBKEYBOARD_H

#include <Arduino.h>
#include "LayoutCommon.h"

#define NUM_ROWS 6
#define NUM_COLS 10

// USB Facing Up
// Row Pins
#define ROW0 4
#define ROW1 5
#define ROW2 7
#define ROW3 8
#define ROW4 6
#define ROW5 9
// Column Pins
#define COL0 0
#define COL1 1
#define COL2 2
#define COL3 3
#define COL4 15
#define COL5 14
#define COL6 17
#define COL7 16
#define COL8 20
#define COL9 21

// // USB Facing Down
// // Row Pins
// #define ROW0 18
// #define ROW1 19
// #define ROW2 21
// #define ROW3 22
// #define ROW4 20
// #define ROW5 23
// // Column Pins
// #define COL0 14
// #define COL1 15
// #define COL2 16
// #define COL3 17
// #define COL4 1
// #define COL5 0
// #define COL6 3
// #define COL7 2
// #define COL8 6
// #define COL9 7

const uint8_t row_pins[] =
  {
   ROW0,
   ROW1,
   ROW2,
   ROW3,
   ROW4,
   ROW5,
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
  };

const char ascii_key_matrix[3][NUM_ROWS][NUM_COLS] =
  {
   // Base Layer
   {
    {ASCII_LEFT,  ASCII_UP,   ASCII_DOWN,  ASCII_RIGHT, '0', '/', '-',      '=', ';',         39}, // 39 = single quote
    {'7',         '8',        '9',         '4',         '5', '6', '1',      '2', '3',         ASCII_ESC},
    {'q',         'w',        'e',         'r',         't', 'y', 'u',      'i', 'o',         'p'},
    {'a',         's',        'd',         'f',         'g', 'h', 'j',      'k', 'l',         ASCII_FN},
    {ASCII_SHIFT, 'z',        'x',         'c',         'v', 'b', 'n',      'm', ASCII_SHIFT, '\b'},
    {'\t',        ASCII_CTRL, ASCII_SUPER, ASCII_ALT,   ' ', ' ', ASCII_FN, ',', '.',         '\n'},
   },
   // Shift Layer
   {
    {0,   0,   0,   0,   ')', '?', '_', '+', ':',  '"'},
    {'&', '*', '(', '$', '%', '^', '!', '@', '#',  0},
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',  0},
    {0,   'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0,    0},
    {0,   0,   0,   0,   0,   0,   0,   '<', '>',  0},
   },
   // Fn Layer
   {
    {ASCII_MOUSE_LEFT,     ASCII_MOUSE_UP, ASCII_MOUSE_DOWN, ASCII_MOUSE_RIGHT,      0, 0,                    ASCII_MOUSE_BTN2, ASCII_MOUSE_BTN3, 0,    96},
    {ASCII_MOUSE_WHEEL_UP, 0,              0,                ASCII_MOUSE_WHEEL_DOWN, 0, ASCII_MOUSE_BTN1,     0,                0,                0,    126},
    {0,                    0,              0,                0,                      0, 0,                    '{',              '}',              '\\', '|'},
    {0,                    0,              0,                0,                      0, '[',                  ']',              '(',              ')',  0},
    {0,                    0,              0,                0,                      0, 0,                    0,                0,                0,    0},
    {0,                    0,              0,                0,                      0, ASCII_FN_LOCK_TOGGLE, 0,                0,                0,    0},
   },
  };


// for all keycodes see:
// ~/apps/arduino-1.8.5/hardware/teensy/avr/cores/teensy3/keylayouts.h
// https://www.pjrc.com/teensy/td_keyboard.html
const uint16_t usb_key_matrix[NUM_ROWS][NUM_COLS] =
  // Base Layer
  {
   {KEY_LEFT,          KEY_UP,           KEY_DOWN,        KEY_RIGHT,       KEY_0,     KEY_SLASH, KEY_MINUS, KEY_EQUAL, KEY_SEMICOLON,     KEY_QUOTE},
   {KEY_7,             KEY_8,            KEY_9,           KEY_4,           KEY_5,     KEY_6,     KEY_1,     KEY_2,     KEY_3,             KEY_ESC},
   {KEY_Q,             KEY_W,            KEY_E,           KEY_R,           KEY_T,     KEY_Y,     KEY_U,     KEY_I,     KEY_O,             KEY_P},
   {KEY_A,             KEY_S,            KEY_D,           KEY_F,           KEY_G,     KEY_H,     KEY_J,     KEY_K,     KEY_L,             ASCII_FN},
   {MODIFIERKEY_SHIFT, KEY_Z,            KEY_X,           KEY_C,           KEY_V,     KEY_B,     KEY_N,     KEY_M,     MODIFIERKEY_SHIFT, KEY_BACKSPACE},
   {KEY_TAB,           MODIFIERKEY_CTRL, MODIFIERKEY_GUI, MODIFIERKEY_ALT, KEY_SPACE, KEY_SPACE, ASCII_FN,  KEY_COMMA, KEY_PERIOD,        KEY_ENTER},
  };

#endif
