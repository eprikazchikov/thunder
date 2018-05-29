#include "commandbuffer.h"

static ICommandBuffer *s_Handler    = nullptr;

void ICommandBuffer::setHandler(ICommandBuffer *handler) {
    if(s_Handler == nullptr) {
        s_Handler   = handler;
    }
}

void ICommandBuffer::clearRenderTarget(bool clearColor, const Vector4 &color, bool clearDepth, float depth) {
    s_Handler->clearRenderTarget(clearColor, color, clearDepth, depth);
}

void ICommandBuffer::drawMesh(const Matrix4 &model, Mesh *mesh, uint32_t surface, uint8_t layer, MaterialInstance *material) {
    s_Handler->drawMesh(model, mesh, surface, layer, material);
}

void ICommandBuffer::setRenderTarget(const TargetBuffer &target, const RenderTexture *depth) {
    s_Handler->setRenderTarget(target, depth);
}

Vector4 ICommandBuffer::idToColor(uint32_t id) {
    uint8_t rgb[4];
    rgb[0]  = id;
    rgb[1]  = id >> 8;
    rgb[2]  = id >> 16;
    rgb[3]  = id >> 24;

    return Vector4((float)rgb[0] / 255.0f, (float)rgb[1] / 255.0f, (float)rgb[2] / 255.0f, (float)rgb[3] / 255.0f);
}

void ICommandBuffer::setColor(const Vector4 &color) {
    s_Handler->setColor(color);
}

void ICommandBuffer::setScreenProjection() {
    s_Handler->setViewProjection(Matrix4(), Matrix4::ortho(0.5f,-0.5f,-0.5f, 0.5f, 0.0f, 1.0f));
}

void ICommandBuffer::setViewProjection(const Matrix4 &view, const Matrix4 &projection) {
    s_Handler->setViewProjection(view, projection);
}

void ICommandBuffer::setGlobalValue(const char *name, const Variant &value) {
    s_Handler->setGlobalValue(name, value);
}

void ICommandBuffer::setGlobalTexture(const char *name, const Texture *value) {
    s_Handler->setGlobalTexture(name, value);
}

Matrix4 ICommandBuffer::projection() const {
    return s_Handler->projection();
}

Matrix4 ICommandBuffer::modelView() const {
    return s_Handler->modelView();
}

const Texture *ICommandBuffer::texture(const char *name) const {
    return s_Handler->texture(name);
}

void ICommandBuffer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    s_Handler->setViewport(x, y, width, height);
}
