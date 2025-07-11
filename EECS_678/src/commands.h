// commands.h

#ifndef SRC_COMMAND_H
#define SRC_COMMAND_H

#include <stdbool.h>

// Bit commands to dictate generic_cmd action
#define REDIRECT_IN     (0x01) // read from stdin
#define REDIRECT_OUT    (0x04) // write to stdout and truncate the original file
#define REDIRECT_APPEND (0x08) // write to stdout and appending the output
#define PIPE_IN         (0x10) // read from a pipe
#define PIPE_OUT        (0x20) // write to a pipe
#define BACKGROUND      (0x40) // run in background

// All types of command
typedef enum CommandType {
  EOC = 0, 
  GENERIC,
  ECHO,
  EXPORT,
  KILL,
  CD,
  PWD,
  JOBS,
  EXIT
} CommandType;

// Command with no arguments
typedef struct SimpleCommand {
  CommandType type; // Type of command 
} SimpleCommand;

// Command that take in non-builtin arguments
typedef struct GenericCommand {
  CommandType type; 
  char** args;      
} GenericCommand;

// Alias for GenericCommand
typedef GenericCommand EchoCommand;

// Command to set environment variables
typedef struct ExportCommand {
  CommandType type; 
  char* env_var;    
  char* val;        
} ExportCommand;

// Command to change directories
typedef struct CDCommand {
  CommandType type; 
  char* dir;        
} CDCommand;

// Command to kill a process based on job id
typedef struct KillCommand {
  CommandType type; 
  int sig;        
  int job;          
  char* sig_str;   
  char* job_str;    
} KillCommand;

// Alias for Simplecmd
typedef SimpleCommand PWDCommand;
typedef SimpleCommand JobsCommand;
typedef SimpleCommand ExitCommand;
typedef SimpleCommand EOCCommand;

// Make all command types the same size and interchangeable
typedef union Command {
  SimpleCommand simple;   // Read structure as a SimpleCommand 
  GenericCommand generic; // Read structure as a GenericCommand 
  EchoCommand echo;       // Read structure as a ExportCommand 
  ExportCommand export;   // Read structure as a ExportCommand 
  CDCommand cd;           // Read structure as a CDCommand 
  KillCommand kill;       // Read structure as a KillCommand 
  PWDCommand pwd;         // Read structure as a PWDCommand 
  JobsCommand jobs;       // Read structure as a JobsCommand 
  ExitCommand exit;       // Read structure as a ExitCommand 
  EOCCommand eoc;         // Read structure as a EOCCommand 
} Command;


typedef struct Cmd_Holder {
  char* redirect_in; 
  char* redirect_out; 
  char flags;         
  Command cmd;      
} Cmd_Holder;


// Command structure constructors

// Copy the Cmd_Holder
Cmd_Holder mk_command_holder(char* redirect_in, char* redirect_out, char flags, Command cmd);

// Create a GenericCmd structure
Command mk_generic_command(char** args);

// Create a EchoCmd structure
Command mk_echo_command(char** args);

// Create a ExportCmd structure
Command mk_export_command(char* env_var, char* val);

// Create a CDCmd structure
Command mk_cd_command(char* dir);

// Create a KillCmd structure
Command mk_kill_command(char* sig, char* job);

// Create a PWDCmd structure
Command mk_pwd_command();

// Create a JobsCmd structure
Command mk_jobs_command();

// Create a ExitCmd structure
Command mk_exit_command();

// Create a EOCCmd structure
Command mk_eoc();

// Get command type
CommandType get_command_type(Command cmd);

// Get command type and corresponding holder
CommandType get_command_holder_type(Cmd_Holder holder);

// Debugger
void debug_print_script(const Cmd_Holder* holders);

#endif