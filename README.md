Kate Paduganao, Jason Le
October 2023

  Basic Unix Shell

How to Compile and Run:
  - To compile the program, type the command “make prog”
  - To run the program, type the command “./prog”

Results (Outputs of Given Commands):
  Our program is able to execute basic UNIX shell commands. These commands include:
   - ls -l: When this command is called, it will list the file contents of the program’s
      current directory in a long listing format. When the command is called, it will show
      the file’s permissions, name of the file’s owner, file size, date and time of when
      the file was last modified, and file name.
  - !!: This command calls the history function which will redo (reprint) the last
      command. If there aren’t any past commands before “!!” is called, it should print
      out “No command history”.
  - date: When this command is called, it will show the current date and time (UTC).
  - grep: These commands look for any text patterns of a given file name.
  - ps -aef: This command will show the process information and statuses of all
    processes that are running in the system. This includes process UID, PID, PPID,
    start time, command, etc.
  - “>” and “<” redirection: When the output redirection argument, “>” gets called,
    the shell would redirect the output of the command into a file. Similarly, when the
    input redirection argument “<” gets called, the shell would redirect the input from
    a file to a command.
  - |: The results of the pipe command is that it will execute the command on the left
    side of the ‘|’ i.e ls -l and then execute the right side of the ‘|’ which we used less
    for our testing.
  Our program also checks and deals with invalid commands. If an invalid command is
  inputted in the shell, it should print out “Command not found.”
  Additionally, the program checks if there is an “&” at the end of a command. If there
  exists an “&”, the parent and child processes will run concurrently with each other;
  otherwise, the parent will wait for the child process to finish before executing.

Features Implemented:
Parse Function
  - parse_command Function: This function takes the parameters “char
    command[]” and “char *args[]”. This function parses a user input and creates
    tokens which are stored in an array of characters.
  - Redirection of input and output: The parse command then checks if there is
    either a “<” or “>” character in the given user input. If there is a “<” character as
    one of the arguments, it calls the output_for_file function. If there is a “>”
    character as one of the arguments, it calls the input_for_file function. See the
    details of those functions down below under the “Additional Functions” section.
Additional Functions
  - history_copy Function: This function takes the parameters “char command[]”,
    “char *args[]”, and “char history_command[]”. This helper function copies the
    most recent command (that is not “!!”) for the history feature to then later use.
  - output_for_file Function: This helper function takes the parameter “char * file”.
    This function copies the output of a command into a file.
  - input_for_file Function: This helper function takes the parameter “char * file”.
    This function opens a file and does the sort command.
Required Features within Main
  - History feature: This feature is implemented after the input is read and stored
    using the history_copy Function. Because of the way the history_copy function is
    implemented, the history command itself won’t be “!!”, or else the history feature
    won’t run correctly. Then, the history feature will run when the user calls “!!”, and
    it will re-execute the most recent command that is not “!!”.
  - “&” Checker: The “&” checker is a feature that checks to see if “&” is included in
      a command. In summary, if there exists an “&” in a command, the checker will
      then take out “&” from the set of arguments, and set a flag to true. Later, if the
      flag is equal to true, the parent function has to call wait.
  - Exit function: If the command is exit, then it should exit the shell program.
  - Executing a command in the child process: With the use of the fork() function,
      we create a child process that will execute with the first argument of char * args[],
      being the command. Then, if the command has “&” then the parent doesn’t have
      to wait for the child process.
  - Pipe feature: When the command has the character ‘|’, the program would split
      the argument into separate arrays, left of the ‘|’ and right of the ‘|’. Then
      separated process would be created using the fork() function, one child for each
      side of the argument. Then both side of the argument would be executed.
