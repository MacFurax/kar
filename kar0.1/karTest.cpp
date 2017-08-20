#include "kar.hpp"

#include <cassert>
#include<conio.h>

using namespace kar;

int main(int argc, char *argv[])
{

  // test message
  Message msg;

  assert(msg.target() == "");
  assert(msg.origine() == "");
  assert(msg.data().size() == 0 );


  Bus bus;
  Service srvc1("\\detection\\0");

  bus.registerService(srvc1);

  srvc1.sendMessage(Message{ "\\EE\\state", srvc1.name()});
  bus.giveMessageToService("\\detection\\0", Message{ "\\detection\\0", "\\SSMaster" });

  std::cout << "All tests passed\n";
  std::cout << "Press any key to continue\n";
  _getch();
  return 0;
}