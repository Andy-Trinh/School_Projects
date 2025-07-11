// quash.c

#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "commands.h"
#include "execute.h"
#include "parsing_interface.h"
#include "memory_pool.h"
#include "quash.h"

// Private Variable
static Quash state;

// Private Function
static Quash initial_state() {
  return (Quash) {
    true,
    isatty(STDIN_FILENO),
    NULL
  };
}

// Prints prompt for command
static void print_prompt() {
  bool should_free = true;
  char* cwd = get_current_directory(&should_free);
  
  printf("[QUASH]$ ");

  fflush(stdout);

  if (should_free)
    free(cwd);
}


// Check if loop is running
bool is_running() {
  return state.running;
}

// Get a copy of the string
char* get_command_string() {
  return strdup(state.parsed_str);
}

// Check if Quash is receiving input from the command line or not
bool is_tty() {
  return state.is_a_tty;
}

// Stop Quash from requesting more input
void end_main_loop() {
  state.running = false;
}


int main(int argc, char** argv) {
  state = initial_state();

  // Main Loop
  while (is_running()) {
    if (is_tty())
      print_prompt();

    initialize_memory_pool(1024);
    Cmd_Holder* script = parse(&state);

    if (script != NULL)
      run(script);

    destroy_memory_pool();
  }

  return EXIT_SUCCESS;
}