#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#define MAX_PATH_LENGTH 260

// Global variable to store the log file path
extern char logFilePath[MAX_PATH_LENGTH];

// Function to initialize the log file path
void initializeLogFilePath();

// Logging function
void logMessage(const char *format, ...);

// Function to read an INI file and calculate the sum of integers
char *GetIniPath();
int* parseGamepadIDs(int *numIDs);

void stringToLower(char *str);

char *concat(const char *s1, const char *s2);

#endif // LOGGER_H
