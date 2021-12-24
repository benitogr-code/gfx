#include "sandbox_app.h"

SandboxApp::SandboxApp() {

}

bool SandboxApp::onInit() {
  return true;
}

void SandboxApp::onShutdown() {
}

void SandboxApp::onInputEvent(const InputEvent& event) {
  LOG_WARN("Input event: Key={0} State={1}", event.keyId, event.state);
}

void SandboxApp::onUpdate(const UpdateContext& ctx) {
}
