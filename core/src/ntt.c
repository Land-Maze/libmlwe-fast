#include <lml/ntt.h>
#include <lml/reduce.h>

const int16_t mlwe_fast_zetas[128] = {
    -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
     -171,   622,  1577,   182,   962, -1202, -1474,  1468,
      573, -1325,   264,   383,  -829,  1458, -1602,  -130,
     -681,  1017,   732,   608, -1542,   411,  -205, -1571,
     1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
      516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
     -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
     -398,   961, -1508,  -725,   448, -1065,   677, -1275,
    -1103,   430,   555,   843, -1251,   871,  1550,   105,
      422,   587,   177,  -235,  -291,  -460,  1574,  1653,
     -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
    -1590,   644,  -872,   349,   418,   329,  -156,   -75,
      817,  1097,   603,   610,  1322, -1285, -1465,   384,
    -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
    -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
     -108,  -308,   996,   991,   958, -1460,  1522,  1628
};

void mlwe_fast_ntt(int16_t r[MLWE_FAST_N]) {
    unsigned int len, start, j, k;
    int16_t zeta, t;

    k = 1;
    for (len = (MLWE_FAST_N >> 1); len >= 2; len >>= 1) {
        for (start = 0; start < MLWE_FAST_N; start = j + len) {
            zeta = mlwe_fast_zetas[k++];
            for (j = start; j < start + len; j++) {
                t = mlwe_fast_montgomery_reduce((int32_t)zeta * r[j + len]);
                r[j + len] = r[j] - t;
                r[j] = r[j] + t;
            }
        }
    }
}

void mlwe_fast_invntt(int16_t r[MLWE_FAST_N]) {
    unsigned int len, start, j, k;
    int16_t zeta, t;

    k = (MLWE_FAST_N >> 1) - 1;
    for (len = 2; len <= (MLWE_FAST_N >> 1); len <<= 1) {
        for (start = 0; start < MLWE_FAST_N; start = j + len) {
            zeta = mlwe_fast_zetas[k--];
            for (j = start; j < start + len; j++) {
                t = r[j];
                r[j] = mlwe_fast_barrett_reduce(t + r[j + len]);
                r[j + len] = t - r[j + len];
                r[j + len] = mlwe_fast_montgomery_reduce((int32_t)-zeta * r[j + len]);
            }
        }
    }

    for (j = 0; j < MLWE_FAST_N; j++) {
        int32_t temp = (int32_t)r[j] * MLWE_FAST_INV_N_MONT;
        r[j] = mlwe_fast_montgomery_reduce(temp);
    }
}
