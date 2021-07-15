all: program1 program2

program1: test.cpp
    g++ test.cpp -o test `pkg-config --cflags --libs opencv`

program2: test_bee.cpp
    g++ test_bee.cpp -o test_bee `pkg-config --cflags --libs opencv`