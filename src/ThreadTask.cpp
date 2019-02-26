/**
 * Flare Director
 * ASM
 * ThreadTask.cpp
 * 
 * This implementation file contains the implementation
 * details for the ThreadTask class.
 */

#include "ThreadTask.hpp"

/**
 * ThreadTask
 ** Instantiates new ThreadTask with arguments
 * @param {cs} The new client socket instance
 * @param {l} The instance of the AltitudeProvider interface.
 * 
 */
ThreadTask::ThreadTask(TCPSocket *cs, AltitudeProvider *ap) {
    this->clientSocket = cs;
    this->altitudeProvider = ap;
}