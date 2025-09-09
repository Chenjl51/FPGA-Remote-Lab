#include "bsp_usart1.h"
#include "bsp_spi.h"
#include "fpga_jtag_basic.h"

// global variable
extern JtagReq jtag_req;
extern JtagSimPinCallback jtag_pin_cb;
extern JtagInterface jtag_inf;
extern FPGADevice fpga_dev;
extern PGIOCallback pg_io_cb;
extern unsigned int total_bitstream_bytes;
extern unsigned int current_bitstream_bytes;
extern const unsigned int bitstream_offset;

/*const unsigned int feature_control_val_a = DEFAULT_FEATURE_CTL | 1;
const unsigned int feature_control_val_b = DEFAULT_FEATURE_CTL | 0;
*/
unsigned int g_user_clk = 1000;   //kHz

void delay_ms(unsigned int m)
{
    unsigned int i;
    m = 10000*m*g_user_clk/JTAG_DEF_FREQ;
    for (i = 0; i < m; ++i);
}

/******************************************* JTAG connected pin define **************************************/
#define            pango_JTAG_TMS_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define            pango_JTAG_TMS_CLK                    RCC_APB2Periph_GPIOB
#define            pango_JTAG_TMS_PORT                   GPIOB
#define            pango_JTAG_TMS_PIN                    GPIO_Pin_6

#define            pango_JTAG_TCK_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define            pango_JTAG_TCK_CLK                    RCC_APB2Periph_GPIOB
#define            pango_JTAG_TCK_PORT                   GPIOB   
#define            pango_JTAG_TCK_PIN                    GPIO_Pin_7

#define            pango_JTAG_TDO_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define            pango_JTAG_TDO_CLK                    RCC_APB2Periph_GPIOB
#define            pango_JTAG_TDO_PORT                   GPIOB 
#define            pango_JTAG_TDO_PIN                    GPIO_Pin_8

#define            pango_JTAG_TDI_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define            pango_JTAG_TDI_CLK                    RCC_APB2Periph_GPIOB
#define            pango_JTAG_TDI_PORT                   GPIOB 
#define            pango_JTAG_TDI_PIN                    GPIO_Pin_9


void pango_JtagSetTMSHigh()
{
    GPIO_SetBits(pango_JTAG_TMS_PORT, pango_JTAG_TMS_PIN);
}

void pango_JtagSetTMSLow()
{
    GPIO_ResetBits(pango_JTAG_TMS_PORT, pango_JTAG_TMS_PIN);
}

void pango_JtagSetTCKHigh()
{
    GPIO_SetBits(pango_JTAG_TCK_PORT, pango_JTAG_TCK_PIN);
}

void pango_JtagSetTCKLow()
{
    GPIO_ResetBits(pango_JTAG_TCK_PORT, pango_JTAG_TCK_PIN);
}

void pango_JtagSetTDIHigh()
{
    GPIO_SetBits(pango_JTAG_TDI_PORT, pango_JTAG_TDI_PIN);
}

void pango_JtagSetTDILow()
{
    GPIO_ResetBits(pango_JTAG_TDI_PORT, pango_JTAG_TDI_PIN);
}

unsigned char pango_JtagReadTDO()
{
    unsigned char tdo_value = GPIO_ReadInputDataBit(pango_JTAG_TDO_PORT, pango_JTAG_TDO_PIN);
    return tdo_value;
}

void pango_JtagIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< Configure JTAG pins: TMS */
    pango_JTAG_TMS_APBxClock_FUN(pango_JTAG_TMS_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TMS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pango_JTAG_TMS_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TCK */
    pango_JTAG_TCK_APBxClock_FUN(pango_JTAG_TCK_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(pango_JTAG_TCK_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TDO */
    pango_JTAG_TDO_APBxClock_FUN(pango_JTAG_TDO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TDO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(pango_JTAG_TDO_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TDI */
    pango_JTAG_TDI_APBxClock_FUN(pango_JTAG_TDI_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TDI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(pango_JTAG_TDI_PORT, &GPIO_InitStructure);

    /* Init the Jtag: TCK, TDO low ,TMS high */
    pango_JtagSetTCKLow();
    pango_JtagSetTDILow();
    pango_JtagSetTMSHigh();
}

void pango_JtagIODeinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< Configure JTAG pins: TMS */
    pango_JTAG_TMS_APBxClock_FUN ( pango_JTAG_TMS_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TMS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(pango_JTAG_TMS_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TCK */
    pango_JTAG_TCK_APBxClock_FUN ( pango_JTAG_TCK_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(pango_JTAG_TCK_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TDO */
    pango_JTAG_TDO_APBxClock_FUN ( pango_JTAG_TDO_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TDO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(pango_JTAG_TDO_PORT, &GPIO_InitStructure);

    /*!< Configure JTAG pins: TDI */
    pango_JTAG_TDI_APBxClock_FUN ( pango_JTAG_TDI_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = pango_JTAG_TDI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        
    GPIO_Init(pango_JTAG_TDI_PORT, &GPIO_InitStructure);    
}

void pango_BitstreamPrepareRead(void *dev)
{
    current_bitstream_bytes = bitstream_offset;
    total_bitstream_bytes = bitstream_offset + ((FPGADevice *)dev)->_fpga_properties.bitstream;
}

void pango_BufferRead(unsigned char * pBuffer, unsigned int ReadAddr, unsigned short NumByteToRead)
{
    pango_SPI_FLASH_BufferRead(pBuffer, ReadAddr, NumByteToRead);
}

void pango_PGC_BitStreamRead(unsigned char *buf, unsigned int byte_to_read, unsigned int *byte_read)
{
    if (current_bitstream_bytes >= total_bitstream_bytes)
    {
        *byte_read = 0;
        return;
    }
    if ((current_bitstream_bytes + byte_to_read) >= total_bitstream_bytes)
    {
        byte_to_read = total_bitstream_bytes - current_bitstream_bytes;
    }
    // read bitstream from SPI Flash, bitstream start from address 0
    pango_BufferRead(buf, current_bitstream_bytes, byte_to_read);
    current_bitstream_bytes += byte_to_read;
    *byte_read = byte_to_read;
}

/*bool pango_PGC_GetFeatureCTL(unsigned int bitstream_offset_addr, unsigned int* pfeature_ctl)
{
    unsigned char buf[256];
    unsigned int i = 0;
    pango_BufferRead(buf, bitstream_offset_addr, 256);
    for(i = 0; i < 248; i++)
        if(buf[i] == 0x00)
            if(buf[i+1] == 0x67)
                if(buf[i+2] == 0x00)
                    if(buf[i+3] == 0x04)
                        break;
    if(i < 248)
    {
        *pfeature_ctl = (buf[i+4]<<24) | (buf[i+5]<<16) | (buf[i+6]<<8) | buf[i+7];
        printf("\n The Feature CTL get from Bitstream is: %x\n", *pfeature_ctl);
        return true;
    }
    else
    {
        *pfeature_ctl = DEFAULT_FEATURE_CTL;
        printf("\n Fail to get Feature CTL from Bitstream, set to default: %x\n", *pfeature_ctl);
        return false;
    }		
}
*/
void pango_PGC_BitstreamProgress(unsigned int progress)
{
    static unsigned int old_progress = 0;
    if((progress >= (old_progress + 10))||(progress < old_progress)){
        old_progress = progress;
        printf("\r\n Current progress %d\r\n", progress);
    }
}

void pango_PGC_BitstreamPrepareWrite(void *dev)
{

}

void pango_PGC_BitStreamWrite(unsigned char *buf, unsigned int byte_to_write, unsigned int *byte_written)
{
    // do not support write bitstream
    *byte_written = 0;
}

void GlobalStructInit()
{
    jtag_pin_cb.SetTMSHigh = pango_JtagSetTMSHigh;
    jtag_pin_cb.SetTMSLow = pango_JtagSetTMSLow;
    jtag_pin_cb.SetTCKHigh = pango_JtagSetTCKHigh;
    jtag_pin_cb.SetTCKLow = pango_JtagSetTCKLow;
    jtag_pin_cb.SetTDIHigh = pango_JtagSetTDIHigh;
    jtag_pin_cb.SetTDILow = pango_JtagSetTDILow;
    jtag_pin_cb.ReadTDO = pango_JtagReadTDO;
    pango_InitJtagInterface(&jtag_inf, &jtag_pin_cb, &jtag_req);

    pg_io_cb.prepare_read = pango_BitstreamPrepareRead;
    pg_io_cb.prepare_write = pango_PGC_BitstreamPrepareWrite;
    pg_io_cb.read = pango_PGC_BitStreamRead;
    pg_io_cb.write = pango_PGC_BitStreamWrite;
    pg_io_cb.prog = pango_PGC_BitstreamProgress;

    pango_FPGA_InitDevice(&fpga_dev, &jtag_inf, &pg_io_cb);
}

