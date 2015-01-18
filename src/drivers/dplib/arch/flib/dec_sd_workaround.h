/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


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
/*
SEC expects descriptors (including inline value to descriptor command)  
to be read from memory in LE. This is at 32bit boundary.
Currently, 64bit address interpretation is as below
Descriptor n      => MS 32bit
Descriptor n+1 => LS   32bit
But both descriptors  n, n+1 is in LE; as LE to BE is done for 
each entry(32 bit) in descriptor buffer.
*/
uint32_t *key_word;
key_word = (uint32_t *)(&(params->authdata.key));
ws_shared_desc[12] = LW_SWAP(key_word + 0); // MSW
ws_shared_desc[13] = LW_SWAP(key_word + 1); // LSW

/*
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
*/

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

