CC = gcc
CFLAGS = -g -Wall
# main executable file first
TARGET = proj02
# object files next
OBJS = shellper.o jobnode.o paths.o runprocesses.o main.o
# header files next
DEPS = shellper.h jobnode.h paths.h runprocesses.h
.PHONY : clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

.c.o: $(DEPS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) *~

