#include "application.h"
#include "file_utils.h"
#include "font.h"

#include <SDL.h>

int startApplication(const StartupParams& params) {
  Logger::init();

  LOG_INFO("Initializing application...");

  FileUtils::init(params.assetsFolder);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LOG_ERROR("Fail to initialize SDL");
    return -1;
  }

  if (!Font::init()) {
    LOG_ERROR("Fail to initialize Font");
    return -1;
  }

  auto application = params.appCreatorFunc();
  if (application->init(params.windowDesc)) {
    application->run();
  }

  LOG_INFO("Closing application...");

  application->shutdown();
  delete application;

  Font::shutdown();

  SDL_Quit();

  return 0;
}
