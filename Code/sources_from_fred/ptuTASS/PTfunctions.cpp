#include "PTfunctions.h"

#include <iostream>
#include<iomanip>

#include <string.h>

/*!
\brief Send and don't wait for an answer.
\param[in] fd Pointer to the device.
\param[in] command Command to send.  The command must be formatted.
\return true if the command was sent successfully.
*/
bool send(int fd, const char* command)
{
   bool success = true;
   for (unsigned int i = 0; i < strlen(command); i++)
   {
      uart_tx(fd, command[i]);
   }
   int response = uart_rx(fd);
   if (response != 6)
   {
      success = false;
      std::cerr << "Didn't get a ACK\n";
   }

   return(success);
}

/*!
\brief Send and don't wait for an answer.
\param[in] fd Pointer to the device.
\param[in] command Command to send.  The command must be formatted.
\return - 0 if the command was sent successfully,
- -1 if a NAK was received,
- -2 if the command was not understood ('?').
*/
int send(int fd, const std::vector<unsigned char>& command)
{
   int success = 0;
   for (unsigned int i = 0; i < command.size(); i++)
   {
      uart_tx(fd, command[i]);
   }
   int response = uart_rx(fd);
   if ((response != 6) && (response == 21))
   {
      success = -1;
      std::cerr << "Got a NAK.\n";
   }
   else
   {
      //response = uart_rx(fd);
      if (response == '?')
      {
         success = -2;
         std::cerr << "The command was not understood by the PTU.\n";
      }
   }
   return(success);
}

/*!
\brief Send and wait for an answer.
\param[in] fd Pointer to the device.
\param[in] command Command to send.  The command must be formatted.
\param[out] result The result returned by the PTU.
\return - 0 if the command was sent successfully,
- -1 if a NAK was received,
- -2 if the command was not understood ('?'),
- -3 if the checksum of the data returned by the PTU was wrong.
*/
int send(int fd,
         const std::vector<unsigned char>& command,
         std::vector<unsigned char>& result)
{
   int success = send(fd, command);

   if (success == 0)
   {
      result.clear();

      // F8
      int response = uart_rx(fd);
      result.push_back(response);
      // Address 1
      response = uart_rx(fd);
      result.push_back(response);
      // Address 2
      response = uart_rx(fd);
      result.push_back(response);
      // Address 3
      response = uart_rx(fd);
      result.push_back(response);
      // Address 4
      response = uart_rx(fd);
      result.push_back(response);
      // Length of the data sent back
      int lengthData = uart_rx(fd);
      result.push_back(lengthData);
      // Data
      for (int ind = 0; ind < lengthData; ++ind)
      {
         response = uart_rx(fd);
         result.push_back(response);
      }
      // Checksum received from the PTU
      int checksumData = uart_rx(fd);
      result.push_back(checksumData);
      // Scan the address and data parts to compute the checksum of the data.
      int checksum = 0;
      for (unsigned short ind = 1; ind < result.size() - 1; ++ind)
      {
         checksum += static_cast<unsigned int>(result[ind]);
      }
      checksum = checksum & 0xFF;
      if (checksum != checksumData)
      {
         success = -3;
         std::cerr << "Checksum error.\n";
      }
   }

   return(success);
}


/*!
\brief Create a command to send to the PTU from the given TASS command.
\param[in] TASS The command to transform.
\return The command as a vector.
*/
std::vector<unsigned char> createCommand(const std::string& TASS)
{
   std::vector<unsigned char> command;

   unsigned int checksum = 0xFF + 0xFF + 0xFF +0xFF;

   command.push_back(0xF8);
   command.push_back(0xFF);
   command.push_back(0xFF);
   command.push_back(0xFF);
   command.push_back(0xFF);

   // Size of the command
   command.push_back(TASS.size());
   checksum += TASS.size();

   for (unsigned short ind = 0; ind < TASS.size(); ++ind)
   {
      command.push_back(TASS[ind]);
      checksum += static_cast<unsigned int>(TASS[ind]);
   }

   checksum = checksum & 0xFF;

   command.push_back(checksum);

   return(command);
}


/*!
\brief Display message to/from the PTU.
\param[in] message The message to display.
*/
void displayMessage(const std::vector<unsigned char>& message)
{
   std::cout << "(char):";
   for (unsigned int ind = 0; ind < message.size(); ++ind)
   {
      std::cout << " !" << message[ind] << "!";
   }
   std::cout << std::endl;
   std::cout << "(hex):";
   for (unsigned int ind = 0; ind < message.size(); ++ind)
   {
      std::cout << " " << std::hex << std::setfill ('0') << std::setw (2)
         << static_cast<int>(message[ind]);
   }
   std::cout << std::endl;
   std::cout << "(dec):";
   for (unsigned int ind = 0; ind < message.size(); ++ind)
   {
      std::cout << " " << std::dec << static_cast<int>(message[ind]);
   }
   std::cout << std::endl;
}
