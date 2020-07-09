# 題目功能
   1. 開機後OLED顯示器全熄滅1秒
   2. 以1Hz速度讀取光感應器之ADC值，並顯示在OLED顯示器 上，同時COM Port送出 "Light Value=xxxx \r\n" 字串。
      (xxxx為ADC的10進制讀值) 
   3. 遮住/放開光感應器，檢查ADC值是否有正確變化。
   4. OLED顯示器之顯示內容 "OLED"Light Meter"
                                "Light Value=xxxx"
# GPIO Define
   1. 光感應器插在擴充板A0位置 
      A0 -> GPIO 57)
   2. OLED顯示器插在I2C位置
      I2C0_CLK -> GPIO 27
      I2C0_DATA -> GPIO 28
