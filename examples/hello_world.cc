// The code below demonstrates simple usage of the mediator library.

#include "../include/cpp_mediator/mediator.hpp"

#include <iostream>

// Define a request.
struct SayHello : holden::request<void> {
  // the output stream through which the handler should "say hello."
  std::ostream& output_stream;

  SayHello(std::ostream& out) : output_stream(out) {}
};

enum class FirstImpression { Good, Great, Stupendous, };

std::ostream& operator<<(std::ostream& os, FirstImpression i) {
  switch (i) {
   case FirstImpression::Good:       os << "good!";
   case FirstImpression::Great:      os << "great!!";
   case FirstImpression::Stupendous: os << "stupendous!!!";
   default: os << "off the charts!";
  }
  
  return os;
}

// Or we can skip the `request<>` interface and add `response_type` manually.
struct SayGoodbye {
  // Let mediator know what return type to infer.
  using response_type = FirstImpression;

  // the output stream through which the handler should "say hello."
  std::ostream& output_stream;

  SayGoodbye(std::ostream& out) : output_stream(out) {}
};

// Configure a request handler.
class Speaker
  : holden::request_handler<SayHello>
  , holden::request_handler<SayGoodbye> {
 public:
  // Say hello on the provided output stream.
  void handle(const SayHello& r) {
    r.output_stream << "Hello, world!\n";
  }

  // Say goodbye on the provided output stream, and return how the interaction 
  // went.
  FirstImpression handle(const SayGoodbye& r) {
    r.output_stream << "Goodbye for now!\n";
    return FirstImpression::Stupendous;
  }
};



int main() {
  Speaker s{};
  auto mediator = holden::make_mediator(s);

  SayHello say_hello{ std::cout };
  mediator.send(say_hello);

  auto first_impression = mediator.send(SayGoodbye{std::cout});

  std::cout << "The speaker's first impression was " << first_impression << "\n";

  return 0;
}

/* output:
 *
 * $ Hello, world!
 * $ Goodbye for now!
 * $ The speaker's first impression was studpendous!!!
*/
