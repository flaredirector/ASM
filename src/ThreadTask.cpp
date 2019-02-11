/**
 * Flare Director
 * ASM
 * ThreadTask.cpp
 * 
 * This implementation file contains the implementation
 * details for the ThreadTask class.
 */

#include "ThreadTask.h"

/**
 * ThreadTask
 ** Instantiates new ThreadTask with arguments
 * TODO: Move this to own class file.
 * @param {cs} The new client socket instance
 * @param {l} The instance of the LIDARInterface
 * 
 */
ThreadTask::ThreadTask(TCPSocket *cs, LIDARInterface *l) {
    this->lidar = l;
    this->clientSocket = cs;
}

ThreadTask::ThreadTask(TCPSocket *cs) {
    this->clientSocket = cs;
}