#ifndef __MYSHELLCLASS_H__
#define __MYSHELLCLASS_H__
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class myShell
{
 private:
  std::string command;
  std::vector<std::string> commandlist;
  bool repeat;
  std::string ECE551PATH;
  std::vector<std::string> pathlist;
  //key=variable, value=value
  std::map<std::string, std::string> variablelist;

 public:
  myShell();
  myShell(const myShell & rhs);
  myShell & operator=(const myShell & rhs);
  ~myShell();

  //print a prompt
  void startmyshell();

  //PATH:
  //get the current path and store it in pathlist
  void parsepath();
  //check if a program is a regular file
  //if the program is a regular file, return true
  bool checkprogram(const char * directory, const char * filename);
  //get the absolute path of command
  //check if the path can excute
  //and return true if the path is valid
  bool checkpath();

  //COMMAND:
  //get one command and store the the whole command in command
  void readin();
  //check wether the variable exists, and replace the variable with its value
  void substitute();
  //check if a space is a separator, return true if it is a separator
  bool separator(std::string & com, size_t space);
  //split the command and arguments and store it, return false it the command is empty
  bool splitargument();
  //deal with backslash in arguments
  void backslash_helper();
  //parse the command:
  //call splitargument() to split the command
  //call backslash_helper() to deal with the backslash in the arguments
  //and call checkpath() to check the command
  //return true if it requires fork
  //return false if it is built-in function or not valid program
  bool parsecommand();

  //BUILT-IN FUNCTION:
  //cd command
  void cddir();
  //check the name of variable, return true if the variable is valid
  bool varname(std::string & var);
  //set command
  void setvar();
  //export command
  void exportvar();
  //check the value of variable, return true if it represents a number in base10
  bool numbervar(std::string & value);
  //inc command
  void incvar();

  //EXECUTE:
  //execute the process
  void executeprocess(char ** newenviron);
  //wait the process
  void parentprocess(pid_t w, pid_t fork_result, int wstatus);

  //EXIT:
  //check whether exit my shell
  bool checkrepeat();

  //RESET:
  //reset the class to wait for new command
  void classreset();
};
#endif
