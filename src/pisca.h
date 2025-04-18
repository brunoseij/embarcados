/*
 * pisca.h
 *
 *  Created on: 14 de mar de 2017
 *      Author: tamandua32
 */

#ifndef PISCA_H_
#define PISCA_H_
#include "qp_port.h"

enum DPPSignals
{
	PORTA1_SIG = Q_USER_SIG, /* published by BSP */
	PORTA2_SIG,
	PORTA3_SIG,
	SOBE1_SIG,
	SOBE2_SIG,
	SOBE3_SIG,
	DESCE1_SIG,
	DESCE2_SIG,
	DESCE3_SIG,
	CABINE1_SIG,
	CABINE2_SIG,
	CABINE3_SIG,
	PORTAABERTA1_SIG,
	PORTAABERTA2_SIG,
	PORTAABERTA3_SIG,
	PORTAFECHADA1_SIG,
	PORTAFECHADA2_SIG,
	PORTAFECHADA3_SIG,
	ANDAR1_SIG,
	ANDAR2_SIG,
	ANDAR3_SIG,
	PARADO1_SIG,
	PARADO2_SIG,
	PARADO3_SIG,
	MAX_PUB_SIG, /* the last published signal */

	B2_SIG, /* posted direclty to Pisca from BSP */
	MAX_SIG /* the last signal */
};

typedef struct PiscaEvtTag
{
	/* protected: */
	QEvt super;

	/* public: */
	uint8_t i;
} PiscaEvt;

#define QUEUESIZE ((uint8_t)5)
#define POOLSIZE ((uint8_t)5)

void Pisca_ctor(void);

extern QActive *const AO_Pisca;

#endif /* PISCA_H_ */
