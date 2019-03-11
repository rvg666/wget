#include <iostream>
#include "network.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Url command line parameter is required\n";
    return 1;
  }

  /*argv[1] =
      "https://dl.bintray.com/boostorg/release/1.69.0/source/"
      "boost_1_69_0.tar.gz"; //*/

  argv[1] = "dl.bintray.com";
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  bzero(&hints, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  // hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0; /* Any protocol */
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  std::cout << "Starting download from: " << argv[1] << std::endl;
  auto status = getaddrinfo(argv[1], nullptr, /*nullptr*/ &hints, &result);
  if (status != 0) {
    std::cerr << "Error: " << hstrerror(h_errno) << std::endl;
    return 1;
  }

  auto it = result;
  while (it != nullptr) {
    std::cout << "Canonname: " << it->ai_protocol << std::endl;
    it = it->ai_next;
  }

  freeaddrinfo(result);  // free the linked list

  /*int sock_fd;
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
  }*/

  return 0;
}
