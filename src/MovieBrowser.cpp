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
    // Maximize window
  	GLFWwindow* win = reinterpret_cast<GLFWwindow*>(window_->native_handle());
    glfwMaximizeWindow(win);
  #endif
  #ifdef _WIN32
    // Maximize window
    auto hwnd_ = reinterpret_cast<HWND>(window_->native_handle());
    ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
  #endif
 
  ///
  /// Create our HTML overlay-- we don't care about its initial size and
  /// position because it'll be calculated when we call OnResize() below.
  ///
  overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

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

void MovieBrowser::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  /// This is called when a frame's DOM has finished loading on the page.
  // std::cout << "pouet" << std::endl;

  // Read the settings in the json file
  this->settingsJson = this->ReadSettings();
  std::cout << "JSON settings : " <<this->settingsJson << std::endl;

  JSContextRef ctx = caller->LockJSContext().get()->ctx();
  
  // Add callback functions
  // open a file/website
  JSStringRef funcNameSysCo = JSStringCreateWithUTF8CString("openFile");
  JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
  JSObjectSetProperty(ctx, globalObj, funcNameSysCo, this->callbbackFunctions.openFile(ctx, funcNameSysCo), 0, 0);
  JSStringRelease(funcNameSysCo);
  // Scan a directory's files and folders
  JSStringRef funcNameScanDir = JSStringCreateWithUTF8CString("scanDirectory");
  JSObjectSetProperty(ctx, globalObj, funcNameScanDir, this->callbbackFunctions.scanDirectory(ctx, funcNameScanDir), 0, 0);
  JSStringRelease(funcNameScanDir);
  // Save settings to file
  JSStringRef funcNameSaveSettings = JSStringCreateWithUTF8CString("saveSettings");
  JSObjectSetProperty(ctx, globalObj, funcNameSaveSettings, this->callbbackFunctions.saveSettings(ctx, funcNameScanDir), 0, 0);

  // call initialize with settings
  JSValueRef arg = JSValueMakeFromJSONString(ctx, JSStringCreateWithUTF8CString(this->settingsJson.c_str()));
  JSValueRef args[] = {arg};
  this->EvaluateJsFunc(caller, "initialize", 1, args);
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

std::string MovieBrowser::ReadSettings() {
  return JsonControler::readFile("..\\..\\assets\\settings\\settings.json");
}

std::string MovieBrowser::getJSValueRefString(JSContextRef ctx, JSValueRef value) {
  if (!JSValueIsString(ctx, value)) {
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
  // Acquire the JS execution context for the current page and get the underlying JSContextRef for use with the JavaScriptCore C API.
  JSContextRef ctx = caller->LockJSContext().get()->ctx();

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
