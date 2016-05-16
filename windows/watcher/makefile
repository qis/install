# Variables.
PROJECT    := `grep -m 1 'project(' CMakeLists.txt | sed 's/.*project(\([^ )]\+\).*/\1/g'`
GENERATOR  ?= Visual Studio 14 2015 Win64
CONFIG     ?= Release
PREFIX     ?= ..

# Configures and builds the project.
all: build/CMakeCache.txt
	@cmake --build build --config $(CONFIG)

# Configures the project.
build/CMakeCache.txt: CMakeLists.txt build
	@cd build && cmake -G "$(GENERATOR)" -DCMAKE_INSTALL_PREFIX:PATH="$(PREFIX)" ..

# Creates the build directory.
build:
	@mkdir build

# Installs the project.
install: build/CMakeCache.txt
	@cmake --build build --config $(CONFIG) --target install

# Compiles and runs the project.
run: all
	@build/$(CONFIG)/$(PROJECT).exe

# Removes the build directory.
clean:
	rm -rf build
