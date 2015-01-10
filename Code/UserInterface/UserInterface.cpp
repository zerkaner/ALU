#include <UserInterface/UserInterface.h>


UserInterface::UserInterface() : 
  _writer(TextWriter("..\\Output\\cali10.glf")) { 
}


void UserInterface::Update() {
  _writer.WriteText("This is the first line.", 10, 20);  
  _writer.WriteText("2nd column.", 10, 35);  
  _writer.WriteText("Hello World (3rd line).", 10, 50);
}