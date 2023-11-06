# ImGuiExamples
추후 개발을 원활하게 진행하기 위해 직접 만들거나 수집한 ImGui 예제 저장소

# 작업환경
- Visual Studio 2022(Debug, x64)
- vcpkg
- SDL2

# vcpkg
- vcpkg install sdl2
- vcpkg install sdl2-image[libjpeg-turbo,libwebp,tiff]
- vcpkg install sdl2-mixer[opusfile,fluidsynth,libflac,libmodplug,mpg123]
- vcpkg install sdl2-ttf
- vcpkg install sdl2-gfx
- vcpkg install imgui[win32-binding,sdl2-binding,sdl2-renderer-binding]:x64-windows

# 참고사항
- 컴파일되어 변환되는 중간 단계의 파일은 솔루션이 위치한 bin-int 폴더에 위치
- 실행파일은 솔루션이 위치한 bin 폴더에 위치
