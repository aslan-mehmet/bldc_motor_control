#include "morse.h"
#include "bits.h"
#include "f4nucleo_board.h"
#include "delay.h"

/* number of symbols in a letter */
#define N1 ((uint8_t) 1)
#define N2 ((uint8_t) 2)
#define N3 ((uint8_t) 3)
#define N4 ((uint8_t) 4)
#define N5 ((uint8_t) 5)

#define NUMBER_MASK (BIT8_0|BIT8_1|BIT8_2)
#define UNIT_DELAY 100 /* in ms */

/* read left to right, 1=dash 0=dot */
#define MA (BIT8_6|N2)
#define MB (BIT8_7|N4)
#define MC (BIT8_7|BIT8_5|N4)
#define MD (BIT8_7|N3)
#define ME (N1)
#define MF (BIT8_5|N4)
#define MG (BIT8_7|BIT8_6|N3)
#define MH (N4)
#define MI (N2)
#define MJ (BIT8_6|BIT8_5|BIT8_4|N4)
#define MK (BIT8_7|BIT8_5|N3)
#define ML (BIT8_6|N4)
#define MM (BIT8_7|BIT8_6|N2)
#define MN (BIT8_7|N2)
#define MO (BIT8_7|BIT8_6|BIT8_5|N3)
#define MP (BIT8_6|BIT8_5|N4)
#define MQ (BIT8_7|BIT8_6|BIT8_4|N4)
#define MR (BIT8_6|N3)
#define MS (N3)
#define MT (BIT8_7|N1)
#define MU (BIT8_5|N3)
#define MV (BIT8_4|N4)
#define MW (BIT8_6|BIT8_5|N3)
#define MX (BIT8_7|BIT8_4|N4)
#define MY (BIT8_7|BIT8_5|BIT8_4|N4)
#define MZ (BIT8_7|BIT8_6|N4)
#define M1 (BIT8_6|BIT8_5|BIT8_4|BIT8_3|N5)
#define M2 (BIT8_5|BIT8_4|BIT8_3|N5)
#define M3 (BIT8_4|BIT8_3|N5)
#define M4 (BIT8_3|N5)
#define M5 (N5)
#define M6 (BIT8_7|N5)
#define M7 (BIT8_7|BIT8_6|N5)
#define M8 (BIT8_7|BIT8_6|BIT8_5|N5)
#define M9 (BIT8_7|BIT8_6|BIT8_5|BIT8_4|N5)
#define M0 (BIT8_7|BIT8_6|BIT8_5|BIT8_4|BIT8_3|N5)

#define buz_on() LED2_PORT->ODR |= LED2_ODR
#define buz_off() LED2_PORT->ODR &= ~LED2_ODR

static uint8_t _letters[] = {MA, MB, MC, MD, ME, MF, MG, MH, MI, MJ, MK, ML, MM,
                             MN, MO, MP, MQ, MR, MS, MT, MU, MV, MW, MX, MY, MZ};

static uint8_t _numbers[] = {M0, M1, M2, M3, M4, M5, M6, M7, M8, M9};

static void buz_letter(uint8_t c)
{
        if (c >= 97 && c <= 122) { /* lowercase ascii letters */
                c = _letters[c - 97];
        } else if (c >= 48 && c <= 57) { /* numbers */
                c = _numbers[c - 48];
        } else { /* undefined */
                return;
        }

        uint8_t n_symbols = c & NUMBER_MASK;

        for (uint8_t i = 0; i < n_symbols; ++i) {
                buz_on();
                if (c & BIT8_7) { /* dash */
                        delay(UNIT_DELAY * 3);
                } else { /* dot */
                        delay(UNIT_DELAY);
                }
                buz_off();

                c = c << 1;

                /* letter not end, separate between parts of same letter */
                if (i != n_symbols - 1) {
                        delay(UNIT_DELAY);
                }
        }
}

void buz_word(char *p)
{
        for (uint8_t i = 0; p[i] != '\0'; ++i) {
                buz_letter((uint8_t) p[i]);

                /* word not end, separate letters */
                if (p[i+1] != '\0') {
                        delay(UNIT_DELAY * 3);
                }
        }

        /* word end, separate words */
        delay(UNIT_DELAY * 7);
}


