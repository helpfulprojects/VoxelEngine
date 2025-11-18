#include "pch.h"
#include "TextureSubImage2D.h"
#include <stb_image.h>
#include "VoxelEngine/Core/Utils.h"
namespace VoxelEngine {
TextureSubImage2D::TextureSubImage2D(const std::string &path) {
  VE_PROFILE_FUNCTION;
  int width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  stbi_uc *data = nullptr;
  {

    VE_PROFILE_SCOPE(
        "stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&))");
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  }
  VE_CORE_ASSERT(data, "Failed to load image!");
  VE_CORE_ASSERT(channels == 4 || channels == 3 || channels == 2 ||
                     channels == 1,
                 "Format not supported!");
  m_Width = width;
  m_Height = height;
  m_Channels = channels;
  m_Data = data;
  m_Name = Utils::ExtractNameFromFilePath(path);
}
TextureSubImage2D::~TextureSubImage2D() {
  if (m_Data) {
    FreeData();
  }
}
void TextureSubImage2D::SetOffset(int xOffset, int yOffset) {
  m_XOffset = xOffset;
  m_YOffset = yOffset;
  m_Name += std::to_string(xOffset) + "," + std::to_string(yOffset);
}
void TextureSubImage2D::Combine(const Ref<TextureSubImage2D> other) {
  if (other->m_Channels < 3)
    return;
  const texture_data *otherData = other->GetData();
  int numPixels = m_Width * m_Height;
  for (int i = 0; i < numPixels; i++) {
    int idx = i * m_Channels;
    int otherIdx = i * other->m_Channels;
    if (otherData[otherIdx + other->m_Channels - 1] == 0)
      continue;
    int minChannel = std::min(m_Channels, other->m_Channels);
    for (int channel = 0; channel < minChannel; channel++) {
      m_Data[idx + channel] = otherData[otherIdx + channel];
    }
  }
}
void TextureSubImage2D::Rotate(int rotation) {
  int newW = m_Width;
  int newH = m_Height;
  if (rotation == 90 || rotation == 270) {
    newW = m_Height;
    newH = m_Width;
  }
  stbi_uc *rotated = new stbi_uc[newW * newH * m_Channels];
  auto index = [&](int x, int y, int w) { return (y * w + x) * m_Channels; };
  for (int y = 0; y < m_Height; y++) {
    for (int x = 0; x < m_Width; x++) {

      int srcIdx = index(x, y, m_Width);
      int dstIdx = 0;

      switch (rotation) {
      case 90:
        dstIdx = index(m_Height - 1 - y, x, newW);
        break;

      case 180:
        dstIdx = index(m_Width - 1 - x, m_Height - 1 - y, newW);
        break;

      case 270:
        dstIdx = index(y, m_Width - 1 - x, newW);
        break;

      default:
        dstIdx = srcIdx;
        break;
      }

      for (int c = 0; c < m_Channels; c++)
        rotated[dstIdx + c] = m_Data[srcIdx + c];
    }
  }
  FreeData();
  m_Data = rotated;
}
void TextureSubImage2D::Colorize(const glm::vec3 &color) {
  // if (m_Channels == 2) {
  //   uint32_t newChannels = 4;
  //   stbi_uc *newData = new stbi_uc[m_Width * m_Height * newChannels];
  //   for (int y = 0; y < m_Height; y++) {
  //     for (int x = 0; x < m_Width; x++) {

  //      int srcIdx = (y * m_Width + x) * newChannels;
  //      int dstIdx = srcIdx;
  //      for (int c = 0; c < newChannels - 1; c++)
  //        newData[dstIdx + c] = m_Data[srcIdx + 0];
  //      newData[dstIdx + 3] = m_Data[srcIdx + 1];
  //    }
  //  }
  //  FreeData();
  //  m_Data = newData;
  //  m_Channels = newChannels;
  //}
  if (m_Channels < 3)
    return;
  glm::vec3 colorNormalized = color / 256.0f;
  int numPixels = m_Width * m_Height;
  for (int i = 0; i < numPixels; i++) {
    int idx = i * m_Channels;
    m_Data[idx + 0] = std::ceil(m_Data[idx + 0] * colorNormalized.r);
    m_Data[idx + 1] = std::ceil(m_Data[idx + 1] * colorNormalized.g);
    m_Data[idx + 2] = std::ceil(m_Data[idx + 2] * colorNormalized.b);
  }
}
void TextureSubImage2D::FreeData() {
  stbi_image_free(m_Data);
  m_Data = nullptr;
}
} // namespace VoxelEngine
