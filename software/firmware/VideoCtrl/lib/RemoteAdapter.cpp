/*
 * RemoteAdapter.cpp
 *
 *  Created on: 06.06.2014
 *      Author: Peter Schuster
 */

#include "RemoteAdapter.h"

bool RemoteAdapter::isRemoteAvailable() {
    return !_client.isTimedout();
}
