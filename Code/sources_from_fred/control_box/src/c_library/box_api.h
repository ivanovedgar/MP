/*!
 * \file box_api.h
 * \brief API of the library to talk to the "AberBox".
 *
 * The box controls relays used to switch on and off devices, has access to an
 * inclinometer and (will) control a stabilised pan and tilt platform
 * (including compensation of its drift).
 *
 * \author Dave Price <dap@aber.ac.uk>
 */

/*!
 * \mainpage
 * This is the documentation of the public API to talk to the "AberBox".  The
 * box controls relays used to switch on and off devices, has access to an
 * inclinometer and (will) control a stabilised pan and tilt platform
 * (including compensation of its drift).
 *
 * Code that uses this API needs to be linked against the library box_api
 * (-lbox_api).
 */

/*!
 * \brief Establish connection to the control box.
 *
 * This must be done before anything else.
 *
 * \param[in] hostname Name of the computer in the box (can be an IP address).
 * \param[in] portnumber Number of the port the box is listening to.
 * \return 0 in case of failure, 1 otherwise.
 */
int box_connect(char * hostname, int portnumber);

/*!
 * \brief Disconnect from the control box.
 */
void box_disconnect();

/*!
 * \brief Shut down the control box.
 *
 * This also disconnects from the box.
 */
void box_shutdown();

/*!
 * \brief Set the pan and tilt values of the PT unit controlled by the box.
 *
 * This is not yet implemented.
 *
 * \param[in] pan The pan between 0 and 360, with 0 looking backwards over the
 * left of the rover.
 * \param[in] tilt The tilt between 0 and 180, with 0 looking down.
 * \param[out] retVal An array of two floats (must be allocated elsewhere):
 * the first element is the pan value, the second element is the
 * tilt value.
 */
void setPanTilt(float pan, float tilt, float * retVal);

/*!
 * \brief Get the attitude of the inclinometer plugged into the box.
 * \param[out] retVal An array of two floats (must be allocated elsewhere):
 * the first element is the side-to-side value, the second element is the
 * fore-aft value.
 */
void getInclinometer(float * retVal);

/*!
 * \brief Set all the relays of the box.
 * \param[in] value The format of the parameter is one bit per relay,
 * 1 meaning on and 0 meaning off.  The least significant bit corresponds to
 * relay 0, up to 32 relays.
 * \return 0 in case of failure, 1 otherwise.
 */
int setRelays(int value);

/*!
 * \brief Set a given relay to the on state.
 * \param[in] relay The relay number (starting from 0).
 * \return 0 in case of failure, 1 otherwise.
 */
int setRelayN(int relay);

/*!
 * \brief Set a given relay to the off state.
 * \param[in] relay The relay number (starting from 0).
 * \return 0 in case of failure, 1 otherwise.
 */
int resetRelayN(int relay);

/*!
 * \brief Get the state of all the relays.
 * \return The state of all the relays.  The format of the returned value is
 * one bit per relay, 1 meaning on and 0 meaning off.  The least significant
 * bit corresponds to relay 0, up to 32 relays.
 */
int getRelays();

/*!
 * \brief Get the state of a given relay.
 * \param[in] relay The relay number (starting from 0).
 * \return The state of the relay, 1 for on, 0 for off.
 */
int getRelayN(int relay);
