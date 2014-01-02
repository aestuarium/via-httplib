//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Ken Barker
// (ken dot barker at via-technology dot co dot uk)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////
/// @file simple_http_client.cpp
/// @brief An example HTTP client.
//////////////////////////////////////////////////////////////////////////////
#include "via/comms/tcp_adaptor.hpp"
#include "via/http_client.hpp"
#include <iostream>

/// Define an HTTP client using std::string to store message bodies
typedef via::http_client<via::comms::tcp_adaptor, std::string> http_client_type;

namespace
{
  /// The handler for incoming HTTP requests.
  /// Prints the response.
  void response_handler(via::http::rx_response const& response,
                        std::string const& body)
  {
    std::cout << response.to_string()
              << response.headers().to_string()
              << body << std::endl;
  }
}

int main(int argc, char *argv[])
{
  // Get a hostname and uri from the user (assume default http port)
  if (argc <= 2)
  {
    std::cout << "Usage: simple_http_client [host] [uri]\n"
              << "E.g. simple_http_client www.boost.org /LICENSE_1_0.txt"
              << std::endl;
    return 1;
  }

  std::string host_name(argv[1]);
  std::string uri(argv[2]);
  std::cout << "HTTP client host: " << host_name
            << " uri: " << uri << std::endl;
  try
  {
    // The asio io_service.
    boost::asio::io_service io_service;

    // Create an http_client, attach the response handler
    // and attempt to connect to the host on the standard http port (80)
    http_client_type::shared_pointer http_client
        (http_client_type::create(io_service));
    http_client->response_received_event(response_handler);
    if (http_client->connect(host_name))
    {
      // Create an http request and send it to the host.
      via::http::tx_request request(via::http::request_method::GET, uri);
      http_client->send(request);

      // run the io_service to start communications
      io_service.run();
    }
    else
    {
      std::cout << "Error, could not resolve host: " << host_name << std::endl;
      return 1;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception:"  << e.what() << std::endl;
  }

  return 0;
}
//////////////////////////////////////////////////////////////////////////////
