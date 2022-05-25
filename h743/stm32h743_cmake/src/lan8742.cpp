////////////////////////////////////////////////////////////////////
//    @@@    @@@          @@@@@   @@@@   @@@@@   @   @   @@@@@    //
//    @  @   @  @         @   @      @   @   @   @   @   @   @    //
//    @   @  @@@   @@@@@  @@@@@   @@@@   @@@@@   @@@@@   @@@@@    //
//    @   @  @            @   @      @   @   @       @   @   @    //
//    @@@@@  @            @@@@@   @@@@   @@@@@       @   @@@@@    //
////////////////////////////////////////////////////////////////////



/** 
  ******************************************************************************
  * @file    lan8742.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the LAN742
  *          PHY devices.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "lan8742.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */ 
  
/** @defgroup LAN8742 LAN8742
  * @{
  */   
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup LAN8742_Private_Defines LAN8742 Private Defines
  * @{
  */
#define LAN8742_SW_RESET_TO    ((uint32_t)500U)
#define LAN8742_INIT_TO        ((uint32_t)2000U)
#define LAN8742_MAX_DEV_ADDR   ((uint32_t)31U)
/**
  * @}
  */
 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup LAN8742_Private_Functions LAN8742 Private Functions
  * @{
  */
    
/**
  * @brief  Register IO functions to component object
  * @param  pObj: device object  of LAN8742_Object_t. 
  * @param  ioctx: holds device IO functions.  
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ERROR if missing mandatory function
  */
int32_t  LAN8742_RegisterBusIO(lan8742_Object_t *pObj, lan8742_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return LAN8742_STATUS_ERROR;
  }
  
  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;
  
  return LAN8742_STATUS_OK;
}

/**
  * @brief  Initialize the lan8742 and configure the needed hardware resources
  * @param  pObj: device object LAN8742_Object_t. 
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ADDRESS_ERROR if cannot find device address
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  *         LAN8742_STATUS_RESET_TIMEOUT if cannot perform a software reset
  */
 int32_t LAN8742_Init(lan8742_Object_t *pObj)
 {
   uint32_t tickstart = 0, regvalue = 0, addr = 0;
   int32_t status = LAN8742_STATUS_OK;
   
   if(pObj->Is_Initialized == 0)
   {
     if(pObj->IO.Init != 0)
     {
       /* GPIO and Clocks initialization */
       pObj->IO.Init();
     }
   
     /* for later check */
     pObj->DevAddr = LAN8742_MAX_DEV_ADDR + 1;
     /* Get the device address from special mode register */  
     for(addr = 0; addr <= LAN8742_MAX_DEV_ADDR; addr ++)
     {
       // Physical address dp83848 = 01
      //  !!!!!!!!!  IN DP83848 LAN8742_SMR = 0x19 but no 0x12
      //if(pObj->IO.ReadReg(addr, LAN8742_SMR, &regvalue) < 0)
      //  { 
      //    status = LAN8742_STATUS_READ_ERROR;
      //    /* Can't read from this device address 
      //       continue with next address */
      //    continue;
      //  }
      // 0x19 PHY Control Register (PHYCR), PHYADDR[4:0]
      // addr =0;
      // while(1){
      //  pObj->IO.ReadReg(addr, 0x19, &regvalue);
      // }
       if(pObj->IO.ReadReg(addr, 0x19, &regvalue) < 0) 
       { 
         status = LAN8742_STATUS_READ_ERROR;
         /* Can't read from this device address 
            continue with next address */
         continue;
       }
        if(addr == 0x1f) {
         addr=0;
        
       }
       if((regvalue & LAN8742_SMR_PHY_ADDR) == addr)
       {
         pObj->DevAddr = addr;
         status = LAN8742_STATUS_OK;
         break;
       }
       
     }
   
     if(pObj->DevAddr > LAN8742_MAX_DEV_ADDR)
     {
       status = LAN8742_STATUS_ADDRESS_ERROR;
     }
     //pObj->DevAddr = 1;
     /* if device address is matched */
     if(status == LAN8742_STATUS_OK)
     {
       /* set a software reset  */
       if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, LAN8742_BCR_SOFT_RESET) >= 0)
       { 
         /* get software reset status */
         if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0)
         { 
           tickstart = pObj->IO.GetTick();
           
           /* wait until software reset is done or timeout occured  */
           while(regvalue & LAN8742_BCR_SOFT_RESET) //breaks when 0 => reset competed
           {
             if((pObj->IO.GetTick() - tickstart) <= LAN8742_SW_RESET_TO)
             {
               if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) < 0)
               { 
                 status = LAN8742_STATUS_READ_ERROR;
                 break;
               }
             }
             else
             {
               status = LAN8742_STATUS_RESET_TIMEOUT;
               break;
             }
           } 
         }
         else
         {
           status = LAN8742_STATUS_READ_ERROR;
         }
       }
       else
       {
         status = LAN8742_STATUS_WRITE_ERROR;
       }
     }
   }
      
#ifdef HUNDRED_MEGA_BITS_FULL_DUP_AUTO
     // AUTONEGOTIATION
    if(status == LAN8742_STATUS_OK)
    {
      tickstart =  pObj->IO.GetTick();
    
      /* Wait for 2s to perform initialization */
      while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
      {
      }
      pObj->Is_Initialized = 1;
    } 
#endif

#ifdef HUNDRED_MEGA_BITS_FULL_DUP
    //////////////////////// 100 HALF MBIT ////////////////////////////////////////////////////////////////////////////
    regvalue = 0;
    regvalue &= ~(LAN8742_BCR_AUTONEGO_EN);
    regvalue |= LAN8742_BCR_SPEED_SELECT;
    regvalue |= (LAN8742_BCR_DUPLEX_MODE);
    if (pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, regvalue) >= 0) {
        /* get  status */
        if (pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0) {

        } else {
            status = LAN8742_STATUS_READ_ERROR;
        }
    } else {
        status = LAN8742_STATUS_WRITE_ERROR;
    }
    /* Wait for 2s to perform initialization */
      while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
      {
      }
      pObj->Is_Initialized = 1;
    /////////////////////////////////////////////////////////////////////////////////////////////////////
#endif 
#ifdef HUNDRED_MEGA_BITS_HALF_DUP
    //////////////////////// 100 HALF MBIT ////////////////////////////////////////////////////////////////////////////
    regvalue = 0;
    regvalue &= ~(LAN8742_BCR_AUTONEGO_EN);
    regvalue |= LAN8742_BCR_SPEED_SELECT;
    regvalue &=~ (LAN8742_BCR_DUPLEX_MODE);
    if (pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, regvalue) >= 0) {
        /* get  status */
        if (pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0) {

        } else {
            status = LAN8742_STATUS_READ_ERROR;
        }
    } else {
        status = LAN8742_STATUS_WRITE_ERROR;
    }
    while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
    {
    }
    pObj->Is_Initialized = 1;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#ifdef TEN_MEGA_BITS_FULL_DUP
    //////////////////////// 10 MBIT FULL////////////////////////////////////////////////////////////////////////////
    regvalue = 0;
    regvalue &= ~(LAN8742_BCR_AUTONEGO_EN);
    regvalue &= LAN8742_BCR_SPEED_SELECT; // 0:10 Mbit  1: 100 Mbit
    regvalue |= (LAN8742_BCR_DUPLEX_MODE); //0: Half;  1: Full
    if (pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, regvalue) >= 0) {
        /* get  status */
        if (pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0) {

        } else {
            status = LAN8742_STATUS_READ_ERROR;
        }
    } else {
        status = LAN8742_STATUS_WRITE_ERROR;
    }
    
    while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
    {
    }
    pObj->Is_Initialized = 1;
    /////////////////////////////////////////////////////////////////////////////////////////////////////
#endif 
#ifdef TEN_MEGA_BITS_HALF_DUP
    //////////////////////// 10 MBIT HALF////////////////////////////////////////////////////////////////////////////
    regvalue = 0;
    regvalue &= ~(LAN8742_BCR_AUTONEGO_EN);
    regvalue &= LAN8742_BCR_SPEED_SELECT; // 0:10 Mbit  1: 100 Mbit
    regvalue &= ~(LAN8742_BCR_DUPLEX_MODE); //0: Half;  1: Full
    if (pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, regvalue) >= 0) {
        /* get  status */
        if (pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &regvalue) >= 0) {

        } else {
            status = LAN8742_STATUS_READ_ERROR;
        }
    } else {
        status = LAN8742_STATUS_WRITE_ERROR;
    }
    while((pObj->IO.GetTick() - tickstart) <= LAN8742_INIT_TO)
      {
      }
      pObj->Is_Initialized = 1;
    /////////////////////////////////////////////////////////////////////////////////////////////////////
#endif 
    

    return status;
 }

/**
  * @brief  De-Initialize the lan8742 and it's hardware resources
  * @param  pObj: device object LAN8742_Object_t. 
  * @retval None
  */
int32_t LAN8742_DeInit(lan8742_Object_t *pObj)
{
  if(pObj->Is_Initialized)
  {
    if(pObj->IO.DeInit != 0)
    {
      if(pObj->IO.DeInit() < 0)
      {
        return LAN8742_STATUS_ERROR;
      }
    }
  
    pObj->Is_Initialized = 0;  
  }
  
  return LAN8742_STATUS_OK;
}

/**
  * @brief  Disable the LAN8742 power down mode.
  * @param  pObj: device object LAN8742_Object_t.  
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_DisablePowerDownMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval &= ~LAN8742_BCR_POWER_DOWN;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Enable the LAN8742 power down mode.
  * @param  pObj: device object LAN8742_Object_t.  
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_EnablePowerDownMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_POWER_DOWN;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Start the auto negotiation process.
  * @param  pObj: device object LAN8742_Object_t.  
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_StartAutoNego(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_AUTONEGO_EN;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Get the link state of LAN8742 device.
  * @param  pObj: Pointer to device object. 
  * @param  pLinkState: Pointer to link state
  * @retval LAN8742_STATUS_LINK_DOWN  if link is down
  *         LAN8742_STATUS_AUTONEGO_NOTDONE if Auto nego not completed 
  *         LAN8742_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         LAN8742_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         LAN8742_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         LAN8742_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD       
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_GetLinkState(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;  
  /* Read Status register  */
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval) < 0)
  {
    return LAN8742_STATUS_READ_ERROR;
  }  
  ////////////////////////////////////////////////////////////
  //                +-------------------+                   //
  //                |       KOSTYL      |                   //
  //                +-------------------+                   //
  ////////////////////////////////////////////////////////////
    //! its not clear but sometimes not establish link from the first time
    //  need to check some more times
    //  need more time to dp83848 start working
    
    uint32_t counter = 0; // for debug check how many times need to do request for properly repply
    if (!(readval & LAN8742_BSR_LINK_STATUS)) {
      do {
          pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval);
          counter++;
          /////////////////////////////////////////////
          // for (volatile int i = 0; i < 5000000; i++) {}
          /////////////////////////////////////////////
          //! TODO: if counter > 50 then reboot dp83848
      } while ((readval & LAN8742_BSR_LINK_STATUS) == 0);
    }    

  if((readval & LAN8742_BSR_LINK_STATUS) == 0)
  {
    // no LINK establish ==== same as dp83848
    /* Return Link Down status */
    //TODO: need to clearify why not cant read status register
    //////////////////////////////////////////////////////////////
    return LAN8742_STATUS_LINK_DOWN;    
    //////////////////////////////////////////////////////////////
  }
  
  /* Check Auto negotiaition */
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) < 0)
  {
    return LAN8742_STATUS_READ_ERROR;
  }

 /* Read Status register again (FURTHER CHECK WHAT WRITED in THAT REGISTER (reg 02 but we need reg 0x10!!!!))*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     ////////////////////
     // if no autonegotiation
     //////////////////////////////////////////
#ifdef HUNDRED_MEGA_BITS_FULL_DUP

#else
    // if(pObj->IO.ReadReg(pObj->DevAddr, 0x10, &readval) < 0)
    // {
    //   return LAN8742_STATUS_READ_ERROR;
    // }
#endif
    /////////////////////////////////////////////////////////////


  // when autonegotiation is false
  if((readval & LAN8742_BCR_AUTONEGO_EN) != LAN8742_BCR_AUTONEGO_EN)
  {
    if(((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT) && ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE)) 
    {
      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT)
    {
        if(pObj->IO.ReadReg(pObj->DevAddr, 0x10, &readval) < 0)
        {
          return LAN8742_STATUS_READ_ERROR;
        }
      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
    }        
    else if ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE)
    {
        // if(pObj->IO.ReadReg(pObj->DevAddr, 0x10, &readval) < 0)
        // {
        //   return LAN8742_STATUS_READ_ERROR;
        // }
      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
    }  		
  }
  else /* Auto Nego enabled */
  {
    // WE MUS REED ANOTHER REGISTER (0x10) FOR RESULT OF AUTONEGOTIATION
    // if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_PHYSCSR, &readval) < 0)
    // {
    //   return LAN8742_STATUS_READ_ERROR;
    // }
    if(pObj->IO.ReadReg(pObj->DevAddr, 0x10, &readval) < 0)
    {
      return LAN8742_STATUS_READ_ERROR;
    }
    
    /* Check if auto nego not done */
    // if((readval & LAN8742_PHYSCSR_AUTONEGO_DONE) == 0)
    // {
    //   return LAN8742_STATUS_AUTONEGO_NOTDONE;
    // }
    if((readval & 0x4) == 0) //4-bit AUTO-NEGCOMPLETE 1:complete 2:not complete
    {
      return LAN8742_STATUS_AUTONEGO_NOTDONE;
    }
    // if((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_FD)
    // {
    //   return LAN8742_STATUS_100MBITS_FULLDUPLEX;
    // }
    if((readval & 0x6) == 0x4) //1=0 -100 Mbit 2=1-Full Duplex mode
    {
      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
    }
    // else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_HD)
    // {
    //   return LAN8742_STATUS_100MBITS_HALFDUPLEX;
    // }
    else if ((readval & 0x6) == 0x00)
    {
      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
    }
    // else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_10BT_FD)
    // {
    //   return LAN8742_STATUS_10MBITS_FULLDUPLEX;
    // }
    else if ((readval & 0x06) == 0x02)
    {
      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
    }				
  }
}

/**
  * @brief  Set the link state of LAN8742 device.
  * @param  pObj: Pointer to device object. 
  * @param  pLinkState: link state can be one of the following
  *         LAN8742_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         LAN8742_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         LAN8742_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         LAN8742_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD   
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_ERROR  if parameter error  
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_SetLinkState(lan8742_Object_t *pObj, uint32_t LinkState)
{
  uint32_t bcrvalue = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &bcrvalue) >= 0)
  {
    /* Disable link config (Auto nego, speed and duplex) */
    bcrvalue &= ~(LAN8742_BCR_AUTONEGO_EN | LAN8742_BCR_SPEED_SELECT | LAN8742_BCR_DUPLEX_MODE);
    
    if(LinkState == LAN8742_STATUS_100MBITS_FULLDUPLEX)
    {
      bcrvalue |= (LAN8742_BCR_SPEED_SELECT | LAN8742_BCR_DUPLEX_MODE);
    }
    else if (LinkState == LAN8742_STATUS_100MBITS_HALFDUPLEX)
    {
      bcrvalue |= LAN8742_BCR_SPEED_SELECT;
    }
    else if (LinkState == LAN8742_STATUS_10MBITS_FULLDUPLEX)
    {
      bcrvalue |= LAN8742_BCR_DUPLEX_MODE;
    }
    else
    {
      /* Wrong link status parameter */
      status = LAN8742_STATUS_ERROR;
    }	
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
  
  if(status == LAN8742_STATUS_OK)
  {
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, bcrvalue) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }
  
  return status;
}

/**
  * @brief  Enable loopback mode.
  * @param  pObj: Pointer to device object. 
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_EnableLoopbackMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval |= LAN8742_BCR_LOOPBACK;
    
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
  
  return status;
}

/**
  * @brief  Disable loopback mode.
  * @param  pObj: Pointer to device object. 
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_DisableLoopbackMode(lan8742_Object_t *pObj)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) >= 0)
  {
    readval &= ~LAN8742_BCR_LOOPBACK;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_BCR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Enable IT source.
  * @param  pObj: Pointer to device object. 
  * @param  Interrupt: IT source to be enabled
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT                     
  *         LAN8742_ENERGYON_IT                
  *         LAN8742_AUTONEGO_COMPLETE_IT       
  *         LAN8742_REMOTE_FAULT_IT            
  *         LAN8742_LINK_DOWN_IT               
  *         LAN8742_AUTONEGO_LP_ACK_IT         
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_EnableIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_IMR, &readval) >= 0)
  {
    readval |= Interrupt;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_IMR, readval) < 0)
    {
      status =  LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Disable IT source.
  * @param  pObj: Pointer to device object. 
  * @param  Interrupt: IT source to be disabled
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT                     
  *         LAN8742_ENERGYON_IT                
  *         LAN8742_AUTONEGO_COMPLETE_IT       
  *         LAN8742_REMOTE_FAULT_IT            
  *         LAN8742_LINK_DOWN_IT               
  *         LAN8742_AUTONEGO_LP_ACK_IT         
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
int32_t LAN8742_DisableIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_IMR, &readval) >= 0)
  {
    readval &= ~Interrupt;
  
    /* Apply configuration */
    if(pObj->IO.WriteReg(pObj->DevAddr, LAN8742_IMR, readval) < 0)
    {
      status = LAN8742_STATUS_WRITE_ERROR;
    }
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
   
  return status;
}

/**
  * @brief  Clear IT flag.
  * @param  pObj: Pointer to device object. 
  * @param  Interrupt: IT flag to be cleared
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT                     
  *         LAN8742_ENERGYON_IT                
  *         LAN8742_AUTONEGO_COMPLETE_IT       
  *         LAN8742_REMOTE_FAULT_IT            
  *         LAN8742_LINK_DOWN_IT               
  *         LAN8742_AUTONEGO_LP_ACK_IT         
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT
  * @retval LAN8742_STATUS_OK  if OK
  *         LAN8742_STATUS_READ_ERROR if connot read register
  */
int32_t  LAN8742_ClearIT(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = LAN8742_STATUS_OK;  
  
  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_ISFR, &readval) < 0)
  {
    status =  LAN8742_STATUS_READ_ERROR;
  }
  
  return status;
}

/**
  * @brief  Get IT Flag status.
  * @param  pObj: Pointer to device object. 
  * @param  Interrupt: IT Flag to be checked, 
  *         should be a value or a combination of the following:
  *         LAN8742_WOL_IT                     
  *         LAN8742_ENERGYON_IT                
  *         LAN8742_AUTONEGO_COMPLETE_IT       
  *         LAN8742_REMOTE_FAULT_IT            
  *         LAN8742_LINK_DOWN_IT               
  *         LAN8742_AUTONEGO_LP_ACK_IT         
  *         LAN8742_PARALLEL_DETECTION_FAULT_IT
  *         LAN8742_AUTONEGO_PAGE_RECEIVED_IT  
  * @retval 1 IT flag is SET
  *         0 IT flag is RESET
  *         LAN8742_STATUS_READ_ERROR if connot read register
  */
int32_t LAN8742_GetITStatus(lan8742_Object_t *pObj, uint32_t Interrupt)
{
  uint32_t readval = 0;
  int32_t status = 0;

  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_ISFR, &readval) >= 0)
  {
    status = ((readval & Interrupt) == Interrupt);
  }
  else
  {
    status = LAN8742_STATUS_READ_ERROR;
  }
	
  return status;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */      
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
