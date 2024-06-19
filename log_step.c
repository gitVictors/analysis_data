#include <stdio.h>
#include <stdint.h>



uint32_t print_bt (uint32_t *hs )
{
    uint32_t var = *hs;

    for (uint8_t i = 0; i < 4 ; ++i){
        printf ("%2x ", ( var >> (i*8) & 0xff ) );
    }
    
    return var;

}