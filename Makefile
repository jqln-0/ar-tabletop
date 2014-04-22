
.PHONY: default clean test configure build ar-tabletop

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

ar-tabletop: configure build
