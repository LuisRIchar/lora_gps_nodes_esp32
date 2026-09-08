
# Consider dependencies only in project.
set(CMAKE_DEPENDS_IN_PROJECT_ONLY OFF)

# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/Users/richar/code/esp-idf/iot/lora-nodes/build/x509_crt_bundle.S" "/Users/richar/code/esp-idf/iot/lora-nodes/build/esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/__/__/x509_crt_bundle.S.obj"
  )
set(CMAKE_ASM_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "ESP_PLATFORM"
  "ESP_PSA_ITS_AVAILABLE"
  [[IDF_VER="v6.0"]]
  [[MBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"]]
  "MBEDTLS_MAJOR_VERSION=4"
  "SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE"
  "SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ"
  [[TF_PSA_CRYPTO_USER_CONFIG_FILE="mbedtls/esp_config.h"]]
  "_GLIBCXX_HAVE_POSIX_SEMAPHORE"
  "_GLIBCXX_USE_POSIX_SEMAPHORE"
  "_GNU_SOURCE"
  "_POSIX_READER_WRITER_LOCKS"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "config"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/port/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/library"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/core"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/src"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/esp_crt_bundle/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/port/psa_driver/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_libc/platform_include"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/config/include"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/config/include/freertos"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/config/xtensa/include"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/FreeRTOS-Kernel/include"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include/freertos"
  "/Users/richar/toolchains/esp/esp-idf/components/freertos/esp_additions/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/include/soc"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/ldo/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/debug_probe/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/etm/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/mspi_timing_tuning/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/mspi_timing_tuning/tuning_scheme_impl/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/power_supply/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/modem/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/include/soc/esp32s3"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/port/esp32s3/."
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/port/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/mspi_timing_tuning/port/esp32s3/."
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hw_support/mspi_timing_tuning/port/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/heap/include"
  "/Users/richar/toolchains/esp/esp-idf/components/heap/tlsf"
  "/Users/richar/toolchains/esp/esp-idf/components/log/include"
  "/Users/richar/toolchains/esp/esp-idf/components/soc/include"
  "/Users/richar/toolchains/esp/esp-idf/components/soc/esp32s3"
  "/Users/richar/toolchains/esp/esp-idf/components/soc/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/soc/esp32s3/register"
  "/Users/richar/toolchains/esp/esp-idf/components/hal/platform_port/include"
  "/Users/richar/toolchains/esp/esp-idf/components/hal/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/hal/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_rom/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_rom/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_rom/esp32s3/include/esp32s3"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_rom/esp32s3"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_common/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_system/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_system/port/soc"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_system/port/include/private"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_stdio/include"
  "/Users/richar/toolchains/esp/esp-idf/components/xtensa/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/xtensa/include"
  "/Users/richar/toolchains/esp/esp-idf/components/xtensa/deprecated_include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_gpio/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_gpio/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_usb/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_usb/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_pmu/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_pmu/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_ana_conv/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_ana_conv/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_dma/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_dma/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/include/apps"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/lwip/src/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/port/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/port/freertos/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/port/esp32xx/include"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/port/esp32xx/include/arch"
  "/Users/richar/toolchains/esp/esp-idf/components/lwip/port/esp32xx/include/sys"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_security/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_security/esp32s3/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_hal_security/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_pm/include"
  "/Users/richar/toolchains/esp/esp-idf/components/esp_driver_dma/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/builtin/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/everest/include"
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/mbedtls/tf-psa-crypto/drivers/p256-m"
  )

# The set of dependency files which are needed:
set(CMAKE_DEPENDS_DEPENDENCY_FILES
  "/Users/richar/toolchains/esp/esp-idf/components/mbedtls/esp_crt_bundle/esp_crt_bundle.c" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/esp_crt_bundle/esp_crt_bundle.c.obj" "gcc" "esp-idf/mbedtls/CMakeFiles/__idf_mbedtls.dir/esp_crt_bundle/esp_crt_bundle.c.obj.d"
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_LINKED_INFO_FILES
  )

# Targets to which this target links which contain Fortran sources.
set(CMAKE_Fortran_TARGET_FORWARD_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
