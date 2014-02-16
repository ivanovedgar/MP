#include <unistd.h>

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "uart.h"

std::string port = "/dev/ttyS2";

bool somethingdone = false;

/* Commands to be sent to the P&T unit. */
/* They must finish by a 0, which is not sent to the unit. */
const char home[] = {248, 255, 255, 255, 255, 2, 72, 79, 149, 0};
const char verifyhome[] = {248, 255, 255, 255, 255, 2, 72, 86, 156, 0};
const char stabilise[] = {248, 255, 255, 255, 255, 2, 72, 73, 143, 0};
const char goto_180[] = {248, 255, 255, 255, 255, 9, 80, 52, 54, 53,
      48, 48, 48, 48, 48, 228, 0};

void send(int fd, const char* command)
{
//    std::cout << "Sending command...";
//    std::cout.flush();
   for (int i = 0; i < strlen(command); i++)
   {
      uart_tx(fd, command[i]);
   }
   int response = uart_rx(fd);
   if (response != 6)
   {
      std::cerr << "Didn't get a ACK\n";
   }
//    std::cout << " done.\n";
//    std::cout << "response: " << uart_rx(fd) << std::cout;
}

int main(int argc, char** argv)
{
   // Declare the supported options.
   po::options_description optionsDesc("navigate options");

   std::ostringstream description;
   description
      << "The program will send commands to the SageBrush PTU.\n"
      << "Commands are sent in the order they are given.";

   optionsDesc.add_options()
      ("help", "display help message")
      ("port,p",
       po::value<std::string>(&port)->default_value("/dev/ttyS2"),
       "port the PTU is plugged in")
      ("verifyhome,v", "sends 'Verify Home' (to be done before anything else)")
      ("home,h", "sends 'Home'")
      ("stabilise,s", "Sends 'Gyrostabilise'")
      ;

   po::variables_map vm;
   po::store(po::parse_command_line(argc, argv, optionsDesc), vm);
   po::notify(vm);

   if (vm.count("help"))
   {
      std::cout << description.str() << std::endl
         << std::endl
         << optionsDesc << std::endl;
      exit(1);
   }

   int fd = uart_init(9600, port.c_str());

   if (vm.count("verifyhome"))
   {
      somethingdone = true;
      std::cout << "Sending 'Verify Home'.\n";
      send(fd, verifyhome);
      sleep(10); // Wait for the command to finish.
   }

   if (vm.count("home"))
   {
      somethingdone = true;
      std::cout << "Sending 'Home'.\n";
      send(fd, home);
      sleep(3); // wait for the command to finish.
   }

   if (vm.count("stabilise"))
   {
      somethingdone = true;
      std::cout << "Sending 'Gyrostabilise'.\n";
      send(fd, stabilise);
   }

   uart_deinit(fd);

   if (!somethingdone)
   {
      std::cout << "You did not ask me to do anything!\n";
   }

   exit(0);
}
