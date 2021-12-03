#include "MovieBrowser.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

MovieBrowser::MovieBrowser() {
  ///
  /// Create our main App instance.
  ///
  app_ = App::Create();

  ///
  /// Create a resizable window by passing by OR'ing our window flags with
  /// kWindowFlags_Resizable.
  ///
  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
    false, kWindowFlags_Resizable | kWindowFlags_Maximizable | kWindowFlags_Titled);

  #ifdef __linux__
  	GLFWwindow* win = reinterpret_cast<GLFWwindow*>(window_->native_handle());
    glfwMaximizeWindow(win);
  #endif
  #ifdef _WIN32
    auto hwnd_ = reinterpret_cast<HWND>(window_->native_handle());
    ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
  #endif
 
  ///
  /// Create our HTML overlay-- we don't care about its initial size and
  /// position because it'll be calculated when we call OnResize() below.
  ///
  overlay_ = Overlay::Create(*window_.get(), 1, 1, 0, 0);

  ///
  /// Force a call to OnResize to perform size/layout of our overlay.
  ///
  OnResize(window_.get(), window_->width(), window_->height());

  ///
  /// Load a page into our overlay's View
  ///
  overlay_->view()->LoadURL("file:///app.html");

  ///
  /// Register our MovieBrowser instance as an AppListener so we can handle the
  /// App's OnUpdate event below.
  ///
  app_->set_listener(this);

  ///
  /// Register our MovieBrowser instance as a WindowListener so we can handle the
  /// Window's OnResize event below.
  ///
  window_->set_listener(this);

  ///
  /// Register our MovieBrowser instance as a LoadListener so we can handle the
  /// View's OnFinishLoading and OnDOMReady events below.
  ///
  overlay_->view()->set_load_listener(this);

  ///
  /// Register our MovieBrowser instance as a ViewListener so we can handle the
  /// View's OnChangeCursor and OnChangeTitle events below.
  ///
  overlay_->view()->set_view_listener(this);
}

MovieBrowser::~MovieBrowser() {
}

void MovieBrowser::Run() {
  app_->Run();
}

void MovieBrowser::OnUpdate() {
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MovieBrowser::OnClose(ultralight::Window* window) {
  app_->Quit();
}

void MovieBrowser::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MovieBrowser::OnFinishLoading(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) {
  ///
  /// This is called when a frame finishes loading on the page.
  ///
}

JSValueRef systemCommand(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef *arguments, JSValueRef *exception) {
  // check if argument is really a string
  if (argumentCount > 1) {
    if (JSValueIsString(ctx, arguments[0]) && JSValueIsString(ctx, arguments[1])) {
      // create the system command
      ultralight::String command = ultralight::String(JSValue(arguments[0]))
                                  + ultralight::String(" '")
                                  + ultralight::String(JSValue(arguments[1]))
                                  + ultralight::String("'");
                                  // example : "prog 'filepath'"
      std::cout << command.utf8().data() << std::endl;
      // execute the command
      system(command.utf8().data());
    } else {
      std::cerr << "arguments are incorrect (not strings)" << std::endl;
    }
  } else {
    std::cerr << "no arguments provided" << std::endl;
  }
  return JSValueMakeNull(ctx);
}

void MovieBrowser::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  /// This is called when a frame's DOM has finished loading on the page.

  JSContextRef ctx = caller->LockJSContext().get();

  // Add the folder paths that will be scanned
  this->addPath("/media/lucas/BAT-external disk/video/films");
  // this->addPath("/home/lucas/Videos");
  this->addPath("D:\\video\\films");
  this->addPath("C:\\Users\\Lucas\\Videos");

  // Scann the paths
  std::string scanData = this->scanPaths();
  std::cout << scanData << std::endl;

  // Create the js function call

  // argument 1 : JSON data of scanned files
  // JSValueRef arg1 = JSValueMakeFromJSONString(ctx, JSStringRef(JSString("{\"content\": [{\"type\": \"movie\",\"name\": \"the Matrix\",\"file name\": \"the Matrix (1999).mkv\", \"fileYear\":\"1999\"}]}")));
  JSValueRef arg1 = JSValueMakeFromJSONString(ctx, JSStringRef(JSString(scanData.c_str())));
  // JSValueRef arg1 = JSValueMakeString(ctx, JSStringRef(JSString(scanData.c_str())));
  if (JSValueIsNull(ctx, arg1)) std::cout << "json error\n";
  // argument 2 : HTML object that will contain the 
  JSValueRef arg2 = JSValueMakeString(ctx, JSStringRef(JSString("movieList")));
  // argument 3 : sorting function
  JSValueRef arg3 = JSValueMakeString(ctx, JSStringRef(JSString("title")));

  // Call the function
  JSValueRef args[] = {arg1, arg2, arg3};
  this->EvaluateJsFunc(caller, "refreshMovieCards", 3, args);

  // ultralight::String mockScan = "refreshMovieCards(JSON.parse(\"{\\\"content\\\": [{\\\"type\\\": \\\"movie\\\",\\\"name\\\": \\\"the Matrix\\\",\\\"file name\\\": \\\"Matrix (1999).mkv\\\"}]}\"), document.getElementById(\"movieList\"), sortByOriginalTitle)";
  // ultralight::String jsFunc = "refreshMovieCards(JSON.parse(\"" + ultralight::String(scanData.c_str()) + "\"), document.getElementById(\"movieList\"), sortByOriginalTitle)";
  // ultralight::String scriptExcep;
  //caller->EvaluateScript(jsFunc, &scriptExcep);
  // if (!scriptExcep.empty())
    // std::cerr << "exception : " << scriptExcep.utf8().data() << std::endl;

  // Set the global event listeners 
  caller->EvaluateScript("setEventListeners()");
  
  // Add callback to start another programm from js
  JSStringRef funcName = JSStringCreateWithUTF8CString("systemCommand");
  JSObjectRef funcCallBack = JSObjectMakeFunctionWithCallback(ctx, funcName, systemCommand);
  JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
  JSObjectSetProperty(ctx, globalObj, funcName, funcCallBack, 0, 0);
  JSStringRelease(funcName);
}

void MovieBrowser::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(kCursor_Pointer);
}

void MovieBrowser::OnChangeTitle(ultralight::View* caller,
                          const String& title) {
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}

std::string MovieBrowser::getJSValueRefString(JSContextRef ctx, JSValueRef value) {
  if (JSValueIsString(ctx, value)) {
    std::cerr << "value is not string" << std::endl;
    return "";
  }
  JSValueRef * ex = 0;
  JSStringRef exceptionStr = JSValueToStringCopy(ctx, value, ex);
  if (ex) {
    std::cerr << "error reading value" << std::endl;
    return "";
  } else {
    return ultralight::String(JSString(exceptionStr)).utf8().data();
  }
}

/**
 * @brief Calls a JS function
 * 
 * @param caller js context view
 * @param funcName name of the function to call
 * @param argc number of parameters
 * @param argv array of parameters
 * @return true script was executed without errors
 * @return false script failed
 */
bool MovieBrowser::EvaluateJsFunc(ultralight::View* caller, const char * funcName, int argc, JSValueRef * argv) {
  // Acquire the JS execution context for the current page.
  Ref<JSContext> context = caller->LockJSContext();
  // Get the underlying JSContextRef for use with the JavaScriptCore C API.
  JSContextRef ctx = context.get();
  
  // Create our string of JavaScript, automatically managed by JSRetainPtr
  JSRetainPtr<JSStringRef> str = adopt(JSStringCreateWithUTF8CString(funcName));
    // Evaluate the function name
  JSValueRef func = JSEvaluateScript(ctx, str.get(), 0, 0, 0, 0);

  // Check if 'func' is actually an Object and not null
  if (JSValueIsObject(ctx, func)) {
    // Cast 'func' to an Object, will return null if typecast failed.
    JSObjectRef funcObj = JSValueToObject(ctx, func, 0);
    // Check if 'funcObj' is a Function and not null
    if (funcObj && JSObjectIsFunction(ctx, funcObj)) {
      // Create our list of arguments
      // JSValueRef args[argc] = {};

      // for (size_t i = 0; i < argc; i++) {
        // Create a JS string from null-terminated UTF8 C-string
        // JSRetainPtr<JSStringRef> msg = adopt(JSStringCreateWithUTF8CString(argv[i]));
        // args[i] = JSValueMakeString(ctx, msg.get());
      // }

      // Count the number of arguments in the array.
      // size_t num_args = sizeof(args) / sizeof(JSValueRef*);

      // Create a place to store an exception, if any
      JSValueRef exception = 0;

      // Call the ShowMessage() function with our list of arguments.
      JSValueRef result = JSObjectCallAsFunction(ctx, funcObj, 0, 
                                                 argc, argv, 
                                                 &exception);

      if (exception) {
        std::cerr << "exception : " << this->getJSValueRefString(ctx, exception) << std::endl;
        return false;
      }
      
      if (result) {
        std::cout << "result : " << this->getJSValueRefString(ctx, result) << std::endl;
      }
      return true;
    }
  }
  return false;
}

/**
 * @brief Add a path to the list for scanning
 * 
 * @param newPath a complete string path
 */
void MovieBrowser::addPath(std::string newPath) {
  this->paths.push_back(newPath);
}

/**
 * @brief Scan all the paths for files
 * 
 * @return std::string the JSON data of the scanned files
 */
std::string MovieBrowser::scanPaths(){
  std::string scanData = "{\"content\":[";
  for(std::string path : paths) {
    // append all the paths scanned data
    std::string output = this->scanDirectory(path);
    if (output.length() > 0) output.append(",");
    scanData.append(output);
  }
  // remove the last comma, before closing the array
  if (scanData.back() == ',') scanData.pop_back();
  scanData.append("]}");
  return scanData;
}

/**
 * @brief iterates through directory recursively to scann all the files in it
 * 
 * @param dir complete string path of the directory
 * @return std::string JSON data of the files scanned
 */
std::string MovieBrowser::scanDirectory(std::string dir){
  std::string jsonData = "";
  try {
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
      // filename of the current entry (could be a file or a directory)
      std::string filename = entry.path().filename().string();
      // regex matches on the element's names
      std::cmatch m; // 1:name 2:type 3:year 6:season 8:episode
      std::regex exp ("^([^\\(\\.]+)(\\((\\d+)?(TV)?(S(\\d+)(E(\\d+))?)?\\))?(\\.\\w+)?$");
      std::regex_match(filename.c_str(), m, exp);

      //std::cout <<filename<<"\n";
      //for (int i = 0; i < 15; i++) { std::cout << m[i] << "\t"; }
      //std::cout << "\n";

      // Add the complete filename
      // jsonData.append("{\"filename\":\"").append(std::regex_replace(entry.path().string(), std::regex("\\"), "\\\\")).append("\",");
      jsonData.append("{\"filename\":\"").append(entry.path().string()).append("\",");
      // Add the element's name only (not the series number, etc)
      jsonData.append("\"name\":\"").append(m[1]).append("\",");
      // Add the year if there is one
      if (m[2].length() == 6) jsonData.append("\"fileYear\":\"").append(m[3]).append("\",");
      else jsonData.append("\"fileYear\":\"\",");

      // If the element is a directory
      if (entry.is_directory()) {
        switch (m[2].length()) {
        case 4:
          // it's a TV show
          jsonData.append("\"type\":\"tvshow\",");
          break;
        case 5:
          // it's a TV season
          jsonData.append("\"type\":\"tvseason\",");
          jsonData.append("\"season\":\"").append(std::to_string(std::stoi(m[6]))).append("\",");
          break;
        default:
          //  it's collection of movies
          jsonData.append("\"type\":\"collection\",");
          break;
        }
        // recursively add directory's content
        // jsonData.append("\"content\":[").append(scanDirectory(entry.path().string())).append("]},");
        jsonData.append("\"content\":[").append("").append("]},");
      } else { // it is a file
        if (m[2].length() > 6) {
          // TV episode
          jsonData.append("\"type\":\"tvep\",");
          // parse to int then back to string to get the actual number
          jsonData.append("\"season\":\"").append(std::to_string(std::stoi(m[6]))).append("\",");
          jsonData.append("\"episode\":\"").append(std::to_string(std::stoi(m[8]))).append("\"},");
        } else {
          jsonData.append("\"type\":\"movie\"},");
        }
      }
    }
    if (jsonData.length())
      if (jsonData.back() == ',')
        jsonData.pop_back();
  } catch(const std::exception& e) {
    // TODO unreadable directory
    std::cerr << e.what() << '\n';
  }
  return jsonData;
}


inline std::string ToUTF8(const String& str) {
  String8 utf8 = str.utf8();
  return std::string(utf8.data(), utf8.length());
}

inline const char* Stringify(MessageSource source) {
  switch(source) {
    case kMessageSource_XML: return "XML";
    case kMessageSource_JS: return "JS";
    case kMessageSource_Network: return "Network";
    case kMessageSource_ConsoleAPI: return "ConsoleAPI";
    case kMessageSource_Storage: return "Storage";
    case kMessageSource_AppCache: return "AppCache";
    case kMessageSource_Rendering: return "Rendering";
    case kMessageSource_CSS: return "CSS";
    case kMessageSource_Security: return "Security";
    case kMessageSource_ContentBlocker: return "ContentBlocker";
    case kMessageSource_Other: return "Other";
    default: return "";
  }
}

inline const char* Stringify(MessageLevel level) {
  switch(level) {
    case kMessageLevel_Log: return "Log";
    case kMessageLevel_Warning: return "Warning";
    case kMessageLevel_Error: return "Error";
    case kMessageLevel_Debug: return "Debug";
    case kMessageLevel_Info: return "Info";
    default: return "";
  }
}

//
// Inherited from ViewListener::OnAddConsoleMessage
//
// Make sure that you bind 'MyApp' to 'View::set_view_listener'
// to receive this event.
//
void MovieBrowser::OnAddConsoleMessage(View* caller,
                                MessageSource source,
                                MessageLevel level,
                                const String& message,
                                uint32_t line_number,
                                uint32_t column_number,
                                const String& source_id) {
  
  std::cout << "[Console]: [" << Stringify(source) << "] [" 
            << Stringify(level) << "] " << ToUTF8(message);
  
  if (source == kMessageSource_JS) {
    std::cout << " (" << ToUTF8(source_id) << " @ line " << line_number 
              << ", col " << column_number << ")";
  }
  
  std::cout << std::endl;
  
}
