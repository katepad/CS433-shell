/**
 * Assignment 2: Simple UNIX Shell
 * @file prog.cpp
 * @author Kate Paduganao, Jason Le
 * @brief This is the main function of a simple UNIX Shell. You may add
 * additional functions in this file for your implementation
 * @version 0.1
 */
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

#define MAX_LINE 80
//The maximum length command
#define READ_END 0
#define WRITE_END 1

//FUNCTION PROTOTYPES:
void input_for_file(char * file);
void output_for_file(char * file);

//ADDITIONAL FUNCTIONS:

/**
 * @brief parse out the command and arguments from the input command separated
 * by spaces
 * @param command The first argument which is the command (arg[0])
 * @param args The list of arguments of a given user input
 * @return int (The number of arguments of the user input)
 */
int parse_command(char command[], char *args[]) {

  int n = strlen(command); //Gets the size of the command
  //if the element at the end of the command is a new line then set the last element to null
  if (command[n - 1] == '\n') {
    command[n - 1] = '\0';
  }
  //Keeping track of how many arguments
  int arg_counter = 0;
  //Make a token to break up when there's spaces in the command
  char *str = strtok(command, " "); 
  //command is equal to the token
  command = str; 
  //In the argument array, the first index will be the command and the rest following will be the arguments
  args[0] = str;
  //While there are still commands and arguments in the array
  while (str != NULL) {
    //If the command contains < for input then run the input_for_file
    if(*str == '<'){
      input_for_file(strtok(NULL, " "));
    }
    //If the command contains > for output then run the output_for_file
     else if(*str == '>'){ 
      output_for_file(strtok(NULL, " "));
    }
      //There's none of the redirecting cases in the command
     else {
      //Whatever the index of the argument array is, will store str which might be the command or more arguments
      args[arg_counter] = str;
      //Increase the argument counter
      arg_counter++;
    }
    //Whenever there is a space, the str gets updated to hold whatever command/argument that would be
    str = strtok(NULL, " ");
  }
  return arg_counter; //return number of arguments
}

///////////////////////////////////////////////////////////////////////////////////////

/*
void pipe(char command[], char *args[], int fd[], pid_t pid, int num_args){
  
  // create the pipe
  if (pipe(fd) == -1) {
    fprintf(stderr,"Pipe failed");
    exit(1);
  }
 
   //fork a child process
   pid = fork();
 
  if (pid < 0) { //error occurred
    fprintf(stderr, "Fork Failed");
    exit(1);
  }
 
   if (pid > 0) { //parent process
     //close the unused end of the pipe
     close(fd[READ_END]);
    // write to the pipe
   for (int i = 0; i < num_args; i++) {
      write(fd[WRITE_END], args[i], strlen(args[i])+1);
   }
    // close the write end of the pipe
    close(fd[WRITE_END]);
   } else { // child process
     //close the unused end of the pipe
     close(fd[WRITE_END]);
 
     //read from the pipe
    for (int i = 0; i < num_args; i++) {
      read(fd[READ_END], args[i], MAX_LINE);
      printf("read %s",args[i]);
    }
     //close the read end of the pipe
     close(fd[READ_END]);
   }
}
*/

///////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief helper function that allows for redirection input when command has "<"
 * @param file This is a pointer which is the str token of the parse function.
 * @return void 
 */
void input_for_file(char * file) {
  //open a file that only reads storing it into a descriptor number
  int in = open(file, O_RDONLY);
  //dupilicate the file with the descriptor number previously
  dup2(in, STDIN_FILENO);
  //Closes the file
  close(in);
}

/**
 * @brief helper function that allows for redirection output when command has ">"
 * @param file This is a pointer which is the str token of the parse function.
 * @return void
 */
void output_for_file(char*file){
  //open a file that writes and creates and stores it into a descriptor number
  int out = open(file, O_WRONLY|O_CREAT);
  //dupilicate the file with the descriptor number previously
  dup2(out, STDOUT_FILENO);
  //Closes the file
  close(out);
}

/**
 * @brief copies the most recent command (excluding “!!”) for history to use later
 * @param command The first argument which is the command (arg[0])
 * @param args The list of arguments of a given user input
 * @param history_command the array that stores the previous input that is not "!!"
 * @return void
 */
void history_copy (char command[], char *args[], char history_command[]) {
  //sets the last character to be null
  command[strlen(command)-1] = '\0';
  //Checks to see if the command is !!
  if(strstr(command,"!!")){
    //Check to see if there's anything in the history array to copy
    if(strlen(history_command)!=0){
      //Prints the history command again
      printf("%s\n", history_command);
      //Copies the history_command into the command to re-execute
      strcpy(command, history_command);
    }else{
      //If there is no command history, print out this message
      cout << "No command history" << endl;
    }
  }
  //Checks to see if there's anything to copy in the command array
  if(strlen(command) > 0){
    //Copies the command into the history_command to store for later use
    strcpy(history_command, command);
  }
}


/**
 * @brief The main function of a simple UNIX Shell. You may add additional
 * functions in this file for your implementation
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The exit status of the program
 */
int main(int argc, char *argv[]) {

  char command[MAX_LINE];       // the command that was entered
  char *args[MAX_LINE / 2 + 1]; // hold parsed out command line arguments
  int should_run = 1;           // flag to determine when to exit program
  char history_command[MAX_LINE]; // command that will be stored for history

  //ADDITIONAL VARIABLES
  bool amp_flag = false; //this flag is used to check if there exists an "&" at the end of a command
  pid_t pid, pid1, pid2; //ProcessID for the child process/parent

  int fd[2]; //file descriptors for pipe.
  bool pipe_flag = false; //this flag is used to check if pipe func is to be called.
  int pipe_arg = -1; //index of where the pipe char '|' is.
  
  //START OF WHILE FUNCTION FOR SHELL
  while (should_run == 1) {
    
    printf("osh>");
    fflush(stdout);
    // Read the input command
    fgets(command, MAX_LINE, stdin);
    
    //call the history_copy command to make a copy of the command and its arguments onto a history command so that it can run when "!!" is called.
    history_copy (command, args, history_command);
    // Parse the input command
    int num_args = parse_command(command, args);

/////////////////////////////////////////////////////////////////////////////////////
  
    //Check for "|" which would call for the pipe.
    for (int i = 0; i < num_args; i++) {
      if (strstr(args[i], "|")) {
        pipe_flag = true;
        pipe_arg = i;
        break;
      }
    }

    //if pipe char is found:
    if (pipe_flag == true){
      //Separate out the side left and right sides of the pipe
      if(pid1 == fork() > 0){
        close(fd[READ_END]);
        write(fd[WRITE_END], command, sizeof(*command)+1);
        close(fd[WRITE_END]);
      }else{
        close(fd[WRITE_END]);
        read(fd[READ_END], args, MAX_LINE);
        close(fd[READ_END]);
      }
      if(pid2 == fork() == 0){
        close(fd[READ_END]);
        write(fd[WRITE_END], command, sizeof(*command)+1);
        close(fd[WRITE_END]);
      }else{
        close(fd[WRITE_END]);
        read(fd[READ_END], args, MAX_LINE);
        close(fd[READ_END]);
      }  
    }else { //If command is not pipe, execute command normally:

      // find if the command is exit, if so then we exit out of the shell
      if (strstr(command, "exit")) {
        exit(0);
        should_run = 0;
      }
      
      // If the parent process doesn't have & at the end then wait else we call it
      if(strstr(args[num_args-1],"&")){
        //Remove the & at the end of the command and sets the flag to true
        args[num_args-1] = NULL;
        amp_flag = true;
      }else{
        //No & but still set the last element to NULL
        args[num_args] = NULL;
        amp_flag = false;
      }
      
      //If command is not pipe, execute command normally:
      if (pipe_flag == false) {
        pid = fork();
        if (pid == 0) { //child process
          //executes the child process
          execvp(command, args);
          //When executed and command and arguments are less than 0, then invalid command prompt.
          if (execvp(command, args) < 0){
              cout << "Command not found" << endl;
              exit(0);
          }
        } else if (pid > 0) { //parent process
          //If command does not contain "&", the parent function will wait for child.
          if(amp_flag == false){
          wait(NULL);
          }
        } else if (pid < 0){ //error, if the pid is negative
          cout << "Error";
          exit(1);
        }
      }
    }
  }
  return 0;
}
