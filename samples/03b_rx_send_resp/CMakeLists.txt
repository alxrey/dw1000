cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(03b_rx_send_resp)
target_sources(app PRIVATE
  main.c
)