#include "hdr/analyzer3.hpp"
#include "../include/imgui/imgui.h"
                        
void windowAnalyzer3(ImVec2 size_, ImVec2 pos_){

  ImGui::SetNextWindowSize(size_, 0); 
  ImGui::SetNextWindowPos(pos_, 0); 

  static float f = 0.0f;
  static int counter = 0;
  
  
  ImGui::Begin("Analyzer 3", nullptr, ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
  
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
  
  if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);
  
  ImGui::End();


}

