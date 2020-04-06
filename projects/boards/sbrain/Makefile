# Makefile for configuring and building an AUSBEE project
#
#Set ausbee directory path. Can be relative or absolute

SHELL := /bin/bash

AUSBEE_DIR=../../../ausbee
BOARD_COM_ID=2

#Get project name
PROJECT_NAME=$(notdir $(patsubst i%/,%,$(lastword $(CURDIR))))

#List all source files
PROJECT_LOCAL_SRC_FILES=$(wildcard *.c)

#Include main Makefile
include $(AUSBEE_DIR)/ausbee.mk
