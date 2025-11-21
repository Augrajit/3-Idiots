#ifndef KEYBOARD_MODULE_H
#define KEYBOARD_MODULE_H

// Key codes
#define KEY_APPROVE 10
#define KEY_DENY 11
#define KEY_RETRY 12
#define KEY_OVERRIDE 13
#define KEY_STAR 14
#define KEY_HASH 15

void keyboard_init();
int keyboard_get_key();
void keyboard_set_callback(void (*callback)(int key));
void keyboard_scan();

#endif

