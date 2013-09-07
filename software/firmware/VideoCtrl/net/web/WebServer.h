/*
 * WebServer.h
 *
 *  Created on: 01.08.2013
 *      Author: Peter Schuster
 */

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include "ch.hpp"


#ifndef WEB_THREAD_STACK_SIZE
#define WEB_THREAD_STACK_SIZE   1024
#endif

#ifndef WEB_THREAD_PORT
#define WEB_THREAD_PORT         80
#endif

#ifndef WEB_THREAD_PRIORITY
#define WEB_THREAD_PRIORITY     (LOWPRIO + 2)
#endif

namespace chibios_rt {

class WebServer: public chibios_rt::BaseStaticThread<WEB_THREAD_STACK_SIZE> {
protected:
	void serve(struct netconn *conn);
	virtual msg_t main(void);
};

}

#endif /* WEBSERVER_H_ */
