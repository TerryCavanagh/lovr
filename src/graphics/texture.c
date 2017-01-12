#include "graphics/texture.h"
#include "graphics/graphics.h"
#include "util.h"
#include <stdlib.h>

Texture* lovrTextureCreate(TextureData* textureData) {
  Texture* texture = lovrAlloc(sizeof(Texture), lovrTextureDestroy);
  if (!texture) return NULL;

  int w = textureData->width;
  int h = textureData->height;

  texture->textureData = textureData;
  glGenTextures(1, &texture->id);
  lovrTextureBind(texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData->data);
  lovrTextureSetFilter(texture, FILTER_LINEAR, FILTER_LINEAR);
  lovrTextureSetWrap(texture, WRAP_REPEAT, WRAP_REPEAT);

  texture->framebuffer = 0;
  texture->renderbuffer = 0;

  return texture;
}

Texture* lovrTextureCreateWithFramebuffer(TextureData* textureData, TextureProjection projection) {
  Texture* texture = lovrTextureCreate(textureData);
  if (!texture) return NULL;

  texture->projection = projection;
  glGenFramebuffers(1, &texture->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, texture->framebuffer);

  if (projection == PROJECTION_PERSPECTIVE) {
    glGenRenderbuffers(1, &texture->renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, texture->renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureData->width, textureData->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->renderbuffer);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return texture;
}

void lovrTextureDestroy(const Ref* ref) {
  Texture* texture = containerof(ref, Texture);
  lovrTextureDataDestroy(texture->textureData);
  if (texture->framebuffer) {
    glDeleteFramebuffers(1, &texture->framebuffer);
  }
  glDeleteTextures(1, &texture->id);
  free(texture);
}

void lovrTextureDataDestroy(TextureData* textureData) {
  free(textureData->data);
  free(textureData);
}

void lovrTextureBind(Texture* texture) {
  glBindTexture(GL_TEXTURE_2D, texture->id);
}

void lovrTextureBindFramebuffer(Texture* texture) {
  if (!texture->framebuffer) {
    error("Texture cannot be used as a canvas");
  }

  int w = texture->textureData->width;
  int h = texture->textureData->height;

  lovrGraphicsBindFramebuffer(texture->framebuffer);
  lovrGraphicsSetViewport(0, 0, w, h);

  if (texture->projection == PROJECTION_ORTHOGRAPHIC) {
    float projection[16];
    mat4_setOrthographic(projection, 0, w, 0, h, -1, 1);
    lovrGraphicsSetProjectionRaw(projection);
  } else if (texture->projection == PROJECTION_PERSPECTIVE) {
    // lovrGraphicsSetProjection(); // TODO perspective
  }
}

int lovrTextureGetHeight(Texture* texture) {
  return texture->textureData->height;
}

int lovrTextureGetWidth(Texture* texture) {
  return texture->textureData->width;
}

void lovrTextureGetFilter(Texture* texture, FilterMode* min, FilterMode* mag) {
  *min = texture->filterMin;
  *mag = texture->filterMag;
}

void lovrTextureSetFilter(Texture* texture, FilterMode min, FilterMode mag) {
  texture->filterMin = min;
  texture->filterMag = mag;
  lovrTextureBind(texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
}

void lovrTextureGetWrap(Texture* texture, WrapMode* horizontal, WrapMode* vertical) {
  *horizontal = texture->wrapHorizontal;
  *vertical = texture->wrapVertical;
}

void lovrTextureSetWrap(Texture* texture, WrapMode horizontal, WrapMode vertical) {
  texture->wrapHorizontal = horizontal;
  texture->wrapVertical = vertical;
  lovrTextureBind(texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontal);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vertical);
}
