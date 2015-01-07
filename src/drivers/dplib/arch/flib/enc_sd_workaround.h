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

/*
auth_alginfo = OP_PCL_IPSEC_HMAC_MD5_96;
cipher_alginfo = OP_PCL_IPSEC_3DES;
	
ws_shared_desc[0] = 0x14028cb8;
ws_shared_desc[1] = 0x78000000;
ws_shared_desc[2] = 0xccd72000;
ws_shared_desc[3] = 0x04042000;
ws_shared_desc[4] = 0x5d000000;
ws_shared_desc[5] = 0xb40c3000;
ws_shared_desc[6] = 0x64045200;
ws_shared_desc[7] = 0x17171717;
ws_shared_desc[8] = 0xedfe0100;
ws_shared_desc[9] = 0x14000000;
ws_shared_desc[10] = 0x14000045;
ws_shared_desc[11] = 0x00000000;
ws_shared_desc[12] = 0x074000a1;
ws_shared_desc[13] = 0x20000304;
ws_shared_desc[14] = 0x00000000;
ws_shared_desc[15] = 0xd00320e0;
ws_shared_desc[16] = 0x18000002;
ws_shared_desc[17] = 0x00000000;
ws_shared_desc[18] = 0xb80320e0;
ws_shared_desc[19] = 0x01031187;

ws_shared_desc[20] = 0xdeadbeef;
*sd_size = 21; // In words
*/

/* Null encryption descriptor */
/* >>>>>> Dump ENCAP descriptor (Little Endian): */
ws_shared_desc[0] = 0x14028fb8;
ws_shared_desc[1] = 0x0c000000;
ws_shared_desc[2] = 0x00000000;
ws_shared_desc[3] = 0x00000000;
ws_shared_desc[4] = 0x00000000;
ws_shared_desc[5] = 0x00000000;
ws_shared_desc[6] = 0x00000000;
ws_shared_desc[7] = 0x00000000;
ws_shared_desc[8] = 0x00000000;
ws_shared_desc[9] = 0x14000000;
ws_shared_desc[10] = 0x14000045;
ws_shared_desc[11] = 0x00000000;
ws_shared_desc[12] = 0x08213278;
ws_shared_desc[13] = 0x14c8c8c8;
ws_shared_desc[14] = 0x0ac8c8c8;
ws_shared_desc[15] = 0x044000a1;
ws_shared_desc[16] = 0x20000304;
ws_shared_desc[17] = 0x00000000; // Key pointer is LE-BE convert correct?
ws_shared_desc[18] = 0xe80320e0; // Key pointer is LE-BE convert correct?
ws_shared_desc[19] = 0x010b1187;
*sd_size = 20; // In words

// LW_SWAP(_addr) - load & swap 4 bytes
// LLLDW_SWAP(_addr) load & swap 8 bytes (fails on compilation error)

/* Insert IP address from application */
ws_shared_desc[10] = LW_SWAP(params->encparams.outer_hdr + 0);
ws_shared_desc[11] = LW_SWAP(params->encparams.outer_hdr + 1);
ws_shared_desc[12] = LW_SWAP(params->encparams.outer_hdr + 2);
ws_shared_desc[13] = LW_SWAP(params->encparams.outer_hdr + 3);
ws_shared_desc[14] = LW_SWAP(params->encparams.outer_hdr + 4);

/* Insert Key address from application */
//uint64_t tmp_swapped_key_addr;
//tmp_swapped_key_addr = LLLDW_SWAP((params->authdata.key));

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
ws_shared_desc[17] = LW_SWAP(key_word + 0); // MSW
ws_shared_desc[18] = LW_SWAP(key_word + 1); // LSW

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
ws_shared_desc[17] = *swapped_key_ptr;

swapped_key_ptr = (uint32_t *)(&(output[4]));
ws_shared_desc[18] = *swapped_key_ptr;
*/

/*
>>>>>> Dump disassembled  ENCAP descriptor (Big Endian) :
[00] B88F0214       shrhdr: stidx=15 share=serial len=20
                       PDB: 
[01] 0000000C    OIHI =2’b11       PDB contains Outer IP Header material to be included in Output Frame
[02] 00000000
[03] 00000000
[04] 00000000
[05] 00000000  
[06] 00000000
[07] 00000000
[08] 00000000
[09] 00000014    Outer Ip Header Length is 20 bytes
[10] 45000014    outer_ip_hdr[0]
[11] 00000000    outer_ip_hdr[1]
[12] 78322108    outer_ip_hdr[2]
[13] C8C8C814   outer_ip_hdr[3] 
[14] C8C8C80A   outer_ip_hdr[4]
[15] A1004004         jump: jsl1 all-match[shared] offset=4 local->[19]
[16] 04030020          key: class2-mdha-split len=32
[17] 00000000               ptr->@0x0e02003e8
[18] E02003E8
[19] 87110B01    operation: encap null & hmac_md5_96
*/


