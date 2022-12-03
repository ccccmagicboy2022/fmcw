#include "flash_def.h"
#include "sys.h"

#define     __IO           volatile
//typedef     __IO uint32_t  vu32;

detect_param_t flash_param, default_param = {
    .magic_num = MAGIC_NUM,
    .crc = 0,
    .delay_times = DELAY_TIME,
    .range_line = RANGE_LINE,
    .background_line = {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40},
    .diff_energy_line = {0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75, 0.75},
};

/*
*********************************************************************************************************
* Function name: Flash_Write
* Function description: Continuous write operation to sector 62
* Parameters:u8Len:Data length,
*            u32TestBuf: pointer to write data \
*            (46064 sectors, 8K each, currently 23 sectors used, nearly 40 sectors available)
* Return value: none
*********************************************************************************************************
*/
void Flash_Write(uint8_t u8Len, detect_param_t *current_detect_param)
{
    uint32_t Counter_Num = 0;
    current_detect_param->crc = calculate_CRC32((uint32_t*)&current_detect_param->delay_times, sizeof(detect_param_t) - FLASH_LENGTH_CRC_MAGIC);

    __set_PRIMASK(1);
    FLASH_Unlock();
    FLASH_DataCacheCmd(DISABLE);

    while (1)
    {
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
        if (FLASH_EraseSector(GetSector(ADDR_FLASH_SECTOR_3), VoltageRange_3) == FLASH_COMPLETE)
        {
            break;
        }
    }

    for (Counter_Num = 0; Counter_Num < u8Len; Counter_Num += 4)
    {
        if (FLASH_COMPLETE != FLASH_ProgramWord(ADDR_FLASH_SECTOR_3 + Counter_Num, *((u32 *)((u8 *)current_detect_param + Counter_Num))))
        {
            //error
            while(1)
            {
                //pass
            }
        }
    }

    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
    __set_PRIMASK(0);
}
/*
*********************************************************************************************************
* Function name: Flash_SectorErase
* Function description: Erase sector 7.
* Parameters: none
* Return value: none
*********************************************************************************************************
*/
void Flash_SectorErase()
{
    __set_PRIMASK(1);
    FLASH_Unlock();
    FLASH_DataCacheCmd(DISABLE);

    while (1)
    {
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                        FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
        if (FLASH_EraseSector(GetSector(ADDR_FLASH_SECTOR_3), VoltageRange_3) == FLASH_COMPLETE)
        {
            break;
        }
    }

    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();
    __set_PRIMASK(0);
}


/*****************************************************************************
Function name : dp_download_load_radar_parameter_handle
Description   : DPID LOAD RADAR PARAMETER processing function
Input   : value: Data source data
        : length: Data length
Return  : SUCCESS/ERROR
Instructions : It can be delivered and reported. After data is processed,
           the processing result must be reported to the APP
*****************************************************************************/

void Flash_Init_3()
{
    uint32_t crc_value = 0;
    uint32_t read_magic_num = 0;
    uint32_t read_crc = 0;

    read_magic_num = *((uint32_t *)ADDR_FLASH_SECTOR_3);

    if (read_magic_num != MAGIC_NUM)
        flash_param = default_param;
    else {
        read_crc = *((uint32_t *)ADDR_FLASH_SECTOR_3 + FLASH_ADDR_OFFSET_CRC);

        crc_value = calculate_CRC32((uint32_t*)(ADDR_FLASH_SECTOR_3 + FLASH_LENGTH_CRC_MAGIC), sizeof(flash_param) - FLASH_LENGTH_CRC_MAGIC);

        if (crc_value == read_crc) {
            for (int i = 0; i < sizeof(flash_param) / 4; i++)
                *((float *)&flash_param + i) = *((float *)ADDR_FLASH_SECTOR_3 + i);
        } else {
            flash_param = default_param;
          }
    }

    set_delay_time(flash_param.delay_times);
    set_range_line(flash_param.range_line);
    set_background_line(&flash_param);
    set_diff_energy_line(&flash_param);
}
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_Sector_11;
  }

  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_Sector_12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_Sector_13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_Sector_14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_Sector_15;
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_Sector_16;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_Sector_17;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_Sector_18;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_Sector_19;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_Sector_20;
  }
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_Sector_21;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_Sector_22;
  }
  else
  {
    sector = FLASH_Sector_23;
  }

  return sector;
}

void check_success(uint8_t dpid)
{
    get_detect_param(&flash_param);
    Flash_Write(sizeof(flash_param), &flash_param);
    mcu_dp_enum_update(dpid, 1);
}