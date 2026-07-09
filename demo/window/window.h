#pragma once

#include "ThING/api.h"
#include "glm/ext/vector_float2.hpp"

struct WindowSize{
    int width;
    int height;
    bool operator==(const WindowSize& other) const {
        if(other.height == height && other.width == width){
            return true;
        }
        return false;
    }
    bool operator!=(const WindowSize& other) const {
        if(other.height == height && other.width == width){
            return false;
        }
        return true;
    }
};

struct WindowData{
    WindowSize size;
    float zoom;
    glm::vec2 offset;
};

glm::vec2 mousePosition(WindowData windowData);
void scrollZoom(ThING::API &api);
bool beginWindow(ThING::API& api, std::string id);