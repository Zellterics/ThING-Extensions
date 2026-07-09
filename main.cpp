#include "ThING//api.h"
#include "ThING/types/apiTypes.h"
#include "decorative.h"
#include "decorative.h"
#include "imgui.h"
#include "demo/window/window.h"
#include <cstdlib>

void update(ThING::API& api, FPSCounter& fps){
    static bool first = true;
    static Entity e = INVALID_ENTITY;
    static int direction = 1;
    if(first){
        fps.setTarget(10000);
        
        e = api.addCircle({0,0}, 10, {1.f,1.f,1.f,1.f});
        api.addLine({0,0}, {100,100}, 10.f);
        api.addRegularPol(8, {100, -100}, {30, 30}, {1,1,1,1});
        first = false;
    }
    
    api.getInstance(e).position.x = api.getInstance(e).position.x + 100.f * fps.getDeltaTime() * direction;
    if(std::abs(api.getInstance(e).position.x) > 500){
        direction *= -1;
    }
}

void ui(ThING::API& api, FPSCounter& fps){
    scrollZoom(api);
    static ThING::ext::BackGroundInfo bg;
    beginWindow(api, "Background");
    ImGui::ColorPicker4("Color", &bg.gridColor.x);
    ImGui::SliderFloat("Margin", &bg.margin, 0.f, 20.f);
    ImGui::SliderFloat("Size", &bg.size, 10.f, 100.f);
    int saveGridSize = bg.gridSize;
    ImGui::SliderInt("Grid Size", &saveGridSize, 0, 16);
    bg.gridSize = saveGridSize;
    ImGui::End();

    // Background Use Example
    ThING::ext::buildHexBackground(api, bg);

    
    ImGuiIO& io = ImGui::GetIO();
    if(io.WantCaptureMouse){
        return;
    }
    if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        glm::vec2 offset = api.getOffset();
        offset.x -= delta.x / api.getZoom();
        offset.y -= delta.y / api.getZoom();
        api.setOffset(offset);
    }
}

int main(){
    ThING::API api(ApiFlags_UpdateCallbackFirst | ApiFlags_UseFullFPS);
    api.setUICallback(ui);
    api.setUpdateCallback(update);
    api.run();
}