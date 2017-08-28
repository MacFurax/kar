#include <iostream>

#include <oscpack\osc\OscReceivedElements.h>
#include <oscpack\osc\OscOutboundPacketStream.h>

#define OUTPUT_BUFFER_SIZE 1024

int main(int argc, char * argv[]) {

  char buffer[OUTPUT_BUFFER_SIZE];
  osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

  p << osc::BeginMessage("/test1")
    << true << 23 << (float)3.1415 << "hello" << osc::EndMessage;

  osc::ReceivedPacket rp{ p.Data(), p.Size() };
  osc::ReceivedMessage rm{ rp };

  std::cout << "Adresse : " << rm.AddressPattern() << "\n";
  osc::ReceivedMessageArgumentStream args = rm.ArgumentStream();
  bool a1;
  osc::int32 a2;
  float a3;
  const char *a4;
  args >> a1 >> a2 >> a3 >> a4 >> osc::EndMessage;

  std::cout << "received '/test1' message with arguments: "
    << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";

}