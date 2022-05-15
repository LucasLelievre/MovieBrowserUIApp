#pragma once
#include <AppCore/AppCore.h>
#include <JavaScriptCore/JSRetainPtr.h>

#ifdef _WIN32
  #include <Windows.h>
#endif
#ifdef __linux__
  #include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <regex>

#include "JScallbackFunctions.h"
#include "JsonControler.h"

using namespace ultralight;

class MovieBrowser : public AppListener,
              public WindowListener,
              public LoadListener,
              public ViewListener {
public:
  MovieBrowser();

  virtual ~MovieBrowser();

  // Start the run loop.
  virtual void Run();

  // This is called continuously from the app's main loop.
  virtual void OnUpdate() override;

  // This is called when the window is closing.
  virtual void OnClose(ultralight::Window* window) override;

  // This is called whenever the window resizes.
  virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override;

  // This is called when the page finishes a load in one of its frames.
  virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) override;

  // This is called when the DOM has loaded in one of its frames.
  virtual void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) override;

  // This is called when the page requests to change the Cursor.
  virtual void OnChangeCursor(ultralight::View* caller, Cursor cursor) override;

  virtual void OnChangeTitle(ultralight::View* caller, const String& title) override;

  std::string ReadSettings();
  std::string getJSValueRefString(JSContextRef ctx, JSValueRef value);
  bool EvaluateJsFunc(ultralight::View* caller, const char * funcName, int argc, JSValueRef * argv);

  // Print js console logs
  virtual void OnAddConsoleMessage(View* caller, MessageSource source, MessageLevel level, const String& message, uint32_t line_number, uint32_t column_number, const String& source_id) override;

protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;

  // User settings
  std::string settingsJson;

  // JS callback functions
  JScallbackFunctions callbbackFunctions;
};
