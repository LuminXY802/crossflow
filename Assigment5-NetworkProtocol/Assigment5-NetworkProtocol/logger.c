#include <stdio.h>

void logAIUsage(const char* prompt, const char* response) {
    FILE* logFile = fopen("AI_Log.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "Prompt: %s\nResponse: %s\n\n", prompt, response);
        fclose(logFile);
    }
}