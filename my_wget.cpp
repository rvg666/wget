#include <fstream>
#include <iostream>
#include <string_view>

#include "network.h"
enum class Status { OK = 200 };

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Url command line parameter is required\n";
    return 1;
  }

  // https://cdn2.hubspot.net/hub/416323/file-2337009921-png/HomeImages/V2/logo_uipath_blue.png?t=1453382687840
  // https://browser.yandex.ru/download?from=wizard__yabro_one_&banerid=0500000000&os=win
  /*argv[1] =
      "https://dl.bintray.com/boostorg/release/1.69.0/source/"
      "boost_1_69_0.tar.gz"; //*/

  argv[1] = "cdn2.hubspot.net";
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  bzero(&hints, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0; /* Any protocol */
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  std::cout << "Resolver host address\n";
  auto status = getaddrinfo(argv[1], "80", /*nullptr*/ &hints, &result);
  if (status != 0) {
    std::cerr << "Error: " << hstrerror(h_errno) << std::endl;
    return 1;
  }

  auto it = result;
  int sock_fd = 0;
  std::cout << "Connecting to host\n";
  while (it != nullptr) {
    std::cout << "Attempt to connect" << std::endl;
    sock_fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
    if (sock_fd < 0) continue;

    if (connect(sock_fd, (struct sockaddr *)it->ai_addr, it->ai_addrlen) == 0) {
      std::cout << "Successfully connected" << std::endl;
      break;  // Successfully connected
    }

    std::cout << "Attempt failed" << std::endl;
    close(sock_fd);
    it = it->ai_next;
  }

  freeaddrinfo(result);  // free the linked list
  if (it == nullptr) {
    std::cerr << "Cannot connect to the server\n";
    return 1;
  }

  std::string request =
      "GET "
      "/hub/416323/file-2337009921-png/HomeImages/V2/"
      "logo_uipath_blue.png?t=1453382687840 HTTP/1.1\r\n"
      "Host: cdn2.hubspot.net\r\n"
      "User-Agent: my_wget\r\n"
      "Accept: */*\r\n"
      "Connection: close\r\n"
      "\r\n";

  write(sock_fd, request.data(), request.size() * sizeof(char));

  std::cout << "Starting download from: " << argv[1] << std::endl;
  ssize_t n;
  int content = 0;

  // read header
  char recv_line[MAXLINE + 1];
  int line_num = 0;
  while ((n = read(sock_fd, recv_line, MAXLINE)) > 0) {
    recv_line[n] = 0;
    std::string_view view(recv_line);
    size_t pos = 0;
    size_t end_line = 0;

    while (end_line != std::string_view::npos) {
      end_line = view.find_first_of("\r\n", pos);
      if (end_line != std::string_view::npos) {
        auto line = view.substr(pos, end_line - pos);
        std::cout << line;
        ++line_num;
        if (line_num == 1 && line.find("200") == std::string_view::npos)
          return 1;

        auto del_pos = line.find(":");
        if (content == 0 &&
            line.find("Content-Length:") != std::string_view::npos)
          content = std::stoi(std::string(line.substr(del_pos + 1)));

        pos = end_line + 2;
      }
    }
  }

  // read content;
  std::ofstream out("test.png", std::ios::binary);
  recv_line[n] = 0;
  out.write(recv_line, n);

  std::cout << std::endl;
  if (n < 0) {
    std::cerr << "Read error" << std::strerror(errno) << std::endl;
  }

  close(sock_fd);
  return 0;
}
