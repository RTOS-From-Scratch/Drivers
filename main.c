#include "tm4c123gh6pm.h"
#include "src/GPIO.h"

int main()
{
    GPIO_pinsMode( PORT_F, 0xE, OUT );
//    GPIO_pinMode( F0, IN_PULLUP );

    while (1)
    {
            GPIO_pinWrite( F1, HIGH );
            GPIO_pinWrite( F2, HIGH );
            GPIO_pinWrite( F3, HIGH );
    }
    return 0;
}
