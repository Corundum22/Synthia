idf_component_register(SRCS "main.c" "synth_audio.c" "waveform_tables.c" "basic_io.c" "uart_handler.c" "display_control.c" "note_handler.c" "ui.c" "data_y_splitter.c" "sequencer.c"
                       PRIV_REQUIRES spi_flash esp_driver_gpio esp_driver_dac esp_driver_gptimer esp_driver_ledc esp_driver_uart esp_adc freertos esp_lcd esp_timer
                       INCLUDE_DIRS "")
