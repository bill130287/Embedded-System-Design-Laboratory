# 題目功能
   1. 開機後LED BAR 全熄滅
   2. 每按下一次USR BUTTON後，LED BAR依序增加一條顯示，同時COM Port送出"LED BAR = xxxx \r\n"字串。
      (xxxx為十進制的MY9221寫入值，Bit0對應第1條紅色LED...Bit9對應第10條黃色，Ex:第一條紅色 = 1...第10條黃色 = 1023)
   3. 當LED BAR全亮後，再按下USR BUTTON，LED BAR 則回復成顯示第一條
# GPIO Define
   1. LED BAR V2.0 插在擴充板D2位置
   2. LED BAR DATA Pin -> GPIO 0
   3. LED BAR CLK Pin  -> GPIO 39
# MY9221 Control Waveform
   ![Alt text]()
   ![Alt text]() 
