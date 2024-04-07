#include "ImageHelper.h"

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
 * 전처리기에 _CRT_SECURE_NO_WARNINGS 추가
 * 혹은 stb 라이브러리를 직접 수정하여 "#ifdef __STDC_LIB_EXT1__" 구문을 수정 
 *   -> #if defined(__STDC_LIB_EXT1__) || defined(_MSC_VER) && _MSC_VER >= 1400
 *   -> #if defined(_WIN32)
 * 
 * GitHub을 보면 PR 요청이 무수히 많은데 왜 반영을 안 하는지 모르겠음.
 */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

extern SDL_Window*   g_Window;
extern SDL_Renderer* g_Renderer;

ImageChunk* LoadImageChunk(const char* filename)
{
    /**
     * https://wiki.libsdl.org/SDL2_image/IMG_Load
     * 위 함수를 써도 될 것 같은데 일단 여기선 예제에 나온 대로 stdi_load()를 쓰도록 함.
     */
    int channels = 0;
    int width  = 0u;
    int height = 0u;

    // https://gamedev.net/forums/topic/707863-would-anybody-know-how-to-properly-load-an-opengl-texture-after-loading-an-image-with-stdasync/5430736/
    // you would have to validate that stbi_load doesn't maintain any global state and is thread-safe( I haven't use it so no idea).
    // 아마도 스레드 안전할 것...
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);

    if (nullptr == data)
    {
        std::cerr << "Failed to load image from " << filename << " | reason : " << stbi_failure_reason();

        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, channels * 8, channels * width,
                                                    0x00'00'00'ff, 0x00'00'ff'00, 0x00'ff'00'00, 0xff'00'00'00);
    
    if (nullptr == surface)
    {
        std::cerr << "Failed to create SDL surface : " << SDL_GetError();

        stbi_image_free(data);

        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(g_Renderer, surface);

    if (nullptr == tex)
    {
        std::cerr << "Failed to create SDL texture : " << SDL_GetError();

        SDL_FreeSurface(surface);
        stbi_image_free(data);

        return nullptr;
    }

    SDL_FreeSurface(surface);
    stbi_image_free(data);

    /**
     * 생성 부분
     */
    auto imgChunk = new ImageChunk{ };
    {
        imgChunk->texID  = (ImTextureID)tex;
        imgChunk->width  = width;
        imgChunk->height = height;
    }

    return imgChunk;
}

void UnloadImageChunk(ImageChunk* imageChunk)
{
    SDL_DestroyTexture((SDL_Texture*)imageChunk->texID);

    delete imageChunk;
}

void DisplayImage(const ImageChunk& imageChunk)
{
    const auto imageSize = ImVec2{ (float)imageChunk.width, (float)imageChunk.height };

    ImGui::Image(imageChunk.texID, imageSize);
}
