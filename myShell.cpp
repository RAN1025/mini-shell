#include "myShellclass.h"

extern char ** environ;

int main() {
  pid_t fork_result = 0;
  pid_t w = 0;
  int wstatus = 0;
  //create my shell
  myShell Shell;
  Shell.startmyshell();
  while (Shell.checkrepeat()) {
    //get the "PATH" and store in the path list
    Shell.parsepath();
    //get one command
    Shell.readin();
    //check if the command is 'exit' or if EOF is encountered reading from stdin
    if (!Shell.checkrepeat()) {
      return EXIT_FAILURE;
    }
    //deal with variables, substitute the command
    Shell.substitute();
    //parse the command and store it in commandlist
    //excute the built-in function like cd, set, export and inc
    if (Shell.parsecommand()) {
      //if the program is valid, fork a new process to excute it
      fork_result = fork();
      if (fork_result == -1) {
        std::cerr << "fork" << std::endl;
        return EXIT_FAILURE;
      }
      else if (fork_result == 0) {
        //child process
        Shell.executeprocess(environ);
      }
      else {
        //parent process
        Shell.parentprocess(w, fork_result, wstatus);
      }
    }
    //reset the Shell class
    Shell.classreset();
    //start again
    Shell.startmyshell();
  }
  return EXIT_SUCCESS;
}
