/**
 * Flare Director
 * ASM
 * ThreadContext.cpp
 * 
 * This implementation file contains the implementation
 * details for the ThreadTask class.
 */

#include "ThreadContext.hpp"

/**
 * ThreadContext
 ** Instantiates new ThreadContext with arguments
 * @param {cs} The new client socket instance
 * @param {l} The instance of the AltitudeProvider interface.
 * 
 */
ThreadContext::ThreadContext(TCPSocket *cs, AltitudeProvider *ap, ASMToggles *t) {
    this->clientSocket = cs;
    this->altitudeProvider = ap;
    this->toggles = t;
}