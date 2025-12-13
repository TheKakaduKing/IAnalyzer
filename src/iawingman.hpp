// wingman window manager for all windows visible inside GLFW
#pragma once
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>
#include "imgui/imgui.h"
#include <string>
#include <vector>
#include <iostream>
#include "../include/implot/implot.h"

namespace iawindow{
  class wingman;
  class window;
  class sPanLeft;
  class sPanRight;
  class seqPlot;
  class activeThreads;
  struct stWinInfo;
  enum windowType {
    WINDOW_TYPE_NONE = 0,
    WINDOW_TYPE_SIDEPANEL_LEFT = 10,
    WINDOW_TYPE_SIDEPANEL_RIGHT = 20,
    WINDOW_TYPE_SIDEPANEL_TOP = 30,
    WINDOW_TYPE_SIDEPANEL_BOTTOM = 40,
    WINDOW_TYPE_SEQUENCE_PLOT = 100,
    WINDOW_TYPE_ACTIVE_THREADS = 1000,
    };
}

struct iawindow::stWinInfo{
  iawindow::windowType type;
  ImVec2 size;
  ImVec2 pos;
};

//*************************************************************************************************
//                      Wingman class
//*************************************************************************************************

class iawindow::wingman{
  public:
    // --Create an instance once that gets worked on
    static wingman& instance(){
      static wingman inst{};
      return inst;
    }

  private:
    static inline int idCounter_;
    static inline std::queue<stWinInfo> windowCreateQueue_;
    static inline std::queue<int> windowDestroyQueue_;
    static inline std::unordered_map<int, bool> idMap_;
    static inline std::vector<std::shared_ptr<iawindow::window>> windows_;

  public:
    static inline int queueCreateWindow(const stWinInfo& winInfo);
    static inline int queueDestroyWindow(int id);
    static inline int drawAll();
    static inline std::vector<std::shared_ptr<iawindow::window>> getWindows(){return windows_;};

  private:
    static inline int createWindow();
    static inline int destroyWindow();
    static inline int preDraw();

};



//*************************************************************************************************
//                      Window class
//*************************************************************************************************

class iawindow::window{
  public:
    window(int id, windowType type, bool show, std::string name, ImVec2 size, ImVec2 pos):
      id_{id}, type_{type}, show_{show}, name_{name}, size_(size), pos_(pos){}

    // --Need virtual base class destructor, otherwise the dervied objects destructor 
    // --will not be called when the SharedPointer of the window is erased from the voector
    virtual ~window() = default;

    virtual int showWindow() = 0;

  public:
    const int id_{0};
    const windowType type_{iawindow::WINDOW_TYPE_NONE};
    bool show_{false};
    const std::string name_;
    const ImVec2 size_{400,400};
    const ImVec2 pos_{300,300};
    bool threadActive_{false};


};


//*************************************************************************************************
//                      Sidepanel left class
//*************************************************************************************************

class iawindow::sPanLeft :public window{
  public:
    sPanLeft(int id, windowType type, bool show, std::string name, ImVec2 size, ImVec2 pos):
      window{id, type, show, name, size, pos}
      {};

  private:
    int showWindow() override;
};


//*************************************************************************************************
//                      Sequence plot class
//*************************************************************************************************

class iawindow::seqPlot :public window{
  public:
    seqPlot(int id, windowType type, bool show, std::string name, ImVec2 size, ImVec2 pos):
      window{id, type, show, name, size, pos}
      {};

  private:
  std::string func_{};
  char function_[128]{""};
  float limitX_{50}, limitY_{50};
  int start_{0}, end_{100}, inc_{1};
  std::vector<double> valuesX_{}, valuesY_{};

  private:
    int showWindow() override;
    void fillValues(std::string func, int start, int end, int inc);
    void onClose();

  public:
    ~seqPlot(){
      onClose();
    }

};


//*************************************************************************************************
//                      Active threads class
//*************************************************************************************************

class iawindow::activeThreads :public window{
  public:
    activeThreads(int id, windowType type, bool show, std::string name, ImVec2 size, ImVec2 pos):
      window{id, type, show, name, size, pos}
      {};

  private:
    int showWindow() override;

  private:
    std::vector<int> threads_{};
};

//*************************************************************************************************
//                      Wingman class member declaration
//*************************************************************************************************

// ***Member function***
// --Queue a new window in a window queue 
//
inline int iawindow::wingman::queueCreateWindow(const stWinInfo& winInfo){
  windowCreateQueue_.push(winInfo);
  return 0;
}


// ***Member function***
// --Create a new window based on the window type 
//
inline int iawindow::wingman::createWindow(){
  stWinInfo current{};
  while (!windowCreateQueue_.empty()) {

    idCounter_++;

    current = windowCreateQueue_.front();
    
    if (idMap_.find(idCounter_) == idMap_.end()) {
      switch (current.type) {
        case WINDOW_TYPE_NONE:{
            break;
          } 
        case WINDOW_TYPE_SIDEPANEL_LEFT:{
            windows_.push_back(std::make_shared<iawindow::sPanLeft>(idCounter_,current.type,true,std::to_string(idCounter_),current.size,current.pos));
            break;
          } 
        case WINDOW_TYPE_SIDEPANEL_RIGHT:{} 
        case WINDOW_TYPE_SIDEPANEL_TOP:{} 
        case WINDOW_TYPE_SIDEPANEL_BOTTOM:{} 
        case WINDOW_TYPE_SEQUENCE_PLOT:{
            windows_.push_back(std::make_shared<iawindow::seqPlot>(idCounter_,current.type,true,"Sequence plotter##" + std::to_string(idCounter_),current.size,current.pos));
            break;
          } 
        case WINDOW_TYPE_ACTIVE_THREADS:{
            windows_.push_back(std::make_shared<iawindow::activeThreads>(idCounter_,current.type,true,"Active Threads##" + std::to_string(idCounter_),current.size,current.pos));
            break;
          } 
        default:{
                  return 1;
                }
      }
    }
  // std::cout << "window container size: " << windows_.size() << std::endl;    
  idMap_.insert({idCounter_, true});
  windowCreateQueue_.pop();
  }

  return 0;
}


// ***Member function***
// --Queue a window to be destroyed 
//
inline int iawindow::wingman::queueDestroyWindow(int id){
  windowDestroyQueue_.push(id);
  return 0;
}


// ***Member function***
// --Destroy all windows in the window destroy queue 
//
inline int iawindow::wingman::destroyWindow(){
  while (!windowDestroyQueue_.empty()) {
    std::erase_if(windows_, [&](auto &w){return w->id_ == windowDestroyQueue_.front();});
    windowDestroyQueue_.pop();
  }
  return 0;
}


// ***Member function***
// --Execute code before drawing all windows
//
inline int iawindow::wingman::preDraw(){
  destroyWindow();
  createWindow();

  return 0;
}



// ***Member function***
// --Draw all windows stored in the windows vector
//
inline int iawindow::wingman::drawAll(){
  preDraw();
  for (std::shared_ptr<iawindow::window>& w : windows_) {
      w->showWindow();
  }
  return 0;
}
