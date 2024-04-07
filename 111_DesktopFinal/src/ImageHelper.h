#pragma once

#include <cstdint>

#include <imgui.h>

/**
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 * 
 * �̹��� ó���� �⺻������ Backend(Graphics API)�� �������̴�.
 * �̿� ���� ������ ���� ��ť��Ʈ�� �����ϵ��� �Ѵ�.
 * 
 * ImTextureID�� ����� ���ε� �̰� �׳� "void*"�̴�.
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
