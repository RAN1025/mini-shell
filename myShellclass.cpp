#include "myShellclass.h"

//default constructor
myShell::myShell() : repeat(true) {}

//copy constructor
myShell::myShell(const myShell & rhs) {
  *this = rhs;
}

//assignment operator
myShell & myShell::operator=(const myShell & rhs) {
  if (this != &rhs) {
    command = rhs.command;
    commandlist = rhs.commandlist;
    repeat = rhs.repeat;
    ECE551PATH = rhs.repeat;
    pathlist = rhs.pathlist;
    variablelist = rhs.variablelist;
  }
  return *this;
}

//destructor
myShell::~myShell() {
  commandlist.clear();
  pathlist.clear();
}

//print a prompt
void myShell::startmyshell() {
  std::cout << "myShell:" << getenv("PWD") << " $" << std::endl;
}

//PATH----------------------------------------------------------------------------------------------------------------
//get the current path and store it in pathlist
void myShell::parsepath() {
  //get the environment variable
  ECE551PATH = getenv("PATH");
  size_t start = 0;
  size_t end = 0;
  //find the separator
  end = ECE551PATH.find_first_of(':', start);
  while (end != std::string::npos) {
    //store each path into pathlist
    pathlist.push_back(ECE551PATH.substr(start, end - start));
    start = end + 1;
    end = ECE551PATH.find_first_of(':', start);
  }
  pathlist.push_back(ECE551PATH.substr(start));
}

//check whether the program is a regular file,return true if it is regular
bool myShell::checkprogram(const char * directory, const char * filename) {
  //reference: code from 098_dedup
  DIR * dir;
  struct dirent * ent;
  //open directory
  dir = opendir(directory);
  if (dir == NULL) {
    //cannot open directory
    return false;
  }
  //read filename
  while ((ent = readdir(dir)) != NULL) {
    //check the regular file
    if ((ent->d_type & DT_REG) == DT_REG) {
      if (strcmp(ent->d_name, filename) == 0) {
        closedir(dir);
        return true;
      }
    }
  }
  closedir(dir);
  return false;
}

//find the absolute path for program and check whether it can excute,and return true if it can excute
bool myShell::checkpath() {
  //does not contain a '/'
  if (commandlist[0].find('/') == std::string::npos) {
    for (size_t i = 0; i < pathlist.size(); i++) {
      //check wether program exist
      if (checkprogram(pathlist[i].c_str(), commandlist[0].c_str())) {
        //get the absolute path
        commandlist[0] = pathlist[i] + '/' + commandlist[0];
        return true;
      }
    }
  }
  //relative path
  //./
  else if (commandlist[0].find('.') == 0 && commandlist[0].find('/') == 1) {
    size_t cut = commandlist[0].find_last_of('/');
    std::string com = commandlist[0].substr(cut + 1);
    std::string path = commandlist[0].substr(0, cut);
    if (checkprogram(path.c_str(), com.c_str())) {
      return true;
    }
  }
  //../
  else if (commandlist[0].find('.') == 0 && commandlist[0].find('/') == 2) {
    size_t cut = commandlist[0].find_last_of('/');
    std::string com = commandlist[0].substr(cut + 1);
    std::string path = commandlist[0].substr(0, cut);
    if (checkprogram(path.c_str(), com.c_str())) {
      return true;
    }
  }
  //absolute path
  else if (commandlist[0].find('/') == 0) {
    std::string directory;
    std::string com;
    size_t cut = commandlist[0].find_last_of('/');
    directory = commandlist[0].substr(0, cut);
    com = commandlist[0].substr(cut + 1);
    if (checkprogram(directory.c_str(), com.c_str())) {
      return true;
    }
  }
  return false;
}

//COMMAND-------------------------------------------------------------------------------------------------------------
//get one command and store the whole command in command
void myShell::readin() {
  if (std::cin.eof()) {
    repeat = false;
    return;
  }
  std::getline(std::cin, command);
  if (command == "exit") {
    repeat = false;
    return;
  }
}

//check if the space is a separator, return true if it is a separtor
bool myShell::separator(std::string & com, size_t space) {
  int count = 0;
  while (com[space - 1] == '\\' && space >= 0) {
    count++;
    space--;
  }
  //odd '\'
  if (count % 2 == 1) {
    return false;
  }
  return true;
}

//check wether the variable exists, and replace the variable with its value
void myShell::substitute() {
  bool repeat = true;
  //start a new cycle
  while (command.find('$') != std::string::npos && repeat == true) {
    //one cycle
    size_t start = 0;
    size_t end = 0;
    start = command.find_first_of('$');
    //'$' exists
    while (start != std::string::npos) {
      //check wether need to be substituted
      if (start == command.length() - 1) {
        //no substitution occurs
        repeat = false;
        break;
      }
      end = command.find_first_of('$', start + 1);
      //next '$' exists
      if (end != std::string::npos) {
        //compare one by one
        for (size_t i = start + 1; i < end; i++) {
          std::string key = command.substr(start + 1, i - start);
          //compare with variable in variablelist
          std::map<std::string, std::string>::const_iterator it = variablelist.find(key);
          //find it and substitute the variable with its value
          if (it != variablelist.end()) {
            command.replace(start, i - start + 1, it->second);
            int change = it->second.length() - (i - start + 1);
            //end changes value because of subssitution
            end = end + change;
            repeat = true;
            break;
          }
          //no substitution occurs
          repeat = false;
        }
        //check next '$'
        start = end;
      }
      //next '$' does not exist
      else {
        //compare one by one
        for (size_t i = start + 1; i < command.length(); i++) {
          std::string key = command.substr(start + 1, i - start);
          //compare with variable in variablelist
          std::map<std::string, std::string>::const_iterator it = variablelist.find(key);
          //find it and substitute the variable with its value
          if (it != variablelist.end()) {
            command.replace(start, i - start + 1, it->second);
            repeat = true;
            break;
          }
          //no substitution occurs
          repeat = false;
        }
        start = end;
      }
    }
  }
}

//split the command and arguments and store it, return false if the command is empty
bool myShell::splitargument() {
  size_t start = 0;
  size_t end = 0;
  start = command.find_first_not_of(' ', 0);
  if (start == std::string::npos) {
    std::cout << "empty command" << std::endl;
    return false;
  }
  end = command.find_first_of(' ', start);
  //space exist
  while (end != std::string::npos) {
    //find the separator
    while (!separator(command, end)) {
      end = command.find_first_of(' ', end + 1);
      if (end == std::string::npos) {
        break;
      }
    }
    if (end == std::string::npos) {
      break;
    }
    //store in the commandlist
    commandlist.push_back(command.substr(start, end - start));
    start = command.find_first_not_of(' ', end + 1);
    if (start == std::string::npos) {
      break;
    }
    end = command.find_first_of(' ', start);
  }
  if (start != std::string::npos) {
    commandlist.push_back(command.substr(start));
  }
  return true;
}

//deal with backslash in arguments
void myShell::backslash_helper() {
  //deal with each arguments in commandlist
  for (size_t n = 1; n < commandlist.size(); n++) {
    std::string temp;
    //check character by character
    for (size_t i = 0; i < commandlist[n].length(); i++) {
      //ignore the backslash
      if (commandlist[n][i] == '\\') {
        i++;
      }
      temp.push_back(commandlist[n][i]);
    }
    //well-turned arguments
    commandlist[n] = temp;
  }
}

//parse the command:
//split the command
//if it is a built in function then excute the corresponding functions
//if it is not a built in function, deal with the backslash in each arguments, and check the path to excute
//return true if it requires fork
//return false if it is built in function or not a valid program
bool myShell::parsecommand() {
  if (splitargument() == false) {
    //empty
    return false;
  }
  //cd command
  if (commandlist[0] == "cd") {
    cddir();
    return false;
  }
  //set command
  if (commandlist[0] == "set") {
    setvar();
    return false;
  }
  //export command
  if (commandlist[0] == "export") {
    exportvar();
    return false;
  }
  //inc command
  if (commandlist[0] == "inc") {
    incvar();
    return false;
  }
  //deal with the backslash in command
  backslash_helper();
  //not a built command, then check if it is a valid command
  if (checkpath() == false) {
    std::cout << "Command " << commandlist[0] << " not found" << std::endl;
    return false;
  }
  //program  exists and the absolute path is stored
  return true;
}

//BUILT-IN FUNCTION---------------------------------------------------------------------------------------------------
//cd command
void myShell::cddir() {
  int togo = 0;
  int errorcheck = 0;
  if (commandlist.size() > 2) {
    std::cout << "cd : too many arguments" << std::endl;
  }
  //only 'cd' then go to HOME
  else if (commandlist.size() == 1) {
    togo = chdir(getenv("HOME"));
    if (togo == -1) {
      std::cout << "cd failed" << std::endl;
      return;
    }
    //overwrite PWD
    errorcheck = setenv("PWD", getenv("HOME"), 1);
  }
  //'cd ~' then go to HOME
  else if (commandlist[1] == "~") {
    togo = chdir(getenv("HOME"));
    if (togo == -1) {
      std::cout << "cd failed" << std::endl;
      return;
    }
    //overwrite PWD
    errorcheck = setenv("PWD", getenv("HOME"), 1);
  }
  else {
    togo = chdir(commandlist[1].c_str());
    if (togo == -1) {
      std::cout << "cd :" << commandlist[1] << " : no such file or directory" << std::endl;
      return;
    }
    //overwrite PWD
    std::string currdir = getenv("PWD");
    //'cd .' stay current directory
    if (commandlist[1] == ".") {
      errorcheck = setenv("PWD", currdir.c_str(), 1);
    }
    //'cd ..' previous directory
    else if (commandlist[1] == "..") {
      size_t cut = currdir.find_last_of('/');
      std::string temp = currdir.substr(0, cut);
      errorcheck = setenv("PWD", temp.c_str(), 1);
    }
    //relative path
    else if (commandlist[1][0] != '/') {
      std::string temp = currdir + '/' + commandlist[1];
      errorcheck = setenv("PWD", temp.c_str(), 1);
    }
    //absolute path
    else {
      errorcheck = setenv("PWD", commandlist[1].c_str(), 1);
    }
  }
  if (errorcheck == -1) {
    std::cerr << "fail to overwrite current directory" << std::endl;
  }
}

//check the name of variable, return true if the variable is valid
bool myShell::varname(std::string & var) {
  for (size_t i = 0; i < var.length(); i++) {
    //only letters, numbers and underscores are allowed
    //reference: C++ Reference http://www.cplusplus.com/reference/cctype/isalnum/
    if (isalnum(var[i]) == 0 && var[i] != '_') {
      return false;
    }
  }
  return true;
}

//set command
void myShell::setvar() {
  size_t start = 0;
  size_t end = 0;
  //find 'set'
  start = command.find_first_not_of(' ', 0);
  end = command.find_first_of(' ', start);
  if (end == std::string::npos) {
    std::cout << "correct format is: set var value" << std::endl;
    return;
  }
  //find 'var'
  start = command.find_first_not_of(' ', end);
  end = command.find_first_of(' ', start);
  if (start == std::string::npos || end == std::string::npos) {
    std::cout << "correct format is: set var value" << std::endl;
    return;
  }
  std::string var = command.substr(start, end - start);
  if (!varname(var)) {
    std::cout << "not valid variable name" << std::endl;
    return;
  }
  //find 'value'
  start = command.find_first_not_of(' ', end);
  if (start == std::string::npos) {
    std::cout << "correct format is: set var value" << std::endl;
    return;
  }
  std::string value = command.substr(start);
  std::map<std::string, std::string>::iterator it = variablelist.find(var);
  if (it != variablelist.end()) {
    //refresh the value of variable
    variablelist.erase(it);
  }
  variablelist.insert(std::pair<std::string, std::string>(var, value));
  char * checkexport = getenv(var.c_str());
  int errorcheck = 0;
  //the variable has been exported into environment
  if (checkexport != NULL) {
    //refresh the value of variable in environment
    errorcheck = setenv(var.c_str(), value.c_str(), 1);
    if (errorcheck == -1) {
      std::cerr << "fail to export to environment" << std::endl;
    }
  }
}

//export command
void myShell::exportvar() {
  int errorcheck = 0;
  if (commandlist.size() == 1) {
    std::cout << "correct format is: export var" << std::endl;
  }
  else if (commandlist.size() > 2) {
    std::cout << "export : too many arguments" << std::endl;
  }
  else {
    std::string var = commandlist[1];
    //check variable name
    if (!varname(var)) {
      std::cout << "not valid variable name" << std::endl;
      return;
    }
    //find the current value of variable
    std::map<std::string, std::string>::iterator it = variablelist.find(var);
    //does not set before
    if (it == variablelist.end()) {
      //adds variable with its original value to environment and allows overwrite
      errorcheck = setenv(var.c_str(), var.c_str(), 1);
      return;
    }
    std::string value = it->second;
    //adds variable with its 'set' value to environment and allows overwrite
    errorcheck = setenv(var.c_str(), value.c_str(), 1);
  }
  if (errorcheck == -1) {
    std::cerr << "fail to export to environment" << std::endl;
  }
}

//check the value of a vriable, and return true if it represents a number in base10
bool myShell::numbervar(std::string & value) {
  for (size_t i = 0; i < value.length(); i++) {
    //reference: C++ Reference http://www.cplusplus.com/reference/cctype/isdigit/
    if (isdigit(value[i]) == EOF) {
      return false;
    }
  }
  return true;
}

//inc command
void myShell::incvar() {
  if (commandlist.size() == 1) {
    std::cout << "correct format is: inc var" << std::endl;
  }
  else if (commandlist.size() > 2) {
    std::cout << "inc : too many arguments" << std::endl;
  }
  else {
    std::string var = commandlist[1];
    //check variable name
    if (!varname(var)) {
      std::cout << "not valid variable name" << std::endl;
      return;
    }
    //check if variable is set before
    std::map<std::string, std::string>::iterator it = variablelist.find(var);
    //does not set before, then its value is treated as 0 and increase it to 1
    if (it == variablelist.end()) {
      variablelist.insert(std::pair<std::string, std::string>(var, "1"));
    }
    //variable is set before
    else {
      std::string value = it->second;
      //value of the variable does not represent a number in base10
      if (numbervar(value) == false) {
        //refresh the value of variable
        variablelist.erase(it);
        //treat the value as 0 and increase it to 1
        variablelist.insert(std::pair<std::string, std::string>(var, "1"));
      }
      else {
        std::string value = it->second;
        //value of the variable can be represented as  a number in base10
        int number = std::atoi(value.c_str()) + 1;
        std::stringstream stream;
        stream << number;
        value = stream.str();
        //refresh the value of variable
        variablelist.erase(it);
        //increase its value
        variablelist.insert(std::pair<std::string, std::string>(var, value));
      }
    }
  }
}

//ECECUTE-------------------------------------------------------------------------------------------------------------
//execute the process
void myShell::executeprocess(char ** newenviron) {
  //reference: code from man page
  char * filename = (char *)commandlist[0].c_str();
  int size = commandlist.size();
  char ** newargv = (char **)malloc((size + 1) * sizeof(*newargv));
  newargv[size] = NULL;
  for (int i = 0; i < size; i++) {
    newargv[i] = (char *)commandlist[i].c_str();
  }
  execve(filename, newargv, newenviron);
  std::cerr << "execve" << std::endl;
  free(newargv);
  exit(EXIT_FAILURE);
}

//wait the process
void myShell::parentprocess(pid_t w, pid_t fork_result, int wstatus) {
  do {
    //reference:code from man page
    w = waitpid(fork_result, &wstatus, WUNTRACED | WCONTINUED);
    if (w == -1) {
      std::cerr << "waitpid" << std::endl;
      return;
    }
    if (WIFEXITED(wstatus)) {
      std::cout << "Program exited with status" << WEXITSTATUS(wstatus) << std::endl;
    }
    else if (WIFSIGNALED(wstatus)) {
      std::cout << "killed by signal" << WTERMSIG(wstatus) << std::endl;
    }
  } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
}

//EXIT----------------------------------------------------------------------------------------------------------------
//check whether exit
bool myShell::checkrepeat() {
  return repeat;
}

//RESET---------------------------------------------------------------------------------------------------------------
//reset the class to wait for new command
void myShell::classreset() {
  command.clear();
  commandlist.clear();
  pathlist.clear();
  ECE551PATH.clear();
}
