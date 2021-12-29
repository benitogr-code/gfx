#pragma once

#include "graphics/buffers.h"
#include "graphics/shader.h"

class Renderer {
public:
    Renderer();

    void init();

    void setClearColor(const ColorRGB& c) { _clearColor = c; }
    void toggleWireframe();

    void drawVertexArray(const VertexArrayRef& va, const ShaderRef& shader);

    void beginFrame();
    void endFrame();

private:
    ColorRGB _clearColor;
    bool     _wireframeEnabled;
};