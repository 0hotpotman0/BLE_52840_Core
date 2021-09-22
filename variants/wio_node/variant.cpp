/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2018, Adafruit Industries (adafruit.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include "nrf.h"
void nfct_as_gpios();

const uint32_t g_ADigitalPinMap[] =
{

   4,  // D0  is P0.04 (AD_ID1)
   5,  // D1  is P0.05 (AD_ID2)
	  
  30,  // D2  is P0.30 (A6)
  31,  // D3  is P0.31 (A7) 

   7,  // D4  is P0.07 (KEY1)
   8,  // D5  is P0.08 (KEY2)
  40,  // D6  is P1.08 (KEY3)
  
  14,  // D7  is P0.14 (HALL_ENABLE)
  15,  // D8  is P0.15 (HALL_INT)


  12,  // D9  is P0.12 (SD_CS)
  17,  // D10 is P0.17 (SD_CLK)
  20,  // D11 is P0.20 (SD_MISO)
  24,  // D12 is P0.24 (SD_MOSI)
  25,  // D13 is P0.25 (SD_DETECT)
  32,  // D14 is P1.00 (SD_POWER)

  25,  // D15 is P0.26 (LIS3DHTR INT1)   
  33,  // D16 is P1.01 (SCL)
  34,  // D17 is P1.02 (SDA)

   9,  // D18 is P0.09 (NFC1)
  10,  // D19 is P0.10 (NFC2)
  
   6,  // D20 is P0.06 (POWER_ON)  
  11,  // D21 is P0.11 (CAT1_PWRKEY_ON)  
  22,  // D22 is P0.22 (CAT1_NET_STATUS)  
  43,  // D23 is P1.11 (CAT1_SLEEP_IND)
  44,  // D24 is P1.12 (CAT1_WAKEUP_IN)
  16,  // D25 is P0.16 (CAT1_MAIN_DCD)
  
  29,  // D26 is P0.29 (RESET_NODE)  

  46,  // D27 is P1.14 (MAIN_RXD)
  28,  // D28 is P0.28 (MAIN_TXD)
   3,  // D29 is P0.03 (MAIN_RTS)
  41,  // D30 is P1.09 (MAIN_CTS) 
  45,  // D31 is P1.13 (MAIN_DTR)
   2,  // D32 is P0.02 (MAIN_RI)
  27,  // D33 is P0.27 (W_DISABLe)
  47,  // D34 is P1.15 (MOSI_D10)
};

void initVariant()
{
//  nfct_as_gpios();
}

void nfct_as_gpios()
{
	if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		//NVIC_SystemReset();
	}
}


