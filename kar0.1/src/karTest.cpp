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


  Node node(R"(\detection\0)");
  Service srvc1(R"(\detection\ssslave\0)");
	Service srvc2(R"(\detection\ssslave\0)");
	Service srvc3(R"(\detection\ssslave\1)");

  node.registerService(srvc1);
	node.registerService(srvc2);
	node.registerService(srvc3);

  srvc1.sendMessage(Message{ R"(\ee\state)", srvc1.name()});
  node.giveMessageToService(R"(\detection\ssslave\0)", Message{ R"(\detection\ssslave\0)", R"(\ssmaster)" });
	node.giveMessageToService(R"(\detection\ssslave\1)", Message{ R"(\detection\ssslave\1)", R"(\ssmaster)" });

  std::cout << "All tests passed\n";
  std::cout << "Press any key to continue\n";
  _getch();

  return 0;
}