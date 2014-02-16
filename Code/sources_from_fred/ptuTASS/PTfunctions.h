#include "uart.h"

#include <vector>
#include <string>

/*!
\brief Send and don't wait for an answer.
\param[in] fd Pointer to the device.
\param[in] command Command to send.  The command must be formatted.
\return true if the command was sent successfully.
 */
bool send(int fd, const char* command);

/*!
\brief Send and don't wait for an answer.
\param[in] fd Pointer to the device.
\param[in] command Command to send.  The command must be formatted.
\return - 0 if the command was sent successfully,
- -1 if a NAK was received,
- -2 if the command was not understood ('?').
*/
int send(int fd, const std::vector<unsigned char>& command);

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
         std::vector<unsigned char>& result);

/*!
\brief Create a command to send to the PTU from the given TASS command.
\param[in] TASS The command to transform.
\return The command as a vector.
*/
std::vector<unsigned char> createCommand(const std::string& TASS);

/*!
\brief Display message to/from the PTU.
\param[in] message The message to display.
*/
void displayMessage(const std::vector<unsigned char>& message);
