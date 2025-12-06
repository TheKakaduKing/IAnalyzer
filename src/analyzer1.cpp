#include "analyzer1.hpp"
#include "../include/imgui/imgui.h"
#include "../include/implot/implot.h"
#include <cmath>
#include <math.h>
#include <iostream>
                        
void windowAnalyzer1(ImVec2 size_, ImVec2 pos_){

  ImGui::SetNextWindowSize(size_, 0); 
  ImGui::SetNextWindowPos(pos_, 0); 

  static float valuesX[50]{};
  static float valuesY[50]{};
  static float limitX{50}, limitY{1};
  static bool newPlot{false};
  static char function[128];

  ImGui::Begin("Analyzer 1", nullptr, ImGuiWindowFlags_NoResize);        
  ImGui::InputTextWithHint("function", "function", function, IM_ARRAYSIZE(function));
  ImGui::SameLine();
  if (ImGui::Button("Scatter Plot")) {
    for (int i=0; i<50; ++i) {
      valuesX[i]=i;
      valuesY[i]=0.1f*i;
      if (valuesY[i] > limitY) {
        limitY = valuesY[i];
      }
    }
    newPlot = true;
  }

  if (ImPlot::BeginPlot("Plot 1", ImVec2(-1, -1))) {
    ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, limitX, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, limitY, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::PlotScatter("scatter", valuesX, valuesY, 50);
    ImPlot::EndPlot();  
  }
   
  ImGui::End();


  newPlot = false;
}
