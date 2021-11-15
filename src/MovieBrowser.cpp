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

  paths.push_back("/media/lucas/BAT-external disk/video/films/");
  for(std::string path : paths) {
    std::cout << this->scanDirectory(path) << std::endl;
  }
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
  caller->EvaluateScript("addMovies()");
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

// Code from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
std::string MovieBrowser::scanDirectory(std::string dir){
  std::string filesJson = "{\"type\":\"directory\",";
  filesJson.append("\"name\":\"").append(dir).append("\",")
    .append("\"content\":[");

  DIR *dp;
  struct dirent *entry;
  struct stat statbuf;
  if ((dp = opendir(dir.c_str())) == NULL) {
    fprintf(stderr, "cannot open directory: %s\n", dir.c_str());
    return "{\"type\":\"error\"}";
  }
  chdir(dir.c_str());
  while ((entry = readdir(dp)) != NULL) {
    lstat(entry->d_name, &statbuf);
    if (S_ISDIR(statbuf.st_mode)) {
      // Found a directory
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue; // ignoring . and ..
      // Recursing inside the directory
      filesJson.append(scanDirectory(entry->d_name)).append(",");
    } else {
      // Found a file
      std::cmatch m;
      std::regex_search(entry->d_name, m, std::regex("^(.*)(.zip|( \\())"));
      filesJson.append("{\"type\": \"file\",\"name\": \"");
      filesJson.append(m[1]).append("\",\"file name\": \"");
      filesJson.append(entry->d_name).append("\"},");
    }
  }
  if (filesJson.back() == ',') filesJson.pop_back();
  filesJson.append("]}");

  chdir("..");
  closedir(dp);
  return filesJson;
}