CXX = g++
CXXFLAGS = -std=c++20 -g \
-Iinclude \
-Iinclude/engine \
-Iinclude/ui \
-Iinclude/config \
-I/usr/include \
-Iexternal/jsoncpp/include \
-Iexternal/rang/include \
-L/usr/lib64 \
-lftxui-component \
-lftxui-dom \
-lftxui-screen

SRC = $(shell find src -name "*.cpp") \
      external/jsoncpp/src/lib_json/json_reader.cpp \
      external/jsoncpp/src/lib_json/json_value.cpp \
      external/jsoncpp/src/lib_json/json_writer.cpp

OUT = build/tictactoe

all:
	@mkdir -p ~/.TicTacToe/language
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)
	cp -rf res/* ~/.TicTacToe/language

clean:
	rm -rf ~/.TicTacToe
	rm -rf build
