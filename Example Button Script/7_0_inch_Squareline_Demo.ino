/* 
  You have to install this Arduino library:
  LovyanGFX https://github.com/lovyan03/LovyanGFX
  lvgl https://github.com/lvgl/lvgl
*/

#include <Wire.h>
#include <SPI.h>

#define GFX_BL  2

#define USE_UI    

#if defined USE_UI
#include <lvgl.h>
#include "ui.h"
#endif

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>


// Define a class named LGFX, inheriting from the LGFX_Device class.
class LGFX : public lgfx::LGFX_Device {
public:
  // Instances for the RGB bus and panel.
  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_RGB   _panel_instance;


  // Constructor for the LGFX class.
  LGFX(void) {
    // Configure the RGB bus.
    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;


      // Configure data pins.
      cfg.pin_d0  = GPIO_NUM_15; // B0
      cfg.pin_d1  = GPIO_NUM_7;  // B1
      cfg.pin_d2  = GPIO_NUM_6;  // B2
      cfg.pin_d3  = GPIO_NUM_5;  // B3
      cfg.pin_d4  = GPIO_NUM_4;  // B4
      
      cfg.pin_d5  = GPIO_NUM_9;  // G0
      cfg.pin_d6  = GPIO_NUM_46; // G1
      cfg.pin_d7  = GPIO_NUM_3;  // G2
      cfg.pin_d8  = GPIO_NUM_8;  // G3
      cfg.pin_d9  = GPIO_NUM_16; // G4
      cfg.pin_d10 = GPIO_NUM_1;  // G5
      
      cfg.pin_d11 = GPIO_NUM_14; // R0
      cfg.pin_d12 = GPIO_NUM_21; // R1
      cfg.pin_d13 = GPIO_NUM_47; // R2
      cfg.pin_d14 = GPIO_NUM_48; // R3
      cfg.pin_d15 = GPIO_NUM_45; // R4


      // Configure sync and clock pins.
      cfg.pin_henable = GPIO_NUM_41;
      cfg.pin_vsync   = GPIO_NUM_40;
      cfg.pin_hsync   = GPIO_NUM_39;
      cfg.pin_pclk    = GPIO_NUM_0;
      cfg.freq_write  = 15000000;


      // Configure timing parameters for horizontal and vertical sync.
      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 40;
      cfg.hsync_pulse_width = 48;
      cfg.hsync_back_porch  = 40;
      
      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 1;
      cfg.vsync_pulse_width = 31;
      cfg.vsync_back_porch  = 13;


      // Configure polarity for clock and data transmission.
      cfg.pclk_active_neg   = 1;
      cfg.de_idle_high      = 0;
      cfg.pclk_idle_high    = 0;


      // Apply configuration to the RGB bus instance.
      _bus_instance.config(cfg);
    }


    // Configure the panel.
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width  = 800;
      cfg.memory_height = 480;
      cfg.panel_width   = 800;
      cfg.panel_height  = 480;
      cfg.offset_x      = 0;
      cfg.offset_y      = 0;


      // Apply configuration to the panel instance.
      _panel_instance.config(cfg);
    }


    // Set the RGB bus and panel instances.
    _panel_instance.setBus(&_bus_instance);
    setPanel(&_panel_instance);
  }
};
LGFX lcd;

/*******************************************************************************
   Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"

#ifdef USE_UI
/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];      //5,7inch: lv_color_t disp_draw_buf[800*480/10]            4.3inch: lv_color_t disp_draw_buf[480*272/10]
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{


  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  


  //lcd.fillScreen(TFT_WHITE);
#if (LV_COLOR_16_SWAP != 0)
 lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
#else
  lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
#endif


  lv_disp_flush_ready(disp);


}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      //Serial.print( "Data x :" );
      //Serial.println( touch_last_x );

      //Serial.print( "Data y :" );
      //Serial.println( touch_last_y );
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
  delay(15);
}
#endif

void setup()
{
  Serial.begin(9600);
  Serial.println("LVGL Widgets Demo");

#if defined(Display_50) || defined(Display_70)
  //IO Port Pins
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(17, OUTPUT);
  digitalWrite(17, LOW);
  pinMode(18, OUTPUT);
  pinMode(42, OUTPUT);
  digitalWrite(42, LOW);
#elif defined(Display_43)
  pinMode(20, OUTPUT);
  digitalWrite(20, LOW);
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
  pinMode(35, OUTPUT);
  digitalWrite(35, LOW);
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);//TOUCH-CS
#endif

  // Init Display
  lcd.begin();
  //lcd.fillScreen(BLACK);
  lcd.setTextSize(2);
  delay(200);
  
#ifdef USE_UI
  lv_init();

  delay(100);
  touch_init();

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  screenWidth = lcd.width();
  screenHeight = lcd.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 10);
  //  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
#endif
  
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

#ifdef USE_UI
  ui_init();//ui from Squareline or GUI Guider
#else
  lcd.fillScreen(RED);
  delay(800);
  lcd.fillScreen(BLUE);
  delay(800);
  lcdfillScreen(YELLOW);
  delay(800);
  lcdfillScreen(GREEN);
  delay(800);
#endif
  Serial.println( "Setup done" );
}

void loop()
{
  while(1)
  {
#ifdef USE_UI
    lv_timer_handler();
    delay(5);
#endif
  }
}
