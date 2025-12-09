#include "analyzer1.hpp"
#include "../include/imgui/imgui.h"
#include "../include/implot/implot.h"
#include "iamath.hpp"
#include <math.h>
#include <ratio>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
                        
void windowAnalyzer1(ImVec2 size_, ImVec2 pos_){

  ImGui::SetNextWindowSize(size_, 0); 
  ImGui::SetNextWindowPos(pos_, 0); 

  static float limitX{50}, limitY{1};
  static bool newPlot{false};
  static char function[128];
  static char start[128];
  static char end[128];
  static char increment[128];
  static std::vector<double> valuesX{}, valuesY{};
  std::string func{};

  ImGui::Begin("Analyzer 1", nullptr, ImGuiWindowFlags_NoResize);        
  ImGui::InputTextWithHint("function", "function", function, IM_ARRAYSIZE(function));
  ImGui::SameLine();
  static std::string s;
  static double a;
  if (ImGui::Button("Scatter Plot")) {
    func = std::string{function};
    valuesY.clear();

    static auto t0 = std::chrono::steady_clock::now();
    t0 = std::chrono::steady_clock::now();
    valuesY = iamath::calcInSeq{func, std::stoi(start), std::stoi(end), std::stoi(increment)};
    static auto t1 = std::chrono::steady_clock::now();
    t1 = std::chrono::steady_clock::now();
    a = std::chrono::duration<double, std::milli>(t1-t0).count();
    double i = 0;
    valuesX.clear();
    for (double d : valuesY) {
      valuesX.push_back(i);
      i++;
    }
    newPlot = true;
  }
  s = std::to_string(a);
  ImGui::Text(s.data());
  ImGui::InputTextWithHint("start", "start", start, IM_ARRAYSIZE(function));
  ImGui::InputTextWithHint("end", "end", end, IM_ARRAYSIZE(function));
  ImGui::InputTextWithHint("increment", "increment", increment, IM_ARRAYSIZE(function));

  if (ImPlot::BeginPlot("Plot 1", ImVec2(-1, -1))) {
    ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 10, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 10, newPlot ? ImGuiCond_Always : ImGuiCond_Once);
    ImPlot::PlotScatter("scatter", valuesX.data(), valuesY.data(), valuesY.size());
    ImPlot::EndPlot();  
  }
   
  ImGui::End();


  newPlot = false;
}
