#pragma once

#include "graphics/mesh.h"
#include "graphics/model3d.h"
#include "graphics/shader.h"

class Renderer {
public:
    Renderer();

    void init();

    void setClearColor(const ColorRGB& c) { _clearColor = c; }
    void toggleWireframe();

    void draw(MeshRef& mesh, ShaderRef& shader);
    void draw(Model3DRef& model, ShaderRef& shader);

    void beginFrame();
    void endFrame();

private:
    ColorRGB _clearColor;
    bool     _wireframeEnabled;
};