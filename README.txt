Programming Assignment 1
Command Line Interpreter myShell

Student/Group Information
-------------------------
Group: 1
Students:
- Baser Abrahim
- Yuxuan(Jack) He
- Bryant Hernandez
- Huu Ngoc Nguyen
- Michael Rueter
Course/Section: CPSC 351 - Section 18102
Assignment: Programming Assignment 1 - Command Line Interpreter (myShell)

Files Included
--------------
- CommandLineInterpreter/CommandLineInterpreter.cpp - Contains the myShell command loop, command parsing and validation, worker-thread function, synchronization, and command dispatch.
- CommandLineInterpreter.slnx - Visual Studio solution file for opening the project.
- CommandLineInterpreter/CommandLineInterpreter.vcxproj - Visual Studio C++ project configuration.
- CommandLineInterpreter/CommandLineInterpreter.vcxproj.filters - Visual Studio project filter definitions.
- myShell.exe - Windows executable produced from CommandLineInterpreter.cpp.
- README.txt - Provides project information, build and run instructions, design notes, testing results, limitations, and team contributions.
- screenshots/Screenshot1 - dir.png - Shows a supported command with no arguments.
- screenshots/Screenshot2 - ping.png - Shows a supported command with a host argument.
- screenshots/Screenshot3 - echo.png - Shows the echo command and its output.
- screenshots/Screenshot4 - color.png - Shows the color command changing the console color.
- screenshots/Screenshot5 - invalidcommand.png - Shows rejection of an unsupported command while myShell continues running.
- screenshots/Screenshot6 - exit.png - Shows clean termination and the closing message.

How to Compile
--------------
The verified build used MSYS2 MinGW-w64 g++ 15.2.0 on Windows. From the repository root, run:

g++ -std=c++20 -Wall -Wextra -pedantic CommandLineInterpreter\CommandLineInterpreter.cpp -o myShell.exe

A successful compilation creates myShell.exe in the repository root. The source can also be opened with CommandLineInterpreter.slnx in Visual Studio with the Desktop development with C++ workload installed.

How to Run
----------
Open Command Prompt or PowerShell in the directory containing myShell.exe and run:

Command Prompt: myShell.exe
PowerShell:     .\myShell.exe

The program displays a welcome message and the "==> " prompt. Enter one supported command at each prompt. Enter exit or quit to terminate myShell.

Supported Commands
------------------
- dir - Displays the current directory listing. Syntax: dir
- help - Displays Windows command help. Syntax: help
- vol - Displays the disk volume label and serial number. Syntax: vol
- path - Displays or sets the executable search path. Syntax: path
- tasklist - Displays running processes. Syntax: tasklist
- notepad - Opens Windows Notepad. Syntax: notepad
- echo - Displays the supplied text. Syntax: echo <text>
- color - Changes the console foreground/background color. Syntax: color <hex value>
- ping - Sends ICMP echo requests to a host. Syntax: ping <host>
- exit or quit - Terminates myShell without creating a worker thread.

myShell accepts a maximum of four whitespace-separated tokens per input line: the command plus up to three additional arguments.

Threading Design
----------------
After main() reads and parses an input line, it handles exit and quit directly and rejects commands that are not in the allowed-command set. For every valid executable command, main() stores the reconstructed command line in a ThreadData structure and calls CreateThread(). The worker executes ExecuteCommandThread(), which invokes the command through the Windows command processor with system(). The parent thread calls WaitForSingleObject(hThread, INFINITE), so myShell does not display another prompt until the worker finishes. The parent then calls CloseHandle(hThread) to release the thread handle. No child thread is created for exit, quit, empty input, or unsupported commands.

Testing
-------
The program was compiled on Windows with the g++ command shown above and completed the following tests with process exit code 0:

1. No-argument command - Entered dir. The directory listing appeared and myShell returned to the prompt. See Screenshot 1.
2. Command with an argument - Entered ping 192.168.1.1. Windows ping ran with the supplied address, printed its statistics, and myShell waited for it to finish before displaying another prompt. See Screenshot 2.
3. Text argument parsing - Entered echo Hello. The program displayed Hello and returned to the prompt. See Screenshot 3.
4. Additional supported command - Entered color 9. Windows processed the command and changed the console color. See Screenshot 4.
5. Unsupported command - Entered invalidCommand. myShell displayed "myShell: command not allowed: invalidcommand" and continued running. See Screenshot 5.
6. Termination - Entered exit. myShell displayed "Thanks for using myShell!" and terminated cleanly without creating a worker thread. See Screenshot 6.

The valid commands dir, ping, echo, and color were also executed consecutively in one session. Each command completed before the next prompt appeared, demonstrating that the parent waits for every child worker thread.

Known Limitations
-----------------
- The implementation is Windows-specific because it uses windows.h, CreateThread(), WaitForSingleObject(), and Windows commands.
- Input is split on whitespace and does not preserve quoted arguments containing spaces.
- Only four tokens are retained, including the command; additional tokens are ignored.
- Commands run through system(), so their behavior depends on the Windows command processor and local environment.
- Network results for ping depend on the selected host, network connection, and firewall rules.
- notepad keeps myShell waiting until Notepad closes because the parent waits for the worker command to finish.

Team Contributions
------------------
- Michael Rueter - Created the repository and Visual Studio project and implemented the initial shell, command execution, and Windows-threading structure.
- Baser Abrahim - Added argument parsing for up to four tokens and updated the source-file member information.
- Yuxuan He - Completed README.txt and organized the compile, run, testing, and submission documentation.
- Bryant Hernandez - Captured and added the six command-test screenshots.
- Huu Ngoc Nguyen - N/A
