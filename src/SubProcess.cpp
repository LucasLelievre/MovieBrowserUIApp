#include "SubProcess.h"

SubProcess::SubProcess() {
}

SubProcess::~SubProcess() {
}


void SubProcess::CreateSubProcess(std::string prog, std::string args) {
    std::cout << prog << ' ' << args << std::endl;
    #ifdef _WIN32

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
    #else
        std::cout << "Starting program with posix_spawn\n";

        pid_t pid;
        int status;

        fflush(NULL);
        
        char* argV[] = {prog.data(), args.data(), (char *) 0};
        status = posix_spawn(&pid, argV[0], NULL, NULL, argV, environ);
        
        if (status == 0) {
            std::cout << "Child id: " << pid << '\n';
            fflush(NULL);
            if (waitpid(pid, &status, 0) != -1) {
                std::cout << "Child exited with status: " << status << '\n';
            } else {
                perror("waitpid");
            }
        } else {
            std::cout << "posix_spawn: "<< status << '\n';
        }
    #endif
}