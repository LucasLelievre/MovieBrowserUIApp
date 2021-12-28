#include "SubProcessWin.h"

SubProcessWin::SubProcessWin() {
}

SubProcessWin::~SubProcessWin() {
}

void SubProcessWin::CreateSubProcess(const char* argv) {
    char* commandLine = new char[strlen(argv)];
    strcpy(commandLine, argv);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        commandLine,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

// void SubProcess::CreateSubProcess(){
//     pid_t pid;
//     char *argv[] = {"ls", (char *) 0};
//     int status;
//     puts("Testing posix_spawn");
//     fflush(NULL);
//     status = posix_spawn(&pid, "/bin/ls", NULL, NULL, argv, environ);
//     if (status == 0) {
//         printf("Child id: %i\n", pid);
//         fflush(NULL);
//         if (waitpid(pid, &status, 0) != -1) {
//             printf("Child exited with status %i\n", status);
//         } else {
//             perror("waitpid");
//         }
//     } else {
//         printf("posix_spawn: %s\n", strerror(status));
//     }
// }