
/* >>>>>> Dump DECAP descriptor (Little Endian): */
ws_shared_desc[0] = 0x0f028ab8;
ws_shared_desc[1] = 0x00001400;
ws_shared_desc[2] = 0x00000000;
ws_shared_desc[3] = 0x00000000;
ws_shared_desc[4] = 0x00000000;
ws_shared_desc[5] = 0x00000000;
ws_shared_desc[6] = 0x00000000;
ws_shared_desc[7] = 0x00000000;
ws_shared_desc[8] = 0x00000000;
ws_shared_desc[9] = 0x00000000;
ws_shared_desc[10] = 0x044000a1;
ws_shared_desc[11] = 0x20000304;
ws_shared_desc[12] = 0x00000000; // Key addr
ws_shared_desc[13] = 0xe80320e0; // Key addr
ws_shared_desc[14] = 0x010b1186;

*sd_size = 15; // In words

/* Insert Key address from application */

uint32_t *swapped_key_ptr;
uint8_t *input_p;
uint8_t output[8];

input_p = (uint8_t *)(&(params->authdata.key));

// Swap
output[7] = *(input_p + 0);
output[6] = *(input_p + 1);
output[5] = *(input_p + 2);
output[4] = *(input_p + 3);
output[3] = *(input_p + 4);
output[2] = *(input_p + 5);
output[1] = *(input_p + 6);
output[0] = *(input_p + 7);

swapped_key_ptr = (uint32_t *)(&(output[0]));
ws_shared_desc[12] = *swapped_key_ptr;

swapped_key_ptr = (uint32_t *)(&(output[4]));
ws_shared_desc[13] = *swapped_key_ptr;

/*
>>>>>> Dump disassembled DECAP descriptor (Big Endian):
[00] B88A020F       shrhdr: stidx=10 share=serial len=15
                       PDB:     
[01] 00140000    Outer Ip Header Length is 20 bytes
[02] 00000000
[03] 00000000
[04] 00000000
[05] 00000000  
[06] 00000000
[07] 00000000
[08] 00000000
[09] 00000000  
[10] A1004004         jump: jsl1 all-match[shared] offset=4 local->[14]
[11] 04030020          key: class2-mdha-split len=32
[12] 00000000               ptr->@0x0e02003e8
[13] E02003E8
[14] 86110B01    operation: decap null & hmac_md5_96

*/

