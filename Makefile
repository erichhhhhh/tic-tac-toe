CXX = g++
CXXFLAGS = -std=c++20 -g \
-Iinclude \
-Iinclude/engine \
-Iinclude/ui \
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
	@mkdir -p build/language
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)
	cp -rf res/* build/language

clean:
	rm -rf build
