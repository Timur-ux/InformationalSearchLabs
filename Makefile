build: generate
	cmake --build ./build -- -j4

generate:
	mkdir -p ./build
	conan install . --output-folder=build --build=missing
	cmake -S . -B ./build \
		-DCMAKE_BUILD_TYPE=RELEASE \
		-DCMAKE_TOOLCHAIN_FILE=./build/build/Release/generators/conan_toolchain.cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1
	mv ./build/compile_commands.json .	

