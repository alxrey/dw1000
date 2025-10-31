cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ex_04a_cont_wave)
target_sources(app PRIVATE
  main.c
)