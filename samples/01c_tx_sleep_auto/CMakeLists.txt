cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ex_01c_tx_sleep_auto)
target_sources(app PRIVATE
  main.c
)