#include "parser.h"

Parser* Parser::pThis = nullptr;
uint16_t Parser::color_background_old = 0xFFFF;
uint16_t Parser::color_figure_old = 0x0000;

Parser::Parser() {
    init();
}

void Parser::fat_init(){
    res = f_mount(&fs, "",0);
    uint8_t path[] = "F.bin";
	res = f_open(&test, (char*)path, FA_READ);
	if(res != FR_OK) { return; }
    uint32_t y = 576*2*224;// (uint32_t)f_size(&test);
    uint8_t count = 0;
    for(int i=0 ; i<y;i+=512){
        res = f_read(&test,(uint8_t*)fileBuf,512,&testBytes);
        for(int j = 0; j < 512; j++) {
            *((uint8_t*)(font) + j + i) = fileBuf[j];
        }
        uint16_t x = testBytes;
        if(count == 100){
            count=0;
        }
        count++;
        res = f_lseek(&test,i+512);
    }	
	res = f_close(&test);
	if(res != FR_OK) {}    
}

void Parser::setColors(uint16_t back, uint16_t fig) {
    uint16_t* charAddress = &font[0][0];
    for(int i=0; i<(224*576); i++){
        if(*((uint16_t*)charAddress + i) == color_background_old) {
            *((uint16_t*)charAddress + i) = back;
        } else if(*((uint16_t*)charAddress + i) == color_figure_old) {
            *((uint16_t*)charAddress + i) = fig;
        }
    }
    color_background_old = back;
    color_figure_old = fig;
}

void Parser::drawSymbol(uint16_t x, uint16_t y, char byte) { 
    const uint16_t* charAddress = byteToChar(byte);
    setRect(x,y,x+24-1,y+24-1);
    writeCmd(0x2C00);
    for(int i=0; i<576; i++) {
        writeData(*((uint16_t*)charAddress + i));
    }
}
void Parser::drawString(uint16_t x, uint16_t y, char* byte) {
    uint16_t dx = 0;
    while(*byte) {
        drawSymbol(x+dx,y,*byte);
        dx+=24;
        byte++;
    }
}
void Parser::drawIntValue(uint16_t x, uint16_t y, char* byte, uint8_t digitsNum) {
    uint8_t count=0; uint16_t dx=0;
    while(*byte) {
        drawSymbol(x+dx,y,*byte);
        dx+=28;
        byte++; count++;
    }
    while(count<digitsNum) {
        drawSymbol(x+dx,y,0x20);
        dx+=28;  count++;
    }
}

uint16_t* Parser::byteToChar(char byte) {
    if(byte == 0x20) { return &font[0][0];}
    else if(byte == 0x21) { return &font[1][0];}
    else if(byte == 0x22) { return &font[2][0];}
    else if(byte == 0x23) { return &font[3][0];}
    else if(byte == 0x24) { return &font[4][0];}
    else if(byte == 0x25) { return &font[5][0];}
    else if(byte == 0x26) { return &font[6][0];}
    else if(byte == 0x27) { return &font[7][0];}
    else if(byte == 0x28) { return &font[8][0];}
    else if(byte == 0x29) { return &font[9][0];}
    else if(byte == 0x2a) { return &font[10][0];}
    else if(byte == 0x2b) { return &font[11][0];}
    else if(byte == 0x2c) { return &font[12][0];}
    else if(byte == 0x2d) { return &font[13][0];}
    else if(byte == 0x2e) { return &font[14][0];}
    else if(byte == 0x2f) { return &font[15][0];}
    else if(byte == 0x30) { return &font[16][0];}
    else if(byte == 0x31) { return &font[17][0];}
    else if(byte == 0x32) { return &font[18][0];}
    else if(byte == 0x33) { return &font[19][0];}
    else if(byte == 0x34) { return &font[20][0];}
    else if(byte == 0x35) { return &font[21][0];}
    else if(byte == 0x36) { return &font[22][0];}
    else if(byte == 0x37) { return &font[23][0];}
    else if(byte == 0x38) { return &font[24][0];}
    else if(byte == 0x39) { return &font[25][0];}
    else if(byte == 0x3a) { return &font[26][0];}
    else if(byte == 0x3b) { return &font[27][0];}
    else if(byte == 0x3c) { return &font[28][0];}
    else if(byte == 0x3d) { return &font[29][0];}
    else if(byte == 0x3e) { return &font[30][0];}
    else if(byte == 0x3f) { return &font[31][0];}
    else if(byte == 0x40) { return &font[32][0];}
    else if(byte == 0x41) { return &font[33][0];}
    else if(byte == 0x42) { return &font[34][0];}
    else if(byte == 0x43) { return &font[35][0];}
    else if(byte == 0x44) { return &font[36][0];}
    else if(byte == 0x45) { return &font[37][0];}
    else if(byte == 0x46) { return &font[38][0];}
    else if(byte == 0x47) { return &font[39][0];}
    else if(byte == 0x48) { return &font[40][0];}
    else if(byte == 0x49) { return &font[41][0];}
    else if(byte == 0x4a) { return &font[42][0];}
    else if(byte == 0x4b) { return &font[43][0];}
    else if(byte == 0x4c) { return &font[44][0];}
    else if(byte == 0x4d) { return &font[45][0];}
    else if(byte == 0x4e) { return &font[46][0];}
    else if(byte == 0x4f) { return &font[47][0];}
    else if(byte == 0x50) { return &font[48][0];}
    else if(byte == 0x51) { return &font[49][0];}
    else if(byte == 0x52) { return &font[50][0];}
    else if(byte == 0x53) { return &font[51][0];}
    else if(byte == 0x54) { return &font[52][0];}
    else if(byte == 0x55) { return &font[53][0];}
    else if(byte == 0x56) { return &font[54][0];}
    else if(byte == 0x57) { return &font[55][0];}
    else if(byte == 0x58) { return &font[56][0];}
    else if(byte == 0x59) { return &font[57][0];}
    else if(byte == 0x5a) { return &font[58][0];}
    else if(byte == 0x5b) { return &font[59][0];}
    else if(byte == 0x5c) { return &font[60][0];}
    else if(byte == 0x5d) { return &font[61][0];}
    else if(byte == 0x5e) { return &font[62][0];}
    else if(byte == 0x5f) { return &font[63][0];}
    else if(byte == 0x60) { return &font[64][0];}
    else if(byte == 0x61) { return &font[65][0];}
    else if(byte == 0x62) { return &font[66][0];}
    else if(byte == 0x63) { return &font[67][0];}
    else if(byte == 0x64) { return &font[68][0];}
    else if(byte == 0x65) { return &font[69][0];}
    else if(byte == 0x66) { return &font[70][0];}
    else if(byte == 0x67) { return &font[71][0];}
    else if(byte == 0x68) { return &font[72][0];}
    else if(byte == 0x69) { return &font[73][0];}
    else if(byte == 0x6a) { return &font[74][0];}
    else if(byte == 0x6b) { return &font[75][0];}
    else if(byte == 0x6c) { return &font[76][0];}
    else if(byte == 0x6d) { return &font[77][0];}
    else if(byte == 0x6e) { return &font[78][0];}
    else if(byte == 0x6f) { return &font[79][0];}
    else if(byte == 0x70) { return &font[80][0];}
    else if(byte == 0x71) { return &font[81][0];}
    else if(byte == 0x72) { return &font[82][0];}
    else if(byte == 0x73) { return &font[83][0];}
    else if(byte == 0x74) { return &font[84][0];}
    else if(byte == 0x75) { return &font[85][0];}
    else if(byte == 0x76) { return &font[86][0];}
    else if(byte == 0x77) { return &font[87][0];}
    else if(byte == 0x78) { return &font[88][0];}
    else if(byte == 0x79) { return &font[89][0];}
    else if(byte == 0x7a) { return &font[90][0];}
    else if(byte == 0x7b) { return &font[91][0];}
    else if(byte == 0x7c) { return &font[92][0];}
    else if(byte == 0x7d) { return &font[93][0];}
    else if(byte == 0x7e) { return &font[94][0];}
    else if(byte == 0x7f) { return &font[95][0];}
    else if(byte == 0x80) { return &font[96][0];}
    else if(byte == 0x81) { return &font[97][0];}
    else if(byte == 0x82) { return &font[98][0];}
    else if(byte == 0x83) { return &font[99][0];}
    else if(byte == 0x84) { return &font[100][0];}
    else if(byte == 0x85) { return &font[101][0];}
    else if(byte == 0x86) { return &font[102][0];}
    else if(byte == 0x87) { return &font[103][0];}
    else if(byte == 0x88) { return &font[104][0];}
    else if(byte == 0x89) { return &font[105][0];}
    else if(byte == 0x8a) { return &font[106][0];}
    else if(byte == 0x8b) { return &font[107][0];}
    else if(byte == 0x8c) { return &font[108][0];}
    else if(byte == 0x8d) { return &font[109][0];}
    else if(byte == 0x8e) { return &font[110][0];}
    else if(byte == 0x8f) { return &font[111][0];}
    else if(byte == 0x90) { return &font[112][0];}
    else if(byte == 0x91) { return &font[113][0];}
    else if(byte == 0x92) { return &font[114][0];}
    else if(byte == 0x93) { return &font[115][0];}
    else if(byte == 0x94) { return &font[116][0];}
    else if(byte == 0x95) { return &font[117][0];}
    else if(byte == 0x96) { return &font[118][0];}
    else if(byte == 0x97) { return &font[119][0];}
    else if(byte == 0x98) { return &font[120][0];}
    else if(byte == 0x99) { return &font[121][0];}
    else if(byte == 0x9a) { return &font[122][0];}
    else if(byte == 0x9b) { return &font[123][0];}
    else if(byte == 0x9c) { return &font[124][0];}
    else if(byte == 0x9d) { return &font[125][0];}
    else if(byte == 0x9e) { return &font[126][0];}
    else if(byte == 0x9f) { return &font[127][0];}
    else if(byte == 0xa0) { return &font[128][0];}
    else if(byte == 0xa1) { return &font[129][0];}
    else if(byte == 0xa2) { return &font[130][0];}
    else if(byte == 0xa3) { return &font[131][0];}
    else if(byte == 0xa4) { return &font[132][0];}
    else if(byte == 0xa5) { return &font[133][0];}
    else if(byte == 0xa6) { return &font[134][0];}
    else if(byte == 0xa7) { return &font[135][0];}
    else if(byte == 0xa8) { return &font[136][0];}
    else if(byte == 0xa9) { return &font[137][0];}
    else if(byte == 0xaa) { return &font[138][0];}
    else if(byte == 0xab) { return &font[139][0];}
    else if(byte == 0xac) { return &font[140][0];}
    else if(byte == 0xad) { return &font[141][0];}
    else if(byte == 0xae) { return &font[142][0];}
    else if(byte == 0xaf) { return &font[143][0];}
    else if(byte == 0xb0) { return &font[144][0];}
    else if(byte == 0xb1) { return &font[145][0];}
    else if(byte == 0xb2) { return &font[146][0];}
    else if(byte == 0xb3) { return &font[147][0];}
    else if(byte == 0xb4) { return &font[148][0];}
    else if(byte == 0xb5) { return &font[149][0];}
    else if(byte == 0xb6) { return &font[150][0];}
    else if(byte == 0xb7) { return &font[151][0];}
    else if(byte == 0xb8) { return &font[152][0];}
    else if(byte == 0xb9) { return &font[153][0];}
    else if(byte == 0xba) { return &font[154][0];}
    else if(byte == 0xbb) { return &font[155][0];}
    else if(byte == 0xbc) { return &font[156][0];}
    else if(byte == 0xbd) { return &font[157][0];}
    else if(byte == 0xbe) { return &font[158][0];}
    else if(byte == 0xbf) { return &font[159][0];}
    else if(byte == 0xc0) { return &font[160][0];}
    else if(byte == 0xc1) { return &font[161][0];}
    else if(byte == 0xc2) { return &font[162][0];}
    else if(byte == 0xc3) { return &font[163][0];}
    else if(byte == 0xc4) { return &font[164][0];}
    else if(byte == 0xc5) { return &font[165][0];}
    else if(byte == 0xc6) { return &font[166][0];}
    else if(byte == 0xc7) { return &font[167][0];}
    else if(byte == 0xc8) { return &font[168][0];}
    else if(byte == 0xc9) { return &font[169][0];}
    else if(byte == 0xca) { return &font[170][0];}
    else if(byte == 0xcb) { return &font[171][0];}
    else if(byte == 0xcc) { return &font[172][0];}
    else if(byte == 0xcd) { return &font[173][0];}
    else if(byte == 0xce) { return &font[174][0];}
    else if(byte == 0xcf) { return &font[175][0];}
    else if(byte == 0xd0) { return &font[176][0];}
    else if(byte == 0xd1) { return &font[177][0];}
    else if(byte == 0xd2) { return &font[178][0];}
    else if(byte == 0xd3) { return &font[179][0];}
    else if(byte == 0xd4) { return &font[180][0];}
    else if(byte == 0xd5) { return &font[181][0];}
    else if(byte == 0xd6) { return &font[182][0];}
    else if(byte == 0xd7) { return &font[183][0];}
    else if(byte == 0xd8) { return &font[184][0];}
    else if(byte == 0xd9) { return &font[185][0];}
    else if(byte == 0xda) { return &font[186][0];}
    else if(byte == 0xdb) { return &font[187][0];}
    else if(byte == 0xdc) { return &font[188][0];}
    else if(byte == 0xdd) { return &font[189][0];}
    else if(byte == 0xde) { return &font[190][0];}
    else if(byte == 0xdf) { return &font[191][0];}
    else if(byte == 0xe0) { return &font[192][0];}
    else if(byte == 0xe1) { return &font[193][0];}
    else if(byte == 0xe2) { return &font[194][0];}
    else if(byte == 0xe3) { return &font[195][0];}
    else if(byte == 0xe4) { return &font[196][0];}
    else if(byte == 0xe5) { return &font[197][0];}
    else if(byte == 0xe6) { return &font[198][0];}
    else if(byte == 0xe7) { return &font[199][0];}
    else if(byte == 0xe8) { return &font[200][0];}
    else if(byte == 0xe9) { return &font[201][0];}
    else if(byte == 0xea) { return &font[202][0];}
    else if(byte == 0xeb) { return &font[203][0];}
    else if(byte == 0xec) { return &font[204][0];}
    else if(byte == 0xed) { return &font[205][0];}
    else if(byte == 0xee) { return &font[206][0];}
    else if(byte == 0xef) { return &font[207][0];}
    else if(byte == 0xf0) { return &font[208][0];}
    else if(byte == 0xf1) { return &font[209][0];}
    else if(byte == 0xf2) { return &font[210][0];}
    else if(byte == 0xf3) { return &font[211][0];}
    else if(byte == 0xf4) { return &font[212][0];}
    else if(byte == 0xf5) { return &font[213][0];}
    else if(byte == 0xf6) { return &font[214][0];}
    else if(byte == 0xf7) { return &font[215][0];}
    else if(byte == 0xf8) { return &font[216][0];}
    else if(byte == 0xf9) { return &font[217][0];}
    else if(byte == 0xfa) { return &font[218][0];}
    else if(byte == 0xfb) { return &font[219][0];}
    else if(byte == 0xfc) { return &font[220][0];}
    else if(byte == 0xfd) { return &font[221][0];}
    else if(byte == 0xfe) { return &font[222][0];}
    else if(byte == 0xff) { 
        return &font[223][0];
    }    
    return nullptr;
}

void Parser::init() {    
    
}
