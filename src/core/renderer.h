#pragma once

#include "camera.h"
#include "graphics/mesh.h"
#include "graphics/model3d.h"
#include "graphics/shader.h"

class Renderer {
public:
    Renderer();

    void init();

    Camera& getViewCamera() { return _viewCamera; };
    const Camera& getViewCamera() const { return _viewCamera; }

    void setClearColor(const ColorRGB& c) { _clearColor = c; }
    void toggleWireframe();

    void draw(MeshRef& mesh, ShaderRef& shader);
    void draw(Model3DRef& model, ShaderRef& shader);

    void beginFrame();
    void endFrame();

private:
    Camera   _viewCamera;
    ColorRGB _clearColor;
    bool     _wireframeEnabled;
};