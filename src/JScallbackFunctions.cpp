#include "JScallbackFunctions.h"

JScallbackFunctions::JScallbackFunctions(/* args */) {}

JScallbackFunctions::~JScallbackFunctions() {}

/**
 * @brief Scan the inside of a directory
 * 
 * @param ctx JS contexte
 * @param funcName name of the JS function to associate with the callback
 * @return JSObjectRef 
 */
JSObjectRef JScallbackFunctions::scanDirFunc(JSContextRef ctx, JSStringRef funcName){
    return JSObjectMakeFunctionWithCallback(ctx, funcName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) -> JSValueRef {
      // TODO scan dirs
      if (argumentCount > 0) {
        // Check if arguments are strings
        if (JSValueIsString(ctx, arguments[0])) {
          ultralight::JSString pathArg;
          std::vector<std::string> paths;
          // Read the paths arguments
          for (size_t i = 0; i < argumentCount; i++) {
            pathArg = JSValueToStringCopy(ctx, arguments[i], NULL);
            paths.push_back(ultralight::String(pathArg).utf8().data());
            std::cout << paths.back() << std::endl;
          }

          std::string subList = DirScanner::scanPaths(paths);
          JSValueRef output = JSValueMakeFromJSONString(ctx, JSStringCreateWithUTF8CString(subList.c_str()));
          return output;
        } else {
          std::cerr << "argument is not a string\n" << std::endl;
        }
      } else {
        std::cerr << "no argument were given\n" << std::endl;
      }
      return JSValueMakeNull(ctx);
  });
}

/**
 * @brief Start a program (vlc or firefox)
 * 
 * @param ctx JS context
 * @param funcName name of the JS function to associate with the callback
 * @return JSObjectRef 
 */
JSObjectRef JScallbackFunctions::startProg(JSContextRef ctx, JSStringRef funcName){
    return JSObjectMakeFunctionWithCallback(ctx, funcName,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) -> JSValueRef {
        if (argumentCount > 1) {
            // Check if arguments are strings
            if (JSValueIsString(ctx, arguments[0]) && JSValueIsString(ctx, arguments[1])) {
            std::string prog = "";
            std::string arg = "";
            #ifdef _WIN32
                // program path
                ultralight::JSString argJS = JSValueToStringCopy(ctx,arguments[0], NULL);
                prog = ultralight::String(argJS).utf8().data();
                if (prog.compare("vlc") == 0)   prog = "C:\\Program Files\\VideoLAN\\VLC\\vlc.exe";
                else                            prog = "C:\\Program Files\\Mozilla Firefox\\firefox.exe";
                // command line argument
                argJS = JSValueToStringCopy(ctx, arguments[1], NULL);
                arg.append("\"").append(ultralight::String(argJS).utf8().data()).append("\"");
                // std::string commandLineStr = prog.append(" \"").append(ultralight::String(commandLineArg).utf8().data()).append("\"");
                // std::cout << commandLineStr << std::endl;
                // SubProcess::CreateSubProcess(commandLineStr.c_str());
                // SubProcess::CreateSubProcess(prog, arg);
                // std::cout << "ding ding" << std::endl;
                // SubProcessWin::Close();
            #else
                // program path
                ultralight::JSString argJS = JSValueToStringCopy(ctx,arguments[0], NULL);
                prog = std::string("/usr/bin/").append(ultralight::String(argJS).utf8().data());
                // command line argument
                argJS = JSValueToStringCopy(ctx,arguments[1], NULL);
                arg = ultralight::String(argJS).utf8().data();
            #endif
            SubProcess::CreateSubProcess(prog, arg);
            } else {
            std::cerr << "arguments are incorrect (not strings)" << std::endl;
            }
        } else {
            std::cerr << "no arguments provided" << std::endl;
        }
        return JSValueMakeNull(ctx);
    });
}