/*
 * pisca.c
 *
 *  Created on: 14 de mar de 2017
 *      Author: tamandua32
 */



#include "pisca.h"
#include "bsp.h"
#include <stdio.h>


/* Active object class -----------------------------------------------------*/
#define ON_TIME    \
    (QTimeEvtCtr)(1 * BSP_TICKS_PER_SEC)

enum InternalSignals {           /* internal signals */
    TIMEOUT_SIG = MAX_SIG
};
typedef struct PiscaTag {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timeEvt;
} Pisca;

/* protected: */
static QState Pisca_initial(Pisca * const me, QEvt const * const e);
static QState Pisca_on(Pisca * const me, QEvt const * const e);
static QState Pisca_off(Pisca * const me, QEvt const * const e);

/* Local objects -----------------------------------------------------------*/
static Pisca l_pisca; /* the single instance of the Table active object */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_Pisca = &l_pisca.super; /* "opaque" AO pointer */

/*..........................................................................*/

void Pisca_ctor(void) {
    Pisca *me = &l_pisca;
    QActive_ctor(&me->super, Q_STATE_CAST(&Pisca_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
}

static QState Pisca_initial(Pisca * const me, QEvt const * const e) {

    (void)e; /* suppress the compiler warning about unused parameter */

    QActive_subscribe(&me->super, PORTA1_SIG);
    QActive_subscribe(&me->super, PORTA2_SIG);
    QActive_subscribe(&me->super, PORTA3_SIG);

    QActive_subscribe(&me->super, SOBE1_SIG);
    QActive_subscribe(&me->super, SOBE2_SIG);
    QActive_subscribe(&me->super, SOBE3_SIG);

    QActive_subscribe(&me->super, DESCE1_SIG);
    QActive_subscribe(&me->super, DESCE2_SIG);
    QActive_subscribe(&me->super, DESCE3_SIG);

    QActive_subscribe(&me->super, CABINE1_SIG);
    QActive_subscribe(&me->super, CABINE2_SIG);
    QActive_subscribe(&me->super, CABINE3_SIG);

    return Q_TRAN(&Pisca_on);
}

static QState Pisca_off(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
    	case Q_ENTRY_SIG: {
//    		bsp_sobeon(1);

//            sleep(10);
//            bsp_sobeoff(1);
    		status = Q_TRAN(&Pisca_on);
    		break;
    	}
        case B2_SIG: {
            status = Q_TRAN(&Pisca_on);
            break;
        }
        case TIMEOUT_SIG:{
        	status = Q_HANDLED();
        	break;
        }
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

static QState Pisca_on(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
    case Q_ENTRY_SIG: {
//                bsp_visor(1);
                // sleep(2);
//                bsp_visor(2);
                // bsp_acionacarro(3);
//                sleep(5);
//                bsp_visor(3);
                // bsp_acionacarro(2);
//                sleep(5);
//                bsp_visor(1);
//                sleep(5);
                //QTimeEvt_postIn(&me->timeEvt, &me->super, ON_TIME);
                // QTimeEvt_armX	(&me->timeEvt , BSP_TICKS_PER_SEC, 0);

                status = Q_HANDLED();
                break;
            }
    case PORTA1_SIG: {
            printf("chegou evento porta1\n");
            fflush(stdout);
            bsp_acionaporta(1, -1);
            status = Q_HANDLED();
            break;
        }
    case PORTA2_SIG: {
            printf("chegou evento porta2\n");
            fflush(stdout);
            bsp_acionaporta(2, -1);
            status = Q_HANDLED();
            break;
        }
    case PORTA3_SIG: {
            printf("chegou evento porta3\n");
            fflush(stdout);
            bsp_acionaporta(3, -1);
            status = Q_HANDLED();
            break;
        }
    case SOBE1_SIG: {
    	printf("alo");
    	fflush(stdout);
    	bsp_sobeon(1);
        status = Q_HANDLED();
        break;
    }
    case SOBE2_SIG: {
		bsp_sobeon(2);
		status = Q_HANDLED();
		break;
	}
    case SOBE3_SIG: {
		bsp_sobeon(3);
		status = Q_HANDLED();
		break;
	}
    case TIMEOUT_SIG:{
    	status = Q_HANDLED();
    	break;
    }
    default: {
    		status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

