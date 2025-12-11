// wingman window manager for all windows visible inside GLFW
#include "../include/imgui/imgui.h"
#include "../include/implot/implot.h"
#include "iawingman.hpp"
#include "iamath.hpp"
#include <cstring>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

iawindow::window::window(int id, windowType type, bool show, std::string name, ImVec2 size, ImVec2 pos):
  id_{id},
  type_{type},
  show_{show},
  name_{name},
  size_(size),
  pos_(pos)
{

}


int iawindow::sPanLeft::showWindow(){

  ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always); 
  ImGui::SetNextWindowPos(ImVec2{0,0}, 0); 

  ImGui::Begin(this->name_.c_str());        
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
      if (ImGui::Button(("X##spl" + std::to_string(w->id_) + ")").c_str())) {
        iawindow::wingman::queueDestroyWindow(w->id_);
      }
    }
  }
  ImGui::End();
  return 0;
}



int iawindow::seqPlot::showWindow(){

  ImGui::SetNextWindowSize(this->size_, ImGuiCond_Once); 
  ImGui::SetNextWindowPos(this->pos_, ImGuiCond_Once); 

if (this->show_) {

  ImGui::Begin(this->name_.c_str(), &this->show_);        
  ImGui::InputTextWithHint(("##InFunc" + this->name_).c_str(), "function", function_, IM_ARRAYSIZE(function_));
  ImGui::SameLine();

  if (ImGui::Button(("Plot sequence##plotSeq" + this->name_).c_str())) {

    func_ = std::string{function_};
    valuesY_.clear();

    //validate user input
    start_ >= 0 ? start_ : start_ = 0;
    (end_ >= 0 && end_ >= start_) ? end_ : end_ = start_;
    inc_ > 0 ? inc_ : inc_ = 1;

    valuesY_ = iamath::calcInSeq{func_, start_, end_, inc_};
    double i = start_;
    valuesX_.clear();
    for (double d : valuesY_) {
      valuesX_.push_back(i);
      i+=inc_;
    }
    newPlot_ = true;
  }

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

  if (ImPlot::BeginPlot((std::string{function_} + "##plot" + this->name_).c_str(), ImVec2(-1, -1))) {
    ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, limitX_, newPlot_ ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, limitY_, newPlot_ ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::PlotScatter("scatter", valuesX_.data(), valuesY_.data(), valuesY_.size());
    ImPlot::EndPlot();  
  }
   
  ImGui::End();

  newPlot_ = false;
}
  return 0;
}
// void menu1(ImVec2 size_, ImVec2 pos_, std::vector<iawindow::seqWindow>& a){
//   //set menu size and pos
//
// }
//
// void iawindow::seqWindow::showWindow(){
//
//
//   ImGui::SetNextWindowSize(this->size_, ImGuiCond_Once); 
//   ImGui::SetNextWindowPos(this->pos_, ImGuiCond_Once); 
//
//   if (this->show_) {
//     ImGui::PushID(this);
//     ImGui::Begin(this->name_.c_str(), &this->show_);        
//     ImGui::End();
//     ImGui::PopID();
//   }
// }
//
//
