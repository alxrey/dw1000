cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ex_06a_ss_twr_init)
target_sources(app PRIVATE
  main.c
)