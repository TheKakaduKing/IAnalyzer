#include "analyzer1.hpp"
#include "../include/imgui/imgui.h"
#include "../include/implot/implot.h"
#include "iamath.hpp"
#include <math.h>
#include <string>
#include <vector>
                        
void windowAnalyzer1(ImVec2 size_, ImVec2 pos_){

  ImGui::SetNextWindowSize(size_, 0); 
  ImGui::SetNextWindowPos(pos_, 0); 

  static float limitX{50}, limitY{1};
  static bool newPlot{false};
  static char function[128];
  static std::vector<double> valuesX{}, valuesY{};
  std::string func{};

  ImGui::Begin("Analyzer 1", nullptr, ImGuiWindowFlags_NoResize);        
  ImGui::InputTextWithHint("function", "function", function, IM_ARRAYSIZE(function));
  ImGui::SameLine();
  if (ImGui::Button("Scatter Plot")) {
    func = std::string{function};
    valuesY.clear();
    valuesY = iamath::calcInSeq{func, 0, 3000, 1};
    double i = 0;
    valuesX.clear();
    for (double d : valuesY) {
      valuesX.push_back(i);
      i++;
    }
    newPlot = true;
  }

  if (ImPlot::BeginPlot("Plot 1", ImVec2(-1, -1))) {
    ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 10, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 10, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::PlotScatter("scatter", valuesX.data(), valuesY.data(), valuesY.size());
    ImPlot::EndPlot();  
  }
   
  ImGui::End();


  newPlot = false;
}
