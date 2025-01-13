# 编译Skia

## Debug

```
clang_win = "C:\Program Files\LLVM"
cc = "clang"
cxx = "clang++"
extra_cflags = [ "/MTd" ]
is_official_build = false
is_debug = false
skia_use_system_expat = false
skia_use_system_libjpeg_turbo = false
skia_use_system_libpng = false
skia_use_system_libwebp = false
skia_use_system_zlib = false
skia_use_system_harfbuzz = false
skia_use_icu = false
skia_use_vulkan = false
skia_use_direct3d = false
skia_use_webgpu = false
skia_use_angle = false
skia_use_metal = false
skia_use_ffmpeg = false
skia_use_dawn = true
skia_use_gl = true   
skia_enable_tools = false
skia_enable_ganesh = true
skia_enable_graphite = true
skia_enable_gpu = true
skia_enable_skparagraph = true
skia_enable_skshaper = true
skia_enable_skunicode = true
```

```
bin/gn gen out/debug
ninja -C out/debug
```

## Release

```
clang_win = "C:\Program Files\LLVM"
cc = "clang"
cxx = "clang++"
extra_cflags = [ "/MT", "-O2", "-fno-exceptions", "-fno-rtti" ]
is_official_build = false
is_debug = false
skia_use_system_expat = false
skia_use_system_libjpeg_turbo = false
skia_use_system_libpng = false
skia_use_system_libwebp = false
skia_use_system_zlib = false
skia_use_system_harfbuzz = false
skia_use_icu = false
skia_use_vulkan = false
skia_use_direct3d = false
skia_use_webgpu = false
skia_use_angle = false
skia_use_metal = false
skia_use_ffmpeg = false
skia_use_dawn = true
skia_use_gl = true   
skia_enable_tools = false
skia_enable_ganesh = true
skia_enable_graphite = true
skia_enable_gpu = true
skia_enable_skparagraph = true
skia_enable_skshaper = true
skia_enable_skunicode = true
```
```
ninja -C out/release
```