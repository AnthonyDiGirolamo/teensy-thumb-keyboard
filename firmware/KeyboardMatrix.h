#ifndef KEYBOARDMATRIX_H
#define KEYBOARDMATRIX_H

#include <Arduino.h>
#include "LinkedList.h"

// Diode Directions

#define DIODE_DIRECTION_ROW_PIN_TO_COL_PIN 1
// Row to Column: switch state is read from the row pins
//   Switch state is read from the row pins one column at a time
//   Row pins are set to input mode with pullups turned on

#define DIODE_DIRECTION_COL_PIN_TO_ROW_PIN 2
// Column to Row:
//   Switch state is read from the column pins one row at a time
//   Column pins are set to input mode with pullups turned on


#define STEADY_COUNT 20
#define TRANSIENT_COUNT 3
#define TRANSIENT_COUNT_ABS 17

struct debounced_switch {
  uint8_t state;
  int counter;
};

class PressedKey {
public:
  PressedKey(uint8_t key_row, uint8_t key_column, uint32_t initial_hold_time);

  uint8_t row;
  uint8_t col;
  uint32_t hold_time;
};

class ReleasedKey {
public:
  ReleasedKey(uint8_t key_row, uint8_t key_column);

  uint8_t row;
  uint8_t col;
};


class KeyboardMatrix {
public:
  KeyboardMatrix(uint8_t num_rows, uint8_t num_cols,
                 uint8_t *row_pins, uint8_t *col_pins,
                 uint8_t diode_direction);
	~KeyboardMatrix();

  uint8_t diode_direction;
  uint8_t num_rows;
  uint8_t num_cols;
  uint8_t *row_pins;
  uint8_t *col_pins;

  uint16_t *matrix_state;
  uint16_t *matrix_state_prev;

  uint32_t delta_micros;

  LinkedList<PressedKey*> pressed_list;
  LinkedList<ReleasedKey*> released_list;

  void begin();
  bool update();
  bool button_pressed(uint8_t row, uint8_t button_bit_position);
  bool button_released(uint8_t row, uint8_t button_bit_position);
  bool button_held(uint8_t row, uint8_t button_bit_position);

 private:
  uint16_t *this_row_read;

  uint32_t last_update_micros;
  uint32_t this_update_micros;

  uint8_t new_pressed_keys_count;

  debounced_switch *key_states;

  bool debounce_update(uint8_t r, uint8_t c);
  void activate_column(uint8_t col);
  void deactivate_column(uint8_t col);
  void activate_row(uint8_t row);
  void deactivate_row(uint8_t row);
};
#endif
