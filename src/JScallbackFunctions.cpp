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
JSObjectRef JScallbackFunctions::scanDirectory(JSContextRef ctx, JSStringRef funcName) {
  return JSObjectMakeFunctionWithCallback(ctx, funcName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) -> JSValueRef {
    // TODO scan dirs
    if (argumentCount > 0) {
      // path array to string
      JSStringRef jsstrref = JSValueToStringCopy(ctx, arguments[0], NULL);
      ultralight::JSString jsstr = ultralight::JSString(jsstrref);
      ultralight::String str = ultralight::String(jsstr);
      std::string pathsStr = str.utf8().data();
      std::cout << pathsStr << std::endl;

      // sting to array of strings
      std::vector<std::string> paths;
      size_t start = 0;
      size_t pos = pathsStr.find(',', start);;
      while (pos < pathsStr.size()) {
        paths.push_back(pathsStr.substr(start, pos-start));
        start = pos+1;
        pos = pathsStr.find(',', start);
      }
      paths.push_back(pathsStr.substr(start, pos-start));
      
      // scan paths
      std::cout << "Scanning " << paths.size() << " directories in c++ : " << std::endl;
      std::string subList = DirScanner::scanPaths(paths);
      // create json value from string
      JSValueRef output = JSValueMakeFromJSONString(ctx, JSStringCreateWithUTF8CString(subList.c_str()));
      // return json
      return output;

    } else {
      std::cerr << "no argument were given\n" << std::endl;
    }
    return JSValueMakeNull(ctx); });
}

/**
 * @brief Start a program (vlc or firefox)
 *
 * @param ctx JS context
 * @param funcName name of the JS function to associate with the callback
 * @return JSObjectRef
 */
JSObjectRef JScallbackFunctions::openFile(JSContextRef ctx, JSStringRef funcName) {
  return JSObjectMakeFunctionWithCallback(ctx, funcName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) -> JSValueRef {
    if (argumentCount >= 1) {
      // Check if arguments are strings
      if (JSValueIsString(ctx, arguments[0])) {
        #ifdef _WIN32
          // Start with shellexecute (default program)
          ultralight::JSString argJS = JSValueToStringCopy(ctx, arguments[0], NULL);
          std::string arg = ultralight::String(argJS).utf8().data();
          std::cout << "openning : " << arg << std::endl;

          ShellExecute(NULL, "open", arg.c_str(), NULL, NULL, SW_SHOWNORMAL);
        #else
          // program path
          // ultralight::JSString argJS = JSValueToStringCopy(ctx, arguments[0], NULL);
          // std::string prog = std::string("/usr/bin/").append(ultralight::String(argJS).utf8().data());
          // command line argument
          // SubProcess::CreateSubProcess(prog, arg);

          std::string command = "xdg-open \"";
          ultralight::JSString argJS = JSValueToStringCopy(ctx, arguments[1], NULL);
          command.append(ultralight::String(argJS).utf8().data());
          command.append("\"");
          std::cout << command.c_str() << std::endl;
          system(command.c_str());

        #endif
      } else {
        std::cerr << "arguments are incorrect (not strings)" << std::endl;
      }
    } else {
      std::cerr << "no arguments provided" << std::endl;
    }
    return JSValueMakeNull(ctx);
  });
}

JSObjectRef JScallbackFunctions::saveSettings(JSContextRef ctx, JSStringRef funcName) {
  return JSObjectMakeFunctionWithCallback(ctx, funcName, [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) -> JSValueRef {
    if (argumentCount > 0) {
      if (JSValueIsString(ctx, arguments[0])) {
        // json from ultralight's JSValue to std string
        JSStringRef jsstrref = JSValueToStringCopy(ctx, arguments[0], NULL);
        ultralight::JSString jsstr = ultralight::JSString(jsstrref);
        ultralight::String str = ultralight::String(jsstr);

        // make a JSON object with a property paths and the array
        std::string json = JsonControler::makeObject(std::vector<std::string> {JsonControler::makeProperty("paths", str.utf8().data())});

        std::cout << "writting json to file : " << json << std::endl;
        if (JsonControler::writeFile("..\\..\\assets\\settings\\settings.json", json))
          std::cout << "success" << std::endl;
        else
          std::cout << "failure" << std::endl;

      } else {
        std::cerr << "argument is not string" << std::endl;
      }
    } else {
      std::cerr << "no arguments provided" << std::endl;
    }
    return JSValueMakeNull(ctx);
  });
}