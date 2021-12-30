CC=g++
CXXFLAGS = -std=c++11 -Wall -Werror -Wextra -Wfatal-errors
TARGET=huffman
OBJS=$(TARGET).o
OTHERDEPS=heap.h heap.cc
OTHERTARGETS= compress uncompress

all: compress uncompress

compress: huffman
	cp huffman compress

uncompress: huffman
	cp huffman uncompress


#link rule:
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

#compile rule:
%.o: %.cc $(OTHERDEPS)
	$(CC) -c $(CXXFLAGS) $< -o $@

#rule to delete object and executable files
clean:
	rm -f $(TARGET) $(OBJS) $(OTHERTARGETS)
