idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES spi_flash esp_driver_dac esp_driver_gptimer esp_driver_gpio
                       INCLUDE_DIRS "")
