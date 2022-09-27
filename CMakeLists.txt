project(rc_car)

cmake_minimum_required(VERSION 3.0)

add_executable(rc_car_hackrf_41
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_41MHz/car.c
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_41MHz/main.c
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_41MHz/rf.c
)

target_include_directories(rc_car_hackrf_41 PRIVATE
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_41MHz/
)

target_link_libraries(rc_car_hackrf_41
    hackrf
    pthread
    curses
)

add_executable(rc_car_hackrf_27
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_27MHz/main.c
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_27MHz/rf.c
)

target_include_directories(rc_car_hackrf_27 PRIVATE
    ${PROJECT_SOURCE_DIR}/rc_car_hackrf_27MHz/
)

target_link_libraries(rc_car_hackrf_27
    hackrf
    pthread
    curses
)
