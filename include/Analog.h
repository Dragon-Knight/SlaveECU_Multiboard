#pragma once
#include <EasyPinD.h>
#include <EasyPinA.h>
#include <AnalogMux.h>

extern ADC_HandleTypeDef hadc1;

namespace Analog
{
	EasyPinA mux_pin(&hadc1, GPIOB, GPIO_PIN_1, ADC_CHANNEL_9);

	// Хранит сырые данные АЦП полученные через мультиплексор
	uint16_t adc_value[16];


	uint16_t ADC_Req(uint8_t address)
	{
		return mux_pin.Get();
	}

	void Result(uint8_t address, uint16_t value)
	{
		if(address > 15) return;
		
		adc_value[address] = value;

		if(address == 15)
		{
			DEBUG_LOG_TOPIC("MUX", "%04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d\n", adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4], 
			adc_value[5], adc_value[6], adc_value[7], adc_value[8], adc_value[9], adc_value[10], adc_value[11], adc_value[12], adc_value[13], adc_value[14], adc_value[15]);
		}
		
		return;
	}

	uint16_t GetValAsMvolt(uint8_t port)
	{
		if(--port >= 8){}

		return 0;
	}

	AnalogMux<4, 30> mux( ADC_Req, Result, 
		EasyPinD::d_pin_t{GPIOB, GPIO_PIN_4}, 
		EasyPinD::d_pin_t{GPIOB, GPIO_PIN_5}, 
		EasyPinD::d_pin_t{GPIOB, GPIO_PIN_6}, 
		EasyPinD::d_pin_t{GPIOB, GPIO_PIN_7}
	);


	inline void Setup()
	{
		mux.Init();
		mux_pin.Init();
		
		return;
	}
	
	inline void Loop(uint32_t &current_time)
	{
		mux.Processing(current_time);
		
		
		current_time = HAL_GetTick();
		
		return;
	}
};
