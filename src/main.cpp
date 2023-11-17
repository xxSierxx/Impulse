#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <cmath>

void delay ();
void interrupt ();
void acp ();
void pwm();


constexpr uint32_t POINT_PERIOD{5'000'00}; //счётных импульсов на период
constexpr uint32_t CK_CNT_Hz{1'000'000}; //  частота счётного импульса. Минимум 10. Это нужно ли увеличить?
int32_t ticks{0}; //Счетчик тиков сиситемного таймера (время в мс)
int s = 1;

int main () {

    //Настройка таймера
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM2);
    timer_set_prescaler(TIM2,rcc_get_timer_clk_freq(TIM2) / CK_CNT_Hz - 1);

    rcc_periph_clock_enable(RCC_GPIOD); //  тактирование канала D для светодиода
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO13);   // запуск голубого светодиода


    timer_set_period(TIM2, POINT_PERIOD - 1);

    nvic_enable_irq(NVIC_TIM2_IRQ);
    nvic_set_priority(NVIC_TIM2_IRQ,1);
    timer_enable_irq(TIM2, TIM_DIER_UIE);

    timer_enable_counter(TIM2);


    void interrupt();


    while (true) {


        //PE9 - синий, PE11 - желтый, PE13 - розовый, PE14 - оранжевый

        if(timer_get_flag(TIM2,TIM_SR_UIF != 0)) {

            gpio_toggle(GPIOD, GPIO13);


        }

    }
}

void tim2_isr (void){

    if (timer_interrupt_source(TIM2, TIM_SR_UIF)) {
        timer_clear_flag(TIM2, TIM_SR_UIF);

        switch (s){
        case 1:
            gpio_set(GPIOE, GPIO14);
            gpio_clear(GPIOE, GPIO9|GPIO11|GPIO13);
            break;
        case 2:
            gpio_set(GPIOE, GPIO11|GPIO14);
            gpio_clear(GPIOE, GPIO9|GPIO13);
            break;
        case 3:
            gpio_set(GPIOE, GPIO11);
            gpio_clear(GPIOE, GPIO9|GPIO13|GPIO14);
            break;
        case 4:
            gpio_set(GPIOE, GPIO11|GPIO13);
            gpio_clear(GPIOE, GPIO9|GPIO14);
        case 5:
            gpio_set(GPIOE, GPIO13);
            gpio_clear(GPIOE, GPIO9|GPIO11|GPIO14);
            break;

        case 6:
            gpio_set(GPIOE, GPIO9|GPIO13);
            gpio_clear(GPIOE, GPIO11|GPIO14);
            break;

        case 7:
            gpio_set(GPIOE, GPIO9);
            gpio_clear(GPIOE, GPIO11|GPIO13|GPIO14);
            break;

        default:
            gpio_set(GPIOE, GPIO9);
            gpio_clear(GPIOE, GPIO11|GPIO13|GPIO14);
            s = 1;
            break;
        }
        s++;
    }

}

/*  void interrupt (){
        rcc_periph_clock_enable(RCC_GPIOE);
        gpio_mode_setup(GPIOE,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO9|GPIO11|GPIO13|GPIO14); //Для Шаг.двиг

    }*/



//void pwm(){
//
//    rcc_periph_clock_enable(RCC_GPIOA); //  тактирование для ШИМ
//    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8); // запуск ШИМ
//    gpio_set_af(GPIOA, GPIO_AF1, GPIO8); // установка АФ для ШИМ

// Настройка ШИМ
//    rcc_periph_clock_enable(RCC_TIM1);  //  тактирование таймера_1
//timer_set_prescaler(TIM1, rcc_get_timer_clk_freq(TIM1)/CK_CNT_Hz - 1);
// 0 - деление на "1",  на единицу меньше того из-за аппаратной составляющей
//timer_set_period(TIM1, POINT_PERIOD - 1);             //период шима
//timer_set_oc_value(TIM1, TIM_OC1, POINT_PERIOD - 90); // установка значения сравнения выхода
//timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);       // устанавливаем для порта_1 генерацию ШИМ
//timer_enable_oc_output(TIM1, TIM_OC1);                // включения выходного порта-сравнения
//timer_enable_break_main_output(TIM1);                 // включает режи простоя
//timer_enable_counter(TIM1);                           // запуск таймера_1
//}


//void acp(){
//     rcc_periph_clock_enable(RCC_ADC1);  //  тактирование для АЦП
//     gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0); // запуск АЦП
//}
//       while (volt<20) {
//          volt-=1;
//        }



//        adc_start_conversion_regular(ADC1);               // включение режима regular для АЦП-ADC1
//        while(!(ADC_SR(ADC1) & ADC_SR_EOC))               // ожидание завершения преобразования
//        {
//         }

//        gpio_toggle(GPIOD, GPIO15);                       // переключение светодиода

//        reg = adc_read_regular(ADC1);                     // запись значения на АЦП

//        if (reg>70) {                   //задаем порог срабатывания для защиты от дребезга
//            reg1 = reg/4095*1145+2950;
//            nums = reg1/4095;           //пересчитываем значения для работы всего диапазона потенциометра только на рабочем значении ШИМ для двигателя
//            timer_set_oc_value (TIM1, TIM_OC1, POINT_PERIOD * nums); // установка значения выхода-сравнения
//        }
//        else {
//            timer_set_oc_value (TIM1, TIM_OC1, POINT_PERIOD * 0);
//        }


//    Настройка  и запуск си nums += 1.25;системного таймера
//    systick_set_frequency(CK_CNT_Hz, rcc_ahb_frequency);
//    systick_interrupt_enable();
//    systick_counter_enable(); // для 1 кГц или 10кГц

//    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);














//  вход 1-АЦП 1 канал PA1 - вход АЦП



//uint32_t ticks{0}; //Счетчик тиков сиситемного таймера (время в мс)
//Обработчик прерывания системного таймера
//void sys_tick_handler (void){
//    ticks++;
//}

//if (ptime < 10'000) timer_set_oc_value(TIM1, TIM_OC4, BLINK_PERIOD_MS/8-1);
//else if (ptime < 20'000) timer_set_oc_value(TIM1, TIM_OC4, BLINK_PERIOD_MS/8*4);
//else if (ptime < 30'000) timer_set_oc_value(TIM1, TIM_OC4, BLINK_PERIOD_MS/8*7);
//else timer_set_oc_value(TIM1, TIM_OC4, BLINK_PERIOD_MS)

//    if (volt<=4000){
//           if(volt == 3900){
//               volt1 = volt;
//               volt+=100;
//               continue;
//           }
//         // delay();
//          dac_load_data_buffer_single(DAC1, volt, DAC_ALIGN_RIGHT12, DAC_CHANNEL1);

//          volt+=100;
//          continue;
//          }

//          while(volt1>=0){
//          //delay();
//          dac_load_data_buffer_single(DAC1, volt1, DAC_ALIGN_RIGHT12, DAC_CHANNEL1);

//          volt1-=100;
//          if(volt1==0)
//           volt = volt1;
//            }
//      }
//}
void delay (){
    for(volatile int i = 0; i<1'000'000; i++){

    }
}
//    //Подключение АЦП

//    adc_power_off(ADC1);                                  // отключение АЦП-ADC1
//    rcc_periph_reset_pulse(RST_ADC);                      // сброс импульса на АЦП
//    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY2);             // установка делителя АЦП
//    adc_disable_scan_mode(ADC1);                          // отключение скан.мода
//    adc_set_single_conversion_mode(ADC1);                 // убирает повторный перебор
//    adc_set_sample_time(ADC1, ADC_CHANNEL0, ADC_SMPR_SMP_15CYC);// установка количество циклов преобразования
//    adc_power_on(ADC1);                                   // включение АЦП

//    // Подключение ЦАП

//    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);    //Подготовить линию порта (PA4 - 1 канал, PA5 -2)
//                                                                        //(режим аналоговой линии, тактирование)
//    rcc_periph_clock_enable(RCC_DAC);                                   //Подготовить ЦАП(тактирование)
//    dac_enable(DAC1, DAC_CHANNEL1);                                     //Разрешить работу
