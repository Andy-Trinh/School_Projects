// execute.h

#ifndef SRC_EXECUTE_H
#define SRC_EXECUTE_H

#include <stdbool.h>
#include <unistd.h>

#include "commands.h"

// Look up given enviroment variables
const char* lookup_env(const char* env_var);

// Set and define environment variables
void write_env(const char* env_var, const char* val);

// Get current directory
char* get_current_directory(bool* should_free);

// Check on background jobs
void check_jobs_bg_status();

// Print a job
void print_job(int job_id, pid_t pid, const char* cmd);

// Print the start up of a background job
void print_job_bg_start(int job_id, pid_t pid, const char* cmd);

// Print the completion of a background job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd);

// Run non-builtin commands
void generic_cmd(GenericCommand cmd);

// Mimic linux command echo
void echo_cmd(EchoCommand cmd);

// Mimic linux command export
void export_cmd(ExportCommand cmd);

// Mimic linux command cd
void cd_cmd(CDCommand cmd);

// Mimic linux command  kill
void kill_cmd(KillCommand cmd);

// Mimic linux command pwd
void pwd_cmd();

// Mimic linux command jobs
void jobs_cmd();

// Main entry point for commands
void run(Cmd_Holder* holders);

#endif