#include "glm/ext/vector_float2.hpp"
#include "imgui.h"
#include "window.h"

bool beginWindow(ThING::API& api, std::string id){
    static std::unordered_map<std::string, bool> wasOpen;
    if(!wasOpen.contains(id)){
        wasOpen[id] = ImGui::Begin(id.c_str());
        return wasOpen[id];
    }
    bool isOpen = ImGui::Begin(id.c_str());

    wasOpen[id] = isOpen;
    return isOpen;
}

glm::vec2 mousePosition(WindowData windowData){
    static int lastFrame = -1;
    int currentFrame = ImGui::GetFrameCount();
    static glm::vec2 cache;
    if(currentFrame == lastFrame){
        return cache;
    }
    lastFrame = currentFrame;
    ImVec2 tempPosition = ImGui::GetMousePos();
    cache = {
        ((tempPosition.x - ((float)windowData.size.width / 2)) / windowData.zoom) + windowData.offset.x,
        ((tempPosition.y - ((float)windowData.size.height / 2)) / windowData.zoom) + windowData.offset.y
    };
    return cache;
}

void scrollZoom(ThING::API &api){
    float scroll = ImGui::GetIO().MouseWheel;

    WindowData windowData;
    windowData.zoom = api.getZoom();
    windowData.offset = api.getOffset();
    api.getWindowSize(&windowData.size.width, &windowData.size.height);
    
    glm::vec2 pos = mousePosition(windowData);
    
    if(scroll != 0){
        scroll /= 20;
        float oldZoom = windowData.zoom;
        windowData.zoom *= 1 + scroll;
        if(windowData.zoom < .05){
            windowData.zoom = .05;
        } else if(windowData.zoom > 120){
            windowData.zoom = 120;
        } else {
            windowData.offset += (pos - windowData.offset) * (1.f - (oldZoom / windowData.zoom));
            api.setZoom(windowData.zoom);
            api.setOffset(windowData.offset);
        }
    }
}