idf_component_register(SRCS "main.c" "synth_audio.c" "waveform_tables.c"
                       PRIV_REQUIRES spi_flash esp_driver_gpio esp_driver_dac esp_driver_gptimer
                       INCLUDE_DIRS "")
