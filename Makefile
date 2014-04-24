
.PHONY: default clean test configure build ar-tabletop format

default: ar-tabletop

clean:
	-rm -rf build/*

test:
	build/test/AllTests

configure:
	cd build; \
		cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

build: configure
	cd build; \
		make

format:
	clang-format -style=Google -i src/*.cpp src/*.h

ar-tabletop: configure build
