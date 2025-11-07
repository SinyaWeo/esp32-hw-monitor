#include <pin_config.h>
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{

    lgfx::Panel_ST7789 _panel_instance;

    lgfx::Bus_SPI        _bus_instance;
    
    lgfx::Light_PWM    _light_instance;

public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = true;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = TFT_SCLK;
      cfg.pin_mosi = TFT_MOSI;
      cfg.pin_miso = TFT_MISO;
      cfg.pin_dc   = TFT_DC;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs           =     TFT_CS;
      cfg.pin_rst          =    TFT_RST;
      cfg.pin_busy         =         -1;

      cfg.panel_width      =    76;
      cfg.panel_height     =   284;
      cfg.offset_x         =    82;
      cfg.offset_y         =    18;
      cfg.offset_rotation  =     1;
      cfg.dummy_read_pixel =     8;
      cfg.dummy_read_bits  =     1;
      cfg.readable         =  true;
      cfg.invert           = false;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       =  true;

      _panel_instance.config(cfg);
    }

    { 
      auto cfg = _light_instance.config();

      cfg.pin_bl = TFT_BL;
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

