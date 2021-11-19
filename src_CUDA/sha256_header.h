/*
 * BitCracker: BitLocker password cracking tool, CUDA version.
 * Copyright (C) 2013-2017  Elena Ago <elena dot ago at gmail dot com>
 *							Massimo Bernaschi <massimo dot bernaschi at gmail dot com>
 * 
 * This file is part of the BitCracker project: https://github.com/e-ago/bitcracker
 * 
 * BitCracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * BitCracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BitCracker. If not, see <http://www.gnu.org/licenses/>.
 */

#define ROR(x, i) (((x) << (32 - (i))) | ((x) >> (i)))

#define BLOCK_SIZE 64

#define ROR7(x) (((x) << 25) | ((x) >> 7))
#define ROR18(x) (((x) << 14) | ((x) >> 18))

#define ROR17(x) (((x) << 15) | ((x) >> 17))
#define ROR19(x) (((x) << 13) | ((x) >> 19))


#if (__CUDA_ARCH__ >= 500)
		static __device__ __forceinline__ uint32_t IADD3(uint32_t a, uint32_t b, uint32_t c) {
			uint32_t d;
			asm("iadd3 %0, %1, %2, %3;" : "=r"(d) : "r"(a), "r"(b), "r"(c));
			return d;
		}

		static __device__ __forceinline__ uint32_t LOP3LUT_XOR(uint32_t a, uint32_t b, uint32_t c) {
			uint32_t d;
			asm("lop3.b32 %0, %1, %2, %3, 0x96;" : "=r"(d) : "r"(a), "r"(b), "r"(c));
			return d;
		}

		static __device__ __forceinline__ uint32_t LOP3LUT_XORAND(uint32_t a, uint32_t b, uint32_t c) {
			uint32_t d;
			asm("lop3.b32 %0, %1, %2, %3, 0xb8;" : "=r"(d) : "r"(a), "r"(b), "r"(c));
			return d;
		}

		static __device__ __forceinline__ uint32_t LOP3LUT_ANDOR(uint32_t a, uint32_t b, uint32_t c) {
			uint32_t d;
			asm("lop3.b32 %0, %1, %2, %3, 0xe8;" : "=r"(d) : "r"(a), "r"(b), "r"(c));
			return d;
		}
#else

        static __device__ __forceinline__ uint32_t IADD3(uint32_t a, uint32_t b, uint32_t c) {
            return a+b+c;
        }

        static __device__ __forceinline__ uint32_t LOP3LUT_XOR(uint32_t a, uint32_t b, uint32_t c) {
            return a^b^c;
        }

        static __device__ __forceinline__ uint32_t LOP3LUT_XORAND(uint32_t g, uint32_t e, uint32_t f) {
            return (g ^ (e & (f ^ g)));
        }

        static __device__ __forceinline__ uint32_t LOP3LUT_ANDOR(uint32_t a, uint32_t b, uint32_t c) {
            return  ((a & (b | c)) | (b & c));
        }
#endif

#define SCHEDULE0()  \
		schedule0 = schedule16 + schedule25 \
			+ LOP3LUT_XOR(ROR7(schedule17) , ROR18(schedule17) , (schedule17 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule30) , ROR19(schedule30) , (schedule30 >> 10));


#define SCHEDULE1()  \
		schedule1 = schedule17 + schedule26 \
			+ LOP3LUT_XOR(ROR7(schedule18) , ROR18(schedule18) , (schedule18 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule31) , ROR19(schedule31) , (schedule31 >> 10));


#define SCHEDULE2()  \
		schedule2 = schedule18 + schedule27 \
			+ LOP3LUT_XOR(ROR7(schedule19) , ROR18(schedule19) , (schedule19 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule0) , ROR19(schedule0) , (schedule0 >> 10));


#define SCHEDULE3()  \
		schedule3 = schedule19 + schedule28 \
			+ LOP3LUT_XOR(ROR7(schedule20) , ROR18(schedule20) , (schedule20 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule1) , ROR19(schedule1) , (schedule1 >> 10));


#define SCHEDULE4()  \
		schedule4 = schedule20 + schedule29 \
			+ LOP3LUT_XOR(ROR7(schedule21) , ROR18(schedule21) , (schedule21 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule2) , ROR19(schedule2) , (schedule2 >> 10));


#define SCHEDULE5()  \
		schedule5 = schedule21 + schedule30 \
			+ LOP3LUT_XOR(ROR7(schedule22) , ROR18(schedule22) , (schedule22 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule3) , ROR19(schedule3) , (schedule3 >> 10));


#define SCHEDULE6()  \
		schedule6 = schedule22 + schedule31 \
			+ LOP3LUT_XOR(ROR7(schedule23) , ROR18(schedule23) , (schedule23 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule4) , ROR19(schedule4) , (schedule4 >> 10));


#define SCHEDULE7()  \
		schedule7 = schedule23 + schedule0 \
			+ LOP3LUT_XOR(ROR7(schedule24) , ROR18(schedule24) , (schedule24 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule5) , ROR19(schedule5) , (schedule5 >> 10));


#define SCHEDULE8()  \
		schedule8 = schedule24 + schedule1 \
			+ LOP3LUT_XOR(ROR7(schedule25) , ROR18(schedule25) , (schedule25 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule6) , ROR19(schedule6) , (schedule6 >> 10));


#define SCHEDULE9()  \
		schedule9 = schedule25 + schedule2 \
			+ LOP3LUT_XOR(ROR7(schedule26) , ROR18(schedule26) , (schedule26 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule7) , ROR19(schedule7) , (schedule7 >> 10));


#define SCHEDULE10()  \
		schedule10 = schedule26 + schedule3 \
			+ LOP3LUT_XOR(ROR7(schedule27) , ROR18(schedule27) , (schedule27 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule8) , ROR19(schedule8) , (schedule8 >> 10));


#define SCHEDULE11()  \
		schedule11 = schedule27 + schedule4 \
			+ LOP3LUT_XOR(ROR7(schedule28) , ROR18(schedule28) , (schedule28 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule9) , ROR19(schedule9) , (schedule9 >> 10));


#define SCHEDULE12()  \
		schedule12 = schedule28 + schedule5 \
			+ LOP3LUT_XOR(ROR7(schedule29) , ROR18(schedule29) , (schedule29 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule10) , ROR19(schedule10) , (schedule10 >> 10));


#define SCHEDULE13()  \
		schedule13 = schedule29 + schedule6 \
			+ LOP3LUT_XOR(ROR7(schedule30) , ROR18(schedule30) , (schedule30 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule11) , ROR19(schedule11) , (schedule11 >> 10));


#define SCHEDULE14()  \
		schedule14 = schedule30 + schedule7 \
			+ LOP3LUT_XOR(ROR7(schedule31) , ROR18(schedule31) , (schedule31 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule12) , ROR19(schedule12) , (schedule12 >> 10));


#define SCHEDULE15()  \
		schedule15 = schedule31 + schedule8 \
			+ LOP3LUT_XOR(ROR7(schedule0) , ROR18(schedule0) , (schedule0 >> 3)) \
			+ LOP3LUT_XOR(ROR17(schedule13) , ROR19(schedule13) , (schedule13 >> 10));

#define SCHEDULE16()  \
		schedule16 = schedule0 + schedule9  \
			+ LOP3LUT_XOR( ROR7(schedule1), ROR18(schedule1), (schedule1 >> 3))  \
			+ LOP3LUT_XOR( ROR17(schedule14), ROR19(schedule14), (schedule14 >> 10));

#define SCHEDULE17()  \
		schedule17 = schedule1 + schedule10  \
			+ LOP3LUT_XOR(ROR7(schedule2) , ROR18(schedule2) , (schedule2 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule15) , ROR19(schedule15) , (schedule15 >> 10));

#define SCHEDULE18()  \
		schedule18 = schedule2 + schedule11  \
			+ LOP3LUT_XOR(ROR7(schedule3) ,ROR18(schedule3) ,(schedule3 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule16), ROR19(schedule16), (schedule16 >> 10));
#define SCHEDULE19()  \
		schedule19 = schedule3 + schedule12  \
			+ LOP3LUT_XOR(ROR7(schedule4) , ROR18(schedule4) , (schedule4 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule17) , ROR19(schedule17) , (schedule17 >> 10));

#define SCHEDULE20()  \
		schedule20 = schedule4 + schedule13  \
			+ LOP3LUT_XOR(ROR7(schedule5) , ROR18(schedule5) , (schedule5 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule18) , ROR19(schedule18) , (schedule18 >> 10));

#define SCHEDULE21()  \
		schedule21 = schedule5 + schedule14  \
			+ LOP3LUT_XOR(ROR7(schedule6) , ROR18(schedule6) , (schedule6 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule19) , ROR19(schedule19) , (schedule19 >> 10));

#define SCHEDULE22()  \
		schedule22 = schedule6 + schedule15  \
			+ LOP3LUT_XOR(ROR7(schedule7) , ROR18(schedule7) , (schedule7 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule20) , ROR19(schedule20) , (schedule20 >> 10));

#define SCHEDULE23()  \
		schedule23 = schedule7 + schedule16  \
			+ LOP3LUT_XOR(ROR7(schedule8) , ROR18(schedule8) , (schedule8 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule21) , ROR19(schedule21) , (schedule21 >> 10));

#define SCHEDULE24()  \
		schedule24 = schedule8 + schedule17  \
			+ LOP3LUT_XOR(ROR7(schedule9) , ROR18(schedule9) , (schedule9 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule22) , ROR19(schedule22) , (schedule22 >> 10));

#define SCHEDULE25()  \
		schedule25 = schedule9 + schedule18  \
			+ LOP3LUT_XOR(ROR7(schedule10) , ROR18(schedule10) , (schedule10 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule23) , ROR19(schedule23) , (schedule23 >> 10));

#define SCHEDULE26()  \
		schedule26 = schedule10 + schedule19  \
			+ LOP3LUT_XOR(ROR7(schedule11) , ROR18(schedule11) , (schedule11 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule24) , ROR19(schedule24) , (schedule24 >> 10));

#define SCHEDULE27()  \
		schedule27 = schedule11 + schedule20  \
			+ LOP3LUT_XOR(ROR7(schedule12) , ROR18(schedule12) , (schedule12 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule25) , ROR19(schedule25) , (schedule25 >> 10));

#define SCHEDULE28()  \
		schedule28 = schedule12 + schedule21  \
			+ LOP3LUT_XOR(ROR7(schedule13) , ROR18(schedule13) , (schedule13 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule26) , ROR19(schedule26) , (schedule26 >> 10));

#define SCHEDULE29()  \
		schedule29 = schedule13 + schedule22  \
			+ LOP3LUT_XOR(ROR7(schedule14) , ROR18(schedule14) , (schedule14 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule27) , ROR19(schedule27) , (schedule27 >> 10));

#define SCHEDULE30()  \
		schedule30 = schedule14 + schedule23  \
			+ LOP3LUT_XOR(ROR7(schedule15) , ROR18(schedule15) , (schedule15 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule28) , ROR19(schedule28) , (schedule28 >> 10));

#define SCHEDULE31()  \
		schedule31 = schedule15 + schedule24  \
			+ LOP3LUT_XOR(ROR7(schedule16) , ROR18(schedule16) , (schedule16 >> 3))  \
			+ LOP3LUT_XOR(ROR17(schedule29) , ROR19(schedule29) , (schedule29 >> 10));

#define ALL_SCHEDULE32() \
		SCHEDULE0() \
		SCHEDULE1() \
		SCHEDULE2() \
		SCHEDULE3() \
		SCHEDULE4() \
		SCHEDULE5() \
		SCHEDULE6() \
		SCHEDULE7() \
		SCHEDULE8() \
		SCHEDULE9() \
		SCHEDULE10() \
		SCHEDULE11() \
		SCHEDULE12() \
		SCHEDULE13() \
		SCHEDULE14() \
		SCHEDULE15() \
		SCHEDULE16() \
		SCHEDULE17() \
		SCHEDULE18() \
		SCHEDULE19() \
		SCHEDULE20() \
		SCHEDULE21() \
		SCHEDULE22() \
		SCHEDULE23() \
		SCHEDULE24() \
		SCHEDULE25() \
		SCHEDULE26() \
		SCHEDULE27() \
		SCHEDULE28() \
		SCHEDULE29() \
		SCHEDULE30() \
		SCHEDULE31() 

#define ALL_SCHEDULE_LAST16() \
		SCHEDULE16() \
		SCHEDULE17() \
		SCHEDULE18() \
		SCHEDULE19() \
		SCHEDULE20() \
		SCHEDULE21() \
		SCHEDULE22() \
		SCHEDULE23() \
		SCHEDULE24() \
		SCHEDULE25() \
		SCHEDULE26() \
		SCHEDULE27() \
		SCHEDULE28() \
		SCHEDULE29() \
		SCHEDULE30() \
		SCHEDULE31() 

#define ROR6(x) (((x) << 26) | ((x) >> 6))
#define ROR11(x) (((x) << 21) | ((x) >> 11))
#define ROR25(x) (((x) << 7) | ((x) >> 25))

#define ROR2(x) (((x) << 30) | ((x) >> 2))
#define ROR13(x) (((x) << 19) | ((x) >> 13))
#define ROR22(x) (((x) << 10) | ((x) >> 22))

#define ROUND(a, b, c, d, e, f, g, h, W, k) \
		h += LOP3LUT_XOR(ROR6(e), ROR11(e), ROR25(e)) + LOP3LUT_XORAND(g,e,f) + k + W; \
		d += h;  \
		h += LOP3LUT_XOR(ROR2(a), ROR13(a), ROR22(a)) + LOP3LUT_ANDOR(a,b,c);

//(tex1D<uint32_t>(texObj_blocks,( indexW+i)));

#define ROUND_SECOND_BLOCK(a, b, c, d, e, f, g, h, i, k, block_w) \
		h += LOP3LUT_XOR(ROR6(e), ROR11(e), ROR25(e)) + LOP3LUT_XORAND(g,e,f) + k + block_w; \
		d += h;  \
		h += LOP3LUT_XOR(ROR2(a), ROR13(a), ROR22(a)) + LOP3LUT_ANDOR(a,b,c);

#define ROUND_SECOND_BLOCK_CONST(a, b, c, d, e, f, g, h, i, k, w) \
		h += LOP3LUT_XOR(ROR6(e), ROR11(e), ROR25(e)) + LOP3LUT_XORAND(g,e,f) + k +  w; \
		d += h;  \
		h += LOP3LUT_XOR(ROR2(a), ROR13(a), ROR22(a)) + LOP3LUT_ANDOR(a,b,c);

#define LOADSCHEDULE_WPRE(i, j)  \
		w_blocks_d[j] =                           \
			  (uint32_t)block[i * 4 + 0] << 24  \
			| (uint32_t)block[i * 4 + 1] << 16  \
			| (uint32_t)block[i * 4 + 2] <<  8  \
			| (uint32_t)block[i * 4 + 3];
	
#define SCHEDULE_WPRE(i)  \
		w_blocks_d[i] = w_blocks_d[i - 16] + w_blocks_d[i - 7]  \
			+ (ROR(w_blocks_d[i - 15], 7) ^ ROR(w_blocks_d[i - 15], 18) ^ (w_blocks_d[i - 15] >> 3))  \
			+ (ROR(w_blocks_d[i - 2], 17) ^ ROR(w_blocks_d[i - 2], 19) ^ (w_blocks_d[i - 2] >> 10));


#define ALL_ROUND_B1_1() \
		ROUND(a, b, c, d, e, f, g, h,  schedule0, 0x428A2F98) \
		ROUND(h, a, b, c, d, e, f, g,  schedule1, 0x71374491) \
		ROUND(g, h, a, b, c, d, e, f,  schedule2, 0xB5C0FBCF) \
		ROUND(f, g, h, a, b, c, d, e,  schedule3, 0xE9B5DBA5) \
		ROUND(e, f, g, h, a, b, c, d,  schedule4, 0x3956C25B) \
		ROUND(d, e, f, g, h, a, b, c,  schedule5, 0x59F111F1) \
		ROUND(c, d, e, f, g, h, a, b,  schedule6, 0x923F82A4) \
		ROUND(b, c, d, e, f, g, h, a,  schedule7, 0xAB1C5ED5) \
		ROUND(a, b, c, d, e, f, g, h,  schedule8, 0xD807AA98) \
		ROUND(h, a, b, c, d, e, f, g,  schedule9, 0x12835B01) \
		ROUND(g, h, a, b, c, d, e, f, schedule10, 0x243185BE) \
		ROUND(f, g, h, a, b, c, d, e, schedule11, 0x550C7DC3) \
		ROUND(e, f, g, h, a, b, c, d, schedule12, 0x72BE5D74) \
		ROUND(d, e, f, g, h, a, b, c, schedule13, 0x80DEB1FE) \
		ROUND(c, d, e, f, g, h, a, b, schedule14, 0x9BDC06A7) \
		ROUND(b, c, d, e, f, g, h, a, schedule15, 0xC19BF174) \
		ROUND(a, b, c, d, e, f, g, h, schedule16, 0xE49B69C1) \
		ROUND(h, a, b, c, d, e, f, g, schedule17, 0xEFBE4786) \
		ROUND(g, h, a, b, c, d, e, f, schedule18, 0x0FC19DC6) \
		ROUND(f, g, h, a, b, c, d, e, schedule19, 0x240CA1CC) \
		ROUND(e, f, g, h, a, b, c, d, schedule20, 0x2DE92C6F) \
		ROUND(d, e, f, g, h, a, b, c, schedule21, 0x4A7484AA) \
		ROUND(c, d, e, f, g, h, a, b, schedule22, 0x5CB0A9DC) \
		ROUND(b, c, d, e, f, g, h, a, schedule23, 0x76F988DA) \
		ROUND(a, b, c, d, e, f, g, h, schedule24, 0x983E5152) \
		ROUND(h, a, b, c, d, e, f, g, schedule25, 0xA831C66D) \
		ROUND(g, h, a, b, c, d, e, f, schedule26, 0xB00327C8) \
		ROUND(f, g, h, a, b, c, d, e, schedule27, 0xBF597FC7) \
		ROUND(e, f, g, h, a, b, c, d, schedule28, 0xC6E00BF3) \
		ROUND(d, e, f, g, h, a, b, c, schedule29, 0xD5A79147) \
		ROUND(c, d, e, f, g, h, a, b, schedule30, 0x06CA6351) \
		ROUND(b, c, d, e, f, g, h, a, schedule31, 0x14292967) 


#define ALL_ROUND_B1_2() \
	ROUND(a, b, c, d, e, f, g, h, schedule0, 0x27B70A85) \
	ROUND(h, a, b, c, d, e, f, g, schedule1, 0x2E1B2138) \
	ROUND(g, h, a, b, c, d, e, f, schedule2, 0x4D2C6DFC) \
	ROUND(f, g, h, a, b, c, d, e, schedule3, 0x53380D13) \
	ROUND(e, f, g, h, a, b, c, d, schedule4, 0x650A7354) \
	ROUND(d, e, f, g, h, a, b, c, schedule5, 0x766A0ABB) \
	ROUND(c, d, e, f, g, h, a, b, schedule6, 0x81C2C92E) \
	ROUND(b, c, d, e, f, g, h, a, schedule7, 0x92722C85) \
	ROUND(a, b, c, d, e, f, g, h, schedule8, 0xA2BFE8A1) \
	ROUND(h, a, b, c, d, e, f, g, schedule9, 0xA81A664B) \
	ROUND(g, h, a, b, c, d, e, f, schedule10, 0xC24B8B70) \
	ROUND(f, g, h, a, b, c, d, e, schedule11, 0xC76C51A3) \
	ROUND(e, f, g, h, a, b, c, d, schedule12, 0xD192E819) \
	ROUND(d, e, f, g, h, a, b, c, schedule13, 0xD6990624) \
	ROUND(c, d, e, f, g, h, a, b, schedule14, 0xF40E3585) \
	ROUND(b, c, d, e, f, g, h, a, schedule15, 0x106AA070) \
	ROUND(a, b, c, d, e, f, g, h, schedule16, 0x19A4C116) \
	ROUND(h, a, b, c, d, e, f, g, schedule17, 0x1E376C08) \
	ROUND(g, h, a, b, c, d, e, f, schedule18, 0x2748774C) \
	ROUND(f, g, h, a, b, c, d, e, schedule19, 0x34B0BCB5) \
	ROUND(e, f, g, h, a, b, c, d, schedule20, 0x391C0CB3) \
	ROUND(d, e, f, g, h, a, b, c, schedule21, 0x4ED8AA4A) \
	ROUND(c, d, e, f, g, h, a, b, schedule22, 0x5B9CCA4F) \
	ROUND(b, c, d, e, f, g, h, a, schedule23, 0x682E6FF3) \
	ROUND(a, b, c, d, e, f, g, h, schedule24, 0x748F82EE) \
	ROUND(h, a, b, c, d, e, f, g, schedule25, 0x78A5636F) \
	ROUND(g, h, a, b, c, d, e, f, schedule26, 0x84C87814) \
	ROUND(f, g, h, a, b, c, d, e, schedule27, 0x8CC70208) \
	ROUND(e, f, g, h, a, b, c, d, schedule28, 0x90BEFFFA) \
	ROUND(d, e, f, g, h, a, b, c, schedule29, 0xA4506CEB) \
	ROUND(c, d, e, f, g, h, a, b, schedule30, 0xBEF9A3F7) \
	ROUND(b, c, d, e, f, g, h, a, schedule31, 0xC67178F2) 

