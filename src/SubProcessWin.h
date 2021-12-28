// code from https://docs.microsoft.com/en-gb/windows/win32/procthread/creating-processes?redirectedfrom=MSDN
#include <windows.h>
#include <iostream>

class SubProcessWin {
public:
    SubProcessWin();
    ~SubProcessWin();

    static void CreateSubProcess(const char* argv);
};