// execute.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "quash.h"
#include "deque.h"
#include "execute.h"

// pid queue
IMPLEMENT_DEQUE_STRUCT(pid_queue, pid_t);
IMPLEMENT_DEQUE(pid_queue, pid_t);
pid_queue pid_q;

// job struct
struct Job {
  int job_id;
  char* cmd;
  pid_queue pid_q;
  pid_t pid;
} Job;

// job queue
IMPLEMENT_DEQUE_STRUCT(job_queue, struct Job);
IMPLEMENT_DEQUE(job_queue, struct Job);
job_queue job_q;
int job_number = 1;

bool init = 0;
static int pipes[2][2];

// Interface Function

// Return a string containing the current working directory.
char* get_current_directory(bool* should_free) {
	char* wd = NULL;
	wd = getcwd (NULL, 0);
	return wd;
}

// Returns the value of an environment variable env_var
const char* lookup_env(const char* env_var) {
  return getenv (env_var);
}

// Check the status of background jobs
void check_jobs_bg_status() {

	// loop through jobs
  int num_jobs = length_job_queue (&job_q);
  for (int j = 0; j < num_jobs; j++) {

    struct Job curr_job = pop_front_job_queue (&job_q);

		// loop through pids
		int num_pids = length_pid_queue (&curr_job.pid_q);
		pid_t front = peek_front_pid_queue (&curr_job.pid_q);
    for (int p = 0; p < num_pids; p++) {

      pid_t curr_pid = pop_front_pid_queue (&curr_job.pid_q);
			int status;
      if (waitpid (curr_pid, &status, WNOHANG) == 0) {
        push_back_pid_queue (&curr_job.pid_q, curr_pid);
      }
    }

    if (is_empty_pid_queue (&curr_job.pid_q)) {
      print_job_bg_complete (curr_job.job_id, front, curr_job.cmd);
    }
    else {
      push_back_job_queue (&job_q, curr_job);
    }
  }
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

// Functions for process commands

// Run a program reachable by the path environment variable, relative path, or
// absolute path
void generic_cmd(GenericCommand cmd) {
  char* exec = cmd.args[0];
  char** args = cmd.args;
  execvp (exec,args);
  perror ("ERROR: Failed to execute program");
}

// Print strings mimic linux cmd echo
void echo_cmd(EchoCommand cmd) {
  char** str = cmd.args;
  int i = 0;
  // Check if the string contains double quotation marks
  while(str[i]!=NULL){
    int j = 0;
    while(str[i][j] != NULL) {
      if (str[i][j] != '"') {
        printf("%c", str[i][j]);
      }
      
      j++;
    }
    printf(" ");
    i++;
  }
  printf ("\n");

	// Flush the buffer before returning
  fflush(stdout);
}

// Sets an environment variable, mimic linux cmd export
void export_cmd(ExportCommand cmd) {
	const char* env_var = cmd.env_var;
  const char* val = cmd.val;
  setenv (env_var, val, 1);
}

// Changes the current working directory, mimic linux cmd cd
void cd_cmd(CDCommand cmd) {
  const char* dir = cmd.dir;

  // Check if the directory is valid
  if (dir == NULL) {
    perror("ERROR: Failed to resolve path");
    return;
  }

  // Change directory if valid and set new PWD
  char path[102];
  realpath (dir, path);
  chdir (path);
  char cwd[1024];
  setenv ("OLD_PWD", getcwd (cwd, sizeof (cwd)), 1);
  setenv ("PWD", dir, 1);
}

// Sends a signal to all processes contained in a job, mimic linux cmd kill
void kill_cmd(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  struct Job curr_job;

	// find job
  for (int j = 0; j < length_job_queue (&job_q); j++) {

    curr_job = pop_front_job_queue (&job_q);
    if (curr_job.job_id == job_id) {

			// kill all processes
			pid_queue curr_pid_q = curr_job.pid_q;
			while (length_pid_queue (&curr_pid_q) != 0) {
		    pid_t curr_pid = pop_front_pid_queue (&curr_pid_q);
		    kill (curr_pid, signal);
		  }
    }
    push_back_job_queue (&job_q, curr_job);
  }
}

// Prints current working directory, mimic linux cmd pwd
void pwd_cmd() {
	char cwd[1024];
  printf ("%s\n", getcwd (cwd, sizeof (cwd)));

  // Flush buffer before returning
  fflush(stdout);
}

// Prints all bg jobs on the list, mimic linux cmd jobs
void jobs_cmd() {
  // Get length of job queue
	int num_jobs = length_job_queue (&job_q);

  // print all jobs
	for (int j = 0; j < num_jobs; j++)
	{
		struct Job curr_job = pop_front_job_queue (&job_q);
		print_job (curr_job.job_id, curr_job.pid, curr_job.cmd);
		push_back_job_queue (&job_q, curr_job);
	}

  // Flush buffer before returning
  fflush(stdout);
}

// Command resolution and process setup

// Resolve the correct command function for the child process
void child_run_command(Command cmd) {
 CommandType type = get_command_type(cmd);

 switch (type) {
 case GENERIC:
   generic_cmd(cmd.generic);
   break;

 case ECHO:
   echo_cmd(cmd.echo);
   break;

 case PWD:
   pwd_cmd();
   break;

 case JOBS:
   jobs_cmd();
   break;

 case EXPORT:
 case CD:
 case KILL:
 case EXIT:
 case EOC:
   break;

 default:
   fprintf(stderr, "Unknown command type: %d\n", type);
 }
}

// Resolve the correct command function for the Quash process
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    export_cmd(cmd.export);
    break;

  case CD:
    cd_cmd(cmd.cd);
    break;

  case KILL:
    kill_cmd(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

// Create a new process based on the Command in the Cmd_Holder
// Sets up redirects and pipes where needed
void create_process(Cmd_Holder holder, int i) {
	// Read the flags field from the parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND;

	int write_end = i % 2;
	int read_end = (i - 1) % 2;

  if (p_out)
	{
		pipe (pipes[write_end]);
	}
  pid_t pid = fork();

  push_back_pid_queue(&pid_q, pid);
  if (pid == 0) {

		if (p_in)
		{
			dup2 (pipes[read_end][0], STDIN_FILENO);
			close (pipes[read_end][0]);
		}
		if (p_out)
		{
			dup2 (pipes[write_end][1], STDOUT_FILENO);
			close (pipes[write_end][1]);
		}
	  if (r_in)
	  {
		  FILE* f = fopen (holder.redirect_in, "r");
		  dup2 (fileno (f), STDIN_FILENO);
	  }
	  if (r_out)
	  {
		  if (r_app)
		  {
			  FILE* f = fopen (holder.redirect_out, "a");
			  dup2 (fileno (f), STDOUT_FILENO);
		  }
		  else
		  {
			  FILE* f = fopen (holder.redirect_out, "w");
			  dup2 (fileno (f), STDOUT_FILENO);
		  }
	  }

    child_run_command (holder.cmd);
    exit(0);
  }
  else {
    if (p_out) {
      close (pipes[write_end][1]);
    }
  }
    parent_run_command(holder.cmd);
}

// Runs a list of commands
void run(Cmd_Holder* holders) {

  if (init == 0){
		job_q = new_job_queue (1);
    init = 1;
  }
  pid_q = new_pid_queue(1);

  if (holders == NULL)
    return;

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT &&
      get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  CommandType type;

  // Runs all commands in 'holder' array
  for (int i = 0; (type = get_command_holder_type(holders[i]) ) != EOC; ++i){
    create_process(holders[i], i );
  }

	// if not a background job
  if (!(holders[0].flags & BACKGROUND)) {
    while (!is_empty_pid_queue (&pid_q)) {
      pid_t curr_pid = pop_front_pid_queue (&pid_q);
      int status;
      waitpid (curr_pid, &status, 0);
    }
    destroy_pid_queue (&pid_q);
  }
  // if a background job
  else {
    struct Job curr_job;
    curr_job.job_id = job_number;
    job_number = job_number + 1;
    curr_job.pid_q = pid_q;
    curr_job.cmd = get_command_string ();
    curr_job.pid = peek_back_pid_queue (&pid_q);
    push_back_job_queue (&job_q, curr_job);
    print_job_bg_start (curr_job.job_id, curr_job.pid, curr_job.cmd);
  }
}
