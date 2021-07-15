all: program1 program2

program1:
	g++ test.cpp -o test `pkg-config --cflags --libs opencv`

program2:
	g++ test_bee.cpp -o test_bee `pkg-config --cflags --libs opencv`