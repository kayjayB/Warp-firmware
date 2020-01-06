/*
 *	See https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino for the Arduino driver.
 */

// Internal Font size settings
#define NORMAL  0
#define WIDE    1
#define HIGH    2
#define WH      3
#define WHx36   4
#define X_width 6 
#define Y_height 8 

// Screen Settings
#define WIDTH   96       // Max X axial direction in screen
#define HEIGHT  64      // Max Y axial direction in screen
#define width   96-1       // Max X axial direction in screen
#define height  64-1       // Max Y axial direction in screen
#define Set_Column_Address  0x15
#define Set_Row_Address     0x75

typedef enum
{
	kSSD1331ColororderRGB		= 1,
	kSSD1331DelaysHWFILL		= 3,
	kSSD1331DelaysHWLINE		= 1,
} SSD1331Constants;

typedef enum
{
	kSSD1331CommandDRAWLINE		= 0x21,
	kSSD1331CommandDRAWRECT		= 0x22,
	kSSD1331CommandCLEAR		= 0x25,
	kSSD1331CommandFILL		= 0x26,
	kSSD1331CommandSETCOLUMN	= 0x15,
	kSSD1331CommandSETROW		= 0x75,
	kSSD1331CommandCONTRASTA	= 0x81,
	kSSD1331CommandCONTRASTB	= 0x82,
	kSSD1331CommandCONTRASTC	= 0x83,
	kSSD1331CommandMASTERCURRENT	= 0x87,
	kSSD1331CommandSETREMAP		= 0xA0,
	kSSD1331CommandSTARTLINE	= 0xA1,
	kSSD1331CommandDISPLAYOFFSET	= 0xA2,
	kSSD1331CommandNORMALDISPLAY	= 0xA4,
	kSSD1331CommandDISPLAYALLON	= 0xA5,
	kSSD1331CommandDISPLAYALLOFF	= 0xA6,
	kSSD1331CommandINVERTDISPLAY	= 0xA7,
	kSSD1331CommandSETMULTIPLEX	= 0xA8,
	kSSD1331CommandSETMASTER	= 0xAD,
	kSSD1331CommandDISPLAYOFF	= 0xAE,
	kSSD1331CommandDISPLAYON	= 0xAF,
	kSSD1331CommandPOWERMODE	= 0xB0,
	kSSD1331CommandPRECHARGE	= 0xB1,
	kSSD1331CommandCLOCKDIV		= 0xB3,
	kSSD1331CommandPRECHARGEA	= 0x8A,
	kSSD1331CommandPRECHARGEB	= 0x8B,
	kSSD1331CommandPRECHARGEC	= 0x8C,
	kSSD1331CommandPRECHARGELEVEL	= 0xBB,
	kSSD1331CommandVCOMH		= 0xBE,
} SSD1331Commands;


static int lpx=1;
static int lpy=1;
// static void FontSizeConvert();

uint8_t chr_size;
uint8_t char_x;
uint8_t char_y;
uint16_t Char_Color;    // text color
uint16_t BGround_Color; // background color

int	devSSD1331init(void);
void PutChar(uint8_t column,uint8_t row, int value);
void pixel(uint8_t x,uint8_t y,char Color);
int _putc( int c );
void locate(uint8_t column, uint8_t row);
uint16_t toRGB(uint16_t R,uint16_t G,uint16_t B);
void foreground(uint16_t color);
void background(uint16_t color);
void write_string(uint8_t x, uint8_t y, const char *pString);
void write_int(uint8_t x, uint8_t y, int* pString, int size);
void clearScreen(uint8_t x_start, uint8_t y_start,uint8_t x_end,uint8_t y_end);

bool compareInt(int val1, int val2);
void count();
unsigned int countDigits(unsigned int i);
void splitInt(int *arr, int num);