#ifndef __MAIN_H
#define __MAIN_H

#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "m_misc.h"
#include "m_language.h"

#define PROGMEM
#define PSTR(x)                     (x)

#define SERIAL_ERROR_START          (m_serialprintPGM(errormagic))
#define SERIAL_ECHO_START           (m_serialprintPGM(echomagic))

#define SERIAL_ECHO(x)              (m_printPGM(PSTR(x))) 
#define SERIAL_ECHOPGM(x)           (m_serialprintPGM(PSTR(x)))
#define SERIAL_ECHOLNPGM(x)         (printf(x), printf("\r\n"))

#define SERIAL_ERRORPGM(x)          (m_serialprintPGM(PSTR(x)))
#define SERIAL_ERRORLN(x)           (m_serialprintlong(x))

#define SERIAL_PROTOCOLLN(x)        (m_serialprintlong(x))
#define SERIAL_PROTOCOLPGM(x)       (m_serialprintPGM(PSTR(x)))
#define SERIAL_PROTOCOLLNPGM(x)     (m_serialprintPGM(PSTR(x)), printf("\r\n"))

bool code_seen(char code);
float code_value(void);
long code_value_long(void);
void m_Get_Command(void);
void process_commands(void);
void m_FlushSerialRequestResend(void);
void ClearToSend(void);

#endif /* __MAIN_H */


