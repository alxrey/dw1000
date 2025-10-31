cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ex_08b_low_power_listen_tx)
target_sources(app PRIVATE
  main.c
)