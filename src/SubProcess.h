// win code from    https://docs.microsoft.com/en-gb/windows/win32/procthread/creating-processes?redirectedfrom=MSDN
// linux code from  
#include <iostream>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <spawn.h>
    #include <sys/wait.h> 
    extern char **environ;
#endif


class SubProcess {
public:
    SubProcess();
    ~SubProcess();

    static void CreateSubProcess(std::string prog, std::string argv);
};