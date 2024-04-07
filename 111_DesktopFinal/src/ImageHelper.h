#pragma once

#include <cstdint>

#include <imgui.h>

/**
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 * 
 * 이미지 처리는 기본적으로 Backend(Graphics API)에 의존적이다.
 * 이에 대한 내용은 공식 도큐먼트를 참고하도록 한다.
 * 
 * ImTextureID를 사용할 것인데 이건 그냥 "void*"이다.
 */

struct ImageChunk
{
    ImTextureID texID;

    std::uint32_t width;
    std::uint32_t height;
};

ImageChunk* LoadImageChunk(const char* filename);
void UnloadImageChunk(ImageChunk* imageChunk);

void DisplayImage(const ImageChunk& imageChunk);
