#include "app/sandbox_app.h"

const char* parseAssetsFolder(int argc, char* argv[]) {
    const char* folder = "";

    for (int i = 0; i < argc - 1; ++i) {
        if (strcmp(argv[i], "--assets") == 0) {
            folder = argv[i+1];
            break;
        }
    }

    return folder;
}

Application* createApplication() {
  return new SandboxApp();
}

int main(int argc, char* argv[]) {
  StartupParams params;
  params.appCreatorFunc = createApplication;
  params.windowDesc.width = 1024;
  params.windowDesc.height = 768;
  params.windowDesc.title = "GFX Sandbox";
  params.assetsFolder = parseAssetsFolder(argc, argv);

  return startApplication(params);
}
