#include <scrimmage/viewer/Viewer.h>
#include <scrimmage/network/Interface.h>

namespace sc = scrimmage;

int main(int, char* [])
{
    sc::InterfacePtr incoming_interface = std::make_shared<sc::Interface>();
    sc::InterfacePtr outgoing_interface = std::make_shared<sc::Interface>();
    
    sc::Viewer viewer;
    viewer.set_enable_network(true);
    viewer.set_incoming_interface(incoming_interface);
    viewer.set_outgoing_interface(outgoing_interface);
    viewer.init();
    viewer.run();
 
    return 0;
}
