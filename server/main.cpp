#include "headers/Server.hpp"
#include <string>

#define PORT "6667"
#define MAX_CLIENTS 20
#define BUF_SIZE 1024

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  std::string port = argv[1];
  Server server("testing", 20, port, "password");

  server.start();
  return 0;
}
