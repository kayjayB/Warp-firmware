#include <stdint.h>

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331.h"

volatile uint8_t	inBuffer[32];
volatile uint8_t	payloadBytes[32];
// uint8_t oled_buf[32];

static const char alphabet[0x60][6] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00 } , /*SPC */
    { 0x00,0x00,0x5F,0x00,0x00,0x00 } , /* !  */
    { 0x04,0x03,0x04,0x03,0x00,0x00 } , /* "  */
    { 0x28,0x7E,0x14,0x3F,0x0A,0x00 } , /* #  */
    { 0x24,0x2A,0x7F,0x2A,0x12,0x00 } , /* $  */
    { 0x23,0x13,0x08,0x64,0x62,0x00 } , /* %  */
    { 0x30,0x4E,0x59,0x26,0x50,0x00 } , /* &  */
    { 0x00,0x00,0x02,0x01,0x00,0x00 } , /* '  */
    { 0x00,0x00,0x1C,0x22,0x41,0x00 } , /* (  */
    { 0x41,0x22,0x1C,0x00,0x00,0x00 } , /* )  */
    { 0x22,0x14,0x08,0x14,0x22,0x00 } , /* *  */
    { 0x08,0x08,0x3E,0x08,0x08,0x00 } , /* +  */
    { 0x50,0x30,0x00,0x00,0x00,0x00 } , /* ,  */
    { 0x08,0x08,0x08,0x08,0x08,0x00 } , /* -  */
    { 0x60,0x60,0x00,0x00,0x00,0x00 } , /* .  */
    { 0x20,0x10,0x08,0x04,0x02,0x00 } , /* /  */
    { 0x3E,0x51,0x49,0x45,0x3E,0x00 } , /* 0  */
    { 0x00,0x42,0x7F,0x40,0x00,0x00 } , /* 1  */
    { 0x62,0x51,0x49,0x49,0x46,0x00 } , /* 2  */
    { 0x22,0x41,0x49,0x49,0x36,0x00 } , /* 3  */
    { 0x18,0x14,0x12,0x7F,0x10,0x00 } , /* 4  */
    { 0x2F,0x45,0x45,0x45,0x39,0x00 } , /* 5  */
    { 0x3E,0x49,0x49,0x49,0x32,0x00 } , /* 6  */
    { 0x01,0x61,0x19,0x05,0x03,0x00 } , /* 7  */
    { 0x36,0x49,0x49,0x49,0x36,0x00 } , /* 8  */
    { 0x26,0x49,0x49,0x49,0x3E,0x00 } , /* 9  */
    { 0x00,0x36,0x36,0x00,0x00,0x00 } , /* :  */
    { 0x00,0x56,0x36,0x00,0x00,0x00 } , /* ;  */
    { 0x00,0x08,0x14,0x22,0x41,0x00 } , /* <  */
    { 0x14,0x14,0x14,0x14,0x14,0x00 } , /* =  */
    { 0x41,0x22,0x14,0x08,0x00,0x00 } , /* >  */
    { 0x02,0x01,0x59,0x09,0x06,0x00 } , /* ?  */
    { 0x3E,0x41,0x5D,0x55,0x2E,0x00 } , /* @  */
    { 0x60,0x1C,0x13,0x1C,0x60,0x00 } , /* A  */
    { 0x7F,0x49,0x49,0x49,0x36,0x00 } , /* B  */
    { 0x3E,0x41,0x41,0x41,0x22,0x00 } , /* C  */
    { 0x7F,0x41,0x41,0x22,0x1C,0x00 } , /* D  */
    { 0x7F,0x49,0x49,0x49,0x41,0x00 } , /* E  */
    { 0x7F,0x09,0x09,0x09,0x01,0x00 } , /* F  */
    { 0x1C,0x22,0x41,0x49,0x3A,0x00 } , /* G  */
    { 0x7F,0x08,0x08,0x08,0x7F,0x00 } , /* H  */
    { 0x00,0x41,0x7F,0x41,0x00,0x00 } , /* I  */
    { 0x20,0x40,0x40,0x40,0x3F,0x00 } , /* J  */
    { 0x7F,0x08,0x14,0x22,0x41,0x00 } , /* K  */
    { 0x7F,0x40,0x40,0x40,0x00,0x00 } , /* L  */
    { 0x7F,0x04,0x18,0x04,0x7F,0x00 } , /* M  */
    { 0x7F,0x04,0x08,0x10,0x7F,0x00 } , /* N  */
    { 0x3E,0x41,0x41,0x41,0x3E,0x00 } , /* O  */
    { 0x7F,0x09,0x09,0x09,0x06,0x00 } , /* P  */
    { 0x3E,0x41,0x51,0x21,0x5E,0x00 } , /* Q  */
    { 0x7F,0x09,0x19,0x29,0x46,0x00 } , /* R  */
    { 0x26,0x49,0x49,0x49,0x32,0x00 } , /* S  */
    { 0x01,0x01,0x7F,0x01,0x01,0x00 } , /* T  */
    { 0x3F,0x40,0x40,0x40,0x3F,0x00 } , /* U  */
    { 0x03,0x1C,0x60,0x1C,0x03,0x00 } , /* V  */
    { 0x0F,0x70,0x0F,0x70,0x0F,0x00 } , /* W  */
    { 0x41,0x36,0x08,0x36,0x41,0x00 } , /* X  */
    { 0x01,0x06,0x78,0x02,0x01,0x00 } , /* Y  */
    { 0x61,0x51,0x49,0x45,0x43,0x00 } , /* Z  */
    { 0x00,0x00,0x7F,0x41,0x41,0x00 } , /* [  */
    { 0x15,0x16,0x7C,0x16,0x11,0x00 } , /* \  */
    { 0x41,0x41,0x7F,0x00,0x00,0x00 } , /* ]  */
    { 0x00,0x02,0x01,0x02,0x00,0x00 } , /* ^  */
    { 0x40,0x40,0x40,0x40,0x40,0x00 } , /* _  */
    { 0x00,0x01,0x02,0x00,0x00,0x00 } , /* `  */
    { 0x00,0x20,0x54,0x54,0x78,0x00 } , /* a  */
    { 0x00,0x7F,0x44,0x44,0x38,0x00 } , /* b  */
    { 0x00,0x38,0x44,0x44,0x28,0x00 } , /* c  */
    { 0x00,0x38,0x44,0x44,0x7F,0x00 } , /* d  */
    { 0x00,0x38,0x54,0x54,0x18,0x00 } , /* e  */
    { 0x00,0x04,0x3E,0x05,0x01,0x00 } , /* f  */
    { 0x00,0x08,0x54,0x54,0x3C,0x00 } , /* g  */
    { 0x00,0x7F,0x04,0x04,0x78,0x00 } , /* h  */
    { 0x00,0x00,0x7D,0x00,0x00,0x00 } , /* i  */
    { 0x00,0x40,0x40,0x3D,0x00,0x00 } , /* j  */
    { 0x00,0x7F,0x10,0x28,0x44,0x00 } , /* k  */
    { 0x00,0x01,0x7F,0x00,0x00,0x00 } , /* l  */
    { 0x7C,0x04,0x7C,0x04,0x78,0x00 } , /* m  */
    { 0x00,0x7C,0x04,0x04,0x78,0x00 } , /* n  */
    { 0x00,0x38,0x44,0x44,0x38,0x00 } , /* o  */
    { 0x00,0x7C,0x14,0x14,0x08,0x00 } , /* p  */
    { 0x00,0x08,0x14,0x14,0x7C,0x00 } , /* q  */
    { 0x00,0x7C,0x08,0x04,0x04,0x00 } , /* r  */
    { 0x00,0x48,0x54,0x54,0x24,0x00 } , /* s  */
    { 0x00,0x04,0x3E,0x44,0x40,0x00 } , /* t  */
    { 0x00,0x3C,0x40,0x40,0x7C,0x00 } , /* u  */
    { 0x00,0x7C,0x20,0x10,0x0C,0x00 } , /* v  */
    { 0x1C,0x60,0x1C,0x60,0x1C,0x00 } , /* w  */
    { 0x00,0x6C,0x10,0x10,0x6C,0x00 } , /* x  */
    { 0x00,0x4C,0x50,0x30,0x1C,0x00 } , /* y  */
    { 0x00,0x44,0x64,0x54,0x4C,0x00 } , /* z  */
    { 0x00,0x08,0x36,0x41,0x41,0x00 } , /* {  */
    { 0x00,0x00,0x7F,0x00,0x00,0x00 } , /* |  */
    { 0x41,0x41,0x36,0x08,0x00,0x00 } , /* }  */
    { 0x08,0x04,0x08,0x10,0x08,0x00 } , /* ~  */
    { 0x00,0x00,0x00,0x00,0x00,0x00 }    /*null*/
};

/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 13),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};

static int
writeCommand(uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	/*
	 *	Drive DC low (command).
	 */
	GPIO_DRV_ClearPinOutput(kSSD1331PinDC);

	payloadBytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
					(uint8_t * restrict)&inBuffer[0],
					1		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}

static int
writeData(uint8_t commandByte)
{
	spi_status_t status;

	/*
	 *	Drive /CS low.
	 *
	 *	Make sure there is a high-to-low transition by first driving high, delay, then drive low.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinDC);
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);
	OSA_TimeDelay(10);
	GPIO_DRV_ClearPinOutput(kSSD1331PinCSn);

	payloadBytes[0] = commandByte;
	status = SPI_DRV_MasterTransferBlocking(0	/* master instance */,
					NULL		/* spi_master_user_config_t */,
					(const uint8_t * restrict)&payloadBytes[0],
					(uint8_t * restrict)&inBuffer[0],
					1		/* transfer size */,
					1000		/* timeout in microseconds (unlike I2C which is ms) */);

	/*
	 *	Drive /CS high
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinCSn);

	return status;
}


int
devSSD1331init(void)
{
	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Re-configure SPI to be on PTA8 and PTA9 for MOSI and SCK respectively.
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 8u, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9u, kPortMuxAlt3);

	enableSPIpins();

	/*
	 *	Override Warp firmware's use of these pins.
	 *
	 *	Reconfigure to use as GPIO.
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 13u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 12u, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 0u, kPortMuxAsGpio);


	/*
	 *	RST high->low->high.
	 */
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_ClearPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);
	GPIO_DRV_SetPinOutput(kSSD1331PinRST);
	OSA_TimeDelay(100);

	/*
	 *	Initialization sequence, borrowed from https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino
	 */
	writeCommand(kSSD1331CommandDISPLAYOFF);	// 0xAE
	writeCommand(kSSD1331CommandSETREMAP);		// 0xA0
	writeCommand(0x72);				// RGB Color
	writeCommand(kSSD1331CommandSTARTLINE);		// 0xA1
	writeCommand(0x0);
	writeCommand(kSSD1331CommandDISPLAYOFFSET);	// 0xA2
	writeCommand(0x0);
	writeCommand(kSSD1331CommandNORMALDISPLAY);	// 0xA4
	writeCommand(kSSD1331CommandSETMULTIPLEX);	// 0xA8
	writeCommand(0x3F);				// 0x3F 1/64 duty
	writeCommand(kSSD1331CommandSETMASTER);		// 0xAD
	writeCommand(0x8E);
	writeCommand(kSSD1331CommandPOWERMODE);		// 0xB0
	writeCommand(0x0B);
	writeCommand(kSSD1331CommandPRECHARGE);		// 0xB1
	writeCommand(0x31);
	writeCommand(kSSD1331CommandCLOCKDIV);		// 0xB3
	writeCommand(0xF0);				// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8A
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGEB);	// 0x8B
	writeCommand(0x78);
	writeCommand(kSSD1331CommandPRECHARGEA);	// 0x8C
	writeCommand(0x64);
	writeCommand(kSSD1331CommandPRECHARGELEVEL);	// 0xBB
	writeCommand(0x3A);
	writeCommand(kSSD1331CommandVCOMH);		// 0xBE
	writeCommand(0x3E);
	writeCommand(kSSD1331CommandMASTERCURRENT);	// 0x87
	writeCommand(0x06);
	writeCommand(kSSD1331CommandCONTRASTA);		// 0x81
	writeCommand(0x91);
	writeCommand(kSSD1331CommandCONTRASTB);		// 0x82
	writeCommand(0x50);
	writeCommand(kSSD1331CommandCONTRASTC);		// 0x83
	writeCommand(0x7D);
	writeCommand(kSSD1331CommandDISPLAYON);		// Turn on oled panel
	// SEGGER_RTT_WriteString(0, "\r\n\tDone with initialization sequence...\n");

	
	/*
	 *	To use fill commands, you will have to issue a command to the display to enable them. See the manual.
	 */
	writeCommand(kSSD1331CommandFILL);
	writeCommand(0x01);
	// SEGGER_RTT_WriteString(0, "\r\n\tDone with enabling fill...\n");

	/*
	 *	Clear Screen
	 */
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
	SEGGER_RTT_WriteString(0, "\r\n\tDone with screen clear...\n");

	// // Command to enter draw rectangle mode
	// writeCommand(0x22);
	// // Set start coordinates
	// writeCommand(0x00);
	// writeCommand(0x00);

	// // Set end coordinates
	// writeCommand(0x7f);
	// writeCommand(0x3f); 
	
	// // Set outline to green
	// writeCommand(0x00);
	// writeCommand(0xff);
	// writeCommand(0x00); 

	// // Set fill to green
	// writeCommand(0x00);
	// writeCommand(0xff);
	// writeCommand(0x00); 

	// // Adjust contrast to brighten the green
	// writeCommand(0x82);
	// // Set green to max contrast
	// writeCommand(0xff);

	// // Set red and blue to min contrast
	// writeCommand(0x81);
	// writeCommand(0x00);
	// writeCommand(0x83);
	// writeCommand(0x00);

	// SEGGER_RTT_WriteString(0, "\r\n\tDone with draw rectangle...\n");

	chr_size = HIGH;
	// locate(1,10);
	// _putc(33);
	// locate(10,10);
	// _putc(34);
	locate(1,10);
	_putc(83);
	locate(8,10);
	_putc(84);
	locate(16,10);
	_putc(69);
	locate(24,10);
	_putc(80);
	locate(32,10);
	_putc(83);
	locate(40,10);
	_putc(58);

	// locate(1, 10);
	// _putc();
	
	return 0;
}

int _putc( int c )
{    
    PutChar( char_x , char_y ,c);
    return c;
}

uint16_t toRGB(uint16_t R,uint16_t G,uint16_t B)
{  
    uint16_t c;
    c = R >> 3;
    c <<= 6;
    c |= G >> 2;
    c <<= 5;
    c |= B >> 3;
    return c;
}

void PutChar(uint8_t column,uint8_t row, int value)
{
	uint8_t chMode = 0;
	if(value == '\n') {
		char_x = 0;
		char_y = char_y + Y_height;
	}
	if ((value < 31) || (value > 127)) return;   // test char range
	if (char_x + X_width > width) {
		char_x = 0;
		char_y = char_y + Y_height;
		if (char_y >= height - Y_height) {
			char_y = 0;
		}
	}
	int i,j,w,lpx,lpy,k,l,xw;
	unsigned char Temp=0;
	j = 0; i = 0;
	w = X_width;
	FontSizeConvert(&lpx, &lpy);
	xw = X_width;
	
	for(i=0; i<xw; i++) {
		for ( l=0; l<lpx; l++) {
			Temp = alphabet[value-32][i];
			for(j=Y_height-1; j>=0; j--) {
				for (k=0; k<lpy; k++) {
					chMode = Temp & 0x80? 1 : 0;
					pixel(char_x+(i*lpx)+l, char_y+(((j+1)*lpy)-1)-k,chMode);
				}
				Temp = Temp << 1;
			}
		}
	}
	FontSizeConvert(&lpx, &lpy);
	char_x += (w*lpx);
}

void locate(uint8_t column, uint8_t row)
{
    char_x  = column;
    char_y = row;
}

void foreground(uint16_t color)
{
    Char_Color = color;
}
void background(uint16_t color)
{
    BGround_Color = color;
}

void pixel(uint8_t x,uint8_t y, char Color)
{
    unsigned char cmd[7]= {Set_Column_Address,0x00,0x00,Set_Row_Address,0x00,0x00};
	// unsigned char cmd[5]= {0x00,0x00,0x00,0x00};
    if ((x>width)||(y>height)) return ;
    // cmd[0] = (unsigned char)x;
    // cmd[1] = (unsigned char)y;
    // cmd[2] = (unsigned char)x;
    // cmd[3] = (unsigned char)y;

	cmd[1] = x;
    cmd[2] = width;
    cmd[4] = y;
    cmd[5] = height;

	for (int i=0; i<6; i++)
	{
		writeCommand(cmd[i]);
	}
	// writeCommand(kSSD1331CommandDRAWLINE);
	// for (int i=0; i<4; i++)
	// {
	// 	writeCommand(cmd[i]);
	// }
	uint16_t white = toRGB(255,255,255);
	uint16_t black = toRGB(0,0,0);
	if (Color)
	{
		writeData(((white >> 8)));
    	writeData((white));
	}
	else 
	{
		writeData((black >> 8));
    	writeData((black));
	}
}

void FontSizeConvert(int *lpx,int *lpy)
{
    switch( chr_size ) {
        case WIDE:
            *lpx=2;
            *lpy=1;
            break;
        case HIGH:
            *lpx=1;
            *lpy=2;
            break;
        case WH  :
            *lpx=2;
            *lpy=2;
            break;
        case WHx36  :
            *lpx=6;
            *lpy=6;
            break;
        case NORMAL:
        default:
            *lpx=1;
            *lpy=1;
            break;
    }
}