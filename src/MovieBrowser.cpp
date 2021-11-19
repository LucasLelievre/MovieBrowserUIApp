#include "MovieBrowser.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 400

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
    false, kWindowFlags_Titled | kWindowFlags_Resizable);

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

void MovieBrowser::OnDOMReady(ultralight::View* caller,
                       uint64_t frame_id,
                       bool is_main_frame,
                       const String& url) {
  ///
  /// This is called when a frame's DOM has finished loading on the page.
  ///
  /// This is the best time to setup any JavaScript bindings.
  ///

  caller->EvaluateScript("pouet()");
  
  this->addPath("/media/lucas/BAT-external disk/video/films/");
  this->addPath("D:\\video\\films");
  std::string scanData = this->scanPaths();
  //std::cout << scanData << std::endl;
  ultralight::String d = "{\\\"data\\\": [{\\\"type\\\": \\\"file\\\",\\\"name\\\": \\\"Matrix\\\",\\\"file name\\\": \\\"Matrix (1999).mkv\\\"}]}";
  ultralight::String jsFunc = "refreshMovieCards(\"" + ultralight::String(scanData.c_str()) + "\")";
  caller->EvaluateScript(jsFunc);
  
  /*ultralight::String jsFunc = "refreshMovieList(\"";
  jsFunc += ultralight::String(scanData.c_str());
  jsFunc += "\")";
  std::cout << jsFunc.utf8().data() << std::endl;*/
  //caller->EvaluateScript(jsFunc);
  caller->EvaluateScript("setEventListeners()");
}

void MovieBrowser::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  if (cursor == kCursor_Hand) window_->SetCursor(kCursor_Pointer);
  else window_->SetCursor(cursor);
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

void MovieBrowser::addPath(std::string newPath) {
  this->paths.push_back(newPath);
}

std::string MovieBrowser::scanPaths(){
  std::string jsonData = "{\\\"data\\\":[";
  for(std::string path : paths) {
    jsonData.append(this->scanDirectory(path));
  }
  jsonData.append("]}");
  return jsonData;
}

// Code from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
std::string MovieBrowser::scanDirectory(std::string dir){
  std::string jsonData = "";
  try {
    //for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
      if (entry.is_directory()) {
        jsonData.append("{\\\"type\\\": \\\"directory\\\",");
        jsonData.append("\\\"name\\\":\\\"").append(entry.path().filename().string()).append("\\\",");
        jsonData.append("\\\"content\\\":[").append("").append("]},");
      } else {
        std::string name = entry.path().filename().string();
        std::cmatch m;
        std::regex_search(name.c_str(), m, std::regex("^(.*)(.zip|( \\())"));
        // find the files not matched by regex
        //if (m[1].length() == 0) std::cout << m[1].length() << "\t" << entry.path().filename().string() << "\t" << m[1] << "\n";
        jsonData.append("{\\\"type\\\": \\\"file\\\",\\\"name\\\": \\\"");
        jsonData.append(m[1]).append("\\\",\\\"file name\\\": \\\"");
        jsonData.append(entry.path().filename().string()).append("\\\"},");
      }
    }
    if (jsonData.back() == ',') jsonData.pop_back();
  } catch(const std::exception& e) {
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
