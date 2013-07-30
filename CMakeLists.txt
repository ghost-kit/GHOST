CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("cxform")

add_library(
            cxform
            STATIC
            cxform-auto.c
            cxform-manual.c
            )
