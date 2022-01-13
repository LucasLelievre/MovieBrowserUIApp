#include <iostream>

#include <AppCore/AppCore.h>

#include "DirScanner.h"
#include "SubProcess.h"

class JScallbackFunctions {
private:
    /* data */
public:
    JScallbackFunctions(/* args */);
    ~JScallbackFunctions();

    JSObjectRef scanDirFunc(JSContextRef ctx, JSStringRef funcName);
    JSObjectRef startProg(JSContextRef ctx, JSStringRef funcName);
};