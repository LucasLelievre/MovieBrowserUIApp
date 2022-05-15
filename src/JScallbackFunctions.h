#include <iostream>

#include <AppCore/AppCore.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "DirScanner.h"
#include "JsonControler.h"

class JScallbackFunctions {
private:
    /* data */
public:
    JScallbackFunctions(/* args */);
    ~JScallbackFunctions();

    JSObjectRef scanDirectory(JSContextRef ctx, JSStringRef funcName);
    JSObjectRef openFile(JSContextRef ctx, JSStringRef funcName);
    JSObjectRef saveSettings(JSContextRef ctx, JSStringRef funcName);
};