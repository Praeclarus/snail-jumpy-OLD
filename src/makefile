IPATHS := -I"C:\Users\Tyler Santos\Documents\Programs\Libs\SDL2-2.0.9\x86_64-w64-mingw32\include"

LPATHS := -L"C:\Users\Tyler Santos\Documents\Programs\Libs\SDL2-2.0.9\x86_64-w64-mingw32\lib"

LIBS   := mingw32 SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer
ULIBS  :=         SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer

CC     := gcc
CFLAGS := -Wall -g

SOURCES := $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
TARGET  := ../build/Snail_Jumpy

run: compile
	$(TARGET)

runonly:
	$(TARGET)

compile: $(TARGET)

ubuntu: $(OBJECTS)
	$(CC) -o $(TARGET) $(CFLAGS) $(SOURCES) $(addprefix -l, $(ULIBS))

compile_file:
	del  $(FILE).o
	make $(FILE).o

%.o: %.c
	$(CC) $^ -c -o $@ $(CFLAGS) $(IPATHS)

# KEEPS LIBS after object files
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS) $(addprefix -l, $(LIBS)) $(LPATHS)

alternate:
	make clean

dump:
	objdump -D -S $(TARGET).exe > $(TARGET).rasm.txt

clean:
	del /S *.o

.PHONY: clean dump
