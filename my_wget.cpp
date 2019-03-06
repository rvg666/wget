#include <iostream>
#include "network.h"

using SA = sockaddr;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Url command line parameter is required\n";
    return 1;
  }

  struct addrinfo hints;
  struct addrinfo *result, *rp;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0; /* Any protocol */
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  int s = getaddrinfo(nullptr, argv[1], &hints, &result);
  if (s != 0) {
    std::cerr << std::strerror(errno) << std::endl;
    return 1;
  }

  int sock_fd;
  ssize_t n;
  char recv_line[MAXLINE + 1];
  struct sockaddr_in servaddr;

  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket error" << std::strerror(errno) << std::endl;
    return 1;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(13);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    std::cerr << "inet_pton error for " << argv[1] << std::endl;
  }

  if (connect(sock_fd, reinterpret_cast<SA *>(&servaddr), sizeof(servaddr)) <
      0) {
    std::cerr << std::strerror(errno) << std::endl;
    return 1;
  }

  while ((n = read(sock_fd, recv_line, MAXLINE)) > 0) {
    recv_line[n] = 0;
    std::cout << recv_line << std::endl;
  }

  if (n < 0) {
    std::cerr << "Read error" << std::strerror(errno) << std::endl;
  }

  return 0;
}
