cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_sie_pit_viewer C)

include_directories("../libsie/src/include")
link_libraries(crt crt_helper c gcc m z png ft sie)
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(SiePITViewer_ELKA ${SOURCES})
target_sdk_setup(SiePITViewer_ELKA ELKA)
target_link_directories(SiePITViewer_ELKA PUBLIC "../libsie/build/lib/ELKA")
target_compile_options(SiePITViewer_ELKA BEFORE PUBLIC -std=gnu99 -Wall -Werror)

add_executable(SiePITViewer_NSG ${SOURCES})
target_sdk_setup(SiePITViewer_NSG NEWSGOLD)
target_link_directories(SiePITViewer_NSG PUBLIC "../libsie/build/lib/NSG")
target_compile_options(SiePITViewer_NSG BEFORE PUBLIC -std=gnu99 -Wall -Werror)

add_executable(SiePITViewer_SG ${SOURCES})
target_sdk_setup(SiePITViewer_SG SG)
target_link_directories(SiePITViewer_SG PUBLIC "../libsie/build/lib/SG")
target_compile_options(SiePITViewer_SG BEFORE PUBLIC -std=gnu99 -Wall -Werror)
