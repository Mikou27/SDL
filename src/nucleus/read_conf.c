#include "read_conf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>  // For tolower()
#include <SDL.h>

char logFilePath[MAX_PATH_LENGTH]; // Global variable for log file path

// Initialize the log file path based on the DLL location
void initializeLogFilePath()
{
    char dllPath[MAX_PATH];

    // Get the full path of the current DLL
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        printf("Error: Could not get module handle\n");
        exit(1);
    }

    if (GetModuleFileName(hModule, dllPath, MAX_PATH) == 0) {
        printf("Error: Could not get DLL path\n");
        exit(1);
    }

    // Extract the directory from the DLL path
    char *lastBackslash = SDL_strrchr(dllPath, '\\');
    if (lastBackslash != NULL) {
        *(lastBackslash + 1) = '\0'; // Terminate the string at the last backslash
    }

    // Construct the full path to the log file
    snprintf(logFilePath, MAX_PATH_LENGTH, "%sSDL2.log", dllPath);

    printf("Log file path initialized: %s\n", logFilePath);
}

void stringToLower(char* str) {
    while (*str) {
        *str = tolower((unsigned char)*str);  // Convert each character to lowercase
        str++;  // Move to the next character
    }
}

char* concat(const char* s1, const char* s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// Global variable to track initialization state
static BOOL Inited = FALSE;

int* parseGamepadIDs(int* numIDs) {
    FILE* file = fopen("SDL2.ini", "r");
    if (!file) {
        perror("Failed to open configuration file");
        *numIDs = 0;
        return NULL;
    }

    int* ids = NULL;
    char line[256]; // Buffer to read each line
    *numIDs = 0;

    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';

        char* context = NULL; // Context for strtok_s
        char* token = strtok_s(line, " ", &context);
        while (token != NULL) {
            int* temp = realloc(ids, (*numIDs + 1) * sizeof(int));
            if (!temp) {
                free(ids);
                fclose(file);
                perror("Memory allocation failed");
                *numIDs = 0;
                return NULL;
            }

            ids = temp;
            ids[*numIDs] = strtol(token, NULL, 10); // Convert token to integer
            (*numIDs)++;

            token = strtok_s(NULL, " ", &context); // Continue tokenizing
        }
    }

    fclose(file);
    return ids;
}


// Read the INI file from the DLL directory and compute the sum of integers in it
char* readIniFileFromDll()
{
    if (!Inited) {
        initializeLogFilePath();
        Inited = TRUE;
    }

    static char dllPath[MAX_PATH];
    static char iniFilePath[MAX_PATH];
    static char buffer[1024];  // Buffer to store a line from the file

    // Get the full path of the current DLL
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        logMessage("Error: Could not get module handle");
        return "Error: Could not get module handle";
    }

    if (GetModuleFileName(hModule, dllPath, MAX_PATH) == 0) {
        logMessage("Error: Could not get DLL path");
        return "Error: Could not get DLL path";
    }

    // Extract the directory from the DLL path
    char* lastBackslash = SDL_strrchr(dllPath, '\\');
    if (lastBackslash != NULL) {
        *(lastBackslash + 1) = '\0'; // Terminate the string at the last backslash
    }

    // Construct the full path to the .ini file
    snprintf(iniFilePath, MAX_PATH, "%sSDL2.ini", dllPath);

    logMessage("Reading .ini file from: %s", iniFilePath);

    // Open the .ini file
    FILE* file = fopen(iniFilePath, "r");
    if (file == NULL) {
        logMessage("Error: Could not open file %s", iniFilePath);
        return "Error: Could not open file";
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        logMessage("Error: Failed to read from file %s", iniFilePath);
        fclose(file);
        return "Error: Failed to read from file";
    }

    fclose(file);

    // Remove newline character if present
    buffer[strcspn(buffer, "\r\n")] = '\0';

    // Return the content of the buffer as the string
    logMessage("Successfully read from .ini file: %s", buffer);
    return buffer;
}

// Logging function to log messages to the log file
void logMessage(const char *format, ...)
{
    if (!Inited) {
        initializeLogFilePath();
        Inited = TRUE;
    }

    FILE *logFile = fopen(logFilePath, "a");
    if (logFile == NULL) {
        printf("Error: Could not open log file for writing.\n");
        return;
    }

    // Get current time
    time_t currentTime = time(NULL);
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    // Write timestamp to the log file
    fprintf(logFile, "[%s] ", timeString);

    // Write the formatted log message
    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    // Newline for each log entry
    fprintf(logFile, "\n");

    fclose(logFile);
}
