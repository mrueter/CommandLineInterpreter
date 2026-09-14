// CommandLineInterpreter.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Names: baser Abrahim, Yuxuan Le, Michael Rueter
// Course/Section: CPSC351
// Assignment: Programming Assignment 1 - myShell

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
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

const int MAX_ARGS = 4; // argv[0] through argv[3]

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

// Splits the input line into up to MAX_ARGS tokens (argv[0]..argv[3]),
// same idea as using strtok() in C.
std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> argv;
    std::istringstream stream(input);
    std::string token;

    while (argv.size() < MAX_ARGS && (stream >> token)) {
        argv.push_back(token);
    }
    return argv;
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
        std::cout << "==> ";

        // Read full line of input
        if (!std::getline(std::cin, input)) {
            break;
        }

        input = trim(input);

        // Skip empty inputs
        if (input.empty()) {
            continue;
        }

        // Parse into argv[0]..argv[3] (up to 4 tokens total)
        std::vector<std::string> argv = parseCommand(input);

        // Check for exit or quit (no thread created for these)
        if (argv[0] == "exit" || argv[0] == "quit") {
            std::cout << "Thanks for using myShell!\n";
            break;
        }

        // Convert base command to lowercase for case-insensitive check
        std::string baseCommand = argv[0];
        std::transform(baseCommand.begin(), baseCommand.end(), baseCommand.begin(), ::tolower);

        if (ALLOWED_COMMANDS.find(baseCommand) == ALLOWED_COMMANDS.end()) {
            std::cout << "myShell: command not allowed: " << baseCommand << "\n";
            continue;
        }

        // Rebuild the parsed tokens (argv[0]..argv[3]) back into one string for system()
        std::string commandLine;
        for (size_t i = 0; i < argv.size(); i++) {
            commandLine += argv[i];
            if (i < argv.size() - 1) commandLine += " ";
        }

        // Prepare data for the child thread
        ThreadData data;
        strcpy_s(data.commandLine, commandLine.c_str());

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