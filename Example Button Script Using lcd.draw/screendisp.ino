/* 
  You have to install this Arduino library:
  LovyanGFX: https://github.com/lovyan03/LovyanGFX
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Bus_RGB _bus_instance;
    lgfx::Panel_RGB _panel_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_GT911 _touch_instance;
    LGFX(void)
    {
        {
            auto cfg = _panel_instance.config();
            cfg.memory_width = 800;
            cfg.memory_height = 480;
            cfg.panel_width = 800;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;

            cfg.pin_d0 = GPIO_NUM_15; // B0
            cfg.pin_d1 = GPIO_NUM_7;  // B1
            cfg.pin_d2 = GPIO_NUM_6;  // B2
            cfg.pin_d3 = GPIO_NUM_5;  // B3
            cfg.pin_d4 = GPIO_NUM_4;  // B4

            cfg.pin_d5 = GPIO_NUM_9;  // G0
            cfg.pin_d6 = GPIO_NUM_46; // G1
            cfg.pin_d7 = GPIO_NUM_3;  // G2
            cfg.pin_d8 = GPIO_NUM_8;  // G3
            cfg.pin_d9 = GPIO_NUM_16; // G4
            cfg.pin_d10 = GPIO_NUM_1; // G5

            cfg.pin_d11 = GPIO_NUM_14; // R0
            cfg.pin_d12 = GPIO_NUM_21; // R1
            cfg.pin_d13 = GPIO_NUM_47; // R2
            cfg.pin_d14 = GPIO_NUM_48; // R3
            cfg.pin_d15 = GPIO_NUM_45; // R4

            cfg.pin_henable = GPIO_NUM_41;
            cfg.pin_vsync = GPIO_NUM_40;
            cfg.pin_hsync = GPIO_NUM_39;
            cfg.pin_pclk = GPIO_NUM_0;
            cfg.freq_write = 12000000;

            cfg.hsync_polarity = 0;
            cfg.hsync_front_porch = 40;
            cfg.hsync_pulse_width = 48;
            cfg.hsync_back_porch = 40;

            cfg.vsync_polarity = 0;
            cfg.vsync_front_porch = 1;
            cfg.vsync_pulse_width = 31;
            cfg.vsync_back_porch = 13;

            cfg.pclk_active_neg = 1;
            cfg.de_idle_high = 0;
            cfg.pclk_idle_high = 0;

            _bus_instance.config(cfg);
        }
        _panel_instance.setBus(&_bus_instance);


        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = GPIO_NUM_2;
            _light_instance.config(cfg);
        }
        _panel_instance.light(&_light_instance);

        {
            auto cfg = _touch_instance.config();
            cfg.x_min      = 0;
            cfg.x_max      = 799;
            cfg.y_min      = 0;
            cfg.y_max      = 479;
            cfg.pin_int    = -1;
            cfg.pin_rst    = -1;
            cfg.bus_shared = false;
            cfg.offset_rotation = 0;
            cfg.i2c_port   = I2C_NUM_1;
            cfg.pin_sda    = GPIO_NUM_19;
            cfg.pin_scl    = GPIO_NUM_20;
            cfg.freq       = 400000;
            cfg.i2c_addr   = 0x14;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

LGFX lcd;

int counter = 0; // Counter for button press
bool buttonPressed = false; // Flag to track button press

void drawButton() {
    // Draw button in the middle of the screen with pink color
    lcd.fillRect(lcd.width() / 2 - 50, lcd.height() / 2 - 25, 100, 50, lgfx::color565(255, 105, 180)); // Pink color
    lcd.setTextSize(2);
    lcd.setTextColor(lgfx::color565(0, 0, 0)); // Black text color
    lcd.setTextDatum(textdatum_t::middle_center);
    lcd.drawString("PRESS", lcd.width() / 2, lcd.height() / 2);
}

void drawCounter() {
    // Draw counter at the bottom middle of the screen
    lcd.fillRect(0, lcd.height() - 20, lcd.width(), 20, lgfx::color565(0, 0, 0)); // Black background
    lcd.setTextSize(2);
    lcd.setTextColor(lgfx::color565(255, 255, 255)); // White text color
    lcd.setTextDatum(textdatum_t::baseline_center);
    lcd.drawString(String(counter), lcd.width() / 2, lcd.height());
}

void setup() {
    Serial.begin(115200); 
    Serial.println();
    Serial.println("LCD Init.");

    lcd.init();
    lcd.setBrightness(255);

    drawButton(); // Draw the button
    drawCounter(); // Draw the initial counter value
}

void loop() {
    int32_t x, y;
    bool touched = lcd.getTouch(&x, &y);

    // Check if touch is within button area
    bool withinButtonArea = (x >= lcd.width() / 2 - 50 && x <= lcd.width() / 2 + 50 &&
                            y >= lcd.height() / 2 - 25 && y <= lcd.height() / 2 + 25);

    if (touched && withinButtonArea) {
        if (!buttonPressed) {
            counter++; // Increment counter on button press
            drawCounter(); // Redraw the counter
            buttonPressed = true; // Set button state to pressed
        }
    } else {
        buttonPressed = false; // Reset button state when released
    }

    delay(10);
}
