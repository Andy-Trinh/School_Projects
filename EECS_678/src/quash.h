// quash.h

#ifndef SRC_QUASH_H
#define SRC_QUASH_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "execute.h"

// Holds info about the state and environment Quash is running in
typedef struct Quash {
  bool running;     // Check if still running
  bool is_a_tty;    // check if recieving from a file or command line
  char* parsed_str; // Holds the parsed string of the command input from the line
} Quash;

// Check if Quash is receiving input from the command line
bool is_tty();

// Get deep copy of command string
char* get_command_string();

// Check if still running
bool is_running();

// end the loop
void end_main_loop();

#endif // QUASH_H
