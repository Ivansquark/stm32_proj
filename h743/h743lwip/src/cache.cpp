#include "cache.h"

void cachePrepare() {
    //!  after reset, the user must invalidate each cache before enabling it,
    //   otherwise an UNPREDICTIBLE behavior can occur.
    SCB_EnableDCache();
    SCB_EnableICache();
    SCB_CleanDCache();
    SCB_DisableDCache();
    SCB_DisableICache();    
}

void MPU_Conf(void) {
    MPU_Region_InitTypeDef MPU_InitStruct;

   /* Disables the MPU */
   HAL_MPU_Disable();
   /**Initializes and configures the Region and the memory to be protected
   */
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER2;
   MPU_InitStruct.BaseAddress = 0x30040000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
   MPU_InitStruct.SubRegionDisable = 0x0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   /**Initializes and configures the Region and the memory to be protected
   */
   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER1;
   MPU_InitStruct.BaseAddress = 0x30042000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
   MPU_InitStruct.SubRegionDisable = 0x0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   MPU_InitStruct.Enable = MPU_REGION_ENABLE;
   MPU_InitStruct.Number = MPU_REGION_NUMBER0;
   MPU_InitStruct.BaseAddress = 0x30040000;
   MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
   MPU_InitStruct.SubRegionDisable = 0x0;
   MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
   MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
   MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
   
   HAL_MPU_ConfigRegion(&MPU_InitStruct);
   /* Enables the MPU */
   HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
   
    /* Enable I-Cache---------------------------------------------------------*/
    SCB_EnableICache();

    /* Enable D-Cache---------------------------------------------------------*/
    SCB_EnableDCache();
    //SCB_CleanDCache() //all dirty cache bytes will be write back in sram
    
    //!!! --- MCU writing to memory ---
    //! 1. making the SRAM buffers not cacheable  --- OUR CASE !!! (most easiest)

    //! 2. making the SRAM buffers cache enabled with write-back policy, 
    //     with the coherency ensured by software   (clean or invalidate D-Cache)

    //! 3.  modifying the SRAM region in the MPU attribute to a shared region

    //! 4.  write back all cache operations
    //SCB->CACR |= SCB_CACR_FORCEWT_Msk; // force write through

    //!!! --- DMA (Ethernet) writing to memory (MCU reads from memory) ---
    //! To ensure the data coherency between the cache and the SRAM,
    //  the software must perform a cache invalidate before reading the updated data from the SRAM
}