#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//==== константы и пеерменные ==================================================

// само сообщение для шифрования
const unsigned char * msg = "hello world";

unsigned int mass_for_commpres [64];

//Инициализация начальных hash значенией
const unsigned int sha256_init_state[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

//Инициализация массива округленных констант
const unsigned int sha256_consts[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, /*  0 */
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, /*  8 */
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, /* 16 */
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, /* 24 */
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, /* 32 */
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, /* 40 */
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, /* 48 */
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, /* 56 */
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


//===================================================================================

//Расширяем одно сообщеение с [0..15] до [0..63]
static void extention_mss (const unsigned char* msg, unsigned char *msg_512 , int cnt) {
    msg_512 = (unsigned char*) malloc (512/8); 
    memcpy (msg_512, msg, cnt);
}

//Функция формирования очереди по 512 бит или 16 слов
//Предворительная обработка входного сообщения. кратное 512 бит
void  get_msg_512 (const char *msg, unsigned char *msg_512) {
    int cnt; //
    int tmp;

    cnt = strlen (msg);
    memcpy ( msg_512, msg, cnt );
    msg_512[cnt] = (unsigned char) 0x80U;//add 0x1 in big-endian
    //число бит в сообщении
    tmp = cnt * 8;
    msg_512[ (512/8-1) ] = tmp; //big-endian
    
}

static unsigned int read_u32 ( unsigned char *src){
    
    unsigned int res;
    
    res = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];

    return res;
}

uint32_t ror(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (-n & 31));
}
//созданеи масс 64 слова. типа uint32_t
//из массива 64 слова топа uint8_t . [0...15]
//остальные слоова [16..64] получаем из свретки 
void  get_msg_uint32 ( uint8_t *msg_512, uint32_t* msg_i32) {
    
    //бежим по массиву 64 байта (64/4 = 16)
    for (int i = 0; i <  16 ; ++i) {
        msg_i32[i] = read_u32(&msg_512[i * 4]);
    } 
    //бежим по массиву i32 и заполняем оствшиеся индексы
    for (int i = 16; i != 64; ++i) {
        w15 = w[i - 15];
        w2 = w[i - 2];
        s0 = ror(w15, 7) ^ ror(w15, 18) ^ (w15 >> 3);
        s1 = ror(w2, 17) ^ ror(w2, 19) ^ (w2 >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }


}

//Основной цикл сжатия 

int main ( int argc, char* argv[]){

    char *msg_512 = malloc ( 512/8 ); // 512 bit
    for (int i =0; i < 64; ++i) 
        msg_512[i] = 0;
    unsigned int *msg_i32 = (unsigned int *) malloc ( sizeof ( uint32_t ) * 64 );
    for (int i =0; i < 64; ++i) 
        msg_i32 [i] = 0;

    // analis sha-256
    printf ("Start sha512 \n");
    get_msg_512 (msg, msg_512); 
    get_msg_uint32 (msg_512, msg_i32);


    return 0 ; 
}









