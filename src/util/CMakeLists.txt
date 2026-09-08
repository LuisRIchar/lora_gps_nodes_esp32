idf_component_register(SRCS "crypto.c"
                    INCLUDE_DIRS "."
                    REQUIRES mbedtls) # Add mbedtls for cryptographic functions