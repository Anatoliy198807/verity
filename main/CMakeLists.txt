# CMakeLists.txt для компонента "main"
idf_component_register(
    SRCS "main.c"                    # список исходников
    INCLUDE_DIRS "."                 # директории с заголовками
    REQUIRES driver                  # зависимости: нужен для dac.h, esp_rom_delay_us
)