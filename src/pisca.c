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
static QState Elevador_initial(Pisca * const me, QEvt const * const e);
static QState Abrindo_Porta(Pisca * const me, QEvt const * const e);
static QState Porta_aberta(Pisca * const me, QEvt const * const e);
static QState Fechando_Porta(Pisca * const me, QEvt const * const e);
static QState Verifica_Destino(Pisca * const me, QEvt const * const e);
static QState Elevador_Descendo(Pisca * const me, QEvt const * const e);
static QState Elevador_Subindo(Pisca * const me, QEvt const * const e);
static QState Embarque_Intermed(Pisca * const me, QEvt const * const e);

/* Local objects -----------------------------------------------------------*/
static Pisca l_pisca; /* the single instance of the Table active object */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_Pisca = &l_pisca.super; /* "opaque" AO pointer */

/*..........................................................................*/

void Pisca_ctor(void) {
    Pisca *me = &l_pisca;
    QActive_ctor(&me->super, Q_STATE_CAST(&Elevador_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
}

static QState Elevador_initial(Pisca * const me, QEvt const * const e) {

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

    return Q_TRAN(&Abrindo_Porta);
}

static QState Abrindo_Porta(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
    case Q_ENTRY_SIG: {
                // status = Q_TRAN(&Abrindo_Porta);
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
    case DESCE1_SIG: {
    	bsp_desceon(1);
    	status = Q_HANDLED();
    	break;
    }
    case DESCE2_SIG: {
        bsp_desceon(2);
        status = Q_HANDLED();
        break;
    }
    case DESCE3_SIG: {
        bsp_desceon(3);
        status = Q_HANDLED();
        break;
    }
    case CABINE1_SIG: {
    	bsp_cabineon(1);
        status = Q_HANDLED();
        break;
    }
    case CABINE2_SIG: {
    	bsp_cabineon(2);
        status = Q_HANDLED();
        break;
    }
    case CABINE3_SIG: {
    	bsp_cabineon(3);
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

static QState Porta_aberta(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
    	case Q_ENTRY_SIG: {
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

static QState Fechando_Porta(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
    		status = Q_TRAN(&Abrindo_Porta);
    		break;
    	}
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

static QState Verifica_destino(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
    		status = Q_TRAN(&Abrindo_Porta);
    		break;
    	}
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

static QState Elevador_Descendo(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
    		status = Q_TRAN(&Abrindo_Porta);
    		break;
    	}
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

static QState Elevador_Subindo(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
    		status = Q_TRAN(&Abrindo_Porta);
    		break;
    	}
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}

static QState Embarque_Intermed(Pisca * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
    		status = Q_TRAN(&Abrindo_Porta);
    		break;
    	}
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}