// wingman window manager for all windows visible inside GLFW
#include "../include/imgui/imgui.h"
#include "../include/implot/implot.h"
#include "iawingman.hpp"
#include "iamath.hpp"
#include "iathread.hpp"
#include <math.h>
#include <string>
#include <vector>
#include <malloc.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


//*************************************************************************************************
//                      Sidepanel left class member declaration
//*************************************************************************************************

// ***Member function***
// --Draw the left sidepanel window 
//
int iawindow::sPanLeft::showWindow(){

  ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always); 
  ImGui::SetNextWindowPos(ImVec2{0,0}, 0); 

  ImGui::Begin(this->name_.c_str());        
  ImGui::SeparatorText("Stats");

  if (ImGui::Button("Threads##spl")) {
    iawindow::wingman::instance().queueCreateWindow(stWinInfo{iawindow::WINDOW_TYPE_ACTIVE_THREADS, ImVec2(400,400), ImVec2(300,300)});
  }
  ImGui::SeparatorText("Plotting");

  if (ImGui::Button("Sequence##spl")) {
    iawindow::wingman::instance().queueCreateWindow(stWinInfo{iawindow::WINDOW_TYPE_SEQUENCE_PLOT, ImVec2(400,400), ImVec2(300,300)});
  }
  for (std::shared_ptr<iawindow::window>& w : iawindow::wingman::instance().getWindows()) {
    if (w->type_ == iawindow::WINDOW_TYPE_SEQUENCE_PLOT) {
      if (ImGui::Button(("Sequence Plotter (id: " + std::to_string(w->id_) + ")").c_str())) {
        w->show_ = true;
      }
      ImGui::SameLine();
      ImGui::BeginDisabled(w->threadActive_);
      if (ImGui::Button(("X##spl" + std::to_string(w->id_) + ")").c_str())) {
        iawindow::wingman::queueDestroyWindow(w->id_);
      }
      ImGui::EndDisabled();
    }
  }
  ImGui::End();
  return 0;
}


//*************************************************************************************************
//                      Sequence plot class member declaration
//*************************************************************************************************

// ***Member function***
// --Draw a window to plot a sequence via implot
//
int iawindow::seqPlot::showWindow(){

  ImGui::SetNextWindowSize(this->size_, ImGuiCond_Once); 
  ImGui::SetNextWindowPos(this->pos_, ImGuiCond_Once); 

  if (this->show_) {
  
    ImGui::Begin(this->name_.c_str(), &this->show_);        
    ImGui::InputTextWithHint(("##InFunc" + this->name_).c_str(), "function", function_, IM_ARRAYSIZE(function_));
    ImGui::SameLine();
  
    if (!(iathread::instance().getWindowThreads(this->id_).empty())) {
      threadActive_ = true;
    } 
    else {
      threadActive_ = false;
    }
    ImGui::BeginDisabled(threadActive_);
  
    if (ImGui::Button(("Plot sequence##plotSeq" + this->name_).c_str())) {

      func_ = std::string{function_};
      valuesX_.clear();
      valuesY_.clear();
  
      //validate user input
      start_ >= 0 ? start_ : start_ = 0;
      (end_ >= 0 && end_ >= start_) ? end_ : end_ = start_;
    inc_ > 0 ? inc_ : inc_ = 1;
  
      iathread::instance().newThread(this->id_, this, &iawindow::seqPlot::fillValues, this->func_, this->start_, this->end_, this->inc_);
    }
    ImGui::EndDisabled();
  
    ImGui::PushItemWidth(50);
  
    ImGui::Text("start:");
    ImGui::SameLine();
    ImGui::InputInt(("##InFuncStart" + this->name_).c_str(), &start_, 0, 0);
    ImGui::SameLine();
    ImGui::Text("end:");
    ImGui::SameLine();
    ImGui::InputInt(("##InFuncEnd" + this->name_).c_str(), &end_, 0, 0);
    ImGui::SameLine();
    ImGui::Text("inc:");
    ImGui::SameLine();
    ImGui::InputInt(("##InFuncInc" + this->name_).c_str(), &inc_, 0, 0);
  
    ImGui::PopItemWidth();
  
    if (ImPlot::BeginPlot((std::string{function_} + "##plot" + this->name_).c_str(), ImVec2(-1, -1), ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText)) {
      ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, limitX_, ImGuiCond_Once);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, limitY_, ImGuiCond_Once);
      ImPlot::PlotScatter(("scatter##" + this->name_).c_str() , valuesX_.data(), valuesY_.data(), valuesY_.size(), 0, sizeof(double));
      ImPlot::EndPlot();  
    }
     
    ImGui::End();
  
  }
  return 0;
}


// ***Member function***
// --For thread use
// --Calls an object of type(iamath::seqplot) to calculate the plotting points
//
void iawindow::seqPlot::fillValues(std::string func, int start, int end, int inc){

  valuesY_ = iamath::calcInSeq(func_, start_, end_, inc_);
  double i = start_;
  valuesX_.clear();
  for (double d : valuesY_) {
    valuesX_.push_back(i);
    i+=inc_;
  }
  
}


// ***Member function***
// --Execute code when the destructor is called
//
void iawindow::seqPlot::onClose(){
  valuesX_.clear(); valuesX_.shrink_to_fit();
  valuesY_.clear(); valuesY_.shrink_to_fit();
}


//*************************************************************************************************
//                      Active threads class member declaration
//*************************************************************************************************

// ***Member function***
// --Draw a window that gets active window threads of all windows and outputs the internal thread ids
//
int iawindow::activeThreads::showWindow(){

  ImGui::SetNextWindowSize(this->size_, ImGuiCond_Once); 
  ImGui::SetNextWindowPos(this->pos_, ImGuiCond_Once); 

  if (this->show_) {
    threads_ = iathread::instance().getActiveThreads();
  
    ImGui::Begin(this->name_.c_str(), &this->show_);        
    ImGui::SeparatorText("Active Threads");
    for (int t : threads_) {
      ImGui::Text("Active thread id: %d", t);
    }
    ImGui::End();
  }
  return 0;
}
