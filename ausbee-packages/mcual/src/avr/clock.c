#include <include/generated/autoconf.h>
#ifdef CONFIG_MCUAL_CLOCK

#include <mcual.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void mcual_clock_init(mcual_clock_source_t source, int32_t target_freq_kHz)
{
  uint8_t clksrc = 0;

  if(source == MCUAL_CLOCK_SOURCE_EXTERNAL)
  {
    if(HSE_VALUE < 2000000)
    {
      OSC.XOSCCTRL = OSC_FRQRANGE_04TO2_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    }
    else if(HSE_VALUE < 9000000)
    {
      OSC.XOSCCTRL = OSC_FRQRANGE_2TO9_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    }
    else if(HSE_VALUE < 12000000)
    {
      OSC.XOSCCTRL = OSC_FRQRANGE_9TO12_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    }
    else
    {
      OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    }
    clksrc = CLK_SCLKSEL_XOSC_gc;
    OSC.CTRL |= OSC_XOSCEN_bm;
    while(!(OSC.STATUS & OSC_XOSCRDY_bm));
  }
  else
  {
    clksrc = CLK_SCLKSEL_RC2M_gc;
    OSC.CTRL |= OSC_RC2MEN_bm;
    while(!(OSC.STATUS & OSC_RC2MRDY_bm));
  }

  if(target_freq_kHz > 0)
  {
    uint8_t pll = 0;
    if(source == MCUAL_CLOCK_SOURCE_EXTERNAL)
    {
      pll = OSC_PLLSRC_XOSC_gc | ((target_freq_kHz / (HSE_VALUE / 1000)) & 0x1F);
    }
    else
    {
      pll = OSC_PLLSRC_RC2M_gc | ((target_freq_kHz / 2000) & 0x1F);
    }
    clksrc = CLK_SCLKSEL_PLL_gc;
    OSC.PLLCTRL = pll;
    OSC.CTRL |= OSC_PLLEN_bm;
    while(!(OSC.STATUS & OSC_PLLRDY_bm));
  }

  CLK.PSCTRL = CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc;
  do 
  {
    mcual_arch_avr_ccpwrite(&CLK.CTRL, clksrc);
  } while( (CLK.CTRL & CLK_SCLKSEL_gm) != clksrc);

  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
  sei();
}

uint32_t mcual_clock_get_frequency_Hz(mcual_clock_id_t clock_id)
{
  (void)clock_id;
  return 32000000;
}

#endif
