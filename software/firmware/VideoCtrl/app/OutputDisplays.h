/*
 * OutputDisplays.h
 *
 *  Created on: 02.04.2014
 *      Author: Peter Schuster
 */

#ifndef OUTPUTDISPLAYS_H_
#define OUTPUTDISPLAYS_H_

#include "../lib/SkaarhojBI8.h"
#include "../lib/ProjectorCtrl.h"
#include "../lib/LgTv.h"

class OutputDisplays {
private:
    ProjectorCtrl _projectorLi;
    ProjectorCtrl _projectorRe;
    LgTv _tvKlSaalLi;
    LgTv _tvKlSaalRe;
    LgTv _tvStageDisplay;

    bool _projetorLi_vmute;
    bool _projetorRe_vmute;

    SkaarhojBI8* _bi8;

    uint8_t _blink;
    uint8_t _run;

    uint8_t _led_color[8];
public:
    OutputDisplays();
    void begin(
            ip_addr_t addr_proj_li, uint16_t port_proj_li,
            ip_addr_t addr_proj_re, uint16_t port_proj_re,
            ip_addr_t addr_klSaal_li, uint16_t port_klSaal_li,
            ip_addr_t addr_klSaal_re, uint16_t port_klSaal_re,
            ip_addr_t addr_stage, uint16_t port_stage,
            SkaarhojBI8* bi8);

    void run();
    void doBlink();

private:
    void _processProjectorLeds(ProjectorCtrl* proj, uint8_t button);
};

#endif /* OUTPUTDISPLAYS_H_ */
