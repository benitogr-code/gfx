#include "gfx_model.h"
#include "file_utils.h"
#include "asset_manager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*static*/ GfxModelRef GfxModel::Create() {
  GfxModelRef model(new GfxModel());

  return model;
}
