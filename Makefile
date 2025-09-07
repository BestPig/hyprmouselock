CXXFLAGS = -shared -fPIC --no-gnu-unique -g -std=c++2b -Wno-c++11-narrowing
INCLUDES = `pkg-config --cflags pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon`
LIBS = `pkg-config --libs pangocairo`

SRC = main.cpp
TARGET = hyprmouselock.so

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ $> -o $@ $(LIBS) -O2

clean:
	rm ./$(TARGET)

load: all unload
	hyprctl plugin load $(PWD)/$(TARGET)

unload:
	hyprctl plugin unload $(PWD)/$(TARGET)

meson-build:
	mkdir -p build
	cd build && meson .. && ninja

.PHONY: all meson-build clean load unload