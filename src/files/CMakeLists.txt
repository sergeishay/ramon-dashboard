cmake_minimum_required(VERSION 3.5)
project(storagedemo C)

file(GLOB SRCS *.c json/*.c)
file(GLOB HDRS *.h json/*.h)

add_executable(storagedemo ${SRCS} ${HDRS})

install(TARGETS storagedemo)

add_custom_target(man_install
    COMMAND sudo cp storagedemo /srv/nfs/versal/bin/)
add_custom_target(super_install
    COMMAND scp storagedemo root@10.1.1.128:/bin)
