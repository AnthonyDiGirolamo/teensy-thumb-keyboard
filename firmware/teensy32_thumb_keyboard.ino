#include "KeyboardMatrix.h"

// Uncomment to show matrix debug messages over serial
// #define DEBUG


// --- LAYOUT OPTIONS ----------------------------------------------------------

// For TeensyThumbKeyboard:
#include "LayoutThumbKeyboard.h"
KeyboardMatrix key_matrix = KeyboardMatrix(NUM_ROWS, NUM_COLS,
                                           (uint8_t*) row_pins,
                                           (uint8_t*) col_pins,
                                           DIODE_DIRECTION_ROW_PIN_TO_COL_PIN);

// For AppleM0110a:
// #include "LayoutAppleM0110a.h"
// KeyboardMatrix key_matrix = KeyboardMatrix(NUM_ROWS, NUM_COLS,
//                                            (uint8_t*) row_pins,
//                                            (uint8_t*) col_pins,
//                                            DIODE_DIRECTION_COL_PIN_TO_ROW_PIN);

// --- ADDITIONAL FEATURES ----------------------------------------------------------

// Optional USB support using the Teensy Keyboard and Mouse classes
#define USE_TEENSY_USB_KEYBOARD

// Allow a single shift or fn key-press to apply the modifier to the next key press
#define ENABLE_ONESHOT_SHIFT_FN

// Auto-repeat Keypresses in firmware
#define ENABLE_AUTOREPEAT

// Initial delay before repeating a keypress
//   Units are in Microseconds: 1 millisecond = 1000 microseconds
#define HOLD_INTERVAL 300000

// Repeat interval after initial delay
#define REPEAT_INTERVAL 200000

// --- Code --------------------------------------------------------------------

// Represents the current keyboard state between updates
class KeyboardState {
public:
#ifdef ENABLE_ONESHOT_SHIFT_FN
  bool oneshot_shift = false;
  bool oneshot_fn = false;
  bool oneshot_ctrl = false;
  bool oneshot_super = false;
  bool oneshot_alt = false;
#endif

  uint8_t current_layer = 0;
  bool fn_lock = false;
  bool modifier_shift_held = false;
  bool modifier_fn_held = false;
  bool modifier_ctrl_held = false;
  bool modifier_alt_held = false;
  bool modifier_super_held = false;

#ifdef USE_TEENSY_USB_KEYBOARD
  // Mouse keystate variables
  uint8_t mouse_btn1_held = 0;
  uint8_t mouse_btn2_held = 0;
  uint8_t mouse_btn3_held = 0;
  uint8_t mousekey_repeat =  0;
  uint8_t mousekey_accel = 0;
  uint32_t mousekey_last_timer = 0;
#endif

};

KeyboardState keyboard_state = KeyboardState();


// --- Mouse key constants and functions ---------------------------------------
#ifdef USE_TEENSY_USB_KEYBOARD

// User configurable mouse speed values
#define MOUSEKEY_DELAY 100
#define MOUSEKEY_INTERVAL 1
#define MOUSEKEY_MAX_SPEED 7
#define MOUSEKEY_TIME_TO_MAX 255
#define MOUSEKEY_WHEEL_MAX_SPEED 7
#define MOUSEKEY_WHEEL_TIME_TO_MAX 255

#define MOUSEKEY_MOVE_MAX       127
#define MOUSEKEY_WHEEL_MAX      127
#define MOUSEKEY_MOVE_DELTA     5
#define MOUSEKEY_WHEEL_DELTA    1

// Mouse Key implementation from the tmk_keyboard firmware:
// https://github.com/tmk/tmk_keyboard/blob/master/tmk_core/common/mousekey.c

/*
 * Mouse keys acceleration algorithm
 *  http://en.wikipedia.org/wiki/Mouse_keys
 *
 *  speed = delta * max_speed * (repeat / time_to_max)**((1000+curve)/1000)
 */
/* milliseconds between the initial key press and first repeated motion event (0-2550) */
const uint8_t mk_delay = MOUSEKEY_DELAY/10;
/* milliseconds between repeated motion events (0-255) */
const uint8_t mk_interval = MOUSEKEY_INTERVAL;
/* steady speed (in action_delta units) applied each event (0-255) */
const uint8_t mk_max_speed = MOUSEKEY_MAX_SPEED;
/* number of events (count) accelerating to steady speed (0-255) */
const uint8_t mk_time_to_max = MOUSEKEY_TIME_TO_MAX;
/* ramp used to reach maximum pointer speed (NOT SUPPORTED) */
//int8_t mk_curve = 0;
/* wheel params */
const uint8_t mk_wheel_max_speed = MOUSEKEY_WHEEL_MAX_SPEED;
const uint8_t mk_wheel_time_to_max = MOUSEKEY_WHEEL_TIME_TO_MAX;

uint8_t mouse_move_unit(void) {
  uint16_t unit;
  if (keyboard_state.mousekey_accel & (1<<0)) {
    unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed)/4;
  } else if (keyboard_state.mousekey_accel & (1<<1)) {
    unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed)/2;
  } else if (keyboard_state.mousekey_accel & (1<<2)) {
    unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed);
  } else if (keyboard_state.mousekey_repeat == 0) {
    unit = MOUSEKEY_MOVE_DELTA;
  } else if (keyboard_state.mousekey_repeat >= mk_time_to_max) {
    unit = MOUSEKEY_MOVE_DELTA * mk_max_speed;
  } else {
    unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed * keyboard_state.mousekey_repeat) / mk_time_to_max;
  }
  return (unit > MOUSEKEY_MOVE_MAX ? MOUSEKEY_MOVE_MAX : (unit == 0 ? 1 : unit));
}

uint8_t mouse_wheel_unit(void) {
  uint16_t unit;
  if (keyboard_state.mousekey_accel & (1<<0)) {
    unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed)/4;
  } else if (keyboard_state.mousekey_accel & (1<<1)) {
    unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed)/2;
  } else if (keyboard_state.mousekey_accel & (1<<2)) {
    unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed);
  } else if (keyboard_state.mousekey_repeat == 0) {
    unit = MOUSEKEY_WHEEL_DELTA;
  } else if (keyboard_state.mousekey_repeat >= mk_wheel_time_to_max) {
    unit = MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed;
  } else {
    unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed * keyboard_state.mousekey_repeat) / mk_wheel_time_to_max;
  }
  return (unit > MOUSEKEY_WHEEL_MAX ? MOUSEKEY_WHEEL_MAX : (unit == 0 ? 1 : unit));
}
#endif
// end mouse key constants and functions


// Printable character test string
char test_string[64];
uint8_t test_string_index = 0;

// Allow printing (eg with Serial) using the stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

int brightness = 200;

static const uint16_t gamma_table_2_5[] = {
        /* gamma = 2.5 */
        0,      0,      0,      1,      2,      4,      6,      8,
        11,     15,     20,     25,     31,     38,     46,     55,
        65,     75,     87,     99,     113,    128,    143,    160,
        178,    197,    218,    239,    262,    286,    311,    338,
        366,    395,    425,    457,    491,    526,    562,    599,
        639,    679,    722,    765,    811,    857,    906,    956,
        1007,   1061,   1116,   1172,   1231,   1291,   1352,   1416,
        1481,   1548,   1617,   1688,   1760,   1834,   1910,   1988,
        2068,   2150,   2233,   2319,   2407,   2496,   2587,   2681,
        2776,   2874,   2973,   3075,   3178,   3284,   3391,   3501,
        3613,   3727,   3843,   3961,   4082,   4204,   4329,   4456,
        4585,   4716,   4850,   4986,   5124,   5264,   5407,   5552,
        5699,   5849,   6001,   6155,   6311,   6470,   6632,   6795,
        6962,   7130,   7301,   7475,   7650,   7829,   8009,   8193,
        8379,   8567,   8758,   8951,   9147,   9345,   9546,   9750,
        9956,   10165,  10376,  10590,  10806,  11025,  11247,  11472,
        11699,  11929,  12161,  12397,  12634,  12875,  13119,  13365,
        13614,  13865,  14120,  14377,  14637,  14899,  15165,  15433,
        15705,  15979,  16256,  16535,  16818,  17104,  17392,  17683,
        17978,  18275,  18575,  18878,  19184,  19493,  19805,  20119,
        20437,  20758,  21082,  21409,  21739,  22072,  22407,  22746,
        23089,  23434,  23782,  24133,  24487,  24845,  25206,  25569,
        25936,  26306,  26679,  27055,  27435,  27818,  28203,  28592,
        28985,  29380,  29779,  30181,  30586,  30994,  31406,  31820,
        32239,  32660,  33085,  33513,  33944,  34379,  34817,  35258,
        35702,  36150,  36602,  37056,  37514,  37976,  38441,  38909,
        39380,  39856,  40334,  40816,  41301,  41790,  42282,  42778,
        43277,  43780,  44286,  44795,  45308,  45825,  46345,  46869,
        47396,  47927,  48461,  48999,  49540,  50085,  50634,  51186,
        51742,  52301,  52864,  53431,  54001,  54575,  55153,  55734,
        56318,  56907,  57499,  58095,  58695,  59298,  59905,  60515,
        61130,  61748,  62370,  62995,  63624,  64258,  64894,  65535,
};

void set_brightness(int b) {
  analogWrite(23, gamma_table_2_5[b]);
}

uint32_t batt_read_millis = millis();

void setup() {
  Serial.begin(115200);
  // delay(2000);
#ifdef DEBUG
  Serial.println("key_matrix.begin();");
#endif

  analogWriteResolution(16);
  set_brightness(brightness);

  key_matrix.begin();

  for (uint8_t i=0; i<63; i++) {
    test_string[i] = ' ';
  }
  test_string[63] = '\0';
#ifdef DEBUG
  Serial.println("Finished setup();");
#endif
}


void print_16_bits_reversed(uint16_t n) {
  char s[17];
  sprintf(s,
          "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
          (n & 0x01 ? 1 : 0),
          (n & 0x02 ? 1 : 0),
          (n & 0x04 ? 1 : 0),
          (n & 0x08 ? 1 : 0),
          (n & 0x010 ? 1 : 0),
          (n & 0x020 ? 1 : 0),
          (n & 0x040 ? 1 : 0),
          (n & 0x080 ? 1 : 0),
          (n & 0x0100 ? 1 : 0),
          (n & 0x0200 ? 1 : 0),
          (n & 0x0400 ? 1 : 0),
          (n & 0x0800 ? 1 : 0),
          (n & 0x01000 ? 1 : 0),
          (n & 0x02000 ? 1 : 0),
          (n & 0x04000 ? 1 : 0),
          (n & 0x08000 ? 1 : 0)
          );
  Serial.println(s);
}

bool printable_character(char ascii_key) {
  return ascii_key >= 32 && ascii_key <= 126;
}

void press_backspace() {
  test_string_index = (test_string_index + 62) % 63;
  test_string[test_string_index] = ' ';
}

void press_printable_character(uint8_t ascii_key) {
  // TODO: handle modifiers other than shift/fn. eg Ctrl-C
  test_string[test_string_index] = ascii_key;
  test_string_index = (test_string_index + 1) % 63;
}

void keyboard_update() {
  // reset these on each scan
  bool matrix_changed = false;
  char ascii_key;

  PressedKey *pkey;
  ReleasedKey *rkey;

  // assume modifiers not held
  keyboard_state.modifier_shift_held = false;
  keyboard_state.modifier_fn_held = false;
  keyboard_state.modifier_ctrl_held = false;
  keyboard_state.modifier_alt_held = false;
  keyboard_state.modifier_super_held = false;

  // Scan the key matrix
  matrix_changed = key_matrix.update();

  // check for held modifiers
  for (uint8_t i=0; i<key_matrix.pressed_list.size(); i++) {
    pkey = key_matrix.pressed_list.get(i);

    // button_held requires that the button was initially pressed on the last
    // keyboard scan technically not neccessary here
    if (key_matrix.button_held(pkey->row, pkey->col)) {
      // get layer 0 key
      ascii_key = ascii_key_matrix[0][pkey->row][pkey->col];
      if (ascii_key == ASCII_CTRL)
        keyboard_state.modifier_ctrl_held = true;
      if (ascii_key == ASCII_ALT)
        keyboard_state.modifier_alt_held = true;
      if (ascii_key == ASCII_SUPER)
        keyboard_state.modifier_super_held = true;
      if (ascii_key == ASCII_SHIFT)
        keyboard_state.modifier_shift_held = true;
      if (ascii_key == ASCII_FN)
        keyboard_state.modifier_fn_held = true;
    }
  }  // end check modifiers

  // if matrix changed
  // There should only be one new key press or release per matrix update
  if (matrix_changed) {
    // Print key matrix for debugging
#ifdef DEBUG
    Serial << "\n\n\n";
    // microseconds since last matrix scan
    Serial << "delta_micros: " << key_matrix.delta_micros << '\n';
    for (uint8_t r=0; r<key_matrix.num_rows; r++) {
      Serial << "ROW" << r << ": ";
      print_16_bits_reversed(key_matrix.matrix_state[r]);
    }
#endif

    // Assume layer 0
    keyboard_state.current_layer = 0;

    // change layers for held modifiers (only shift and fn)
    if (keyboard_state.modifier_shift_held)
      keyboard_state.current_layer = 1;
    else if (keyboard_state.modifier_fn_held || keyboard_state.fn_lock)
      keyboard_state.current_layer = 2;

    // process pressed keys
    for (uint8_t i=0; i<key_matrix.pressed_list.size(); i++) {
      pkey = key_matrix.pressed_list.get(i);

#ifdef DEBUG
      Serial << "pressed key: " << pkey->row << ", " << pkey->col << ", " << pkey->hold_time << '\n';
#endif

      // if button just pressed (not being held)
      // if pkey->hold_time > 0 then this is the second or more time the key was seen
      if (pkey->hold_time == 0) {

        ascii_key = ascii_key_matrix[0][pkey->row][pkey->col];

#ifdef ENABLE_ONESHOT_SHIFT_FN
        if (!keyboard_state.fn_lock) {
          // enable or disable oneshot
          if (ascii_key == ASCII_SHIFT) {
            if (keyboard_state.oneshot_shift) {
              // Serial.println("keyboard_state.oneshot_shift = false");
              keyboard_state.oneshot_shift = false;
            }
            // disable onshot if modifier is pressed a second time
            else {
              // Serial.println("keyboard_state.oneshot_shift = true");
              keyboard_state.oneshot_shift = true;
            }
          }
          else if (ascii_key == ASCII_FN) {
            if (keyboard_state.oneshot_fn) {
              // Serial.println("keyboard_state.oneshot_fn = false");
              keyboard_state.oneshot_fn = false;
            }
            // disable onshot if modifier is pressed a second time
            else {
              // Serial.println("keyboard_state.oneshot_fn = true");
              keyboard_state.oneshot_fn = true;
            }
          }

          // change layers for oneshot modes
          if (keyboard_state.oneshot_shift)
            keyboard_state.current_layer = 1;
          else if (keyboard_state.oneshot_fn)
            keyboard_state.current_layer = 2;
        }
#endif

        ascii_key = ascii_key_matrix[keyboard_state.current_layer][pkey->row][pkey->col];

        // Use base layer if key is undefined
        if (ascii_key == 0)
          ascii_key = ascii_key_matrix[0][pkey->row][pkey->col];

#ifdef USE_TEENSY_USB_KEYBOARD
        if (keyboard_state.current_layer == 2) {
          if (printable_character(ascii_key)) {
            Keyboard.print(ascii_key);
          }
          else if (ascii_key == ASCII_MOUSE_BTN1)
            Mouse.click();
          else if (ascii_key == ASCII_MOUSE_BTN2)
            Mouse.click(MOUSE_RIGHT);
          else if (ascii_key == ASCII_MOUSE_BTN3)
            Mouse.click(MOUSE_MIDDLE);
          // TODO: else other keys like mouse buttons?
        }
        else
          Keyboard.press(usb_key_matrix[pkey->row][pkey->col]);
#endif

#ifdef ENABLE_ONESHOT_SHIFT_FN
        // if oneshot is active, and this is not the modifier key, oneshot is used up so set to false
        if (ascii_key != ASCII_SHIFT && keyboard_state.oneshot_shift) {
          // Serial.println("clear keyboard_state.oneshot_shift = false");
          keyboard_state.oneshot_shift = false;
        }
        else if (ascii_key != ASCII_FN && keyboard_state.oneshot_fn) {
          // Serial.println("clear keyboard_state.oneshot_fn = false");
          keyboard_state.oneshot_fn = false;
        }
#endif

        // if backspace
        if (ascii_key == '\b') {
          press_backspace();
        }
        // if printable character
        else if (printable_character(ascii_key)) {
          press_printable_character(ascii_key);
        }
        else if (ascii_key == ASCII_FN_LOCK_TOGGLE)
          keyboard_state.fn_lock = !keyboard_state.fn_lock;

        // TODO: handle more keys
        // else if (ascii_key == ASCII_CTRL) {
        // }
        // else if (ascii_key == ASCII_ALT) {
        // }
        // else if (ascii_key == ASCII_SUPER) {
        // }
        // else if (ascii_key == ASCII_ESC) {
        // }
        // else if (ascii_key == ASCII_UP) {
        // }
        // else if (ascii_key == ASCII_DOWN) {
        // }
        // else if (ascii_key == ASCII_LEFT) {
        // }
        // else if (ascii_key == ASCII_RIGHT) {
        // }
      }  // end if just pressed

    }  // end process pressed keys


    // process released keys
    for (uint8_t i=0; i<key_matrix.released_list.size(); i++) {
      rkey = key_matrix.released_list.get(i);

      // ascii_key = ascii_key_matrix[0][rkey->row][rkey->col];
#ifdef DEBUG
      Serial << "released key: " << rkey->row << ", " << rkey->col << ", " << '\n';
#endif

#ifdef USE_TEENSY_USB_KEYBOARD
      Keyboard.release(usb_key_matrix[rkey->row][rkey->col]);
#endif
    }

    // print test string
#ifdef DEBUG
    Serial.println(test_string);
#endif

  }  // end if matrix updated


#ifdef ENABLE_AUTOREPEAT
  // Auto-repeat the last key held
  if (key_matrix.pressed_list.size() > 0) {
    // get the last key press
    pkey = key_matrix.pressed_list.last_item();

    // check that it's been held long enough
    if (pkey->hold_time > HOLD_INTERVAL) {
      // get keycode value
      ascii_key = ascii_key_matrix[keyboard_state.current_layer][pkey->row][pkey->col];

      if (printable_character(ascii_key)) {
        press_printable_character(ascii_key);
#ifdef DEBUG
        Serial.println(test_string);
#endif
      }
      else if (ascii_key == '\b') {
        press_backspace();
#ifdef DEBUG
        Serial.println(test_string);
#endif
      }
      pkey->hold_time = HOLD_INTERVAL - REPEAT_INTERVAL;
    }
  }
#endif

#ifdef USE_TEENSY_USB_KEYBOARD
  // Mouse key update

  // assume no buttons are held
  keyboard_state.mouse_btn1_held = 0;
  keyboard_state.mouse_btn2_held = 0;
  keyboard_state.mouse_btn3_held = 0;

  bool mouse_left = false;
  bool mouse_up = false;
  bool mouse_down = false;
  bool mouse_right = false;
  bool mouse_wheel_left = false;
  bool mouse_wheel_up = false;
  bool mouse_wheel_down = false;
  bool mouse_wheel_right = false;

  if (millis() - keyboard_state.mousekey_last_timer >
      (keyboard_state.mousekey_repeat ? mk_interval : mk_delay*10)) {

    for (uint8_t i=0; i<key_matrix.pressed_list.size(); i++) {
      pkey = key_matrix.pressed_list.get(i);
      ascii_key = ascii_key_matrix[keyboard_state.current_layer][pkey->row][pkey->col];

      // check for a mousekey press
      if (ascii_key == ASCII_MOUSE_BTN1)
        keyboard_state.mouse_btn1_held = 1;
      else if (ascii_key == ASCII_MOUSE_BTN2)
        keyboard_state.mouse_btn2_held = 1;
      else if (ascii_key == ASCII_MOUSE_BTN3)
        keyboard_state.mouse_btn3_held = 1;

      else if (ascii_key == ASCII_MOUSE_LEFT)
        mouse_left = true;
      else if (ascii_key == ASCII_MOUSE_UP)
        mouse_up = true;
      else if (ascii_key == ASCII_MOUSE_DOWN)
        mouse_down = true;
      else if (ascii_key == ASCII_MOUSE_RIGHT)
        mouse_right = true;

      else if (ascii_key == ASCII_MOUSE_WHEEL_LEFT)
        mouse_wheel_left = true;
      else if (ascii_key == ASCII_MOUSE_WHEEL_UP)
        mouse_wheel_up = true;
      else if (ascii_key == ASCII_MOUSE_WHEEL_DOWN)
        mouse_wheel_down = true;
      else if (ascii_key == ASCII_MOUSE_WHEEL_RIGHT)
        mouse_wheel_right = true;
    }

    // is there a mousekey (move command)
    if (mouse_left || mouse_up || mouse_down || mouse_right ||
        mouse_wheel_left || mouse_wheel_up || mouse_wheel_down || mouse_wheel_right) {
      if (keyboard_state.mousekey_repeat != UINT8_MAX)
        keyboard_state.mousekey_repeat++;

      Mouse.set_buttons(keyboard_state.mouse_btn1_held, keyboard_state.mouse_btn2_held, keyboard_state.mouse_btn3_held);

      int8_t x = 0, y = 0;

      if (mouse_left)
        x = -mouse_move_unit();
      else if (mouse_right)
        x = mouse_move_unit();

      if (mouse_up)
        y = -mouse_move_unit();
      else if (mouse_down)
        y = mouse_move_unit();

      // slow down diagonal moves
      if (x != 0 && y != 0) {
        x *= 0.7;
        y *= 0.7;
      }

      int8_t wheelx = 0, wheely = 0;

      if (mouse_wheel_up)
        wheelx = -mouse_wheel_unit();
      else if (mouse_wheel_down)
        wheelx = mouse_wheel_unit();

      if (mouse_wheel_left)
        wheely = -mouse_wheel_unit();
      else if (mouse_wheel_right)
        wheely = mouse_wheel_unit();

#ifdef DEBUG
      Serial << "[Mouse] repeat: " << keyboard_state.mousekey_repeat << " accel: " << keyboard_state.mousekey_accel << " move: (" << x << ", " << y << ")\n";
#endif
      Mouse.move(x, y, wheelx, wheely);
    }
    else {
      // release any held buttons
      Mouse.set_buttons(0, 0, 0);
      // not moving reset repeat and accel
      keyboard_state.mousekey_repeat = 0;
      keyboard_state.mousekey_accel = 0;
    }

    keyboard_state.mousekey_last_timer = millis();
  }


#endif

}


void loop() {
  // Check Battery pin every 3seconds
  if (millis() > (batt_read_millis + 3000)) {
    float batt = 3.3 * ((float) analogRead(22) / 1024.0);
    Serial << "Batt:" << batt << "\n";
    batt_read_millis = millis();
  }

  // Run the keyboard update routine
  keyboard_update();

  // Check for brightness up / down keys
  PressedKey *pkey;
  uint8_t ak;

  // Is the Fn modifier key held?
  if (keyboard_state.modifier_fn_held == true) {

    // for each pressed key
    for (uint8_t i=0; i<key_matrix.pressed_list.size(); i++) {
      // get the key
      pkey = key_matrix.pressed_list.get(i);

      // If key just pressed then hold_time == 0
      // (if pkey->hold_time > 0 then this is the second or higher time the key was seen)
      if (pkey->hold_time == 0) {
        ak = ascii_key_matrix[keyboard_state.current_layer][pkey->row][pkey->col];
        // Use base layer if key is undefined
        if (ak == 0)
          ak = ascii_key_matrix[0][pkey->row][pkey->col];

        // Fn + < (comma key)
        if (ak == '.') {
          brightness += 5;
          if (brightness > 255)
            brightness = 255;
          set_brightness(brightness);
        }
        // Fn + > (period key)
        else if (ak == ',') {
          brightness -= 5;
          if (brightness < 5)
            brightness = 5;
          set_brightness(brightness);
        }
      }
    }
  }
}
