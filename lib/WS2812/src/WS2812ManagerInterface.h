#pragma once
#include <inttypes.h>

class WS2812ManagerInterface
{
	public:
		
		struct __attribute__((__packed__)) color_t
		{
			uint8_t G;
			uint8_t R;
			uint8_t B;
		};
		
		struct frame_buffer_t
		{
			bool is_drawing = false;							// Флаг отрисовки буфера на экран
			bool is_ready = false;								// Флаг готовности буфера к отрисовке
			static constexpr uint8_t width = DISPLAY_WIDTH;		// Ширина кадра
			static constexpr uint8_t height = DISPLAY_HEIGHT;	// Высота кадра
			union
			{
				uint8_t raw[(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(color_t))];
				color_t pixel[(DISPLAY_WIDTH * DISPLAY_HEIGHT)];
			};
			
			uint16_t (*Convertor)(uint16_t input, uint8_t width, uint8_t height) = [](uint16_t input, uint8_t width, uint8_t height){ return input; };
		};
		
};
