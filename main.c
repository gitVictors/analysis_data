#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


//==== константы и пеерменные ==================================================

// само сообщение для шифрования
unsigned char * msg = "hello worll";

unsigned int mass_for_commpres [64];

struct mss512
{

    uint8_t* mass_512;
    uint8_t one; 
    uint64_t cnt;

};

struct mss512 msg_512;


//Инициализация начальных hash значенией
const unsigned int sha256_init_state[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

//Инициализация массива округленных констант
const uint32_t sha256_consts[] = {
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

static void write_u64(uint8_t* dest, uint64_t x)
{
    *dest++ = (x >> 56) & 0xff;
    *dest++ = (x >> 48) & 0xff;
    *dest++ = (x >> 40) & 0xff;
    *dest++ = (x >> 32) & 0xff;
    *dest++ = (x >> 24) & 0xff;
    *dest++ = (x >> 16) & 0xff;
    *dest++ = (x >> 8) & 0xff;
    *dest++ = (x >> 0) & 0xff;
}

//Расширяем одно сообщеение с [0..15] до [0..63]
static void extention_mss (const unsigned char* msg, unsigned char *msg_512 , int cnt) {
    msg_512 = (unsigned char*) malloc (512/8); 
    memcpy (msg_512, msg, cnt);
}

//Функция формирования очереди по 512 бит или 16 слов
//Предворительная обработка входного сообщения. кратное 512 бит
void  get_msg_512 (const char *msg, struct mss512  *msg_512) {

    int cnt; //
    uint64_t tmp;
   // uint64_t res;

    cnt = strlen (msg);
    memcpy ( msg_512->mass_512, msg, cnt );
    msg_512->one = (unsigned char) 0x80U;//add 0x1 but mast in big-endian
    //memcpy ( &msg_512->mass_512[cnt], &msg_512->one, 1 );
    printf ( "%x %x %x %x \n\r", msg_512->mass_512[0],  msg_512->mass_512[1] ,  msg_512->mass_512[2] ,  msg_512->mass_512[3] );
    //Длинна сообщения в битах. 
    tmp = cnt * 8;
    msg_512->cnt = tmp; 
    
}

//===================================================================================

//преобразование uint8_t массива в массив uint32_t big endian
static uint32_t read_u32 ( uint8_t *src){
    
    unsigned int res;
    
    res = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];

    return res;
}

//преобразование uint8_t массива в массив uint64 big endian
static uint64_t read_u64 ( uint8_t * src) {

    uint64_t res;

    res = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | (src[3] << 0) ; 
    res = ( res << 32)  |  (src[4] << 24) | (src[5] << 16) | (src[6] << 8) | (src[7] << 0);

    return res;
    
}

//John Regehr's https://blog.regehr.org/archives/1063
static uint32_t ror(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (-n & 31));
}

// SHA-2 uses big-endian integers.
static void write_u32(uint8_t* dest, uint32_t x)
{
    *dest++ = (x >> 24) & 0xff;
    *dest++ = (x >> 16) & 0xff;
    *dest++ = (x >> 8) & 0xff;
    *dest++ = (x >> 0) & 0xff; 
}

//созданеи масс 64 слова. типа uint32_t
//из массива 64 слова топа uint8_t . [0...15]
//остальные слоова [16..64] получаем из свретки 
void  get_msg_uint32 ( struct mss512  *msg_512, uint32_t* msg_i32) {
    
    //бежим по массиву 64 байта (64/4 = 16)
    for (int i = 0; i <  16 ; ++i) {
       // msg_i32[i] = read_u32(&msg_512->mass_512[i * 4]);
        msg_i32[i] = *(uint32_t*) &msg_512->mass_512[i*4];
    } 
    //Добавляем единицу в последний свободный байт 0x80.
    uint8_t* pfbt = (uint8_t*) &msg_i32[0]; //нужен авто перещет 
    //printf ("msg_i32 0 = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));
    pfbt = pfbt + 11;
    *pfbt =(uint8_t) 0x80U;
    //debug
   // pfbt =  (uint8_t*) &msg_i32[1];
   // printf ("msg_i32 1 = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));
    //printf (" msg_512 = %x \n\r", msg_512->mass_512[10]);
   // printf (" msg_i32 = %x \n\r", msg_i32[2]);
    
    //добаляю число бит сообщения в i64 слово
    uint64_t* pcnt64 = (uint64_t*) &msg_i32 [14]; 
    *pcnt64 = read_u64((uint8_t*) &msg_512->cnt);
    //debug
    pfbt =  (uint8_t*) &msg_i32[14];
    printf ("msg_i32 = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));
    pfbt =  (uint8_t*) &msg_i32[15];
    printf ("msg_i32 = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));

    uint32_t tmp1 = read_u32 ( (uint8_t*) &msg_i32 [1] ); 
    pfbt =  (uint8_t*) &tmp1;
    printf ("tmp1 = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));
    uint32_t tmp1_rt = ror(tmp1, 7); //tmp1 >> 8; // ror(tmp1, 7);
    pfbt =  (uint8_t*) &tmp1_rt;
    printf ("tmp1_rt = %x %x %x %x \n\r", *pfbt, *(pfbt+1), *(pfbt+2), *(pfbt+3));

    //бежим по массиву i32 и заполняем оставшиеся индексы
    for (int i = 16; i != 64; ++i) {
        uint32_t  w15 = read_u32 ( (uint8_t*) &msg_i32 [i - 15] );
        uint32_t  w2 = read_u32 ((uint8_t*) &msg_i32 [i - 2]);
        uint32_t  s0  = ror(w15, 7) ^ ror(w15, 18) ^ (w15 >> 3);
        uint32_t  s1  = ror(w2, 17) ^ ror(w2, 19) ^ (w2 >> 10);
        uint32_t res = read_u32 (( uint8_t*) &msg_i32 [i - 16] ) + s0 + read_u32 ( (uint8_t*) &msg_i32 [i - 7] ) + s1;
        msg_i32 [i] = read_u32 ((uint8_t*) &res);  // меняем обратно на LE, что бы в памяти было удобно читать.
    }

}

//==============================================================================

//======Основной цикл сжатия====================================================

void sequence_compression ( uint32_t msg [], uint32_t hash256 []) {

    uint32_t a = 0x6a09e667;
    uint32_t b = 0xbb67ae85;
    uint32_t c = 0x3c6ef372;
    uint32_t d = 0xa54ff53a;
    uint32_t e = 0x510e527f;
    uint32_t f = 0x9b05688c;
    uint32_t g = 0x1f83d9ab;
    uint32_t h = 0x5be0cd19;

    uint32_t h1 = 0x6a09e667;
    uint32_t h2 = 0xbb67ae85;
    uint32_t h3 = 0x3c6ef372;
    uint32_t h4 = 0xa54ff53a;
    uint32_t h5 = 0x510e527f;
    uint32_t h6 = 0x9b05688c;
    uint32_t h7 = 0x1f83d9ab;
    uint32_t h8 = 0x5be0cd19;


    uint32_t S0, S1;
    uint32_t ch;
    uint32_t tmp1;
    uint32_t mjr;
    uint32_t tmp2;

    for (int i = 0; i != 64; ++i) {

        S1 = ror(e , 6) ^ ror(e, 11) ^ ror(e, 25);
        ch = (e & f) ^ (~e & g);
        tmp1 = h + S1 + ch +  sha256_consts[i] + read_u32( ( uint8_t* ) &msg[i] ); 
        S0 = ror(a, 2)  ^ ror (a, 13) ^ ror(a, 22);
        mjr = (a & b) ^ (a & c) ^ (b & c);
        tmp2 = S0 + mjr;

        h = g;
        g = f;
        f = e; 
        e = d + tmp1;
        d = c;
        c = b;
        b = a;
        a = tmp1 + tmp2;

    }

    h1 += a;
    h2 += b;
    h3 += c;
    h4 += d;
    h5 += e;
    h6 += f;
    h7 += g;
    h8 += h;


    hash256 [0] = h1;
    hash256 [1] = h2;
    hash256 [2] = h3;
    hash256 [3] = h4;
    hash256 [4] = h5;
    hash256 [5] = h6;
    hash256 [6] = h7;
    hash256 [7] = h8;

    printf ("a, b, c, d, e, f, g, h\n");
    printf ("%x %x %x %x %x %x %x %x\n", a, b, c, d, e, f, g, h);
    printf ("hash \n");
    printf ("%x %x %x %x %x %x %x %x\n", hash256[0] , hash256[1], hash256[2], hash256[3] , hash256[4], hash256[5], \
            hash256 [6] , hash256 [7] );

}
//==============================================================================

int main ( int argc, char* argv[]){

    msg_512.mass_512 = malloc ( 512/8 ); // 512 bit
    for (int i =0; i < 64; ++i) 
       msg_512.mass_512[i]  = 0;
    unsigned int *msg_i32 = (unsigned int *) malloc ( sizeof ( uint32_t ) * 64 );
    for (int i =0; i < 64; ++i) 
        msg_i32 [i] = 0;

    uint32_t  *hash256 = (unsigned int *) malloc ( sizeof ( uint32_t ) * 8 );
    for(int i = 0; i < 8; ++i)
        hash256 [i] = 0;
   

    // analis sha-256
    printf ("Start sha512 \n");
    get_msg_512 (msg, &msg_512); 
    get_msg_uint32 (&msg_512, msg_i32);
    sequence_compression (msg_i32, hash256);
/*
    for (int i = 0; i < 8; ++i) {
        printf ("%8x ", hash256[i] );
    }
    printf ("\n\r");
*/
    return 0 ; 
}



