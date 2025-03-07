#pragma once
#include <inttypes.h>
#include "WS2812Manager.h"
#include "WS2812EffectInterface.h"

class WS2812EffectSphere : public WS2812EffectInterface
{
	static constexpr uint8_t _count = 6;

	
	public:
		
		virtual void Init() override
		{
			for(uint8_t i = 0; i < _count; ++i)
			{
				balls[i].x = Random(0, 127);
				balls[i].y = Random(0, 15);
				
				// Исключение строго вертикального и горизонтального движения
				do
				{
					balls[i].vx = (Random(-2, 2));
					balls[i].vy = (Random(-2, 2));
				} while (balls[i].vx == 0 || balls[i].vy == 0);
				
				balls[i].radius = Random(1, 3);
				balls[i].r = Random(0, 16);
				balls[i].g = Random(0, 16);
				balls[i].b = Random(0, 16);
			}
			
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			if(_frame_buffer->is_drawing) return;
			
			updateEffect();
			_frame_buffer->is_ready = true;
			
			return;
		}
		
	private:
		
		void updateEffect()
		{
			Ball *ball = nullptr;
			for(uint8_t i = 0; i < _count; ++i)
			{
				ball = &balls[i];
				
				// Пропуск обновления позиции для неподвижных шаров
				if(ball->vx == 0 && ball->vy == 0)
					continue;
				
				ball->x += ball->vx;
				ball->y += ball->vy;
				
				if(ball->x - ball->radius < 0 || ball->x + ball->radius >= _frame_buffer->width)
				{
					ball->vx *= -1;
					ball->x += ball->vx;
				}
				if(ball->y - ball->radius < 0 || ball->y + ball->radius >= _frame_buffer->height) {
					ball->vy *= -1;
					ball->y += ball->vy;
				}
				
				if(ball->vx == 0)
					ball->vx = Random(-2, 2);
				
				if(ball->vy == 0)
					ball->vy = Random(-2, 2);
			}
			
			render();
		}
		
		void render()
		{
			memset(_frame_buffer->raw, 0x00, sizeof(_frame_buffer->raw));
			
			static constexpr uint8_t width = _frame_buffer->width;
			static constexpr uint8_t height = _frame_buffer->height;
			Ball *ball = nullptr;
			for(uint8_t i = 0; i < _count; ++i)
			{
				ball = &balls[i];
				
				for(int dx = -ball->radius; dx <= ball->radius; dx++)
				{
					for(int dy = -ball->radius; dy <= ball->radius; dy++)
					{
						int px = (int)(ball->x) + dx;
						int py = (int)(ball->y) + dy;
						if(px >= 0 && px < width && py >= 0 && py < height && dx * dx + dy * dy <= ball->radius * ball->radius)
						{
							uint16_t index = _frame_buffer->Convertor( (py * width + px), width, height);
							_frame_buffer->pixel[index] = {ball->g, ball->r, ball->b};
						}
					}
				}
			}
		}
		
		struct Ball
		{
			uint8_t x, y;
			uint8_t vx, vy;
			uint8_t r, g, b;
			uint8_t radius;
		} balls[_count];
		
};
