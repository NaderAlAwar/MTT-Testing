build:	bin_directory
	g++ src/cpp/*.cpp -lz3 -o bin/run_mtt

bin_directory:
	mkdir -p bin