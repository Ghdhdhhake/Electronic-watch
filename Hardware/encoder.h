#ifndef _ENCODER_H
#define _ENCODER_H

typedef enum
{
    ENCODER_NONE = 0,
    ENCODER_LEFT,
    ENCODER_RIGHT,
    ENCODER_PRESS
} Encoder_Event;

void Encoder_Init(void);
Encoder_Event Encoder_Scan(void);

#endif
