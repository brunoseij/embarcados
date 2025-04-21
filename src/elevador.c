#include "elevador.h"
#include "bsp.h"
#include <stdio.h>

/* Active object class -----------------------------------------------------*/
#define DOOR_TIMEOUT \
	(QTimeEvtCtr)(5 * BSP_TICKS_PER_SEC)

enum InternalSignals
{ /* internal signals */
  TIMEOUT_SIG = MAX_SIG
};
typedef struct ElevadorTag
{
	/* protected: */
	QActive super;
	int pos;
	bool subindo;
	bool descendo;
	/* private: */
	QTimeEvt timeEvt;
} Elevador;

typedef struct Andar
{
	bool subir;	  // status do botao subir
	bool descer;  // status do botao descer
	bool destino; // se alguem quer ir pra aquele andar (cabine)
} Andar;

/* protected: */
static QState Elevador_initial(Elevador *const me, QEvt const *const e);
static QState Abrindo_Porta(Elevador *const me, QEvt const *const e);
static QState Porta_aberta(Elevador *const me, QEvt const *const e);
static QState Fechando_Porta(Elevador *const me, QEvt const *const e);
static QState Elevador_Descendo(Elevador *const me, QEvt const *const e);
static QState Elevador_Subindo(Elevador *const me, QEvt const *const e);

/* Local objects -----------------------------------------------------------*/
static Elevador l_elevador; /* the single instance of the Table active object */
static Andar andar1;
static Andar andar2;
static Andar andar3;

/* Global-scope objects ----------------------------------------------------*/
QActive *const AO_Elevador = &l_elevador.super; /* "opaque" AO pointer */

/*..........................................................................*/

void Elevador_ctor(void)
{
	Elevador *me = &l_elevador;
	QActive_ctor(&me->super, Q_STATE_CAST(&Elevador_initial));
	QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);

	me->pos = 1;
	me->subindo = true;
	me->descendo = false;
}

void iniciarElevador()
{
	bsp_acionacarro(1);
	bsp_visor(1);
	for (int i = 1; i < 4; i++)
	{
		apagaBotaoSubindo(i);
		apagaBotaoDescendo(i);
		apagaBotaoCabine(i);
		fecharPorta(i);
	}
}

static QState Elevador_initial(Elevador *const me, QEvt const *const e)
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

	iniciarElevador();

	return Q_TRAN(&Abrindo_Porta);
}

int identificaDestinoSubida(Elevador *const me)
{
	int posicaoElevador = me->pos;

	if ((andar2.destino || andar2.subir) && posicaoElevador <= 2)
	{
		return 2;
	}

	if ((andar3.destino || andar3.subir) && posicaoElevador <= 3)
	{
		return 3;
	}
	return me->pos;
}

int identificaDestinoDescida(Elevador *const me)
{
	int posicaoElevador = me->pos;

	if ((andar2.destino || andar2.descer) && posicaoElevador >= 2)
	{
		return 2;
	}

	if ((andar1.destino || andar1.descer) && posicaoElevador >= 1)
	{
		return 1;
	}

	return me->pos;
}

void trataSinalPorta(enum DPPSignals sinal)
{
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

void trataSinalSobe(enum DPPSignals sinal)
{
	switch (sinal)
	{
	case SOBE1_SIG:
	{
		if (!andar1.subir)
		{
			bsp_sobeon(1);
			andar1.subir = true;
		}
		break;
	}
	case SOBE2_SIG:
	{
		if (!andar2.subir)
		{
			bsp_sobeon(2);
			andar2.subir = true;
		}
		break;
	}
	case SOBE3_SIG:
	{
		if (!andar3.subir)
		{
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

void trataSinalDesce(enum DPPSignals sinal)
{
	switch (sinal)
	{
	case DESCE1_SIG:
	{
		if (!andar1.descer)
		{
			bsp_desceon(1);
			andar1.descer = true;
		}
		break;
	}
	case DESCE2_SIG:
	{
		if (!andar2.descer)
		{
			bsp_desceon(2);
			andar2.descer = true;
		}
		break;
	}
	case DESCE3_SIG:
	{
		if (!andar3.descer)
		{
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

void trataSinalCabine(enum DPPSignals sinal)
{
	switch (sinal)
	{
	case CABINE1_SIG:
	{
		bsp_cabineon(1);
		andar1.destino = true;
		break;
	}
	case CABINE2_SIG:
	{
		bsp_cabineon(2);
		andar2.destino = true;
		break;
	}
	case CABINE3_SIG:
	{
		bsp_cabineon(3);
		andar3.destino = true;
		break;
	}
	default:
		break;
	}
	return;
}

int getAndar(enum DPPSignals sinal)
{
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

void atualizaPosicao(Elevador *const me, int andar)
{
	me->pos = andar;
	bsp_visor(me->pos);
}

void apagaBotaoSubindo(int andar)
{
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

void apagaBotaoDescendo(int andar)
{
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

void apagaBotaoCabine(int andar)
{
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

bool temChamadaNaCabine()
{
	return andar1.destino || andar2.destino || andar3.destino;
}

bool temChamadaAndar()
{
	return andar1.subir || andar2.subir || andar3.subir || andar1.descer || andar2.descer || andar3.descer;
}

bool deveSubir(Elevador *const me)
{
	if (me->descendo)
	{
		return false;
	}

	if (identificaDestinoSubida(me) == me->pos)
	{
		return false;
	}

	bool subir;
	switch (me->pos)
	{
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

bool deveDescer(Elevador *const me)
{
	if (me->subindo)
	{
		return false;
	}

	if (identificaDestinoDescida(me) == me->pos)
	{
		return false;
	}

	bool descer;
	switch (me->pos)
	{
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
		descer = andar1.destino || andar2.destino || andar2.descer || andar1.descer;
		break;
	}
	}
	return descer;
}

void abrirPorta(int andar)
{
	bsp_acionaporta(andar, -1);
}

void fecharPorta(int andar)
{
	bsp_acionaporta(andar, 1);
}

static QState Abrindo_Porta(Elevador *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		abrirPorta(me->pos);
		apagaBotaoCabine(me->pos);
		switch (me->pos)
		{
		case 1:
		{
			apagaBotaoCabine(me->pos);
			apagaBotaoSubindo(me->pos);
			apagaBotaoDescendo(me->pos);
			me->subindo = true;
			me->descendo = false;
			break;
		}
		case 2:
		{
			if (me->subindo)
			{
				if (andar2.subir)
				{
					apagaBotaoSubindo(me->pos);
				}
				else if (!temChamadaNaCabine())
				{
					apagaBotaoDescendo(me->pos);
				}
			}
			else
			{
				if (andar2.descer)
				{
					apagaBotaoDescendo(me->pos);
				}
				else if (!temChamadaNaCabine())
				{
					apagaBotaoSubindo(me->pos);
				}
			}
			break;
		}
		case 3:
		{
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
		if (getAndar(e->sig) != me->pos)
		{
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

void armarTimerPorta(Elevador *const me)
{
	if (QTimeEvt_currCtr(&me->timeEvt) == 0)
	{
		QTimeEvt_armX(&me->timeEvt, DOOR_TIMEOUT, 0);
	}
}

static QState Porta_aberta(Elevador *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		if (me->subindo && !deveSubir(me))
		{
			me->subindo = false;
		}

		if (me->descendo && !deveDescer(me))
		{
			me->descendo = false;
		}

		if (temChamadaNaCabine() || temChamadaAndar())
		{
			armarTimerPorta(me);
		}
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
		if (getAndar(e->sig) != me->pos)
		{
			trataSinalSobe(e->sig);
			trataSinalDesce(e->sig);
			trataSinalCabine(e->sig);
			armarTimerPorta(me);
		}
		status = Q_HANDLED();
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

void verificarChamadasAndar(Elevador *const me)
{
	switch (me->pos)
	{
	case 1:
	{
		if (andar2.subir)
		{
			andar2.destino = true;
			break;
		}
		if (andar3.subir || andar3.descer)
		{
			andar3.destino = true;
			break;
		}
		if (andar2.descer)
		{
			andar2.destino = true;
			break;
		}
	}
	case 2:
	{
		if (me->subindo)
		{
			if (andar3.subir || andar3.descer)
			{
				andar3.destino = true;
			}
			else if (andar1.subir || andar1.descer)
			{
				andar1.subir = true;
			}
		}
		else
		{
			if (andar1.subir || andar1.descer)
			{
				andar1.subir = true;
			}
			else if (andar3.subir || andar3.descer)
			{
				andar3.destino = true;
			}
		}
		break;
	}
	case 3:
	{
		if (andar2.descer)
		{
			andar2.destino = true;
			break;
		}
		if (andar1.descer || andar1.subir)
		{
			andar1.destino = true;
			break;
		}
		if (andar2.subir)
		{
			andar2.destino = true;
			break;
		}
	}
	}
}

static QState Fechando_Porta(Elevador *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		fecharPorta(me->pos);
		if (!temChamadaNaCabine() || temChamadaAndar())
		{
			verificarChamadasAndar(me);
		}
		status = Q_HANDLED();
		break;
	}
	case PORTAFECHADA1_SIG:
	case PORTAFECHADA2_SIG:
	case PORTAFECHADA3_SIG:
	{
		if (deveSubir(me))
		{
			status = Q_TRAN(&Elevador_Subindo);
			break;
		}

		if (deveDescer(me))
		{
			status = Q_TRAN(&Elevador_Descendo);
			break;
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
		if (getAndar(e->sig) == me->pos)
		{
			status = Q_TRAN(&Abrindo_Porta);
		}
		else
		{
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

static QState Elevador_Subindo(Elevador *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		me->subindo = true;
		me->descendo = false;
		bsp_acionacarro(identificaDestinoSubida(me));
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
		atualizaPosicao(me, getAndar(e->sig));

		status = Q_TRAN(&Abrindo_Porta);
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

static QState Elevador_Descendo(Elevador *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
	case Q_ENTRY_SIG:
	{
		me->descendo = true;
		me->subindo = false;
		bsp_acionacarro(identificaDestinoDescida(me));
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

		break;
	}
	case DESCE1_SIG:
	case DESCE2_SIG:
	case DESCE3_SIG:
	case CABINE1_SIG:
	case CABINE2_SIG:
	case CABINE3_SIG:
	{
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
