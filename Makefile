.PHONY: all clean clean-output

# Main compiler
CXX = g++
NVCC = nvcc

# Directories
INC = include
SRC = src
MODULES = $(SRC)/modules

# Command line components
COMMAND_LINE_OBJS = $(MODULES)/command_line_parser.o \
                    $(MODULES)/application.o \
                    $(MODULES)/command_line_options.o

# Main program
MAIN_OBJ = $(SRC)/Program.o

# Image processing modules
MODULES_IMPLS = $(MODULES)/impls/image_tools.o
MODULES_IMPLS_CPU = $(MODULES)/impls_cpu/image_codec_lodepng.o \
                    $(MODULES)/impls_cpu/image_tools.o
MODULES_SHARED = $(MODULES)/impls_shared/image_tools.o

# LodePNG components
LODE_OBJ = $(INC)/lodepng/lodepng.o

# Compiler flags
CXXFLAGS = -I$(INC) -I$(INC)/lodepng -I$(MODULES) -Wall -Wextra -pedantic -O0 -g
NVCCFLAGS = -I$(INC) -I$(MODULES) --debug --device-debug
LDFLAGS_CUDA = -L/opt/cuda/lib -lcuda -lcudart -lnvjpeg_static -lculibos

# Main target
all: graphics-lode.out

# Pattern rules
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -c -o $@ $<

# Command line components
$(MODULES)/command_line_parser.o: $(MODULES)/command_line_parser.cpp $(INC)/command_line_parser.h
$(MODULES)/application.o: $(MODULES)/application.cpp $(INC)/application.h
$(MODULES)/command_line_options.o: $(MODULES)/command_line_options.cpp $(INC)/command_line_options.h

# LodePNG implementation
graphics-lode.out: $(COMMAND_LINE_OBJS) $(MAIN_OBJ) $(MODULES_IMPLS) $(MODULES_IMPLS_CPU) $(MODULES_SHARED) $(LODE_OBJ)
	$(CXX) $(CXXFLAGS) -DLODE_IMPL $^ -o $@ -lboost_program_options

# CUDA implementation (example)
graphics-cuda.out: $(COMMAND_LINE_OBJS) $(MAIN_OBJ) $(MODULES)/impls_hw_accel/image_codec_cuda.o
	$(NVCC) $(NVCCFLAGS) -DCUDA_IMPL $^ -o $@ $(LDFLAGS_CUDA) -lboost_program_options

# Cleanup
clean:
	rm -f $(COMMAND_LINE_OBJS) \
	      $(MAIN_OBJ) \
	      $(MODULES_IMPLS) \
	      $(MODULES_IMPLS_CPU) \
	      $(MODULES_SHARED) \
	      $(LODE_OBJ) \
	      graphics-lode.out \
	      graphics-cuda.out

clean-output:
	rm -f output/*
