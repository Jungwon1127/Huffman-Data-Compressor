#include <iostream>
#include <fstream>
#include "heap.h"


struct hnode {  // huffman node based on 10/26 lc_node
  unsigned char letter;
  size_t count;
  hnode *left;
  hnode *right;
  hnode (unsigned char let, size_t count,
         hnode*left = nullptr, hnode *right=nullptr) :
         letter(let), count(count), left(left), right(right) {}
};


int hnode_compare(hnode * const& a, hnode * const& b)
{
  if (a == nullptr and b == nullptr) return 0;
  if (a == nullptr) return -1;
  if (b == nullptr) return 1;
  if (a->count == b->count) return a->letter - b->letter;
  return a->count - b->count;
}

void usage(std::string message)
{
  std::cerr << "Usage: " << message << std::endl;
  exit(1);
}

// Forward declarations for helper methods (compress):
void write_tree(std::ostream & ostr, hnode *tree);
void get_codes(hnode *tree, std::string *codes, std::string path);
unsigned char bit_string_to_byte(const std::string& bit_string);

// Forward declarations for helper methods (uncompress):
std::string byte_to_bit_string(unsigned char byte);
hnode *read_tree();
bool is_leaf(hnode * node);

// Writes down the tree
void write_tree(std::ostream & ostr, hnode *tree) {
  // Base case to stop function when nullptr
  if (!tree) {
    return;
  }

  // checks if node is a leaf, then outputs an L if it is
  if (is_leaf(tree)) {
    std::cout << 'L' << tree->letter;
  }
  else {
    // Outputs I when not a leaf node
    std::cout << 'I';

    // recursively calls function, first checking left then right
    write_tree(ostr, tree->left);
    write_tree(ostr, tree->right);
  }
}

//creates a path string
void get_codes(hnode *tree, std::string *codes, std::string path) {
  // Base case
  if (tree == nullptr) return;

  //if node is a leaf, add letter to path
  if (is_leaf(tree)) {
    codes[tree->letter] = path;
    return;
  }
  else {
    //recursivelty calls get_codes with adding 0 when going down left
    //subtree and 1 when going down right subtree
    get_codes(tree->left, codes, path + '0');
    get_codes(tree->right, codes, path + '1');
  }
}

// Changing bit string into a byte
unsigned char bit_string_to_byte(const std::string& bit_string) {
  // Intializing variables
  unsigned char byte = 0;
  size_t bit = 128;

  // For loop that converts the bit_string into a byte by adding the
  // corresponding byte value of the current bit being checked.
  for (size_t i = 0; i < bit_string.length(); i++) {
    if (bit_string[i] == '1') {
      byte += bit;
    }
    bit /= 2;
  }
  return byte;
}

// Converts a byte into a bit_string
std::string byte_to_bit_string(unsigned char byte) {
  // Initializaing variables
  size_t bit = 128;
  std::string bitstring = "";

  // For loop that converts byte into bit string by checking if the given
  // byte is larger than the current bit value and adding a one and subtracting
  //if it is and adding a zero otherwise.
  for (size_t i = 0; i < 8; i++) {
    if (byte >= bit) {
      bitstring += '1';
      byte -= bit;
    }
    else {
      bitstring += '0';
    }
    bit /= 2;
  }
  return bitstring;
}

// Reads the tree by adding new nodes and using cin.get()
hnode *read_tree() {
  // Initializing variables
  char character = std::cin.get();

  // If statement that checks if the node is a leaf node, then creates a new
  // node and return it
  if (character == 'L') {
    hnode* new_node = new hnode(std::cin.get(), 0);
    return new_node;
  }

  // Else statement that creates a new node with a space letter value and
  // returns that
  else {
    hnode* other_node = new hnode(' ', 0, read_tree(), read_tree());
    return other_node;
  }
}

// Checks if node is a leaf
bool is_leaf(hnode * node) {
  // Checks if left and right subtree node pointers are nullptr to check
  // if the given node is a leaf.
  if (node->left == nullptr and node->right == nullptr) return true;
  return false;
}

void get_num_bytes_and_counts(std::ifstream &fin, size_t &num_bytes,
                              size_t *counts)
{
  num_bytes = 0;  // number of bytes in original file
  while (fin) {
    int v = fin.get();
    if (fin) {
      counts[v]++;
      num_bytes++;
    }
  }
}


void write_content(char *filename, std::string* codes)
{
  // read the file a second time, writing encoded bits as you go
  std::ifstream fin2(filename);
  unsigned char ch;
  std::string buffer = "";
  while ((ch = fin2.get()), fin2) {
    buffer += codes[ch];
    //std::cerr << codes[ch];
    while (buffer.size() >= 8) {
      std::string bit_string = buffer.substr(0,8);
      buffer.erase(0,8);
      std::cout << bit_string_to_byte(bit_string);
    }
  }
  // pad the buffer to a length that is a multiple of 8
  while (buffer.size() % 8 != 0) buffer += "0";
  while (buffer.size() > 0) {
    std::string bit_string = buffer.substr(0,8);
    buffer.erase(0,8);
    std::cout << bit_string_to_byte(bit_string);
  }
}

int compress(int argc, char **argv)
{
  if (argc != 2)
    usage(argv[0] + std::string(" filename"));
  size_t counts[256] = {};
  std::ifstream fin(argv[1]);
  if (!fin) {
    std::cout << "Can't open file " << argv[1] << std::endl;
    exit(1);
  }

  size_t num_bytes;
  get_num_bytes_and_counts(fin, num_bytes, counts);

  heap<hnode*> pq(hnode_compare);
  for (size_t i = 0; i < 256; i++) {
    if (counts[i] > 0) {
      hnode *p = new hnode(static_cast<unsigned char>(i), counts[i]);
      pq.add(p);
    }
  }
  // Build the tree!
  while (pq.size() > 1) {
    hnode *lft = pq.remove();
    hnode *rt = pq.remove();
    pq.add(new hnode(lft->letter, lft->count + rt->count, lft, rt));
  }
  hnode *tree = pq.remove();

  // Now that we have the tree, determine the codes for each letter
  std::string codes[256];
  get_codes(tree, codes, "");  // Third parameter is path.
  // Write the number of bytes, and the tree,
  // (and content)...
  std::cout << num_bytes;
  write_tree(std::cout, tree);
  write_content(argv[1], codes);
  return 0;
}


hnode *read_bytes_to_leaf(hnode *tree, std::string &buffer)
{
  while (not is_leaf(tree)) {
    if (buffer == "") {
      unsigned char byte = std::cin.get();
      std::string bits = byte_to_bit_string(byte);
      buffer += bits;
    }  // maybe make that shorter??
    char dir = buffer[0];
    //buffer = buffer.substr(1);  // choice 1
    buffer.erase(0, 1); // choice 2
    if (dir == '0')
      tree = tree->left;
    else
      tree = tree->right;
    // want to use ?: operator here? sure.
  }
  return tree;
}

int uncompress(int argc, char **argv)
{
  if (argc > 1) usage(argv[0]);
  size_t num_bytes;
  hnode *tree;
  std::cin >> num_bytes;
  tree = read_tree();
  std::string buffer = "";
  for (size_t i = 0; i < num_bytes; i++) {
    hnode *leaf = read_bytes_to_leaf(tree, buffer);
    std::cout << leaf->letter;
  }
  return 0;
}

int main(int argc, char** argv)
{
  // Dispatch to the correct function based on the name
  // of the program
  std::string program = argv[0];
  if (program.size() >= 10 and
      program.substr(program.size()-10) == "uncompress")
    return uncompress(argc, argv);
  else
    return compress(argc, argv);
}
