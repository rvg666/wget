#include <fstream>
#include <iostream>
#include <string_view>
#include <sstream>

#include "network.h"
enum class Status { OK = 200 };

int main(int argc, char *argv[]) {
 // if (argc != 2) {
    std::cerr << "Url commandline parameter is required\n";
   // return 1;
  //}

  // https://cdn2.hubspot.net/hub/416323/file-2337009921-png/HomeImages/V2/logo_uipath_blue.png?t=1453382687840
  // https://browser.yandex.ru/download?from=wizard__yabro_one_&banerid=0500000000&os=win
  /*argv[1] =
      "https://dl.bintray.com/boostorg/release/1.69.0/source/"
      "boost_1_69_0.tar.gz"; //*/

    argv[1] = "https://cdn2.hubspot.net/hub/416323/file-2337009921-png/HomeImages/V2/logo_uipath_blue.png?t=1453382687840";
  
  std::string url(argv[1]);
  std::transform(url.begin(), url.end(), url.begin(), ::tolower);
  std::string_view url_view(url);
  size_t pos = 0;
  if((pos=url_view.find("://")) != -1) {
      url_view.remove_prefix(pos + 3);
  }
  
  pos = url_view.find('/');
  std::string host(url_view.substr(0, pos));
  std::string_view path =url_view.substr(pos, -1);
  pos = url_view.rfind('/');
  size_t param_pos = url_view.find('?', pos);
  std::string  filename(url_view.substr(pos + 1, param_pos-pos -1));
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
  auto status = getaddrinfo(host.c_str(), "80",  &hints, &result);
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

    std::stringstream ss;
    ss <<
      "GET "
       << path << " HTTP/1.1\r\n"
      "Host: " << host << "\r\n"
      "User-Agent: my_wget\r\n"
      "Accept: */*\r\n"
      "Connection: close\r\n"
      "\r\n";
    auto request = ss.str();
  write(sock_fd, request.c_str(), request.size() * sizeof(char));

  std::cout << "Starting download from: " << argv[1] << std::endl;
  ssize_t n;

  // read header
  std::string header;
  header.reserve(MAXLINE);
    
  char ch;
  while ((n = read(sock_fd, &ch, 1)) > 0) {
    header.push_back(ch);
      if (header.size() >= 4 && std::string_view(header.c_str() + header.size() - 4) =="\r\n\r\n")
          break;
  }
    
  if (n < 0) {
    std::cerr << "Read error" << std::strerror(errno) << std::endl;
    return 1;
  }
    
    if (header.find("200 OK") == std::string::npos){ // TODO redirect
        std::cerr << "The server doesnot return HTTP OK response\n";
        return 1;
    }
    
  // read content
  char recv_line[MAXLINE + 1];
  std::ofstream out(filename, std::ios::binary);
  while ((n = read(sock_fd, recv_line, MAXLINE)) > 0) {
      recv_line[n] = '\0';
      out.write(recv_line, n);
  }
  if (n < 0) {
    std::cerr << "Read error" << std::strerror(errno) << std::endl;
  }

  close(sock_fd);
  return 0;
}
