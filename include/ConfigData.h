#pragma once
#include <inttypes.h>

namespace Config
{
	struct __attribute__((packed)) eeprom_body_t
	{
		struct
		{
			bool enable = true;
			bool realtime = true;
			uint16_t interval_ms = 100;
		} in1;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in2;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in3;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in4;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in5;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in6;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in7;
		struct
		{
			bool enable = true;
			bool realtime = false;
			uint16_t interval_ms = 100;
		} in8;
	};
};
