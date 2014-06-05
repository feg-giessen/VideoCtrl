/*
 * RemoteAdapter.h
 *
 *  Created on: 06.06.2014
 *      Author: Peter Schuster
 */

#ifndef REMOTEADAPTER_H_
#define REMOTEADAPTER_H_

#include "net/TcpSerialAdapter2.h"

class RemoteAdapter {
protected:
    TcpSerialAdapter2 _client;
public:
    bool isRemoteAvailable();
};

#endif /* REMOTEADAPTER_H_ */
