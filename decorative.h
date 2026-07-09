#pragma once
#include "ThING/api.h"
#include "ThING/types/apiTypes.h"
#include "ThING/types/vertex.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

namespace ThING::ext {

struct BackGroundInfo{
    glm::vec4 gridColor = (glm::vec4){.036f, .072f, .06f, 1.f};
    float margin = 5;
    float size = 50;
    uint32_t gridSize = 8;
    bool operator==(const BackGroundInfo& other) const {
        if(this->gridColor == other.gridColor &&
            this->margin == other.margin &&
            this->size == other.size &&
            this->gridSize == other.gridSize
        ){
            return true;
        }
        return false;
    }
    bool operator!=(const BackGroundInfo& other) const {
        if(*this == other){
            return false;
        }
        return true;
    }
};

static inline void buildHexBackground(ThING::API& api, const BackGroundInfo& info){
    static BackGroundInfo savedInfo = {{0.f, 0.f, 0.f, 0.f}, 0, 0, 0};
    
    
    static float cameraZoom = api.getZoom();
    static glm::vec2 cameraPos = api.getOffset(); // Y inverted

    if(savedInfo == info && api.getZoom() == cameraZoom && cameraPos == api.getOffset()){
        return;
    }

    static bool update = false;

    static glm::vec2 backgroundPan = api.getOffset();

    static std::vector<std::vector<Entity>> tiles;
    static std::vector<Entity> tempTile;

    float margin = info.margin;
    float size = info.size;
    int gridSize = info.gridSize;
    glm::vec4 gridColor = info.gridColor;

    float horizontalMargin = cos(glm::pi<float>() / 6) * margin;
    float b = sqrt((size * size) - ((size / 2) * (size / 2)));

    float iStep = (size * 3) + (horizontalMargin * 2);
    float jStep = (b * 2) + margin;

    float iOffset = (iStep / 2);
    float jOffset = (jStep / 2);

    glm::vec2 vSize = {size, size};

    if(cameraPos != api.getOffset() && cameraZoom == api.getZoom()){
        backgroundPan += (cameraPos - api.getOffset()) * cameraZoom;
        
        cameraPos = api.getOffset();

        if(backgroundPan.x > iStep){
            backgroundPan.x -= iStep;
            update = true;
        }
        if(backgroundPan.x < -iStep){
            backgroundPan.x += iStep;
            update = true;
        }
        if(backgroundPan.y > jStep){
            backgroundPan.y -= jStep;
            update = true;
        }
        if(backgroundPan.y < -jStep){
            backgroundPan.y += jStep;
            update = true;
        }
    }

    if(cameraZoom != api.getZoom()){
        cameraZoom = api.getZoom();
        cameraPos = api.getOffset();
        update = true;
    }

    if(savedInfo.gridSize != info.gridSize){
        for(const std::vector<Entity>& tile : tiles){
            for(const Entity& til : tile){
                if(api.exists(til)){
                    api.deleteInstance(til);
                }
            }
        }
        api.cleanRenderData();

        tempTile.clear();
        tiles.clear();
        tempTile.insert(tempTile.end(), gridSize * 2, INVALID_ENTITY);
        tiles.insert(tiles.end(), gridSize * 2 * 2, tempTile);

        for(int i = -gridSize; i < gridSize; i++){
            for(int j = -gridSize; j < gridSize; j++){
                const glm::vec2 worldPos = {(i * iStep),(j * jStep)};
                const glm::vec2 pos = (worldPos + backgroundPan) / cameraZoom;
                const glm::vec2 scale = vSize / cameraZoom;
                Entity e = api.addRegularPol(6, pos + api.getOffset(), scale, gridColor);
                api.getInstance(e).rotation = glm::pi<float>() / 6.f;
                api.getInstance(e).drawIndex = -102;
                
                tiles[(i + gridSize) * 2][j + gridSize] = e;
            }
        }
        for(int i = -gridSize; i < gridSize; i++){
            for(int j = -gridSize; j < gridSize; j++){
                const glm::vec2 worldPos = {((i * iStep) + iOffset), ((j * jStep) + jOffset)};
                const glm::vec2 pos = (worldPos + backgroundPan) / cameraZoom;
                const glm::vec2 scale = vSize / cameraZoom;
                Entity e = api.addRegularPol(6, pos + api.getOffset(), scale, gridColor);
                api.getInstance(e).rotation = glm::pi<float>() / 6.f;
                api.getInstance(e).drawIndex = -102;
                tiles[((i + gridSize) * 2) + 1][j + gridSize] = e;
            }
        }
        savedInfo = info;
        return;
    }
    if(savedInfo != info){
        update = true;
        savedInfo = info;
    }
    
    if(update == false){
        return;
    }
    update = false;
    for(int i = -gridSize; i < gridSize; i++){
        for(int j = -gridSize; j < gridSize; j++){
            const glm::vec2 worldPos = {(i * iStep),(j * jStep)};
            const glm::vec2 pos = (worldPos + backgroundPan) / cameraZoom;
            const glm::vec2 scale = vSize / cameraZoom;
            Entity e = tiles[(i + gridSize) * 2][j + gridSize];
            api.getInstance(e).position = pos + api.getOffset();
            api.getInstance(e).rotation = glm::pi<float>() / 6.f;
            api.getInstance(e).color = gridColor;
            api.getInstance(e).scale = scale;
        }
    }
    
    for(int i = -gridSize; i < gridSize; i++){
        for(int j = -gridSize; j < gridSize; j++){
            const glm::vec2 worldPos = {((i * iStep) + iOffset), ((j * jStep) + jOffset)};
            const glm::vec2 pos = (worldPos + backgroundPan) / cameraZoom;
            const glm::vec2 scale = vSize / cameraZoom;
            Entity e = tiles[((i + gridSize) * 2) + 1][j + gridSize];
            api.getInstance(e).position = pos + api.getOffset();
            api.getInstance(e).rotation = glm::pi<float>() / 6.f;
            api.getInstance(e).color = gridColor;
            api.getInstance(e).scale = scale;
        }
    }
    
    return;
}

struct AspectRatio{
    int width = -1;
    int height = -1;
};

static inline void keepAspectRatio(ThING::API& api, const AspectRatio info){
    if(info.height == -1 || info.width == -1){
        return;
    }
    int width, height;
    AspectRatio aspectRatio = info;
    static bool first = true;
    api.getWindowSize(&width, &height);
    static std::array<Entity, 4> borders;
    float ratio = (float)aspectRatio.width / aspectRatio.height;
    float customHeight = height;
    float customWidth = width;
    float wthickness = 0;
    float hthickness = 0;
    if(((float)width / height) < ratio){
        customHeight = (width / ratio) / 2;
        hthickness = (height - customHeight) * 2;
    } else {
        customWidth = (height * ratio) / 2;
        wthickness = (width - customWidth) * 2;
    }

    if(first){
        static std::array<Vertex, 4> QUAD_VERTICES = {{
            {{-1.f, -1.f}, {-1.0f, -1.0f}},
            {{1.f, -1.f}, {1.0f, -1.0f}},
            {{1.f, 1.f}, {1.0f, 1.0f}},
            {{-1.f, 1.f}, {-1.0f, 1.0f}}
        }};

        static std::array<uint16_t, 6> QUAD_INDICES = {0,1,2,2,3,0};
        
        const glm::vec4 color = {0,0,0,1};

        borders[0] = api.addPolygon({customWidth + wthickness, 0}, color, {wthickness, height / 2}, QUAD_VERTICES, QUAD_INDICES);
        borders[1] = api.addPolygon({-customWidth - wthickness, 0}, color, {wthickness, height / 2}, QUAD_VERTICES, QUAD_INDICES);
        borders[2] = api.addPolygon({0, customHeight + hthickness}, color, {width / 2, hthickness}, QUAD_VERTICES, QUAD_INDICES);
        borders[3] = api.addPolygon({0, -customHeight - hthickness}, color, {width / 2, hthickness}, QUAD_VERTICES, QUAD_INDICES);
        
        api.getInstance(borders[0]).drawIndex = 201;
        api.getInstance(borders[1]).drawIndex = 201;
        api.getInstance(borders[2]).drawIndex = 201;
        api.getInstance(borders[3]).drawIndex = 201;
        first = false;
        return;
    }

    glm::vec2 offset = api.getOffset();
    float zoom = api.getZoom();

    api.getInstance(borders[0]).position = {((customWidth + wthickness) / zoom) + offset.x, offset.y};
    api.getInstance(borders[0]).scale = {wthickness / zoom, height / (2 * zoom)};
    api.getInstance(borders[1]).position = {((-customWidth - wthickness) / zoom) + offset.x, offset.y};
    api.getInstance(borders[1]).scale = {wthickness / zoom, height / (2 * zoom)};
    api.getInstance(borders[2]).position = {offset.x, ((customHeight + hthickness) / zoom) + offset.y};
    api.getInstance(borders[2]).scale = {width / (2 * zoom), hthickness / zoom};
    api.getInstance(borders[3]).position = {offset.x, ((-customHeight - hthickness) / zoom) + offset.y};
    api.getInstance(borders[3]).scale = {width / (2 * zoom), hthickness / zoom};
}

struct WindowBorder{
    glm::vec4 color = {1,1,1,1};
    float width = 10.f;
    AspectRatio aspectRatio;

    bool operator==(const WindowBorder& other){
        if(this->color == other.color && this->width == other.width){
            return true;
        }
        return false;
    }
    bool operator!=(const WindowBorder& other){
        if(*this == other){
            return false;
        }
        return true;
    }
};

static inline void buildWindowBorder(ThING::API& api, const WindowBorder info){
    static WindowBorder windowBorder = info;
    static bool first = true;

    static std::pair<int, int> windowSize;
    api.getWindowSize(&windowSize.first, &windowSize.second);
    static std::array<Entity, 4> borders;
    float width = (float)windowSize.first / 2;
    float height = (float)windowSize.second / 2;
    float thickness = info.width * 2;

    if(!(info.aspectRatio.width == -1 || info.aspectRatio.height == -1)){
        float ratio = (float)info.aspectRatio.width / info.aspectRatio.height;
        float inverseRatio = (float)info.aspectRatio.height / info.aspectRatio.width;
        float customHeight = height;
        float customWidth = width;
        if(((float)width / height) < ratio){
            customHeight = (width / ratio);
        } else {
            customWidth = (height * ratio);
        }
        height = customHeight;
        width = customWidth;
    }

    if(first){
        static std::array<Vertex, 4> QUAD_VERTICES = {{
            {{-1.f, -1.f}, {-1.0f, -1.0f}},
            {{1.f, -1.f}, {1.0f, -1.0f}},
            {{1.f, 1.f}, {1.0f, 1.0f}},
            {{-1.f, 1.f}, {-1.0f, 1.0f}}
        }};

        static std::array<uint16_t, 6> QUAD_INDICES = {0,1,2,2,3,0};
        
        borders[0] = api.addPolygon({width, 0}, info.color, {thickness, height}, QUAD_VERTICES, QUAD_INDICES);
        borders[1] = api.addPolygon({-width, 0}, info.color, {thickness, height}, QUAD_VERTICES, QUAD_INDICES);
        borders[2] = api.addPolygon({0, height}, info.color, {width, thickness}, QUAD_VERTICES, QUAD_INDICES);
        borders[3] = api.addPolygon({0, -height}, info.color, {width, thickness}, QUAD_VERTICES, QUAD_INDICES);
        api.getInstance(borders[0]).drawIndex = 200;
        api.getInstance(borders[1]).drawIndex = 200;
        api.getInstance(borders[2]).drawIndex = 200;
        api.getInstance(borders[3]).drawIndex = 200;
        first = false;
        return;
    }
    float zoom = api.getZoom();
    glm::vec2 offset = api.getOffset();
    api.getInstance(borders[0]).color = info.color;
    api.getInstance(borders[0]).position = {(width / zoom) + offset.x, offset.y};
    api.getInstance(borders[0]).scale = {thickness / zoom, height / zoom};
    api.getInstance(borders[1]).color = info.color;
    api.getInstance(borders[1]).position = {(-width / zoom) + offset.x, offset.y};
    api.getInstance(borders[1]).scale = {thickness / zoom, height / zoom};
    api.getInstance(borders[2]).color = info.color;
    api.getInstance(borders[2]).position = {offset.x, (height / zoom) + offset.y};
    api.getInstance(borders[2]).scale = {width / zoom, thickness / zoom};
    api.getInstance(borders[3]).color = info.color;
    api.getInstance(borders[3]).position = {offset.x, (-height / zoom) + offset.y};
    api.getInstance(borders[3]).scale = {width / zoom, thickness / zoom};
}



}