// CommandLineInterpreter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

// Allowed list of commands
const std::set<std::string> ALLOWED_COMMANDS = {
    "dir",
    "help",
    "vol",
	"path",
	"tasklist",
	"notepad",
    "echo",
	"color",
    "ping"
};

// Structure to pass command string to the thread function
struct ThreadData {
    char commandLine[MAX_PATH];
};

// Thread function that executes the system command
DWORD WINAPI ExecuteCommandThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;

    // Use system() to run the command via the Windows command processor
    int result = system(data->commandLine);

    return (DWORD)result;
}

// Function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main() {
    std::string input;

    std::cout << "========================================\n";
    std::cout << "           Welcome to myShell           \n";
    std::cout << "========================================\n";
    std::cout << "Allowed commands: ";
    for (const auto& cmd : ALLOWED_COMMANDS) {
        std::cout << cmd << " ";
    }
    std::cout << "\n\n";

    while (true) {
        // Display prompt
        std::cout << "myShell> ";

        // Read full line of input
        if (!std::getline(std::cin, input)) {
            break;
        }

        input = trim(input);

        // Skip empty inputs
        if (input.empty()) {
            continue;
        }

        // Check for exit or quit
        if (input == "exit" || input == "quit") {
            std::cout << "Thanks for using myShell! Goodbye!\n";
            break;
        }

        // Parse the base command (first token) to validate against allowed list
        std::string baseCommand = input.substr(0, input.find(' '));

        // Convert base command to lowercase for case-insensitive check
        std::transform(baseCommand.begin(), baseCommand.end(), baseCommand.begin(), ::tolower);

        if (ALLOWED_COMMANDS.find(baseCommand) == ALLOWED_COMMANDS.end()) {
            std::cout << "myShell: command not allowed: " << baseCommand << "\n";
            continue;
        }

        // Prepare data for the child thread
        ThreadData data;
        strcpy_s(data.commandLine, input.c_str());

        // Create the child thread using CreateThread()
        HANDLE hThread = CreateThread(
            NULL,                   // Default security attributes
            0,                      // Default stack size
            ExecuteCommandThread,   // Thread function
            &data,                  // Parameter to thread function
            0,                      // Default creation flags
            NULL                    // Receive thread identifier
        );

        if (hThread == NULL) {
            std::cerr << "myShell: Failed to create thread. Error: " << GetLastError() << "\n";
            continue;
        }

        // Wait for the child thread to finish execution
        WaitForSingleObject(hThread, INFINITE);

        // Close thread handle to clean up resources
        CloseHandle(hThread);

        std::cout << "\n"; // Extra spacing after command output
    }

    return 0;
}