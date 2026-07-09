#pragma once
#include "ThING/api.h"
#include "ThING/types/apiTypes.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include <cstdint>
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
                api.getInstance(e).drawIndex = -20;
                
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
                api.getInstance(e).drawIndex = -20;
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

}