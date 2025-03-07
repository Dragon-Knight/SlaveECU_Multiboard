#pragma once
#include <inttypes.h>
#include <string.h>
#include <math.h>

//#include <stm32f1xx_hal_def.h>
#include <CUtils.h>
#include "WS2812HW.h"




#define DISPLAY_WIDTH	128
#define DISPLAY_HEIGHT	16
#include <WS2812Manager.h>
#include <effects\WS2812EffectFire.h>
#include <effects\WS2812EffectSphere.h>


//extern TIM_HandleTypeDef htim2;
//extern DMA_HandleTypeDef hdma_tim2_ch1;







namespace WS2812Logic
{

	/* Настройки */
	static constexpr uint8_t CFG_Layers = 8;		// Кол-во слоёв анимации.
	static constexpr uint8_t CFG_Width = 128;		// Ширина экрана.
	static constexpr uint8_t CFG_Height = 16;		// Высота экрана.
	static constexpr uint16_t CFG_Delay = 200;		// Интервал обновления экрана.
	static constexpr uint8_t CFG_Brightness = 10;	// Яркость матрицы.
	#define ROOT_DIRECTORY ("/pxl_r")				// Папка с файлами pxl.
	/* */
	
	//MatrixLed<CFG_Layers, CFG_Width, CFG_Height> matrixObj(CFG_Delay);


	typedef struct __attribute__((__packed__))
	{
		uint8_t G;
		uint8_t R;
		uint8_t B;
	} color_t;

	//color_t _frame_buff[(CFG_Width * CFG_Height)];

	
	uint8_t *frame_buffer_ptr;
	uint16_t frame_buffer_len;
	volatile uint16_t frame_buffer_idx = 0;
	volatile uint8_t dma_buffer[ (8 * 3 * 4) ];		// 8 бит * 3 цвета * 4 пикселя.
	


/*
	struct frame_buffer_t
	{
		bool is_drawing;		// Флаг отрисовки буфера на экран
		bool is_ready;			// Флаг готовности буфера к отрисовке
		union
		{
			uint8_t raw[(CFG_Width * CFG_Height * sizeof(color_t))];
			color_t pixel[(CFG_Width * CFG_Height)];
		};
	} frame_buffer;
*/






	WS2812Manager manager;
	WS2812EffectFire effect_fire;
	WS2812EffectSphere effect_sphere;








static void RGB_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma);
static void RGB_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);








void DMADraw()
{

	//frame_buffer_idx = 0;

    //ARGB_LOC_ST = ARGB_BUSY;
    if (frame_buffer_idx != 0 || DMA_HANDLE.State != HAL_DMA_STATE_READY) {
        return;
    } 
		else {
        for (volatile uint8_t i = 0; i < 8; i++) {
            // set first transfer from first values
            dma_buffer[i] = (((frame_buffer_ptr[0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 8] = (((frame_buffer_ptr[1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 16] = (((frame_buffer_ptr[2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 24] = (((frame_buffer_ptr[3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 32] = (((frame_buffer_ptr[4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 40] = (((frame_buffer_ptr[5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
			dma_buffer[i + 48] = (((frame_buffer_ptr[6] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 56] = (((frame_buffer_ptr[7] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 64] = (((frame_buffer_ptr[8] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
			dma_buffer[i + 72] = (((frame_buffer_ptr[9] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 80] = (((frame_buffer_ptr[10] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 88] = (((frame_buffer_ptr[11] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
        }
        HAL_StatusTypeDef DMA_Send_Stat = HAL_ERROR;
        while (DMA_Send_Stat != HAL_OK) {
            if (TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_BUSY) {
                DMA_Send_Stat = HAL_BUSY;
                continue;
            } else if (TIM_CHANNEL_STATE_GET(&TIM_HANDLE, TIM_CH) == HAL_TIM_CHANNEL_STATE_READY) {
                TIM_CHANNEL_STATE_SET(&TIM_HANDLE, TIM_CH, HAL_TIM_CHANNEL_STATE_BUSY);
            } else {
                DMA_Send_Stat = HAL_ERROR;
                continue;
            }
						// Callback
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferCpltCallback = RGB_TIM_DMADelayPulseCplt;
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferHalfCpltCallback = RGB_TIM_DMADelayPulseHalfCplt;
            TIM_HANDLE.hdma[ARGB_TIM_DMA_ID]->XferErrorCallback = TIM_DMAError;
						// DMA init
            if (HAL_DMA_Start_IT(TIM_HANDLE.hdma[ARGB_TIM_DMA_ID], (uint32_t) dma_buffer,
                                 (uint32_t) &TIM_HANDLE.Instance->ARGB_TIM_CCR,
                                 (uint32_t) sizeof(dma_buffer)) != HAL_OK) {
                DMA_Send_Stat = HAL_ERROR;
                continue;
            }
            __HAL_TIM_ENABLE_DMA(&TIM_HANDLE, ARGB_TIM_DMA_CC);
            if (IS_TIM_BREAK_INSTANCE(TIM_HANDLE.Instance) != RESET)
                __HAL_TIM_MOE_ENABLE(&TIM_HANDLE);
            if (IS_TIM_SLAVE_INSTANCE(TIM_HANDLE.Instance)) {
                uint32_t tmpsmcr = TIM_HANDLE.Instance->SMCR & TIM_SMCR_SMS;
                if (!IS_TIM_SLAVEMODE_TRIGGER_ENABLED(tmpsmcr))
                    __HAL_TIM_ENABLE(&TIM_HANDLE);
            } else
                __HAL_TIM_ENABLE(&TIM_HANDLE);
            DMA_Send_Stat = HAL_OK;
        }
        frame_buffer_idx = 12;
        return;
    }
}

static void RGB_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma) {

    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *) ((DMA_HandleTypeDef *) hdma)->Parent;
    // if wrong handlers
    if (hdma != &DMA_HANDLE || htim != &TIM_HANDLE) return;
    if (frame_buffer_idx == 0) return; // if no data to transmit - return
    // if data transfer
    if (frame_buffer_idx < frame_buffer_len) {
        // fill first part of buffer
			

        for (volatile uint8_t i = 0; i < 8; i++) {
					
            dma_buffer[i] = (((frame_buffer_ptr[frame_buffer_idx + 0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 8] = (((frame_buffer_ptr[frame_buffer_idx + 1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 16] = (((frame_buffer_ptr[frame_buffer_idx + 2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
            dma_buffer[i + 24] = (((frame_buffer_ptr[frame_buffer_idx + 3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 32] = (((frame_buffer_ptr[frame_buffer_idx + 4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 40] = (((frame_buffer_ptr[frame_buffer_idx + 5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;		
					
        }
        frame_buffer_idx += 6;			// old set ++
    } else if (frame_buffer_idx < frame_buffer_len + 12) { // if RET transfer
        memset((uint8_t *) &dma_buffer[0], 0, (sizeof(dma_buffer) / 2)); // first part
        frame_buffer_idx += 6;			// old set ++
    }
}

static void RGB_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma) {
	
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *) ((DMA_HandleTypeDef *) hdma)->Parent;
    // if wrong handlers
    if (hdma != &DMA_HANDLE || htim != &TIM_HANDLE) return;
    if (frame_buffer_idx == 0) return; // if no data to transmit - return
	

    if (hdma == htim->hdma[TIM_DMA_ID_CC1]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_1;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_1, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC2]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_2;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_2, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC3]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_3;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_3, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else if (hdma == htim->hdma[TIM_DMA_ID_CC4]) {
        htim->Channel = HAL_TIM_ACTIVE_CHANNEL_4;
        if (hdma->Init.Mode == DMA_NORMAL) {
            TIM_CHANNEL_STATE_SET(htim, TIM_CHANNEL_4, HAL_TIM_CHANNEL_STATE_READY);
        }
    } else {
        //nothing to do 
    }


// if data transfer
    if (frame_buffer_idx < frame_buffer_len) {
        // fill second part of buffer
			

        for (volatile uint8_t i = 0; i < 8; i++) {
					
            dma_buffer[i + 48] = (((frame_buffer_ptr[frame_buffer_idx + 0] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 56] = (((frame_buffer_ptr[frame_buffer_idx + 1] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 64] = (((frame_buffer_ptr[frame_buffer_idx + 2] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
            dma_buffer[i + 72] = (((frame_buffer_ptr[frame_buffer_idx + 3] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 80] = (((frame_buffer_ptr[frame_buffer_idx + 4] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
            dma_buffer[i + 88] = (((frame_buffer_ptr[frame_buffer_idx + 5] << i) & 0x80) > 0) ? PWM_HI : PWM_LO;
					
					
        }
        frame_buffer_idx += 6;				// old set ++
    } else if (frame_buffer_idx < frame_buffer_len + 12) { // if RET transfer
        memset((uint8_t *) &dma_buffer[sizeof(dma_buffer) / 2], 0, (sizeof(dma_buffer) / 2)); // second part
        frame_buffer_idx += 6;				// old set ++
    } else { // if END of transfer
			
        frame_buffer_idx = 0;
        // STOP DMA:


        __HAL_TIM_DISABLE_DMA(htim, ARGB_TIM_DMA_CC);
        (void) HAL_DMA_Abort_IT(htim->hdma[ARGB_TIM_DMA_ID]);


        if (IS_TIM_BREAK_INSTANCE(htim->Instance) != RESET) {
            /* Disable the Main Output */
            __HAL_TIM_MOE_DISABLE(htim);
        }
        /* Disable the Peripheral */
        __HAL_TIM_DISABLE(htim);
        /* Set the TIM channel state */
        TIM_CHANNEL_STATE_SET(htim, TIM_CH, HAL_TIM_CHANNEL_STATE_READY);
        //ARGB_LOC_ST = ARGB_READY;

		manager.frame_buffer.is_drawing = false;
		manager.frame_buffer.is_ready = false;
				
    }
    htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
}






















//#define FIRE_HEIGHT 16


uint16_t iterator11(uint16_t i) {
    int width = 128;
    int height = 16;
    
	int row = i % height;
	int col = i / height;
	
	int index = (col % 2 == 0) ? (row * width + col) : ((height - row - 1) * width + col);
	
	return index;
}
/*
uint16_t iterator111(uint16_t i) {
    int width = 128;
    int height = 16;

    int row = i / width;
    int col = i % width;

    int index;
    
    if (col % 2 == 0) {
        index = row + col * height;
    } else {
        index = (height - row - 1) + col * height;
    }

    return index;
}


// Простая функция для генерации псевдослучайного числа
float randFloat() {
    return (float)rand() / (float)RAND_MAX;
}

// Простая 2D версия шума Перлина (упрощенная для МК)
float perlinNoise(float x, float y) {
    int n = (int)x + (int)y * 57;
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

// Функция для обновления эффекта огня
void updateFireEffect() {
    static uint8_t firePixels[CFG_Width * FIRE_HEIGHT] = {0};

    // Распространение огня вверх
    for (int y = 0; y < FIRE_HEIGHT - 1; y++) {
        for (int x = 0; x < CFG_Width; x++) {
            int src = x + (y + 1) * CFG_Width;
            int dst = x + y * CFG_Width;
            uint8_t decay = rand() % 50;
            firePixels[dst] = (firePixels[src] > decay) ? firePixels[src] - decay : 0;
        }
    }

    // Генерация шума в нижней части буфера
    for (int x = 0; x < CFG_Width; x++) {
        firePixels[x + (FIRE_HEIGHT - 1) * CFG_Width] = (uint8_t)(perlinNoise(x * 0.1, randFloat() * 10) * 255);
    }

    // Преобразование в RGB буфер
    for (int y = 0; y < FIRE_HEIGHT; y++) {
        for (int x = 0; x < CFG_Width; x++) {
            uint8_t intensity = firePixels[x + y * CFG_Width] / 16;
            color_t color;
            color.G = intensity / 8;//(intensity > 128) ? (255 - intensity) * 2 : intensity / 2;
            color.R = intensity;
            color.B = 0;
            frame_buffer.pixel[ iterator111(x + y * CFG_Width) ] = color;
        }
    }
}
*/

/*
#define STAR_COUNT 50
#define MAX_DEPTH 32.0f
#define SPEED 0.5f

static float ship_sway = 0.0f;
static float sway_direction = 5.0f;

typedef struct {
    float x, y, z;
} Star;

Star stars[STAR_COUNT];

void init_stars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].x = (float)(rand() % CFG_Width) - (CFG_Width / 2);
        stars[i].y = (float)(rand() % CFG_Height) - (CFG_Height / 2);
        stars[i].z = (float)(rand() % (int)MAX_DEPTH + 1);
    }
}

void draw_pixel(int x, int y, color_t color) {
    if (x >= 0 && x < CFG_Width && y >= 0 && y < CFG_Height) {
        _frame_buff[ iterator111(y * CFG_Width + x) ] = color;
    }
}

void clear_screen() {
    memset(_frame_buff, 0, sizeof(_frame_buff));
}

void update_stars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].z -= SPEED;
        if (stars[i].z <= 0) {
            stars[i].x = (float)(rand() % CFG_Width) - (CFG_Width / 2);
            stars[i].y = (float)(rand() % CFG_Height) - (CFG_Height / 2);
            stars[i].z = MAX_DEPTH;
        }
    }

    // Добавляем небольшое покачивание
    ship_sway += 0.05f * sway_direction;
    if (fabs(ship_sway) > 2.0f) {
        sway_direction *= -1.0f;
    }
}

void render_stars() {
    for (int i = 0; i < STAR_COUNT; i++) {
        int sx = (int)((stars[i].x / stars[i].z) * 40 + (CFG_Width / 2) + ship_sway);
        int sy = (int)((stars[i].y / stars[i].z) * 40 + (CFG_Height / 2));

        uint8_t brightness = (uint8_t)((1.0f - stars[i].z / MAX_DEPTH) * 255);
        color_t star_color = { brightness, brightness, brightness };

        draw_pixel(sx, sy, star_color);

        // Рисуем хвост
        int tail_length = (int)(5 * (1.0f - stars[i].z / MAX_DEPTH));
        for (int t = 1; t <= tail_length; t++) {
            int tail_x = (int)((stars[i].x / (stars[i].z + t * 2)) * 40 + (CFG_Width / 2) + ship_sway);
            int tail_y = (int)((stars[i].y / (stars[i].z + t * 2)) * 40 + (CFG_Height / 2));
			uint8_t tmp = (brightness / (uint8_t)(t + 1)) / 32;
            color_t tail_color = { tmp, tmp, tmp };
            draw_pixel(tail_x, tail_y, tail_color);
        }
    }
}

void space_flight_loop() {
    clear_screen();
    update_stars();
    render_stars();
}

// Вызов init_stars() при старте
// В основном цикле вызывай space_flight_loop() для обновления и отрисовки
*/







/*
	Конвертор индексов 2D кадрового буфера в вертикальный зиг-заг, сверху-вниз, слево-направо (светодиодне панели)
*/
uint16_t iterator1(uint16_t input, uint8_t width = 128, uint8_t height = 16)
{
	uint8_t row = input / width;
	uint8_t col = input % width;
	uint16_t index = col * height + (col % 2 == 0 ? row : (height - row - 1));
	
	return index;
}

/*
	Конвертор индексов 2D кадрового буфера в горизонтальный зиг-заг, слево-направо, сверху-вниз (светодиодне ленты)
*/
uint16_t iterator2(uint16_t input, uint8_t width = 128, uint8_t height = 16)
{
	uint8_t row = input / width;
	uint8_t col = input % width;
	uint16_t index = row * width + (row % 2 == 0 ? col : (width - col - 1));
	
	return index;
}





inline void Setup()
{
	srand( Analog::mux.Get(10) * 10 );

	manager.frame_buffer.Convertor = iterator1;
	manager.SelectEffect(effect_fire);

#ifdef ROOT_DIRECTORY
	//f_chdir(ROOT_DIRECTORY);
#endif
/*
	matrixObj.RegLayer("layer0.pxl", 0);	// 0 - Фон / Заливка;
	matrixObj.RegLayer("layer1.pxl", 1);	// 1 - Анимация;
	matrixObj.RegLayer("layer2.pxl", 2);	// 2 - Габариты;
	matrixObj.RegLayer("layer3.pxl", 3);	// 3 - Задних ход;
	matrixObj.RegLayer("layer4.pxl", 4);	// 4 - Стопы;
	matrixObj.RegLayer("layer5.pxl", 5);	// 5 - Повтороты лево;
	matrixObj.RegLayer("layer6.pxl", 6);	// 6 - Повтороты право;
	matrixObj.RegLayer("layer7.pxl", 7);	// 7 - Аварийка;

	matrixObj.ShowLayer(0);
	matrixObj.ShowLayer(1);
	//matrixObj.ShowLayer(2);
	//matrixObj.ShowLayer(3);
	//matrixObj.ShowLayer(4);
	//matrixObj.ShowLayer(5);
	//matrixObj.ShowLayer(6);
	//matrixObj.ShowLayer(7);
	
	matrixObj.SetBrightness(CFG_Brightness);
	
	matrixObj.GetFrameBuffer(frame_buffer_ptr, frame_buffer_len);

	//matrixObj.ManualMode(true);
	//matrixObj.DrawPixel(5, 0xFF0000FF);
	//matrixObj.DrawPixel(6, 0xFF00FF00);
	//matrixObj.DrawPixel(7, 0xFFFF0000);
	//matrixObj.DrawPixel(8, 0xAAFFFFFF);
	//matrixObj.DrawPixel(0, 0, 0xFF0000FF);
	//matrixObj.DrawPixel(5, 2, 0xFF00FF00);
	//matrixObj.DrawPixel(6, 3, 0xFFFF0000);
	//matrixObj.ManualDraw();
*/	


		frame_buffer_ptr = manager.frame_buffer.raw;
		frame_buffer_len = sizeof(manager.frame_buffer.raw);


	
		SetupHW();

		//frame_buffer.is_ready = trie;

	//init_stars();
	
	return;
}

uint32_t timer1, timer2, timer3, timer12, timer23;

inline void Loop(uint32_t &current_time)
{

	static uint8_t idx = 0;
	static uint32_t tick = 0;
	if(current_time - tick > (30 * 1000))
	{
		tick = current_time;

		if(idx == 0)
		{
			manager.SelectEffect(effect_fire);
			idx = 1;
		}
		
		else if(idx == 1)
		{
			manager.SelectEffect(effect_sphere);
			idx = 0;
		}
	}




	manager.Tick(current_time);

/*
	static uint32_t tick = 0;
	if(current_time - tick > 1000)
	{
		tick = current_time ;
		
		srand(Analog::mux.adc_value[11]);
	}
*/

	
	static uint16_t draw_idx = 0;
	if(manager.frame_buffer.is_ready == true && manager.frame_buffer.is_drawing == false)
	{
/*
		//memset(_frame_buff, 0x00, sizeof(_frame_buff));
		_frame_buff[ iterator111(draw_idx) ] = {0x00, 0x00, 0x00};
		
		if(++draw_idx > (sizeof(_frame_buff) / sizeof(_frame_buff[0])) )
			draw_idx = 0;
		
		_frame_buff[ iterator111(draw_idx) ] = {0x33, 0x00, 0x00};
*/		
		//memset(_frame_buff, 0x00, sizeof(_frame_buff));
		//updateFireEffect();
		//space_flight_loop();
		DMADraw();

		manager.frame_buffer.is_drawing = true;
	}








	/*
	timer1 = HAL_GetTick();
	matrixObj.Processing(current_time);
	timer2 = HAL_GetTick();
	
	if(matrixObj.IsBufferReady() == true)
	{
		timer12 = timer2 - timer1;
		timer23 = HAL_GetTick();
		
		matrixObj.SetFrameDrawStart();
		
		DMADraw();
		
		//Serial::Print("+PXL=128,16,2\r\n");
		//Serial::Print(frame_buffer_ptr, frame_buffer_len);
	}
	
	if( matrixObj.GetFrameIsDraw() == true && frame_buffer_idx == 0 )
	{
		matrixObj.SetFrameDrawEnd();
		
		timer23 = HAL_GetTick() - timer23;
		//DEBUG_LOG_TOPIC("PXLTime", "render: %d ms, draw: %d ms, total: %d ms\n", timer12, timer23, (timer23 + timer12));
		Logger.PrintTopic("PXLTime").Printf("render: %d ms, draw: %d ms, total: %d ms;\n", timer12, timer23, (timer23 + timer12));
	}
	*/
	
	current_time = HAL_GetTick();
	
	return;
}
};
