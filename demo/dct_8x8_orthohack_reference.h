/*
Orthohack
Copyright 2016 Russell Leidich
http://orthohack.blogspot.com

This collection of files constitutes the Orthohack Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Orthohack Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Orthohack Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Orthohack Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Using 64-bit double precision for file input.
Using 80-padded-to-96-bit long double precision for internal calculation.
Reading ./demo/dct_8x8/dct_8x8.ohk...
precision_ratio_limit_input=1.0000000010000001E+00
precision_ratio_limit_local=1.0000000010000001E+00
precision_zero_limit_input=9.9999999999999994E-12
sample_count_after_inserting_zero=0000000000000041
coeff_count=0000000000000040
binomial_count=000000000000010E
monomial_count=000000000000014F
number_count_raw=0000000000000242
Globally denoising all numbers in order to reduce the number of uniques...
number_count_consolidated=0000000000000020
magnitude_least_nonzero=9.5150584360891577E-03
magnitude_greatest=2.4142135623730945E+00
Denoising monomial and binomial ratios...
denoising_error_max=5.1803179801357402E-16
Making transform...
transform_idx_max=00000000000002D1
*/
#define LITERAL_LIST \
1.0000000000000000E+00, \
9.5150584360891577E-03, \
2.7096593915592410E-02, \
3.4487422410367882E-02, \
4.0552918602682231E-02, \
7.7164570954363770E-02, \
9.8211869798387780E-02, \
1.3622377669395466E-01, \
1.4698445030241986E-01, \
1.7283542904563623E-01, \
1.7337998066526844E-01, \
2.0387328921222930E-01, \
2.4048494156391087E-01, \
4.1421356237309509E-01, \
2.4142135623730945E+00, \
1.8664458512585656E-02, \
4.5059988875434248E-02, \
4.7835429045636230E-02, \
5.3151880922953539E-02, \
6.7649512518274640E-02, \
7.9547411285802130E-02, \
9.3832569379466324E-02, \
1.1548494156391086E-01, \
1.2500000000000003E-01, \
1.2831999178983419E-01, \
1.6332037060954707E-01, \
1.9204443917785408E-01, \
2.2653186158822197E-01, \
3.6611652351681567E-02, \
8.8388347648318460E-02, \
2.1338834764831843E-01, \
0.0000000000000000E+00

#define NUMBER_LIST \
0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x3F, \
0xD0,0x95,0x6B,0x86,0xA1,0x7C,0x83,0x3F, \
0xE8,0x45,0xCD,0xA2,0x35,0xBF,0x9B,0x3F, \
0xB6,0x71,0xC0,0xDE,0x55,0xA8,0xA1,0x3F, \
0x38,0xCE,0x4F,0x26,0x5A,0xC3,0xA4,0x3F, \
0x4E,0xAB,0xA8,0xAC,0x0E,0xC1,0xB3,0x3F, \
0x2E,0xF3,0xDC,0xC0,0x69,0x24,0xB9,0x3F, \
0x3F,0x48,0xEB,0xDC,0xC7,0x6F,0xC1,0x3F, \
0x1A,0xE3,0x88,0xEF,0x62,0xD0,0xC2,0x3F, \
0x59,0xAA,0xAB,0xA9,0x78,0x1F,0xC6,0x3F, \
0x36,0x85,0x5E,0xB1,0x50,0x31,0xC6,0x3F, \
0x8A,0xE4,0xD8,0x1A,0x85,0x18,0xCA,0x3F, \
0xA4,0x46,0x99,0xE7,0x35,0xC8,0xCE,0x3F, \
0x33,0xEF,0xFC,0x99,0x79,0x82,0xDA,0x3F, \
0xE5,0x9D,0x3F,0x33,0x4F,0x50,0x03,0x40, \
0x36,0xEC,0xA2,0x9B,0xC6,0x1C,0x93,0x3F, \
0x8C,0xFE,0x27,0x55,0x1A,0x12,0xA7,0x3F, \
0x64,0xA9,0xAE,0xA6,0xE2,0x7D,0xA8,0x3F, \
0xD1,0xE7,0x91,0x2C,0xB9,0x36,0xAB,0x3F, \
0x96,0x38,0xDB,0x7B,0x7A,0x51,0xB1,0x3F, \
0x21,0x38,0xF4,0x19,0x38,0x5D,0xB4,0x3F, \
0x4C,0xD2,0xC8,0x48,0x69,0x05,0xB8,0x3F, \
0x47,0x8D,0x32,0xCF,0x6B,0x90,0xBD,0x3F, \
0x01,0x00,0x00,0x00,0x00,0x00,0xC0,0x3F, \
0x93,0x85,0x14,0x1C,0xCA,0x6C,0xC0,0x3F, \
0xFC,0xF0,0x44,0x91,0xAE,0xE7,0xC4,0x3F, \
0xBC,0xE2,0xD2,0x84,0xE9,0x94,0xC8,0x3F, \
0x2A,0xFF,0x82,0xFC,0xFE,0xFE,0xCC,0x3F, \
0x68,0x88,0x01,0x33,0xC3,0xBE,0xA2,0x3F, \
0xCE,0x3B,0x7F,0x66,0x9E,0xA0,0xB6,0x3F, \
0xE6,0x9D,0x3F,0x33,0x4F,0x50,0xCB,0x3F, \
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

#define FIXED_LIST \
0x0000000100000000LL,0x00000000026F9430LL,0x0000000006EFCD68LL, \
0x0000000008D42AEFLL,0x000000000A61AD13LL,0x0000000013C10EACLL, \
0x00000000192469C0LL,0x0000000022DF8FB9LL,0x0000000025A0C5DFLL, \
0x000000002C3EF153LL,0x000000002C62A162LL,0x0000000034310A35LL, \
0x000000003D906BCFLL,0x000000006A09E667LL,0x000000026A09E667LL, \
0x0000000004C731A6LL,0x000000000B890D2ALL,0x000000000C3EF153LL, \
0x000000000D9B5C96LL,0x0000000011517A7BLL,0x00000000145D3819LL, \
0x0000000018056948LL,0x000000001D906BCFLL,0x0000000020000000LL, \
0x0000000020D99438LL,0x0000000029CF5D22LL,0x000000003129D309LL, \
0x0000000039FDFDF9LL,0x00000000095F6199LL,0x0000000016A09E66LL, \
0x0000000036A09E66LL,0x0000000000000000LL

#define CUSTOM_LIST \
0x01000000L,0x00026F94L,0x0006EFCDL,0x0008D42AL,0x000A61ADL, \
0x0013C10EL,0x00192469L,0x0022DF8FL,0x0025A0C5L,0x002C3EF1L, \
0x002C62A1L,0x0034310AL,0x003D906BL,0x006A09E6L,0x026A09E6L, \
0x0004C731L,0x000B890DL,0x000C3EF1L,0x000D9B5CL,0x0011517AL, \
0x00145D38L,0x00180569L,0x001D906BL,0x00200000L,0x0020D994L, \
0x0029CF5DL,0x003129D3L,0x0039FDFDL,0x00095F61L,0x0016A09EL, \
0x0036A09EL,0x00000000L

#define BINOMIAL_LIST \
0x0407040606040805ULL,0x0E04180D04020804ULL,0x04121004140F0416ULL, \
0x2417042616042815ULL,0x1E04381D04221804ULL,0x04322004341F0436ULL, \
0x0407000606000805ULL,0x0E00180D00020800ULL,0x00121000140F0016ULL, \
0x2417002616002815ULL,0x1E00381D00221800ULL,0x00322000341F0036ULL, \
0x4427044626044825ULL,0x2E04582D04422804ULL,0x04523004542F0456ULL, \
0x6437046636046835ULL,0x3E04783D04623804ULL,0x04724004743F0476ULL, \
0x4427004626004825ULL,0x2E00582D00422800ULL,0x00523000542F0056ULL, \
0x6437006636006835ULL,0x3E00783D00623800ULL,0x00724000743F0076ULL, \
0xAC5704A25404A453ULL,0x5C04B45B04AA5804ULL,0x04BA6004BC5F04B2ULL, \
0x9E63049C62049A61ULL,0x6604926504A06404ULL,0x0498680496670494ULL, \
0x8E6B048C6A048A69ULL,0x6E04826D04906C04ULL,0x04887004866F0484ULL, \
0xAC5700A25400A453ULL,0x5C00B45B00AA5800ULL,0x00BA6000BC5F00B2ULL, \
0x9E63009C62009A61ULL,0x6600926500A06400ULL,0x0098680096670094ULL, \
0x8E6B008C6A008A69ULL,0x6E00826D00906C00ULL,0x00887000866F0084ULL, \
0xEC7704E27404E473ULL,0x7C04F47B04EA7804ULL,0x04FA8004FC7F04F2ULL, \
0xEC7700E27400E473ULL,0x7C00F47B00EA7800ULL,0x00FA8000FC7F00F2ULL, \
0x3A9E05369C05329AULL,0xAA054AA9053EA005ULL,0x0550AC054EAB054CULL, \
0x46AF0544AE0542ADULL,0xB2050EB10548B005ULL,0x050CB4050AB30510ULL, \
0x02B70508B60506B5ULL,0x9C01329A0504B805ULL,0x013EA0013A9E0136ULL, \
0x4EAB014CAA014AA9ULL,0xAE0142AD0150AC01ULL,0x0148B00146AF0144ULL, \
0x0AB30110B2010EB1ULL,0xB60106B5010CB401ULL,0x0104B80102B70108ULL, \
0x7ABE0576BC0572BAULL,0xBC0172BA057EC005ULL,0x017EC0017ABE0176ULL, \
0xB6DF05B4DE05B2DDULL,0xE605C6E505B8E005ULL,0x05C4E805C2E705C8ULL, \
0x84EB0586EA0588E9ULL,0xEE05B0ED0582EC05ULL,0x05AAF005ACEF05AEULL, \
0x128F011497051690ULL,0xD47590CC758ECB01ULL,0x758AC9758CCA75A6ULL, \
0xA2D2759ED0759ACEULL,0xDE01B2DD01189575ULL,0x01B8E001B6DF01B4ULL, \
0xC2E701C8E601C6E5ULL,0xEA0188E901C4E801ULL,0x0182EC0184EB0186ULL, \
0xACEF01AEEE01B0EDULL,0x8D05169601AAF001ULL,0x6990CC0112920114ULL, \
0x8CCA698ECB69A6D4ULL,0xD0699ACE698AC969ULL,0x01209669A2D2699EULL, \
0x1691011C95051893ULL,0x94011A97011E9201ULL,0x0520910512980114ULL, \
0x1E98012897051C93ULL,0x8E05249805229605ULL,0x011A940526950118ULL, \
0x2D17062514062313ULL,0x1C06351B062B1806ULL,0x6E5F3105EAF80633ULL, \
0x053076192C763B38ULL,0x1402231301ECF776ULL,0x022B18022D170225ULL, \
0x053002331C02351BULL,0x316A192C6A3B386AULL,0x000000000000725FULL

#define TRANSFORM_STRING \
0xEC5A936512141089ULL,0x873B62F4992CD0A7ULL,0x4B3C29EB14A4F945ULL, \
0xA9165D214E59AD27ULL,0xB63F4892AD0A3EC7ULL,0xC28EB1CA47975871ULL, \
0x65321C659ED234ABULL,0xF4B92DD0E5EC1A97ULL,0x6B04A5F94D852B60ULL, \
0x214A592D2F4B7C39ULL,0x92BD0A1ECBA9565FULL,0xCA5797D870B65F4AULL, \
0x2586D2B4AFC286B2ULL,0xEA4A8C66423C1158ULL,0x8D235274613651E4ULL, \
0x4D9C792A90A33991ULL,0xE16DE25C28558D19ULL,0xCDB84B7B970B2F56ULL, \
0xE78A588B86183C4BULL,0x8E0D1C53C44C36C1ULL,0x1B0DF079628623E1ULL, \
0xA508B06103C31571ULL,0xD185384483681E70ULL,0xDE079428423C18C0ULL, \
0x7D641E24215611A0ULL,0xA920B160480F2B3AULL,0x58D3BC99BD3D5DF3ULL, \
0xCC6BCE9D65DDF9ADULL,0x757E1D5BD4DD2FCFULL,0x39C98BF33AB7A719ULL, \
0xABA97A439F9BD5ADULL,0xA3D57D49D2DCFCFAULL,0x01006EA8EA7E91E7ULL, \
0x14783A82A219808FULL,0x8E606340D41D104DULL,0x434453471E0AA028ULL, \
0xD9D6904890383415ULL,0xBD7ECE3486459FAEULL,0xA8623BA7AAB8AF0CULL, \
0xBE09C10ADDAD6AC7ULL,0x5F54715618AEC8EAULL,0x2FB2BAA78230C4F6ULL, \
0x5433ADAAD5AC0A86ULL,0xEB06A08D6DD6A563ULL,0x9277E45A24AB62B5ULL, \
0x295FD38B25F1236BULL,0x6C48FAE59FE91489ULL,0x62C0201B57FCE248ULL, \
0xF21D163AB9E7F245ULL,0x93F8B0180EB5F93CULL,0x4EEC8BC484AD69EDULL, \
0xEB567BA472246563ULL,0x890B5A939F23D121ULL,0x48EC59DAEC9F7906ULL, \
0x9C5321601356BCF6ULL,0x5A66A4CA2CA6994DULL,0x099619A132532D26ULL, \
0x6D3ED5974B7490CBULL,0x4AD8DB895F1B31AEULL,0x8532DBB4BA54C6FCULL, \
0x25E25C82C86990D9ULL,0xB9417817292E12E5ULL,0xD36C5EDA5DAA0B80ULL, \
0x2B89D897F69362EEULL

#define ALIAS_COUNT_BIT_COUNT 3
#define ALIAS_COUNT_MASK 0x00000007UL
#define BINOMIAL_COUNT 0x0000010EUL
#define BINOMIAL_IDX_MAX 0x0000010DUL
#define BINOMIAL_LIST_U64_COUNT 0x00000066UL
#define COEFF_COUNT 0x00000040UL
#define COEFF_IDX_BIT_COUNT 6
#define COEFF_IDX_MASK 0x0000003FUL
#define COEFF_IDX_MAX 0x0000003FUL
#define MONOMIAL_COUNT 0x0000014FUL
#define MONOMIAL_COUNT_BIT_COUNT 8
#define MONOMIAL_COUNT_MASK 0x000000FFUL
#define MONOMIAL_IDX_BIT_COUNT 9
#define MONOMIAL_IDX_MASK 0x000001FFUL
#define MONOMIAL_IDX_MAX 0x0000014EUL
#define NUMBER_COUNT 0x00000020UL
#define NUMBER_IDX_BIT_COUNT 6
#define NUMBER_IDX_MASK 0x0000003FUL
#define NUMBER_IDX_MAX 0x0000001FUL
#define NUMBER_LIST_SIZE 0x00000100UL
#define ORTHOHACK_DEFINED 0x00000001UL
#define PRECISION_CUSTOM 1
#define PRECISION_FIXED 0
#define PRECISION_INTERNAL 1
#define SAMPLE_IDX_MAX 0x00000040UL
#define TRANSFORM_IDX_MAX 0x000002D1UL
#define TRANSFORM_STRING_U64_COUNT 0x00000040UL
/*
transform_size=000000000000052A
Done.
*/