CONAN_TOOLCHAIN ?=./build/build/Release/generators/conan_toolchain.cmake 
CMAKE_COMMON_FLAGS ?=  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 #-DCMAKE_TOOLCHAIN_FILE=$(CONAN_TOOLCHAIN)
CMAKE_DEBUG_FLAGS ?= -DUSERVER_SANITIZE='' -DCMAKE_BUILD_TYPE=DEBUG $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS ?=	-DCMAKE_BUILD_TYPE=RELEASE $(CMAKE_COMMON_FLAGS)
NPROCS ?= $(shell nproc)

all: build-debug

build:
	mkdir build

.PHONY: build-debug build-release
build-debug build-release: build-%: build cmake-% 
	cmake --build ./build/build-$* -j $(NPROC) 

.PHONY: test-debug test-release
test-debug test-release: test-%: build cmake-%
	cmake --build ./build/build-$* -j $(NPROC) --target tests
	cd ./build/build-$* && ctest

$(CONAN_TOOLCHAIN):
	conan install . --output-folder=build --build=missing

.PHONY: cmake-debug
cmake-debug: #$(CONAN_TOOLCHAIN)
	cmake -S . -B ./build/build-debug $(CMAKE_DEBUG_FLAGS)
	mv ./build/build-debug/compile_commands.json .

.PHONY: cmake-release
cmake-release: #$(CONAN_TOOLCHAIN)
	cmake -S . -B ./build/build-release $(CMAKE_RELEASE_FLAGS)
	mv ./build/build-release/compile_commands.json .

.PHONY: clean
clean:
	rm -rf build
