#pragma once
#include <inttypes.h>

class WS2812Manager;

class WS2812EffectInterface
{
	public:

		void PrepareInit(WS2812Manager *manager, WS2812ManagerInterface::frame_buffer_t &frame_buffer)
		{
			_manager = manager;
			_frame_buffer = &frame_buffer;
			
			return;
		};

		virtual void Init() = 0;
		virtual void Tick(uint32_t time) = 0;
		
	protected:
		
		int32_t Random() { return rand(); }
		int32_t Random(int32_t min, int32_t max) { return (rand() % (max - min + 1)) + min; }
		
		WS2812Manager *_manager = nullptr;
		WS2812ManagerInterface::frame_buffer_t *_frame_buffer = nullptr;
		
};
