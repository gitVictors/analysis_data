#include <stdio.h>
#include <stdint.h>



uint32_t print_bt (uint32_t *hs )
{
    uint32_t var = *hs;
    uint8_t tmp ;

    for (int8_t i = 3; i >=  0 ; --i){
        tmp = ( var >> (i*8) & 0xff ) ;
        if (tmp == 0)
            printf ("\033[42m%02x\033[0m ", tmp);
        else 
            printf ("%02x ", tmp);
    }
    
    return var;

}

void print_bt_mr (uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h){

    print_bt (&a);
    print_bt (&b);
    print_bt (&c);
    print_bt (&d);
    print_bt (&e);
    print_bt (&f);
    print_bt (&g);
    print_bt (&h);

}