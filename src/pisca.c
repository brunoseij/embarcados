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
	int pos;
	bool subindo;
	bool descendo;
	/* private: */
	QTimeEvt timeEvt;
} Pisca;

typedef struct Andar{
	bool subir; // status do botao subir
	bool descer; // status do botao descer
	bool destino; // se alguem quer ir pra aquele andar (cabine)
} Andar;

/* protected: */
static QState Elevador_initial(Pisca *const me, QEvt const *const e);
static QState Abrindo_Porta(Pisca *const me, QEvt const *const e);
static QState Porta_aberta(Pisca *const me, QEvt const *const e);
static QState Fechando_Porta(Pisca *const me, QEvt const *const e);
static QState Elevador_Descendo(Pisca *const me, QEvt const *const e);
static QState Elevador_Subindo(Pisca *const me, QEvt const *const e);

/* Local objects -----------------------------------------------------------*/
static Pisca l_pisca; /* the single instance of the Table active object */
static Andar andar1;
static Andar andar2;
static Andar andar3;

/* Global-scope objects ----------------------------------------------------*/
QActive *const AO_Pisca = &l_pisca.super; /* "opaque" AO pointer */

/*..........................................................................*/

void Pisca_ctor(void)
{
	Pisca *me = &l_pisca;
	QActive_ctor(&me->super, Q_STATE_CAST(&Elevador_initial));
	QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);

	me->pos = 1;
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

int identificaDestinoSubida(Pisca *const me) {
	int posicaoElevador = me->pos;

	// if ((andar1.destino || andar1.subir) && posicaoElevador == 1) {
	// 	return 1;
	// }

	if ((andar2.destino || andar2.subir || andar2.descer) && posicaoElevador <= 2) {
		return 2;
	}

	if ((andar3.destino || andar3.subir || andar3.descer) && posicaoElevador <= 3) {
		return 3;
	}
	return me->pos;
}

int identificaDestinoDescida(Pisca *const me) {
	int posicaoElevador = me->pos;

//	if ((andar3.destino || andar3.descer) && posicaoElevador == 3) {
//		return 3;
//	}

	if ((andar2.destino || andar2.descer || andar2.subir) && posicaoElevador >= 2) {
		return 2;
	}

	if ((andar1.destino || andar1.descer || andar1.subir) && posicaoElevador >= 1) {
		return 1;
	}

	return me->pos;
}

bool DeveSubirParaDescer(Pisca *const me) {
	int posicaoElevador = me->pos;

	if ((andar2.descer) && posicaoElevador <=2 ){
		return true;
	}

	if ((andar3.descer) && posicaoElevador <=3 ){
		return true;
	}

	return false;
}

bool DeveDescerParaSubir(Pisca *const me) {
	int posicaoElevador = me->pos;

	if (andar2.subir && posicaoElevador > 2) {
		return true;
	}

	if (andar1.subir && posicaoElevador > 1) {
		return true;
	}
	return false;
}



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
		if (!andar1.subir) {
			bsp_sobeon(1);
			andar1.subir = true;
		}
		break;
	}
	case SOBE2_SIG:
	{
		if (!andar2.subir) {
			bsp_sobeon(2);
			andar2.subir = true;
		}
		break;
	}
	case SOBE3_SIG:
	{
		if (!andar3.subir) {
			bsp_sobeon(3);
			andar3.subir = true;
		}
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
		if (!andar1.descer) {
			bsp_desceon(1);
			andar1.descer = true;
		}
		break;
	}
	case DESCE2_SIG:
	{
		if (!andar2.descer) {
			bsp_desceon(2);
			andar2.descer = true;
		}
		break;
	}
	case DESCE3_SIG:
	{
		if (!andar3.descer) {
			bsp_desceon(3);
			andar3.descer = true;
		}
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
		if (!andar1.destino) {
			bsp_cabineon(1);
			andar1.destino = true;
		}
		break;
	}
	case CABINE2_SIG:
	{
		if (!andar2.destino) {
			bsp_cabineon(2);
			andar2.destino = true;
		}
		break;
	}
	case CABINE3_SIG:
	{
		if (!andar3.destino) {
			bsp_cabineon(3);
			andar3.destino = true;
		}
		break;
	}
	default:
		break;
	}
	return;
}

int getAndar(enum DPPSignals sinal) {
	int andar;
	switch (sinal)
	{
	case PARADO1_SIG:
	case ANDAR1_SIG:
	case CABINE1_SIG:
	case SOBE1_SIG:
	case DESCE1_SIG:
	{
		andar = 1;
		break;
	}
	case PARADO2_SIG:
	case ANDAR2_SIG:
	case CABINE2_SIG:
	case SOBE2_SIG:
	case DESCE2_SIG:
	{
		andar = 2;
		break;
	}
	case PARADO3_SIG:
	case ANDAR3_SIG:
	case CABINE3_SIG:
	case SOBE3_SIG:
	case DESCE3_SIG:
	{
		andar = 3;
		break;
	}
	default:
		break;
	}
	return andar;
}

void atualizaPosicao(Pisca *const me, int andar){
	me->pos = andar;
	bsp_visor(me->pos);
}

void apagaBotaoSubindo(int andar) {
	switch (andar)
	{
		case 1:
		{
			andar1.subir = false;
			bsp_sobeoff(1);
			break;
		}
		case 2:
		{
			andar2.subir = false;
			bsp_sobeoff(2);
			break;
		}
		case 3:
		{
			andar3.subir = false;
			bsp_sobeoff(3);
			break;
		}
	}
}

void apagaBotaoDescendo(int andar) {
	printf("apagando botao descer%d\n", andar);
	fflush(stdout);
	switch (andar)
	{
		case 1:
		{
			andar1.descer = false;
			bsp_desceoff(1);
			break;
		}
		case 2:
		{
			andar2.descer = false;
			bsp_desceoff(2);
			break;
		}
		case 3:
		{
			andar3.descer = false;
			bsp_desceoff(3);
			break;
		}
	}
}

void apagaBotaoCabine(int andar) {
	switch (andar)
	{
		case 1:
		{
			andar1.destino = false;
			bsp_cabineoff(1);
			break;
		}
		case 2:
		{
			andar2.destino = false;
			bsp_cabineoff(2);
			break;
		}
		case 3:
		{
			andar3.destino = false;
			bsp_cabineoff(3);
			break;
		}
	}
}

bool temChamadaNaCabine(){
	return andar1.destino || andar2.destino || andar3.destino;
}

bool temChamadaAndar(){
	return andar1.subir || andar2.subir || andar3.subir || andar1.descer || andar2.descer || andar3.descer;
}

bool deveSubir(Pisca *const me){
	if (me->descendo){
		return false;
	}

	if (identificaDestinoSubida(me) == me->pos) {
		return false;
	}

	bool subir;
	// olhar a posicao do elevador e a posicao do destino
	switch(me->pos) {
		case 1:
		{
			subir = andar2.destino || andar3.destino || andar2.subir || andar3.subir;
			break;
		}
		case 2:
		{
			subir = andar3.destino || andar3.subir;
			break;
		}
		case 3:
		{	
			subir = false;
			break;
		}
	}

	return subir;
}

bool deveDescer(Pisca *const me){
	if (me->subindo){
		return false;
	}

	if (identificaDestinoDescida(me) == me->pos) {
		return false;
	}

	bool descer;
	// olhar a posicao do elevador e a posicao do destino
	switch(me->pos) {
		case 1:
		{
			descer = false;
			break;
		}
		case 2:
		{
			descer = andar1.destino || andar1.descer;
			break;
		}
		case 3:
		{	
			descer = andar1.destino || andar2.destino || andar1.descer || andar2.descer;
			break;
		}
	}
	return descer;
}

void abrirPorta(int andar) {
	bsp_acionaporta(andar, -1);
}

void fecharPorta(int andar) {
	bsp_acionaporta(andar, 1);
}

static QState Abrindo_Porta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		abrirPorta(me->pos);
		switch(me->pos) {
			case 1: {
				apagaBotaoCabine(me->pos);
				apagaBotaoSubindo(me->pos);
				apagaBotaoDescendo(me->pos);
				me->subindo = true;
				me->descendo = false;
				break;
			}
			case 2: {
				apagaBotaoCabine(me->pos);
				if (me->subindo) {
					apagaBotaoSubindo(me->pos);
					break;
				} else {
					apagaBotaoDescendo(me->pos);
					break;
				}
			}
			case 3: {
				apagaBotaoCabine(me->pos);
				apagaBotaoSubindo(me->pos);
				apagaBotaoDescendo(me->pos);
				me->subindo = false;
				me->descendo = true;
				break;
			}
		}
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
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	case SOBE1_SIG:
	case SOBE2_SIG:
	case SOBE3_SIG:
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	{
		if (getAndar(e->sig) != me->pos) {
			trataSinalCabine(e->sig);
			trataSinalSobe(e->sig);
			trataSinalDesce(e->sig);
		}
		status = Q_HANDLED();
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

static QState Porta_aberta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		if (me->subindo && !deveSubir(me)) {
			me->subindo = false;
		}

		if (me->descendo && !deveDescer(me)) {
			me->descendo = false;
		}

		if (temChamadaNaCabine() || temChamadaAndar()) { // se tem chamada de andar que nao é o que a cabine ta parada
			QTimeEvt_armX(&me->timeEvt , DOOR_TIMEOUT, 0);
		}

		if (!temChamadaNaCabine() && temChamadaAndar()) {
			if (DeveSubirParaDescer(me)) {
				me->subindo = true;
				me->descendo = false;
			} else if (DeveDescerParaSubir(me)) {
				me->descendo = true;
				me->subindo = false;
			}
		}
		// se nao tem chamada na cabine, ir até o andar que chamou

		printf("Porta aberta\n");
		fflush(stdout);
		status = Q_HANDLED();
		break;
	}
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	case SOBE1_SIG:
	case SOBE2_SIG:
	case SOBE3_SIG:
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	{
		if (getAndar(e->sig) != me->pos) {
			trataSinalSobe(e->sig);
			trataSinalDesce(e->sig);
			trataSinalCabine(e->sig);
			status = Q_TRAN(&Fechando_Porta);
		} else {
			status = Q_HANDLED();
		}
		break;
	}
	case TIMEOUT_SIG:
	{
		status = Q_TRAN(&Fechando_Porta);
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

static QState Fechando_Porta(Pisca *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		printf("Fechando porta\n");
		fflush(stdout);
		fecharPorta(me->pos);
		status = Q_HANDLED();
		break;
	}
	case PORTAFECHADA1_SIG:
	case PORTAFECHADA2_SIG:
	case PORTAFECHADA3_SIG:
	{
		if (deveSubir(me)) {
			status = Q_TRAN(&Elevador_Subindo);
			break;
//			printf("elevador deve subir\n");
//			fflush(stdout);
		}
		
		if(deveDescer(me)) {
//			printf("elevador deve descer\n");
			status = Q_TRAN(&Elevador_Descendo);
			break;
		}

		if (!temChamadaNaCabine() && temChamadaAndar()) {
			if (DeveSubirParaDescer(me)) {
				status = Q_TRAN(&Elevador_Subindo);
				break;
			} else if (DeveDescerParaSubir(me)) {
				status = Q_TRAN(&Elevador_Descendo);
				break;
			}
		}

		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	case PORTA1_SIG:
	case PORTA2_SIG:
	case PORTA3_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		break;
	}
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	case SOBE1_SIG:
	case SOBE2_SIG:
	case SOBE3_SIG:
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	{
		if (getAndar(e->sig) == me->pos) {
			status = Q_TRAN(&Abrindo_Porta);
		} else {
			trataSinalSobe(e->sig);
			trataSinalDesce(e->sig);
			trataSinalCabine(e->sig);
			status = Q_HANDLED();
		}
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
		printf("elevador subindo para %d\n", identificaDestinoSubida(me));
		fflush(stdout);
		me->subindo = true;
		me->descendo = false;
		bsp_acionacarro(identificaDestinoSubida(me));// verificar pra qual q tem q subir, identificar o mais perto
		status = Q_HANDLED();
		break;
	}
	case ANDAR1_SIG:
	case ANDAR2_SIG:
	case ANDAR3_SIG:
	{
		atualizaPosicao(me, getAndar(e->sig));
		status = Q_HANDLED();
		break;
	}
	case PARADO1_SIG:
	case PARADO2_SIG:
	case PARADO3_SIG:
	{
		// atualizar posicao, apagar a luz subindo e a cabine
		atualizaPosicao(me, getAndar(e->sig));

		status = Q_TRAN(&Abrindo_Porta);

		// apagaBotaoSubindo(getAndar(e->sig));
		// apagaBotaoCabine(getAndar(e->sig));	
		// se subiu pra descer, apaga o descer tbm
		// trataSinalParado(e->sig, me);
		break;
	}
	case SOBE1_SIG:
	case SOBE2_SIG:
	case SOBE3_SIG:
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	{
		trataSinalCabine(e->sig);
		trataSinalSobe(e->sig);
//		printf("elevador subindo para %d\n", identificaDestinoSubida(me));
		fflush(stdout);
		bsp_acionacarro(identificaDestinoSubida(me));
		status = Q_HANDLED();
	}
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	{
		trataSinalDesce(e->sig);
		status = Q_HANDLED();
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
		printf("elevador descendo para %d\n", identificaDestinoDescida(me));
		fflush(stdout);
		me->descendo = true;
		me->subindo = false;
		bsp_acionacarro(identificaDestinoDescida(me)); // verificar pra qual q tem q subir, identificar o mais perto
		status = Q_HANDLED();
		break;
	}
	case ANDAR1_SIG:
	case ANDAR2_SIG:
	case ANDAR3_SIG:
	{
		atualizaPosicao(me, getAndar(e->sig));
		status = Q_HANDLED();
		break;
	}
	case PARADO1_SIG:
	case PARADO2_SIG:
	case PARADO3_SIG:
	{
		status = Q_TRAN(&Abrindo_Porta);
		atualizaPosicao(me, getAndar(e->sig));

		// apagaBotaoDescendo(getAndar(e->sig));
		// apagaBotaoCabine(getAndar(e->sig));

		break;
	}
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	{
//		printf("elevador descendo para %d\n", identificaDestinoDescida(me));
		fflush(stdout);
		trataSinalDesce(e->sig);
		trataSinalCabine(e->sig);
		bsp_acionacarro(identificaDestinoDescida(me));
		status = Q_HANDLED();
		break;
	}
	case SOBE1_SIG:
	case SOBE2_SIG:
	case SOBE3_SIG:
	{
		trataSinalSobe(e->sig);
		status = Q_HANDLED();
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
