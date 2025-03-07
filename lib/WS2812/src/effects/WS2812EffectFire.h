#pragma once
#include <inttypes.h>
#include "WS2812Manager.h"
#include "WS2812EffectInterface.h"

class WS2812EffectFire : public WS2812EffectInterface
{
	public:
		
		virtual void Init() override
		{
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			if(_frame_buffer->is_drawing == true) return;
			
			updateFireEffect();
			_frame_buffer->is_ready = true;
			
			return;
		}
		
	private:
		
		// Простая функция для генерации псевдослучайного числа
		float randFloat() {
			return (float)rand() / (float)RAND_MAX;
		}
		
		// Простая 2D версия шума Перлина (упрощенная для МК)
		float perlinNoise(float x, float y)
		{
			uint32_t n = (int)x + ((int)y * 57);
			n = (n << 13) ^ n;
			return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		}
		
		// Функция для обновления эффекта огня
		void updateFireEffect()
		{
			static constexpr uint8_t width = _frame_buffer->width;
			static constexpr uint8_t height = _frame_buffer->height;
			static uint8_t firePixels[width * height] = {};
			
			uint16_t src, dst;
			uint8_t decay, intensity;
			WS2812Manager::color_t color;
			
			// Распространение огня вверх
			for(uint8_t y = 0; y < height - 1; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					src = x + ((y + 1) * width);
					dst = x + (y * width);
					decay = rand() % 48;

					if(x > 0 && x < width && firePixels[src] < 48)	// firePixels[src] < 48
						dst += Random(-1, 1);
					
					firePixels[dst] = (firePixels[src] > decay) ? firePixels[src] - decay : 0;
				}
			}
			
			// Генерация шума в нижней части буфера
			for(uint8_t x = 0; x < width; ++x)
			{
				dst = x + ((height - 1) * width);
				
				firePixels[dst] = (uint8_t)(perlinNoise(x * 1.1, randFloat() * 10) * 255);	// x * 0.1
			}
			
			// Преобразование в RGB буфер
			for(uint8_t y = 0; y < height; ++y)
			{
				for(uint8_t x = 0; x < width; ++x)
				{
					dst = x + (y * width);
					intensity = firePixels[dst] / 16;

					color.G = intensity / 5;/*(intensity > 128) ? (255 - intensity) * 2 : intensity / 2;*/
					color.R = intensity;
					color.B = 0;
					
					uint16_t index = _frame_buffer->Convertor( dst, width, height);
					_frame_buffer->pixel[index] = color;
				}
			}
		}
};
