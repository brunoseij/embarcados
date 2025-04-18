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
#define DOOR_TIMEOUT \
	(QTimeEvtCtr)(5 * BSP_TICKS_PER_SEC)

enum InternalSignals
{ /* internal signals */
  TIMEOUT_SIG = MAX_SIG
};
typedef struct PiscaTag
{
	/* protected: */
	QActive super;

	/* private: */
	QTimeEvt timeEvt;
} Pisca;

/* protected: */
static QState Elevador_initial(Pisca *const me, QEvt const *const e);
static QState Abrindo_Porta(Pisca *const me, QEvt const *const e);
static QState Porta_aberta(Pisca *const me, QEvt const *const e);
static QState Fechando_Porta(Pisca *const me, QEvt const *const e);
static QState Verifica_Destino(Pisca *const me, QEvt const *const e);
static QState Elevador_Descendo(Pisca *const me, QEvt const *const e);
static QState Elevador_Subindo(Pisca *const me, QEvt const *const e);
static QState Embarque_Intermed(Pisca *const me, QEvt const *const e);

/* Local objects -----------------------------------------------------------*/
static Pisca l_pisca; /* the single instance of the Table active object */

/* Global-scope objects ----------------------------------------------------*/
QActive *const AO_Pisca = &l_pisca.super; /* "opaque" AO pointer */

/*..........................................................................*/

void Pisca_ctor(void)
{
	Pisca *me = &l_pisca;
	QActive_ctor(&me->super, Q_STATE_CAST(&Elevador_initial));
	QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
}

static QState Elevador_initial(Pisca *const me, QEvt const *const e)
{

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

	QActive_subscribe(&me->super, PORTAABERTA1_SIG);
	QActive_subscribe(&me->super, PORTAABERTA2_SIG);
	QActive_subscribe(&me->super, PORTAABERTA3_SIG);

	QActive_subscribe(&me->super, PORTAFECHADA1_SIG);
	QActive_subscribe(&me->super, PORTAFECHADA2_SIG);
	QActive_subscribe(&me->super, PORTAFECHADA3_SIG);

	QActive_subscribe(&me->super, ANDAR1_SIG);
	QActive_subscribe(&me->super, ANDAR2_SIG);
	QActive_subscribe(&me->super, ANDAR3_SIG);

	QActive_subscribe(&me->super, PARADO1_SIG);
	QActive_subscribe(&me->super, PARADO2_SIG);
	QActive_subscribe(&me->super, PARADO3_SIG);

	return Q_TRAN(&Abrindo_Porta);
}

int andar = 2;

void trataSinalPorta(enum DPPSignals sinal) {
	switch (sinal)
	{
	case PORTA1_SIG:
	{
		bsp_acionaporta(1, -1);
		break;
	}
	case PORTA2_SIG:
	{
		bsp_acionaporta(2, -1);
		break;
	}
	case PORTA3_SIG:
	{
		bsp_acionaporta(3, -1);
		break;
	}
	default:
		break;
	}
	return;
}

void trataSinalSobe(enum DPPSignals sinal) {
	switch (sinal)
	{
	case SOBE1_SIG:
	{
		bsp_sobeon(1);
		break;
	}
	case SOBE2_SIG:
	{
		bsp_sobeon(2);
		break;
	}
	case SOBE3_SIG:
	{
		bsp_sobeon(3);
		break;
	}
	default:
		break;
	}
	return;
}

void trataSinalDesce(enum DPPSignals sinal) {
	switch (sinal)
	{
	case DESCE1_SIG:
	{
		bsp_desceon(1);
		break;
	}
	case DESCE2_SIG:
	{
		bsp_desceon(2);
		break;
	}
	case DESCE3_SIG:
	{
		bsp_desceon(3);
		break;
	}
	default:
		break;
	}
	return;
}

void trataSinalCabine(enum DPPSignals sinal) {
	switch (sinal)
	{
	case CABINE1_SIG:
	{
		bsp_cabineon(1);
		break;
	}
	case CABINE2_SIG:
	{
		bsp_cabineon(2);
		break;
	}
	case CABINE3_SIG:
	{
		bsp_cabineon(3);
		break;
	}
	default:
		break;
	}
	return;
}

static QState Abrindo_Porta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		bsp_acionaporta(andar, -1);
		status = Q_HANDLED();
		break;
	}
	case PORTAABERTA1_SIG:
	case PORTAABERTA2_SIG:
	case PORTAABERTA3_SIG:
	{
		status = Q_TRAN(&Porta_aberta);
		break;
	}
	default:
	{
		trataSinalSobe(e->sig);
		trataSinalDesce(e->sig);
		trataSinalCabine(e->sig);
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Porta_aberta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		printf("Porta_aberta");
		fflush(stdout);
		status = Q_HANDLED();
		break;
	}
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	{
		trataSinalCabine(e->sig);
		status = Q_TRAN(&Fechando_Porta);
		break;
	}
	case TIMEOUT_SIG:
	{
		printf("tentar fechar a porta");
		fflush(stdout);
		status = Q_TRAN(&Fechando_Porta);
		break;
	}
	default:
	{
		trataSinalSobe(e->sig);
		trataSinalDesce(e->sig);
		trataSinalCabine(e->sig);
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Fechando_Porta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		bsp_acionaporta(andar, 1);
		status = Q_HANDLED();
		break;
	}
	case PORTAFECHADA1_SIG:
	case PORTAFECHADA2_SIG:
	case PORTAFECHADA3_SIG:
	{
		status = Q_TRAN(&Verifica_Destino);
		break;
	}
	case PORTA1_SIG:
	case PORTA2_SIG:
	case PORTA3_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		QTimeEvt_armX	(&me->timeEvt , DOOR_TIMEOUT, 0);
		break;
	}
	default:
	{
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Verifica_Destino(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	default:
	{
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Elevador_Descendo(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	default:
	{
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Elevador_Subindo(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	default:
	{
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}

static QState Embarque_Intermed(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	default:
	{
		status = Q_SUPER(&QHsm_top);
		break;
	}
	}
	return status;
}
