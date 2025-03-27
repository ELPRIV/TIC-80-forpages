################################
# Sokol
################################

if(BUILD_SOKOL)

    set(SOKOL_LIB_SRC ${CMAKE_SOURCE_DIR}/src/system/sokol/sokol.c)

    add_library(sokol STATIC ${SOKOL_LIB_SRC})

    if(APPLE)
        target_compile_definitions(sokol PUBLIC SOKOL_METAL)
    elseif(LINUX)
        target_compile_definitions(sokol PUBLIC SOKOL_GLCORE)
    elseif(WIN32)
        target_compile_definitions(sokol PUBLIC SOKOL_D3D11 UNICODE)
    elseif(EMSCRIPTEN)
        target_compile_definitions(sokol PUBLIC SOKOL_WGPU)
    endif()

    if(APPLE)

        target_compile_options(sokol PRIVATE -x objective-c)

        target_link_libraries(sokol PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework QuartzCore"
            "-framework Metal"
            "-framework MetalKit"
            "-framework AudioToolbox"
        )

    elseif(LINUX)
        target_link_libraries(sokol PRIVATE X11 GL Xi Xcursor m dl asound)
    elseif(WIN32)
        target_link_libraries(sokol PRIVATE D3D11)
    endif()

    target_include_directories(sokol PRIVATE ${THIRDPARTY_DIR}/sokol)
endif()

################################
# Sokol standalone cart player
################################

if(BUILD_PLAYER AND BUILD_SOKOL)

    add_executable(player-sokol WIN32 ${CMAKE_SOURCE_DIR}/src/system/sokol/player.c)

    target_include_directories(player-sokol PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${THIRDPARTY_DIR}/sokol
        ${CMAKE_SOURCE_DIR}/src)

    target_link_libraries(player-sokol PRIVATE tic80core sokol)
endif()

################################
# TIC-80 app (Sokol)
################################

if(BUILD_SOKOL)

    # !TODO: make it working on CI
    # if(APPLE)
    #     set(SHDC osx_arm64/sokol-shdc)
    # else()
    #     set(SHDC win32/sokol-shdc.exe)
    # endif()

    # add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/build/crt.h
    #     COMMAND ${CMAKE_SOURCE_DIR}/vendor/sokol-tools-bin/bin/${SHDC} -i ${CMAKE_SOURCE_DIR}/src/system/sokol/crt.glsl -o ${CMAKE_SOURCE_DIR}/build/crt.h -l glsl410:glsl300es:hlsl4:metal_macos:metal_ios:wgsl --ifdef
    #     DEPENDS ${CMAKE_SOURCE_DIR}/src/system/sokol/crt.glsl)

    set(TIC80_SRC 
        ${CMAKE_SOURCE_DIR}/src/system/sokol/main.c
        ${CMAKE_SOURCE_DIR}/build/crt.h
    )

    if(WIN32)

        configure_file("${CMAKE_SOURCE_DIR}/build/windows/tic80.rc.in" "${CMAKE_SOURCE_DIR}/build/windows/tic80.rc")
        set(TIC80_SRC ${TIC80_SRC} "${CMAKE_SOURCE_DIR}/build/windows/tic80.rc")

        add_executable(tic80 WIN32 ${TIC80_SRC})
    else()
        add_executable(tic80 ${TIC80_SRC})
    endif()

    if(EMSCRIPTEN)
        set_target_properties(tic80 PROPERTIES LINK_FLAGS "-s USE_WEBGPU=1 -s ALLOW_MEMORY_GROWTH=1 -s FETCH=1 --pre-js ${CMAKE_SOURCE_DIR}/build/html/prejs.js -lidbfs.js")
    endif()


    target_include_directories(tic80 PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${THIRDPARTY_DIR}/sokol)

    target_link_libraries(tic80 PRIVATE tic80studio sokol)

endif()
