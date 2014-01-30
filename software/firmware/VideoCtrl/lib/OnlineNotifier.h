/*
 * OnlineNotifier.h
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 */

#ifndef ONLINENOTIFIER_H_
#define ONLINENOTIFIER_H_

class OnlineNotifier {
public:
    virtual void notify(bool online) = 0;
};

#endif /* ONLINENOTIFIER_H_ */
