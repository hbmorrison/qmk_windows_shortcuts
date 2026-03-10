// Copyright 2026 Hannah Blythe Morrison
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include QMK_KEYBOARD_H

// True if Windows alt-tab behaviour is being processed.

static bool ws_alt_tab_state = false;

// Process Windows shortcut keys.

bool process_record_windows_shortcuts(uint16_t keycode, keyrecord_t *record) {

  // Process standard Windows shortcut keys.

  switch (keycode) {

    // Manage next and previous tab.

    case WS_NTAB:
      tap_code16(C(KC_TAB));
      return false;
    case WS_PTAB:
      tap_code16(S(C(KC_TAB)));
      return false;

    // Manage next and previous desk.

    case WS_NDESK:
      tap_code16(C(G(KC_RIGHT)));
      return false;
    case WS_PDESK:
      tap_code16(C(G(KC_LEFT)));
      return false;
  }

  // Process alt-tab behaviour as a special case.

  switch (keycode) {

    // Manage Windows alt-tab behaviour by artificially holding the alt key down
    // while the next or previous window keys are being pressed. Both the
    // standard Windows shortcut keys and the custom keycodes provided by this
    // module are recognised.

    case A(KC_TAB):
    case S(A(KC_TAB)):
    case WS_NWIN:
    case WS_PWIN:
      if (record->event.pressed) {

        // When the next or previous window keys are first pressed, register the
        // alt key.

        if (! ws_alt_tab_state) {
          ws_alt_tab_state = true;
          register_code(KC_LALT);
        }

        // Apply a oneshot shift modifier if previous window was pressed, in
        // order to cycle backwards instead of forwards.

        if (keycode == A(S(KC_TAB)) || keycode == WS_PWIN)
          set_oneshot_mods(MOD_BIT(KC_LSFT));

        // Tap the tab key and immediately return false to indicate that no
        // further processing is required.

        tap_code(KC_TAB);
        return false;
      }

    // If another key is pressed while the Windows alt-tab behaviour is active,
    // unregister the alt key and reset the alt-tab state.

    default:
      if (record->event.pressed && ws_alt_tab_state) {
        ws_alt_tab_state = false;
        unregister_code(KC_LALT);

        // Returning false here means that the keycode that interrupted the
        // alt-tab behaviour is swallowed. This is done so that the interrupting
        // key press is not transmitted into the window that was selected from
        // the carousel unintentionally.

        return false;
      }
  }

  // Continue processing the key.

  return true;
}
