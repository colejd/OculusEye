//
//  OFCVBridge.cpp
//  OculusEye
//
//  Created by Jonathan Cole on 9/22/14.
//
//

#include "YUVBuffer.h"

/*
//These functions optimize well, but are outdone by the matching macros (in OFCVBridge.h) on both debug and release.
inline const int _max(const int a, const int b){
    return ((a) > (b)) ? (a) : (b);
    
}

inline const uint8_t _saturate(const int v){
    return static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0);
}
 */

void yuv422_to_rgba(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height)
{
    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 4) * j; // 4 channels
        
        for (int i = 0; i < 2 * width; i += 4, row += 8)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;
            
            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;
            
            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            row[3]      = (0xff);
            
            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[6-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[5]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[4+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            row[7]      = (0xff);
        }
    }
}

/**
 * Converts a YUV422 array (yuv_src) to a RGB888 array (dst).
 * yuv_src is a flattened 2D array.
 *
 * <params>
 * stride: width of yuv_src row
 * width: width of dst row
 * height: height of dst column
 */
void yuv422_to_rgb(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height)
{
    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        //Point to the first element of the current row (specified by "height") in the flattened 2D array "dst".
        uint8_t* row = dst + (width * 3) * j; // 3 channels
        
        //Iterate by 4 in yuv_src (4 channel), iterate by 3 twice in dst (3 channel)
        for (int i = 0; i < 2 * width; i += 4, row += 6)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;
            
            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;
            
            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            //row[3]      = (0xff);
            
            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[5-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[4]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[3+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            //row[7]      = (0xff);
        }
    }
}

void rgba_to_rgb(const uint8_t *src, uint8_t *dest, const int srcLength, const int destLength){
    int j=0;
    for(int i=0; i<srcLength; i+=4) {
        dest[j] = src[i];
        dest[j+1] = src[i+1];
        dest[j+2] = src[i+2];
        j += 3;
    }
}

YUVBuffer::YUVBuffer(){
    
}

YUVBuffer::~YUVBuffer(){
    
}

void YUVBuffer::LoadData(uint8_t *source, const int stride, uint8_t *dest, const int width, const int height){
    src = source;
    dst = dest;
    
    this->width = width;
    this->height = height;
    this->stride = stride;
    
}

void YUVBuffer::Convert(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height)
{
    //START_TIMER();
    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        //Point to the first element of the current row (specified by "height") in the flattened 2D array "dst".
        uint8_t* row = dst + (width * 3) * j; // 3 channels
        
        //Iterate by 4 in yuv_src (4 channel), iterate by 3 twice in dst (3 channel)
        for (int i = 0; i < 2 * width; i += 4, row += 6)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;
            //printf("%i\n", v);
            
            int ruv = LUT_RV[v + 128];
            int guv = SHIFT_THING + LUT_GV[v + 128] + LUT_GU[u + 128];
            int buv = LUT_BU[u + 128];
            
            //Here we convert two pixels at a time.
            //Perfect opportunity for SIMD speedup in the future.
            //Set 1
            //int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            int y00 = _max(0, (yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            //row[3]      = (0xff);
            
            //Set 2
            //int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            int y01 = _max(0, (yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[5-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[4]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[3+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            //row[7]      = (0xff);
            
            /*
            printf("int LUT_SATURATE[256] = {\n");
            //printf("Thing: %i\n", (y01 + ruv) >> ITUR_BT_601_SHIFT);
            for(int i = 0; i < 256; i++){
                printf("    %i\n", (y01 + ruv) >> ITUR_BT_601_SHIFT);
            }
            printf("}\n");
            */
            
        }
    }
    //STOP_TIMER();
    //PRINT_TIMER("Time");
    //printf("Conversion time: %fs\n", timer.value);
}

void YUVBuffer::Build_LUTs(){
    /*
     printf("Building LUT_RV\n");
     for(int i=0; i<256; i++){
     LUT_RV[i] = (i-128) * ((1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR);
     }
     
     printf("Building LUT_GU\n");
     for(int i=0; i<256; i++){
     LUT_GU[i] = (i - 128) * ITUR_BT_601_CUG;
     }
     
     printf("Building LUT_GV\n");
     for(int i=0; i<256; i++){
     LUT_GV[i] = (i-128) * ITUR_BT_601_CVG;
     }
     
     printf("Building LUT_BU\n");
     for(int u=0; u<256; u++){
     LUT_BU[u] = SHIFT_THING + ITUR_BT_601_CUB * (u-128);
     }
     */
}

//These define the lookup tables for color conversion. They're here for optimization reasons; see http://www.stackoverflow.com/questions/9251014/what-is-a-best-way-of-defining-a-very-large-array-lookup-table for more information.
int const LUT_RV[256] = {
    -281320320,
    -279122505,
    -276924690,
    -274726875,
    -272529060,
    -270331245,
    -268133430,
    -265935615,
    -263737800,
    -261539985,
    -259342170,
    -257144355,
    -254946540,
    -252748725,
    -250550910,
    -248353095,
    -246155280,
    -243957465,
    -241759650,
    -239561835,
    -237364020,
    -235166205,
    -232968390,
    -230770575,
    -228572760,
    -226374945,
    -224177130,
    -221979315,
    -219781500,
    -217583685,
    -215385870,
    -213188055,
    -210990240,
    -208792425,
    -206594610,
    -204396795,
    -202198980,
    -200001165,
    -197803350,
    -195605535,
    -193407720,
    -191209905,
    -189012090,
    -186814275,
    -184616460,
    -182418645,
    -180220830,
    -178023015,
    -175825200,
    -173627385,
    -171429570,
    -169231755,
    -167033940,
    -164836125,
    -162638310,
    -160440495,
    -158242680,
    -156044865,
    -153847050,
    -151649235,
    -149451420,
    -147253605,
    -145055790,
    -142857975,
    -140660160,
    -138462345,
    -136264530,
    -134066715,
    -131868900,
    -129671085,
    -127473270,
    -125275455,
    -123077640,
    -120879825,
    -118682010,
    -116484195,
    -114286380,
    -112088565,
    -109890750,
    -107692935,
    -105495120,
    -103297305,
    -101099490,
    -98901675,
    -96703860,
    -94506045,
    -92308230,
    -90110415,
    -87912600,
    -85714785,
    -83516970,
    -81319155,
    -79121340,
    -76923525,
    -74725710,
    -72527895,
    -70330080,
    -68132265,
    -65934450,
    -63736635,
    -61538820,
    -59341005,
    -57143190,
    -54945375,
    -52747560,
    -50549745,
    -48351930,
    -46154115,
    -43956300,
    -41758485,
    -39560670,
    -37362855,
    -35165040,
    -32967225,
    -30769410,
    -28571595,
    -26373780,
    -24175965,
    -21978150,
    -19780335,
    -17582520,
    -15384705,
    -13186890,
    -10989075,
    -8791260,
    -6593445,
    -4395630,
    -2197815,
    0,
    2197815,
    4395630,
    6593445,
    8791260,
    10989075,
    13186890,
    15384705,
    17582520,
    19780335,
    21978150,
    24175965,
    26373780,
    28571595,
    30769410,
    32967225,
    35165040,
    37362855,
    39560670,
    41758485,
    43956300,
    46154115,
    48351930,
    50549745,
    52747560,
    54945375,
    57143190,
    59341005,
    61538820,
    63736635,
    65934450,
    68132265,
    70330080,
    72527895,
    74725710,
    76923525,
    79121340,
    81319155,
    83516970,
    85714785,
    87912600,
    90110415,
    92308230,
    94506045,
    96703860,
    98901675,
    101099490,
    103297305,
    105495120,
    107692935,
    109890750,
    112088565,
    114286380,
    116484195,
    118682010,
    120879825,
    123077640,
    125275455,
    127473270,
    129671085,
    131868900,
    134066715,
    136264530,
    138462345,
    140660160,
    142857975,
    145055790,
    147253605,
    149451420,
    151649235,
    153847050,
    156044865,
    158242680,
    160440495,
    162638310,
    164836125,
    167033940,
    169231755,
    171429570,
    173627385,
    175825200,
    178023015,
    180220830,
    182418645,
    184616460,
    186814275,
    189012090,
    191209905,
    193407720,
    195605535,
    197803350,
    200001165,
    202198980,
    204396795,
    206594610,
    208792425,
    210990240,
    213188055,
    215385870,
    217583685,
    219781500,
    221979315,
    224177130,
    226374945,
    228572760,
    230770575,
    232968390,
    235166205,
    237364020,
    239561835,
    241759650,
    243957465,
    246155280,
    248353095,
    250550910,
    252748725,
    254946540,
    257144355,
    259342170,
    261539985,
    263737800,
    265935615,
    268133430,
    270331245,
    272529060,
    274726875,
    276924690,
    279122505
};
int const LUT_GU[256] = {
    52479104,
    52069111,
    51659118,
    51249125,
    50839132,
    50429139,
    50019146,
    49609153,
    49199160,
    48789167,
    48379174,
    47969181,
    47559188,
    47149195,
    46739202,
    46329209,
    45919216,
    45509223,
    45099230,
    44689237,
    44279244,
    43869251,
    43459258,
    43049265,
    42639272,
    42229279,
    41819286,
    41409293,
    40999300,
    40589307,
    40179314,
    39769321,
    39359328,
    38949335,
    38539342,
    38129349,
    37719356,
    37309363,
    36899370,
    36489377,
    36079384,
    35669391,
    35259398,
    34849405,
    34439412,
    34029419,
    33619426,
    33209433,
    32799440,
    32389447,
    31979454,
    31569461,
    31159468,
    30749475,
    30339482,
    29929489,
    29519496,
    29109503,
    28699510,
    28289517,
    27879524,
    27469531,
    27059538,
    26649545,
    26239552,
    25829559,
    25419566,
    25009573,
    24599580,
    24189587,
    23779594,
    23369601,
    22959608,
    22549615,
    22139622,
    21729629,
    21319636,
    20909643,
    20499650,
    20089657,
    19679664,
    19269671,
    18859678,
    18449685,
    18039692,
    17629699,
    17219706,
    16809713,
    16399720,
    15989727,
    15579734,
    15169741,
    14759748,
    14349755,
    13939762,
    13529769,
    13119776,
    12709783,
    12299790,
    11889797,
    11479804,
    11069811,
    10659818,
    10249825,
    9839832,
    9429839,
    9019846,
    8609853,
    8199860,
    7789867,
    7379874,
    6969881,
    6559888,
    6149895,
    5739902,
    5329909,
    4919916,
    4509923,
    4099930,
    3689937,
    3279944,
    2869951,
    2459958,
    2049965,
    1639972,
    1229979,
    819986,
    409993,
    0,
    -409993,
    -819986,
    -1229979,
    -1639972,
    -2049965,
    -2459958,
    -2869951,
    -3279944,
    -3689937,
    -4099930,
    -4509923,
    -4919916,
    -5329909,
    -5739902,
    -6149895,
    -6559888,
    -6969881,
    -7379874,
    -7789867,
    -8199860,
    -8609853,
    -9019846,
    -9429839,
    -9839832,
    -10249825,
    -10659818,
    -11069811,
    -11479804,
    -11889797,
    -12299790,
    -12709783,
    -13119776,
    -13529769,
    -13939762,
    -14349755,
    -14759748,
    -15169741,
    -15579734,
    -15989727,
    -16399720,
    -16809713,
    -17219706,
    -17629699,
    -18039692,
    -18449685,
    -18859678,
    -19269671,
    -19679664,
    -20089657,
    -20499650,
    -20909643,
    -21319636,
    -21729629,
    -22139622,
    -22549615,
    -22959608,
    -23369601,
    -23779594,
    -24189587,
    -24599580,
    -25009573,
    -25419566,
    -25829559,
    -26239552,
    -26649545,
    -27059538,
    -27469531,
    -27879524,
    -28289517,
    -28699510,
    -29109503,
    -29519496,
    -29929489,
    -30339482,
    -30749475,
    -31159468,
    -31569461,
    -31979454,
    -32389447,
    -32799440,
    -33209433,
    -33619426,
    -34029419,
    -34439412,
    -34849405,
    -35259398,
    -35669391,
    -36079384,
    -36489377,
    -36899370,
    -37309363,
    -37719356,
    -38129349,
    -38539342,
    -38949335,
    -39359328,
    -39769321,
    -40179314,
    -40589307,
    -40999300,
    -41409293,
    -41819286,
    -42229279,
    -42639272,
    -43049265,
    -43459258,
    -43869251,
    -44279244,
    -44689237,
    -45099230,
    -45509223,
    -45919216,
    -46329209,
    -46739202,
    -47149195,
    -47559188,
    -47969181,
    -48379174,
    -48789167,
    -49199160,
    -49609153,
    -50019146,
    -50429139,
    -50839132,
    -51249125,
    -51659118,
    -52069111
};
int const LUT_GV[256] = {
    109118976,
    108266484,
    107413992,
    106561500,
    105709008,
    104856516,
    104004024,
    103151532,
    102299040,
    101446548,
    100594056,
    99741564,
    98889072,
    98036580,
    97184088,
    96331596,
    95479104,
    94626612,
    93774120,
    92921628,
    92069136,
    91216644,
    90364152,
    89511660,
    88659168,
    87806676,
    86954184,
    86101692,
    85249200,
    84396708,
    83544216,
    82691724,
    81839232,
    80986740,
    80134248,
    79281756,
    78429264,
    77576772,
    76724280,
    75871788,
    75019296,
    74166804,
    73314312,
    72461820,
    71609328,
    70756836,
    69904344,
    69051852,
    68199360,
    67346868,
    66494376,
    65641884,
    64789392,
    63936900,
    63084408,
    62231916,
    61379424,
    60526932,
    59674440,
    58821948,
    57969456,
    57116964,
    56264472,
    55411980,
    54559488,
    53706996,
    52854504,
    52002012,
    51149520,
    50297028,
    49444536,
    48592044,
    47739552,
    46887060,
    46034568,
    45182076,
    44329584,
    43477092,
    42624600,
    41772108,
    40919616,
    40067124,
    39214632,
    38362140,
    37509648,
    36657156,
    35804664,
    34952172,
    34099680,
    33247188,
    32394696,
    31542204,
    30689712,
    29837220,
    28984728,
    28132236,
    27279744,
    26427252,
    25574760,
    24722268,
    23869776,
    23017284,
    22164792,
    21312300,
    20459808,
    19607316,
    18754824,
    17902332,
    17049840,
    16197348,
    15344856,
    14492364,
    13639872,
    12787380,
    11934888,
    11082396,
    10229904,
    9377412,
    8524920,
    7672428,
    6819936,
    5967444,
    5114952,
    4262460,
    3409968,
    2557476,
    1704984,
    852492,
    0,
    -852492,
    -1704984,
    -2557476,
    -3409968,
    -4262460,
    -5114952,
    -5967444,
    -6819936,
    -7672428,
    -8524920,
    -9377412,
    -10229904,
    -11082396,
    -11934888,
    -12787380,
    -13639872,
    -14492364,
    -15344856,
    -16197348,
    -17049840,
    -17902332,
    -18754824,
    -19607316,
    -20459808,
    -21312300,
    -22164792,
    -23017284,
    -23869776,
    -24722268,
    -25574760,
    -26427252,
    -27279744,
    -28132236,
    -28984728,
    -29837220,
    -30689712,
    -31542204,
    -32394696,
    -33247188,
    -34099680,
    -34952172,
    -35804664,
    -36657156,
    -37509648,
    -38362140,
    -39214632,
    -40067124,
    -40919616,
    -41772108,
    -42624600,
    -43477092,
    -44329584,
    -45182076,
    -46034568,
    -46887060,
    -47739552,
    -48592044,
    -49444536,
    -50297028,
    -51149520,
    -52002012,
    -52854504,
    -53706996,
    -54559488,
    -55411980,
    -56264472,
    -57116964,
    -57969456,
    -58821948,
    -59674440,
    -60526932,
    -61379424,
    -62231916,
    -63084408,
    -63936900,
    -64789392,
    -65641884,
    -66494376,
    -67346868,
    -68199360,
    -69051852,
    -69904344,
    -70756836,
    -71609328,
    -72461820,
    -73314312,
    -74166804,
    -75019296,
    -75871788,
    -76724280,
    -77576772,
    -78429264,
    -79281756,
    -80134248,
    -80986740,
    -81839232,
    -82691724,
    -83544216,
    -84396708,
    -85249200,
    -86101692,
    -86954184,
    -87806676,
    -88659168,
    -89511660,
    -90364152,
    -91216644,
    -92069136,
    -92921628,
    -93774120,
    -94626612,
    -95479104,
    -96331596,
    -97184088,
    -98036580,
    -98889072,
    -99741564,
    -100594056,
    -101446548,
    -102299040,
    -103151532,
    -104004024,
    -104856516,
    -105709008,
    -106561500,
    -107413992,
    -108266484
};
int const LUT_BU[256] = {
    -270327040,
    -268211014,
    -266094988,
    -263978962,
    -261862936,
    -259746910,
    -257630884,
    -255514858,
    -253398832,
    -251282806,
    -249166780,
    -247050754,
    -244934728,
    -242818702,
    -240702676,
    -238586650,
    -236470624,
    -234354598,
    -232238572,
    -230122546,
    -228006520,
    -225890494,
    -223774468,
    -221658442,
    -219542416,
    -217426390,
    -215310364,
    -213194338,
    -211078312,
    -208962286,
    -206846260,
    -204730234,
    -202614208,
    -200498182,
    -198382156,
    -196266130,
    -194150104,
    -192034078,
    -189918052,
    -187802026,
    -185686000,
    -183569974,
    -181453948,
    -179337922,
    -177221896,
    -175105870,
    -172989844,
    -170873818,
    -168757792,
    -166641766,
    -164525740,
    -162409714,
    -160293688,
    -158177662,
    -156061636,
    -153945610,
    -151829584,
    -149713558,
    -147597532,
    -145481506,
    -143365480,
    -141249454,
    -139133428,
    -137017402,
    -134901376,
    -132785350,
    -130669324,
    -128553298,
    -126437272,
    -124321246,
    -122205220,
    -120089194,
    -117973168,
    -115857142,
    -113741116,
    -111625090,
    -109509064,
    -107393038,
    -105277012,
    -103160986,
    -101044960,
    -98928934,
    -96812908,
    -94696882,
    -92580856,
    -90464830,
    -88348804,
    -86232778,
    -84116752,
    -82000726,
    -79884700,
    -77768674,
    -75652648,
    -73536622,
    -71420596,
    -69304570,
    -67188544,
    -65072518,
    -62956492,
    -60840466,
    -58724440,
    -56608414,
    -54492388,
    -52376362,
    -50260336,
    -48144310,
    -46028284,
    -43912258,
    -41796232,
    -39680206,
    -37564180,
    -35448154,
    -33332128,
    -31216102,
    -29100076,
    -26984050,
    -24868024,
    -22751998,
    -20635972,
    -18519946,
    -16403920,
    -14287894,
    -12171868,
    -10055842,
    -7939816,
    -5823790,
    -3707764,
    -1591738,
    524288,
    2640314,
    4756340,
    6872366,
    8988392,
    11104418,
    13220444,
    15336470,
    17452496,
    19568522,
    21684548,
    23800574,
    25916600,
    28032626,
    30148652,
    32264678,
    34380704,
    36496730,
    38612756,
    40728782,
    42844808,
    44960834,
    47076860,
    49192886,
    51308912,
    53424938,
    55540964,
    57656990,
    59773016,
    61889042,
    64005068,
    66121094,
    68237120,
    70353146,
    72469172,
    74585198,
    76701224,
    78817250,
    80933276,
    83049302,
    85165328,
    87281354,
    89397380,
    91513406,
    93629432,
    95745458,
    97861484,
    99977510,
    102093536,
    104209562,
    106325588,
    108441614,
    110557640,
    112673666,
    114789692,
    116905718,
    119021744,
    121137770,
    123253796,
    125369822,
    127485848,
    129601874,
    131717900,
    133833926,
    135949952,
    138065978,
    140182004,
    142298030,
    144414056,
    146530082,
    148646108,
    150762134,
    152878160,
    154994186,
    157110212,
    159226238,
    161342264,
    163458290,
    165574316,
    167690342,
    169806368,
    171922394,
    174038420,
    176154446,
    178270472,
    180386498,
    182502524,
    184618550,
    186734576,
    188850602,
    190966628,
    193082654,
    195198680,
    197314706,
    199430732,
    201546758,
    203662784,
    205778810,
    207894836,
    210010862,
    212126888,
    214242914,
    216358940,
    218474966,
    220590992,
    222707018,
    224823044,
    226939070,
    229055096,
    231171122,
    233287148,
    235403174,
    237519200,
    239635226,
    241751252,
    243867278,
    245983304,
    248099330,
    250215356,
    252331382,
    254447408,
    256563434,
    258679460,
    260795486,
    262911512,
    265027538,
    267143564,
    269259590
};
//int LUT_SATURATE[256] {};






