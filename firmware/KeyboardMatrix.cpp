#include "KeyboardMatrix.h"

PressedKey::PressedKey(uint8_t key_row, uint8_t key_column, uint32_t initial_hold_time) {
  row = key_row;
  col = key_column;
  hold_time = initial_hold_time;
}

// uint8_t PressedKey::row() {return _row;}
// uint8_t PressedKey::col() {return _col;}
// uint32_t PressedKey::hold_time() {return _hold_time;}

ReleasedKey::ReleasedKey(uint8_t key_row, uint8_t key_column) {
  row = key_row;
  col = key_column;
}

// uint8_t ReleasedKey::row() {return _row;}
// uint8_t ReleasedKey::col() {return _col;}

KeyboardMatrix::KeyboardMatrix(uint8_t numrows, uint8_t numcols,
                               uint8_t *rowpins, uint8_t *colpins,
                               uint8_t diodedir) {
  diode_direction = diodedir;
  num_rows = numrows;
  num_cols = numcols;
  row_pins = (uint8_t*) rowpins;
  col_pins = (uint8_t*) colpins;

  delta_micros = 0;
  last_update_micros = 0;
  this_update_micros = 0;

  this_row_read = new uint16_t[num_rows];
  matrix_state = new uint16_t[num_rows];
  matrix_state_prev = new uint16_t[num_rows];
  key_states = new debounced_switch[num_rows*num_cols];
}

KeyboardMatrix::~KeyboardMatrix(void) {
  delete [] this_row_read;
  delete [] matrix_state_prev;
  delete [] matrix_state;
  delete [] key_states;
}

void KeyboardMatrix::begin(void) {
  if (diode_direction == DIODE_DIRECTION_COL_PIN_TO_ROW_PIN) {
    // Set col pins to input and turn on pullups
    for (uint8_t i=0; i<num_cols; i++) {
      pinMode(col_pins[i], INPUT_PULLUP);
    }

    // Set row pins to input - this is the 'deactivated' state
    for (uint8_t i=0; i<num_rows; i++) {
      pinMode(row_pins[i], INPUT);
    }
  }
  else if (diode_direction == DIODE_DIRECTION_ROW_PIN_TO_COL_PIN) {
    // Set row pins to input and turn on pullups
    for (uint8_t i=0; i<num_rows; i++) {
      pinMode(row_pins[i], INPUT_PULLUP);
    }

    // Set col pins to input - this is the 'deactivated' state
    for (uint8_t i=0; i<num_cols; i++) {
      pinMode(col_pins[i], INPUT);
    }
  }

  // init default values
  for (uint8_t row=0; row<num_rows; row++) {
    this_row_read[row] = 0xFFFF;
    matrix_state[row] = 0xFFFF;
    matrix_state_prev[row] = 0xFFFF;

    for (uint8_t col=0; col<num_cols; col++) {
      // hold_time[row][col] = 0;

      // init key_states to not pressed
      key_states[row*num_cols+col].counter = -STEADY_COUNT;
      key_states[row*num_cols+col].state = 0;
    }
  }
}

bool KeyboardMatrix::debounce_update(uint8_t r, uint8_t c) {
  uint16_t index = r*num_cols+c;
  // if button state is active
  if (0 == (this_row_read[r] & (1<<c))) {
    // Serial.print("positive read row: ");
    // Serial.print(r);
    // Serial.print("col: ");
    // Serial.print(c);
    // Serial.print("counter increment ");
    // Serial.print(key_states[index].counter);
    // Serial.print(" -> ");
    if (key_states[index].counter < +STEADY_COUNT)
      ++key_states[index].counter;
    // Serial.println(key_states[index].counter);
  }
  else {
    if (key_states[index].counter > -STEADY_COUNT)
      --key_states[index].counter;
  }
  switch (key_states[index].state) {
  case 0: // steady-state lo
    if (key_states[index].counter >= -TRANSIENT_COUNT_ABS) {
      // => transient lo-hi
      // Serial.print("Pressed Transient ");
      // Serial.print(ascii_key_matrix[0][r][c]);
      // Serial.print(" counter: ");
      // Serial.println(key_states[index].counter);
      key_states[index].counter = 0;
      key_states[index].state = 1;
      return true;
    } else {
      return false;
    }
  case 1: // transient lo-hi
    switch (key_states[index].counter) {
    case +STEADY_COUNT:
      // => steady-state hi
      // Serial.print("Pressed Steady ");
      // Serial.print(ascii_key_matrix[0][r][c]);
      // Serial.print(" counter: ");
      // Serial.println(key_states[index].counter);
      key_states[index].state = 2;
      return false;
    case -STEADY_COUNT:
      // => steady-state lo
      key_states[index].state = 0;
      return true;
    default:
      return false;
    }
  case 2: // steady-state hi
    if (key_states[index].counter <= +TRANSIENT_COUNT_ABS) {
      // => transient hi-lo
      // Serial.print("Released Transient ");
      // Serial.print(ascii_key_matrix[0][r][c]);
      // Serial.print(" counter: ");
      // Serial.println(key_states[index].counter);
      key_states[index].counter = 0;
      key_states[index].state = 3;
      return true;
    } else {
      return false;
    }
  case 3: // transient hi-lo
    switch (key_states[index].counter) {
    case +STEADY_COUNT:
      // => steady-state hi
      key_states[index].state = 2;
      return true;
    case -STEADY_COUNT:
      // => steady-state lo
      // Serial.print("Released Steady ");
      // Serial.print(ascii_key_matrix[0][r][c]);
      // Serial.print(" counter: ");
      // Serial.println(key_states[index].counter);
      key_states[index].state = 0;
      return false;
    default:
      return false;
    }
  default:
    // should panic, but `throw` isn't always available...
    return false;
  }
}

// Set the row pin we want to scan to LOW (ground)
void KeyboardMatrix::activate_row(uint8_t row) {
  pinMode(row_pins[row], OUTPUT);
  digitalWrite(row_pins[row], LOW);
}

// Set the row to INPUT to deactivate
void KeyboardMatrix::deactivate_row(uint8_t row) {
  pinMode(row_pins[row], INPUT);
}

// Set the column pin we want to scan to LOW (ground)
void KeyboardMatrix::activate_column(uint8_t col) {
  pinMode(col_pins[col], OUTPUT);
  digitalWrite(col_pins[col], LOW);
}

// Set the column to INPUT to deactivate
void KeyboardMatrix::deactivate_column(uint8_t col) {
  pinMode(col_pins[col], INPUT);
}

bool KeyboardMatrix::button_pressed(uint8_t row, uint8_t button_bit_position) {
  // (this button == 0) and (last_button == 1)
  return (!(matrix_state[row] & (1<<button_bit_position))
          && (matrix_state_prev[row] & (1<<button_bit_position)));
}

bool KeyboardMatrix::button_released(uint8_t row, uint8_t button_bit_position) {
  // (this button == 1) and (last_button == 0)
  return ((matrix_state[row] & (1<<button_bit_position))
          && !(matrix_state_prev[row] & (1<<button_bit_position)));
}

bool KeyboardMatrix::button_held(uint8_t row, uint8_t button_bit_position) {
  // !(this button == 0) and (last_button == 0)
  return (!(matrix_state[row] & (1<<button_bit_position))
          && !(matrix_state_prev[row] & (1<<button_bit_position)));
}

bool KeyboardMatrix::update(void) {
  bool matrix_changed = false;
  uint8_t row, col, r, c;
  uint16_t btn_bit = 0;

  last_update_micros = this_update_micros;

  if (diode_direction == DIODE_DIRECTION_COL_PIN_TO_ROW_PIN) {

    // Scan the matrix one row at a time
    // Column pins are the input
    for (row=0; row<num_rows; row++) {
      activate_row(row);

      // Read each key (each column pin) in the activated row
      for (col=0; col<num_cols; col++) {
        // Left-most key in a row == LSB
        // Right-most key in a row == MSB
        btn_bit = 1 << col;
        if (digitalRead(col_pins[col]) == LOW) {
          // Key is pressed
          this_row_read[row] = this_row_read[row] & ~btn_bit;
        }
        else {
          // Key is released
          this_row_read[row] = this_row_read[row] | btn_bit;
        }
      }
      deactivate_row(row);
    }

  }
  else if (diode_direction == DIODE_DIRECTION_ROW_PIN_TO_COL_PIN) {

    // Scan the matrix one column at a time
    // Row pins are the input
    for (col=0; col<num_cols; col++) {
      activate_column(col);
      // Read each key (each row pin) in the activated column
      for (row=0; row<num_rows; row++) {
        // Left-most key in a row == LSB
        // Right-most key in a row == MSB
        btn_bit = 1 << col;
        if (digitalRead(row_pins[row]) == LOW) {
          // Key is pressed
          this_row_read[row] = this_row_read[row] & ~btn_bit;
        }
        else {
          // Key is released
          this_row_read[row] = this_row_read[row] | btn_bit;
        }
      }
      deactivate_column(col);
    }

  }

  this_update_micros = micros();
  delta_micros = this_update_micros - last_update_micros;

  // delete all keys and data objects
  ReleasedKey *rkey;
	while(released_list.size() > 0) {
    rkey = released_list.shift();
    delete rkey;
  }

  // Save matrix_state_prev
  for (row=0; row<num_rows; row++) {
    matrix_state_prev[row] = matrix_state[row];
  }

  // debounce and count new keys
  new_pressed_keys_count = 0;

  for (r=0; r<num_rows; r++) {
    for (c=0; c<num_cols; c++) {

      if (debounce_update(r, c)) {
        btn_bit = 1 << c;
        // if key is pressed
        if (key_states[r*num_cols+c].state == 1 || key_states[r*num_cols+c].state == 2) {
          new_pressed_keys_count++;

          PressedKey *new_key = new PressedKey(r, c, 0);
          PressedKey *last_key = pressed_list.last_item();

          // Reject keys if ghost
          if (new_pressed_keys_count > 1) {
            // ignore this new_key
            // Serial.print("THIS GHOST KEY: ");
            // Serial.print(ascii_key_matrix[0][r][c]);
            // Serial.print("newpressedcount: ");
            // Serial.print(new_pressed_keys_count);
            // Serial.print(" -> ");
            new_pressed_keys_count -= 1;
            // Serial.println(new_pressed_keys_count);

            // check for past ghost presses
            while (new_key->hold_time == last_key->hold_time) {
              // Serial.print("PAST GHOST KEY: ");
              // Serial.print(ascii_key_matrix[0][last_key->row][last_key->col]);
              // Serial.print(" newpressedcount: ");
              // Serial.print(new_pressed_keys_count);
              // Serial.print(" -> ");
              new_pressed_keys_count -= 1;
              // Serial.println(new_pressed_keys_count);

              // turn off extra ghost key being deleted
              matrix_state[last_key->row] = matrix_state[last_key->row] | (1 << (last_key->col));

              // remove last pressed key
              last_key = pressed_list.pop();
              delete last_key;
              // get new last key
              last_key = pressed_list.last_item();
            }
          }
          else {
            // Serial.print(ascii_key_matrix[0][r][c]);
            // Serial.println(" bit to 0");

            // Key is now pressed - Set matrix bit to 0
            matrix_state[r] = matrix_state[r] & ~btn_bit;

            // add the new pressed key
            pressed_list.add(new_key);
          }
        }
        // else key was released
        else {
          // Serial.print(ascii_key_matrix[0][r][c]);
          // Serial.println(" bit to 1");

          // Key is now released -> Set matrix bit to 1
          matrix_state[r] = matrix_state[r] | btn_bit;

          PressedKey *key_to_delete;
          // remove key from pressed_list
          for (uint8_t i=0; i<pressed_list.size(); i++) {
            key_to_delete = pressed_list.get(i);
            if (key_to_delete->row == r &&
                key_to_delete->col == c) {
              key_to_delete = pressed_list.remove(i);
              delete key_to_delete;
            }
          }

          ReleasedKey *new_released_key = new ReleasedKey(r, c);
          released_list.add(new_released_key);
        }
      }
    }
  }

  // end debounce

  // increment hold times for pressed keys
  PressedKey *pkey;
  for (uint8_t i=0; i<pressed_list.size(); i++) {
    pkey = pressed_list.get(i);
    if (button_held(pkey->row, pkey->col)) {
      pkey->hold_time += delta_micros;
    }
  }

  for (row=0; row<num_rows; row++) {
    if (matrix_state[row] != matrix_state_prev[row]) {
      matrix_changed = true;
      break;
    }
  }

  return matrix_changed;
}

