
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
	mkdir -p build
	cd build; \
		make -j 8

format:
	clang-format -style=Google -i src/*.cpp src/*.h test/*.cpp

ar-tabletop: configure build
