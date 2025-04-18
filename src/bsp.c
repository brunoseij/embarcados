//============================================================================
// Product: BSP for DPP example (console)
// Last updated for version 8.0.0
// Last updated on  2024-09-18
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses/>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpc.h"   // QP/C real-time embedded framework
#include "pisca.h" // DPP Application interface
#include "bsp.h"   // Board Support Package

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <windows.h>
#include <pthread.h>

#include "safe_std.h" // portable "safe" <stdio.h>/<string.h> facilities
#include <stdlib.h>	  // for exit()

Q_DEFINE_THIS_FILE

// Local objects -------------------------------------------------------------

static uint32_t l_rnd; // random seed

char *in_signals[] = {
	"porta1",		 // 0
	"porta2",		 // 1
	"porta3",		 // 2
	"sobe1",		 // 3
	"sobe2",		 // 4
	"sobe3",		 // 5
	"desce1",		 // 6
	"desce2",		 // 7
	"desce3",		 // 8
	"cabine1",		 // 9
	"cabine2",		 // 10
	"cabine3",		 // 11
	"PortaAberta1",	 // 12
	"PortaAberta2",	 // 13
	"PortaAberta3",  // 14
	"PortaFechada1", // 15
	"PortaFechada2", // 16
	"PortaFechada3", // 17
	"Andar1",        // 18
	"Andar2",        // 19
	"Andar3",        // 20
	"Parado1",       // 21
	"Parado2",       // 22
	"Parado3",       // 23
};

static uint32_t l_rnd;
struct sockaddr_in si_other;
int s;
WSADATA wsaData;
LPHOSTENT lpHostEntry;
pthread_t tudpServer;

void sendUDP(char *sig)
{

	int slen = sizeof(si_other);
	int siglen;

	if (s != -1)
	{
		// zero out the structure
		siglen = strlen(sig);
		sendto(s, sig, siglen, 0, (struct sockaddr *)&si_other, slen);
	}
}

void *udpServer()
{

	struct sockaddr_in si_other;
	struct sockaddr_in si_me;
	int i;
	int slen = sizeof(si_other), recv_len;
	char buf[BUFLEN];
	int ss = -1;

	ss = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s != -1)
	{
		// zero out the structure
		memset((char *)&si_me, 0, sizeof(si_me));
		si_me.sin_family = AF_INET;
		si_me.sin_port = htons(PORTIN);
		si_me.sin_addr.s_addr = htonl(INADDR_ANY);

		//
		if (bind(ss, (struct sockaddr *)&si_me, sizeof(si_me)) != -1)
		{

			while (1)
			{

				if ((recv_len = recvfrom(ss, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) != -1)
				{

					buf[recv_len] = '\0';
					printf("%s \n", buf);
					fflush(stdout);
					for (i = 0; i < 24; i++)
					{
						if (strcmp(buf, in_signals[i]) == 0)

						{
							switch (i)
							{
							case 0:
							{
								static QEvt const p1Evt = QEVT_INITIALIZER(PORTA1_SIG);
								QACTIVE_PUBLISH(&p1Evt, NULL);
								break;
							}
							case 1:
							{
								static QEvt const p2Evt = QEVT_INITIALIZER(PORTA2_SIG);
								QACTIVE_PUBLISH(&p2Evt, NULL);
								break;
							}
							case 2:
							{
								static QEvt const p3Evt = QEVT_INITIALIZER(PORTA3_SIG);
								QACTIVE_PUBLISH(&p3Evt, NULL);
								break;
							}
							case 3:
							{
								static QEvt const s1Evt = QEVT_INITIALIZER(SOBE1_SIG);
								QACTIVE_PUBLISH(&s1Evt, NULL);
								break;
							}
							case 4:
							{
								static QEvt const s2Evt = QEVT_INITIALIZER(SOBE2_SIG);
								QACTIVE_PUBLISH(&s2Evt, NULL);
								break;
							}
							case 5:
							{
								static QEvt const s3Evt = QEVT_INITIALIZER(SOBE3_SIG);
								QACTIVE_PUBLISH(&s3Evt, NULL);
								break;
							}
							case 6:
							{

								static QEvt const d1Evt = QEVT_INITIALIZER(DESCE1_SIG);
								QACTIVE_PUBLISH(&d1Evt, NULL);
								break;
							}
							case 7:
							{
								static QEvt const d2Evt = QEVT_INITIALIZER(DESCE2_SIG);
								QACTIVE_PUBLISH(&d2Evt, NULL);
								break;
							}
							case 8:
							{
								static QEvt const d3Evt = QEVT_INITIALIZER(DESCE3_SIG);
								QACTIVE_PUBLISH(&d3Evt, NULL);
								break;
							}
							case 9:
							{
								static QEvt const c1Evt = QEVT_INITIALIZER(CABINE1_SIG);
								QACTIVE_PUBLISH(&c1Evt, NULL);
								break;
							}
							case 10:
							{
								static QEvt const c2Evt = QEVT_INITIALIZER(CABINE2_SIG);
								QACTIVE_PUBLISH(&c2Evt, NULL);
								break;
							}
							case 11:
							{
								static QEvt const c3Evt = QEVT_INITIALIZER(CABINE3_SIG);
								QACTIVE_PUBLISH(&c3Evt, NULL);
								break;
							}
							case 12:
							{
								static QEvt const pa1Evt = QEVT_INITIALIZER(PORTAABERTA1_SIG);
								QACTIVE_PUBLISH(&pa1Evt, NULL);
								break;
							}
							case 13:
							{
								static QEvt const pa2Evt = QEVT_INITIALIZER(PORTAABERTA2_SIG);
								QACTIVE_PUBLISH(&pa2Evt, NULL);
								break;
							}
							case 14:
							{
								static QEvt const pa3Evt = QEVT_INITIALIZER(PORTAABERTA3_SIG);
								QACTIVE_PUBLISH(&pa3Evt, NULL);
								break;
							}
							case 15:
							{
								static QEvt const pf1Evt = QEVT_INITIALIZER(PORTAFECHADA1_SIG);
								QACTIVE_PUBLISH(&pf1Evt, NULL);
								break;
							}
							case 16:
							{
								static QEvt const pf2Evt = QEVT_INITIALIZER(PORTAFECHADA2_SIG);
								QACTIVE_PUBLISH(&pf2Evt, NULL);
								break;
							}
							case 17:
							{
								static QEvt const pf3Evt = QEVT_INITIALIZER(PORTAFECHADA3_SIG);
								QACTIVE_PUBLISH(&pf3Evt, NULL);
								break;
							}
							case 18:
							{
								static QEvt const a1Evt = QEVT_INITIALIZER(ANDAR1_SIG);
								QACTIVE_PUBLISH(&a1Evt, NULL);
								break;
							}
							case 19:
							{
								static QEvt const a2Evt = QEVT_INITIALIZER(ANDAR2_SIG);
								QACTIVE_PUBLISH(&a2Evt, NULL);
								break;
							}
							case 20:
							{
								static QEvt const a3Evt = QEVT_INITIALIZER(ANDAR3_SIG);
								QACTIVE_PUBLISH(&a3Evt, NULL);
								break;
							}
							case 21:
							{
								static QEvt const pr1Evt = QEVT_INITIALIZER(PARADO1_SIG);
								QACTIVE_PUBLISH(&pr1Evt, NULL);
								break;
							}
							case 22:
							{
								static QEvt const pr2Evt = QEVT_INITIALIZER(PARADO2_SIG);
								QACTIVE_PUBLISH(&pr2Evt, NULL);
								break;
							}
							case 23:
							{
								static QEvt const pr3Evt = QEVT_INITIALIZER(PARADO3_SIG);
								QACTIVE_PUBLISH(&pr3Evt, NULL);
								break;
							}
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

// elevadorsobeon
void bsp_sobeon(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadorsobeon1";
		break;
	}
	case 2:
	{
		signal = "elevadorsobeon2";
		break;
	}
	case 3:
	{
		signal = "elevadorsobeon3";
		break;
	}
	}
	printf("sobe on %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}

void bsp_sobeoff(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadorsobeoff1";
		break;
	}
	case 2:
	{
		signal = "elevadorsobeoff2";
		break;
	}
	case 3:
	{
		signal = "elevadorsobeoff3";
		break;
	}
	}
	printf("sobe off %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}

// elevadordesceon
void bsp_desceon(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadordesceon1";
		break;
	}
	case 2:
	{
		signal = "elevadordesceon2";
		break;
	}
	case 3:
	{
		signal = "elevadordesceon3";
		break;
	}
	}
	printf("desce on %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}

void bsp_desceoff(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadordesceoff1";
		break;
	}
	case 2:
	{
		signal = "elevadordesceoff2";
		break;
	}
	case 3:
	{
		signal = "elevadordesceoff3";
		break;
	}
	}
	printf("desce off %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}
// -------------------------------------------------------------------------------------------------------
// elevadorcabine
void bsp_cabineon(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadorcabineon1";
		break;
	}
	case 2:
	{
		signal = "elevadorcabineon2";
		break;
	}
	case 3:
	{
		signal = "elevadorcabineon3";
		break;
	}
	}
	printf("cabine on %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}

void bsp_cabineoff(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadorcabineoff1";
		break;
	}
	case 2:
	{
		signal = "elevadorcabineoff2";
		break;
	}
	case 3:
	{
		signal = "elevadorcabineoff3";
		break;
	}
	}
	printf("cabine off %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}
// -------------------------------------------------------------------------------------------------------
// acionacarro
void bsp_acionacarro(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "acionacarro1";
		break;
	}
	case 2:
	{
		signal = "acionacarro2";
		break;
	}
	case 3:
	{
		signal = "acionacarro3";
		break;
	}
	}
	printf("acionacarro %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}
// ---------------------------------
// acionar porta
void bsp_acionaporta(int andar, int sentido)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		switch (sentido)
		{
		case -1:
		{
			signal = "acionaporta1-1";
			break;
		}
		case 0:
		{
			signal = "acionaporta100";
			break;
		}
		case 1:
		{
			signal = "acionaporta1+1";
			break;
		}
		}
		break;
	}
	case 2:
	{
		switch (sentido)
		{
		case -1:
		{
			signal = "acionaporta2-1";
			break;
		}
		case 0:
		{
			signal = "acionaporta200";
			break;
		}
		case 1:
		{
			signal = "acionaporta2+1";
			break;
		}
		}
		break;
	}
	case 3:
	{
		switch (sentido)
		{
		case -1:
		{
			signal = "acionaporta3-1";
			break;
		}
		case 0:
		{
			signal = "acionaporta300";
			break;
		}
		case 1:
		{
			signal = "acionaporta3+1";
			break;
		}
		}
		break;
	}
	}
	printf("acionaporta %d %d\n", andar, sentido);
	fflush(stdout);
	sendUDP(signal);
}

// acionacarro
void bsp_visor(int andar)
{
	char *signal;
	switch (andar)
	{
	case 1:
	{
		signal = "elevadordigito1";
		break;
	}
	case 2:
	{
		signal = "elevadordigito2";
		break;
	}
	case 3:
	{
		signal = "elevadordigito3";
		break;
	}
	}
	printf("atualizando visor %d\n", andar);
	fflush(stdout);
	sendUDP(signal);
}
// ---------------------------------

#ifdef Q_SPY
enum
{
	PHILO_STAT = QS_USER,
	PAUSED_STAT,
};

// QSpy source IDs
static QSpyId const l_clock_tick = {QS_AP_ID};
#endif

//============================================================================
Q_NORETURN Q_onError(char const *const module, int_t const id)
{
	QS_ASSERTION(module, id, 10000U); // report assertion to QS
	QF_onCleanup();
	QS_EXIT();
	exit(-1);
}
//............................................................................
void assert_failed(char const *const module, int_t const id); // prototype
void assert_failed(char const *const module, int_t const id)
{
	Q_onError(module, id);
}

//============================================================================
void BSP_init(int argc, char *argv[])
{
	Q_UNUSED_PAR(argc);
	Q_UNUSED_PAR(argv);

	BSP_randomSeed(1234U);

	// initialize the QS software tracing
	if (QS_INIT((argc > 1) ? argv[1] : (void *)0) == 0U)
	{
		Q_ERROR();
	}

	QS_OBJ_DICTIONARY(&l_clock_tick);

	QS_USR_DICTIONARY(PHILO_STAT);
	QS_USR_DICTIONARY(PAUSED_STAT);

	QS_ONLY(produce_sig_dict());

	// setup the QS filters...
	QS_GLB_FILTER(QS_ALL_RECORDS);
	QS_GLB_FILTER(-QS_QF_TICK); // exclude the tick record

	WSAStartup(MAKEWORD(2, 1), &wsaData);
	lpHostEntry = gethostbyname("127.0.0.1");
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORTOUT);
	si_other.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	pthread_create(&tudpServer, NULL, udpServer, NULL);
}
//............................................................................
void BSP_start(void)
{
	// initialize event pools

	static QF_MPOOL_EL(PiscaEvt) smlPoolSto[POOLSIZE];
	QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

	// initialize publish-subscribe
	static QSubscrList subscrSto[MAX_PUB_SIG];
	QActive_psInit(subscrSto, Q_DIM(subscrSto));

	// instantiate and start AOs/threads...

	static QEvtPtr l_piscaQueueSto[QUEUESIZE];
	Pisca_ctor();
	QActive_start(AO_Pisca,
				  7U,					  // QP prio. of the AO
				  l_piscaQueueSto,		  // event queue storage
				  Q_DIM(l_piscaQueueSto), // queue length [events]
				  (void *)0, 0U,		  // no stack storage
				  (void *)0);			  // no initialization param
}
//............................................................................
void BSP_terminate(int16_t result)
{
	(void)result;
	QF_stop(); // stop the main "ticker thread"
}

//............................................................................
uint32_t BSP_random(void)
{ // a very cheap pseudo-random-number generator
	// "Super-Duper" Linear Congruential Generator (LCG)
	// LCG(2^32, 3*7*11*13*23, 0, seed)
	//
	uint32_t rnd = l_rnd * (3U * 7U * 11U * 13U * 23U);
	l_rnd = rnd;
	return rnd >> 8;
}
//............................................................................
void BSP_randomSeed(uint32_t seed)
{
	l_rnd = seed;
}

//============================================================================
#if CUST_TICK
#include <sys/select.h> // for select() call used in custom tick processing
#endif

void QF_onStartup(void)
{
	QF_consoleSetup();

#if CUST_TICK
	// disable the standard clock-tick service by setting tick-rate to 0
	QF_setTickRate(0U, 10U); // zero tick-rate / ticker thread prio.
#else
	QF_setTickRate(BSP_TICKS_PER_SEC, 50); // desired tick rate/ticker-prio
#endif
}
//............................................................................
void QF_onCleanup(void)
{
	printf("\n%s\n", "Bye! Bye!");
	QF_consoleCleanup();
}
//............................................................................
void QF_onClockTick(void)
{

#if CUST_TICK
	// NOTE:
	// The standard clock-tick service has been DISABLED in QF_onStartup()
	// by setting the clock tick rate to zero.
	// Therefore QF_onClockTick() must implement an alternative waiting
	// mechanism for the clock period. This particular implementation is
	// based on the select() system call to block for the desired timeout.

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = (1000000 / BSP_TICKS_PER_SEC);
	select(0, NULL, NULL, NULL, &tv); // block for the timevalue
#endif

	QTIMEEVT_TICK_X(0U, &l_clock_tick); // process time events at rate 0

	QS_RX_INPUT(); // handle the QS-RX input
	QS_OUTPUT();   // handle the QS output

	switch (QF_consoleGetKey())
	{
	case '\33':
	{ // ESC pressed?
		BSP_terminate(0);
		break;
	}
	default:
	{
		break;
	}
	}
}

//============================================================================
#ifdef Q_SPY // define QS callbacks

//............................................................................
//! callback function to execute user commands
void QS_onCommand(uint8_t cmdId,
				  uint32_t param1, uint32_t param2, uint32_t param3)
{
	Q_UNUSED_PAR(cmdId);
	Q_UNUSED_PAR(param1);
	Q_UNUSED_PAR(param2);
	Q_UNUSED_PAR(param3);
}

#endif // Q_SPY
