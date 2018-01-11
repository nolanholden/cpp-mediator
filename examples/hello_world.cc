// The code below demonstrates simple usage of the mediator library.

#include "../mediator.hpp"

#include <iostream>

// Let's shorten some names for convenience.
using mediator = holden::mediator::mediator;
template <typename T, typename U>
using request = holden::mediator::request<T, U>;
template <typename T>
using request_handler = holden::mediator::request_handler<T>;


// We must forward declare our WeatherSensor because requests require their 
// handler as a template argument to permit template deduction when sending 
// requests through the mediator (due to fundamental limitations of C++).
class Speaker;


// Define a request to be called from anywhere.
class SayHello : public request<void, Speaker> { // void because we dont need 
 public:                                         // a response from the Speaker.
  SayHello(std::ostream& out) : output_stream(out) {}

  // the output stream with which we wish to say hello
  std::ostream& output_stream;
};


// Configure the request handler.
class Speaker : public request_handler<SayHello> {
 public:
  // Say hello on the provided output stream.
  void handle(const SayHello& r) {
    r.output_stream << "Hello, world!\n";
  }
};


int main() {
  mediator mediator{};

  auto speaker = std::make_shared<Speaker>();
  mediator.register_handler(speaker);

  SayHello request{ std::cout };
  mediator.send(request);

  return 0;
}

/* output:
 *
 * $ Hello, world!
*/