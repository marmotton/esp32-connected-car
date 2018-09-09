#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <freertos/timers.h>

void timerCallbackThatDoesNothing( TimerHandle_t xTimer );
int twosComplementToInt(uint32_t twosComplement, uint32_t nBits);

#endif
