#pragma once
#ifndef COLOR_DEBTH
#define COLOR_DEBTH 3 // по умолчанию 24 бита
#endif

#include "color_utility.h"

// чип
enum M_chip
{
	LED_WS2812, // GRB
	LED_WS6812, // BGR
};

#define CHIP4COLOR (chip == LED_WS6812)

enum M_order
{
	// порядок цвета: r-00, g-01, b-10
	ORDER_RGB = 0b000110,
	ORDER_RBG = 0b001001,
	ORDER_BRG = 0b100001,
	ORDER_BGR = 0b100100,
	ORDER_GRB = 0b010010,
	ORDER_GBR = 0b011000,
};

const byte SAVE_MILLIS = 1;

// ========== ПОДКЛЮЧЕНИЕ МАТРИЦЫ ==========
enum M_type
{
	ZIGZAG,
	PARALLEL,
};
enum M_connection
{
	LEFT_BOTTOM,
	LEFT_TOP,
	RIGHT_TOP,
	RIGHT_BOTTOM,
};
enum M_dir
{
	DIR_RIGHT,
	DIR_UP,
	DIR_LEFT,
	DIR_DOWN,
};

void systemUptimePoll(void); // дёрнуть миллисы

// ============================================== КЛАСС ==============================================
// <amount, pin, clock pin, chip, order, cli, mls> ()
// <amount, pin, clock pin, chip, order, cli, mls> (width, height, type, conn, dir)
// количество, пин, чип, порядок, прерывания, миллис
template <int amount, int8_t pin, M_chip chip, M_order order, uint8_t uptime = 0>
class microLED
{
public:
	int oneLedMax = 46;
	int oneLedIdle = 2000;
	mData leds[amount];
	byte white[(CHIP4COLOR) ? amount : 0];

	void init()
	{
		_dat_mask = digitalPinToBitMask(pin);
		_dat_port = portOutputRegister(digitalPinToPort(pin));
		_dat_ddr = portModeRegister(digitalPinToPort(pin));
		*_dat_ddr |= _dat_mask;
		switch (chip)
		{
		case LED_WS2812:
			oneLedMax = 30;
			oneLedIdle = 660;
			break; // 28/240 для ECO, 32/700 матрица
		}
		// oneLedMax = (ток ленты с одним горящим) - (ток выключенной ленты)
		// oneLedIdle = (ток выключенной ленты) / (количество ледов)
	}

	microLED()
	{
		init();
	}

	microLED(uint8_t width, uint8_t height, M_type type, M_connection conn, M_dir dir) : _width(width), _height(height), _matrixConfig((uint8_t)conn | ((uint8_t)dir << 2)), _matrixType((uint8_t)type)
	{
		init();
		if (_matrixConfig == 4 || _matrixConfig == 13 || _matrixConfig == 14 || _matrixConfig == 7)
			_matrixW = height;
		else
			_matrixW = width;
	}

	void setBrightness(uint8_t newBright)
	{
		_bright = getCRT(newBright);
	}

	void clear()
	{
		for (int i = 0; i < amount; i++)
			leds[i] = 0;
	}

	void fill(mData color)
	{
		for (int i = 0; i < amount; i++)
			leds[i] = color;
	}

	void fill(int from, int to, mData color)
	{
		for (int i = from; i <= to; i++)
			leds[i] = color;
	}

	void fillGradient(int from, int to, mData color1, mData color2)
	{
		for (int i = from; i < to; i++)
			leds[i] = getBlend(i - from, to - from, color1, color2);
	}

	void set(int n, mData color)
	{
		leds[n] = color;
	}

	mData get(int num)
	{
		return leds[num];
	}

	void fade(int num, byte val)
	{
		leds[num] = getFade(leds[num], val);
	}

	// ============================================== МАТРИЦА ==============================================
	uint16_t getPixNumber(int x, int y)
	{
		int thisX, thisY;
		switch (_matrixConfig)
		{
		case 0:
			thisX = x;
			thisY = y;
			break;
		case 4:
			thisX = y;
			thisY = x;
			break;
		case 1:
			thisX = x;
			thisY = (_height - y - 1);
			break;
		case 13:
			thisX = (_height - y - 1);
			thisY = x;
			break;
		case 10:
			thisX = (_width - x - 1);
			thisY = (_height - y - 1);
			break;
		case 14:
			thisX = (_height - y - 1);
			thisY = (_width - x - 1);
			break;
		case 11:
			thisX = (_width - x - 1);
			thisY = y;
			break;
		case 7:
			thisX = y;
			thisY = (_width - x - 1);
			break;
		}

		if (_matrixType || !(thisY % 2))
			return (thisY * _matrixW + thisX); // если чётная строка
		else
			return (thisY * _matrixW + _matrixW - thisX - 1); // если нечётная строка
	}

	void set(int x, int y, mData color)
	{
		if (x * y >= amount || x < 0 || y < 0 || x >= _width || y >= _height)
			return;
		leds[getPixNumber(x, y)] = color;
	}

	mData get(int x, int y)
	{
		return leds[getPixNumber(x, y)];
	}

	void fade(int x, int y, byte val)
	{
		int pix = getPixNumber(x, y);
		leds[pix] = getFade(leds[pix], val);
	}

	void drawBitmap8(int X, int Y, const uint8_t *frame, int width, int height)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				set(x + X, y + Y, pgm_read_byte(&frame[x + (height - 1 - y) * width]));
	}
	void drawBitmap16(int X, int Y, const uint16_t *frame, int width, int height)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				set(x + X, y + Y, pgm_read_word(&frame[x + (height - 1 - y) * width]));
	}
	void drawBitmap32(int X, int Y, const uint32_t *frame, int width, int height)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
				set(x + X, y + Y, pgm_read_dword(&frame[x + (height - 1 - y) * width]));
	}

	// ============================================== УТИЛИТЫ ==============================================
	void setMaxCurrent(int ma)
	{
		_maxCurrent = ma;
	}

	uint8_t correctBright(uint8_t bright)
	{
		long sum = 0;
		for (int i = 0; i < amount; i++)
		{
			sum += fade8R(leds[i], bright);
			sum += fade8G(leds[i], bright);
			sum += fade8B(leds[i], bright);
		}

		sum = ((long)sum >> 8) * oneLedMax / 3;		 // текущий "активный" ток ленты
		int idle = (long)oneLedIdle * amount / 1000; // холостой ток ленты
		if (sum == 0)
			return bright;
		if ((sum + idle) < _maxCurrent)
			return bright; // ограничения нет
		else
			return ((float)(_maxCurrent - idle) * bright / sum); // пересчёт яркости
	}

// ============================================== ВЫВОД ==============================================
// асм задержки
#define _1_NOP "NOP 		\n\t"
#define _2_NOP "RJMP .+0	\n\t"
#define _4_NOP _2_NOP _2_NOP
#define _8_NOP _4_NOP _4_NOP
	// Издержки для бита H - 5 тактов, для бита L - 3 такта

	void begin()
	{
		_mask_h = _dat_mask | *_dat_port;
		_mask_l = ~_mask_h & *_dat_port;

		_showBright = _bright;
	}

	void show()
	{
		begin();
		if (_maxCurrent != 0 && amount != 0)
			_showBright = correctBright(_bright);
		if (CHIP4COLOR)
			for (int i = 0; i < amount; i++)
				send(leds[i], white[i]);
		else
			for (int i = 0; i < amount; i++)
				send(leds[i]);
	}

	void send(mData color, byte thisWhite = 0)
	{
		uint8_t data[3];
		// компилятор посчитает сдвиги
		data[(order >> 4) & 0b11] = fade8R(color, _showBright);
		data[(order >> 2) & 0b11] = fade8G(color, _showBright);
		data[order & 0b11] = fade8B(color, _showBright);
		if (CHIP4COLOR)
			thisWhite = fade8(thisWhite, _showBright);

		// отправляем RGB и W если есть
		for (uint8_t i = 0; i < 3; i++)
			sendRaw(data[i]);
		if (CHIP4COLOR)
			sendRaw(thisWhite);
	}

	void sendRaw(byte data)
	{
		cli();
		asm volatile(
			"MOVI.N %[CNT], 8        \n\t"		  // Загружаем в счетчик циклов 8
			"l_start:       \n\t"				  // Начало основного цикла
			"S32I %[SET_H], %[PORT], 0     \n\t"  // Устанавливаем на выходе HIGH
			"BBSI %[DATA], 7, b_set    \n\t"	  // Если текущий бит установлен - пропуск след. инстр.
			"S32I %[SET_L], %[PORT], 0     \n\t"  // Устанавливаем на выходе LOW
			"b_set:       \n\t"					  // Начало основного цикла
			"SLLI %[DATA], %[DATA], 1       \n\t" // Двигаем данные влево на один бит
#if (F_CPU == 16000000UL)
			_8_NOP
#elif (F_CPU == 8000000UL)
			_1_NOP
#endif
			"S32I %[SET_L], %[PORT], 0     \n\t" // Устанавливаем на выходе LOW
			"ADDI.N %[CNT], %[CNT], -1    \n\t"	 // Декремент счетчика циклов
			"BNEZ %[CNT], l_start  \n\t"		 // Переход на новый цикл, если счетчик не иссяк
			:
			: [DATA] "r"(data),
			  [PORT] "r"(_dat_port),
			  [SET_H] "r"(_mask_h),
			  [SET_L] "r"(_mask_l),
			  [CNT] "r"(_cnt)
			: "memory");

		sei();
	}

private:
	uint8_t _bright = 50, _showBright = 50;
	const uint8_t _matrixConfig = 0, _matrixType = 0, _width = 0, _height = 0;
	uint8_t _matrixW = 0;
	int _maxCurrent = 0;

	volatile uint32_t *_dat_port, *_dat_ddr;
	uint32_t _dat_mask;
	uint32_t _mask_h, _mask_l;
	volatile uint8_t _cnt;
}; // класс