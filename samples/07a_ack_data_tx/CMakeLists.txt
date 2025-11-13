cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(07a_ack_data_tx)
target_sources(app PRIVATE
  main.c
)