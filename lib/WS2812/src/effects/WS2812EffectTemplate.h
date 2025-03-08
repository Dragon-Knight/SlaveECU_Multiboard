#pragma once
#include <inttypes.h>
#include "WS2812Manager.h"
#include "WS2812EffectInterface.h"

/*
	_manager - объект менеджера
	_frame_buffer - объект кадрового буфера с параметрами и флагами
	Init() - Вызывается при инициализации эффекта
	Tick() - Вызывается при каждые 5 мс
	Для расчёта положения пикселя в кадровом буфере нужно воспользоваться такой конструкцией:
		uint16_t index = _frame_buffer->Convertor(index_2d, width, height);
		_frame_buffer->pixel[index] = color;
*/

class WS2812EffectTemplate : public WS2812EffectInterface
{
	public:
		
		virtual void Init() override
		{
			return;
		}
		
		virtual void Tick(uint32_t time) override
		{
			if(_frame_buffer->is_drawing == true) return;
			
			//updateEffect();
			_frame_buffer->is_ready = true;
			
			return;
		}
		
	private:
		
};
