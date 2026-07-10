#include "ThING//api.h"
#include "ThING/types/apiTypes.h"
#include "decorative.h"
#include "decorative.h"
#include "imgui.h"
#include "demo/window/window.h"
#include <cstdlib>

void start(ThING::API& api, FPSCounter& fps){
    fps.setTarget(10000);
    api.addLine({0,0}, {100,100}, 10.f);
    Entity pol;
    pol = api.addRegularPol(8, {100, -100}, {30, 30}, {1,1,1,1});
    
}

void update(ThING::API& api, FPSCounter& fps){
    static bool first = true;
    static Entity e = api.addCircle({0,0}, 10, {1.f,1.f,1.f,1.f});;
    static int direction = 1;
    
    api.getInstance(e).position.x = api.getInstance(e).position.x + 100.f * fps.getDeltaTime() * direction;
    if(std::abs(api.getInstance(e).position.x) > 500){
        direction *= -1;
    }
}

void ui(ThING::API& api, FPSCounter& fps){
    scrollZoom(api);
    ImGuiIO& io = ImGui::GetIO();
    if(!io.WantCaptureMouse){
        if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            glm::vec2 offset = api.getOffset();
            offset.x -= delta.x / api.getZoom();
            offset.y -= delta.y / api.getZoom();
            api.setOffset(offset);
        }
    }
    static ThING::ext::BackGroundInfo bg;
    beginWindow(api, "Background");
    ImGui::ColorPicker4("Color", &bg.gridColor.x);
    ImGui::SliderFloat("Margin", &bg.margin, 0.f, 20.f);
    ImGui::SliderFloat("Size", &bg.size, 10.f, 100.f);
    int saveGridSize = bg.gridSize;
    ImGui::SliderInt("Grid Size", &saveGridSize, 0, 16);
    bg.gridSize = saveGridSize;
    ImGui::End();


    static ThING::ext::WindowBorderInfo wb;
    beginWindow(api, "border");
    ImGui::ColorPicker4("Color", &wb.color.x);
    ImGui::SliderFloat("thickness", &wb.width, 0, 40);
    ImGui::SliderInt("width", &wb.aspectRatio.width, 0, 100);
    ImGui::SliderInt("height", &wb.aspectRatio.height, 0, 100);
    ImGui::End();

    // Use Examples
    ThING::ext::WorldBorder borders = ThING::ext::getWorldBorder(api, wb);
    static Entity entitys[4];
    static bool first = true;
    if(first){
        entitys[0] = api.addCircle({borders.left, 0}, 50, {1,0,0,1});
        entitys[1] = api.addCircle({borders.right, 0}, 50, {0,1,0,1});
        entitys[2] = api.addCircle({0, borders.up}, 50, {0,0,1,1});
        entitys[3] = api.addCircle({0, borders.down}, 50, {1,0,1,1});
        first = false;
    }
    api.getInstance(entitys[0]).position = {borders.left, 0};
    api.getInstance(entitys[1]).position = {borders.right, 0};
    api.getInstance(entitys[2]).position = {0, borders.up};
    api.getInstance(entitys[3]).position = {0, borders.down};
    ThING::ext::buildHexBackground(api, bg);
    ThING::ext::buildWindowBorder(api, wb);
    ThING::ext::keepAspectRatio(api, wb.aspectRatio);
}

int main(){
    ThING::API api(ApiFlags_UpdateCallbackFirst | ApiFlags_UseFullFPS);
    api.setStartCallback(start);
    api.setUICallback(ui);
    api.setUpdateCallback(update);
    api.run();
}