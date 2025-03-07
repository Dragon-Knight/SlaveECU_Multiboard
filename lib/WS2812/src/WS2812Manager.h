#pragma once
#include <inttypes.h>
#include "WS2812ManagerInterface.h"
#include "WS2812EffectInterface.h"

class WS2812Manager : public WS2812ManagerInterface
{

	public:

		void SelectEffect(WS2812EffectInterface &effect)
		{

			_effect = &effect;
			effect.PrepareInit(this, frame_buffer);
			effect.Init();
		
			return;
		}
		void Tick(uint32_t time)
		{
			if(_effect == nullptr) return;
			if(time - last_tick < 5) return;
			last_tick = time;
			
			_effect->Tick(time);

			return;
		}
		
		frame_buffer_t frame_buffer = {};
		
	private:
		
		WS2812EffectInterface *_effect = nullptr;
		uint32_t last_tick = 0;

};
