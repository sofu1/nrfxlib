#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

#
# Internal function to append all unnamed parameters with 'prefix'
# and add it to the list 'var'
#
function(append_with_prefix var prefix)
  set(listVar ${${var}})
  foreach(f ${ARGN})
    nrf_security_debug("Adding: ${f} to ${var}")
    list(APPEND listVar "${prefix}${f}")
  endforeach(f)
  set(${var} ${listVar} PARENT_SCOPE)
endfunction(append_with_prefix)

#
# Internal macro to append all unnamed parameters with 'prefix' if condition
# is met
#
macro(append_with_prefix_ifdef var cond prefix)
  if (cond)
    append_with_prefix(prefix ${ARGN})
  endif()
endmacro()


nrf_security_debug("Creating list of files for base mbed TLS lib")

#
# Files only in the base library (not glued, unmodified from vanilla mbed TLS)
#
append_with_prefix(src_crypto ${ARM_MBEDTLS_PATH}/library/
    aesni.c
    arc4.c
    aria.c
    asn1parse.c
    asn1write.c
    base64.c
    bignum.c
    blowfish.c
    camellia.c
    cipher.c
    cipher_wrap.c
    ctr_drbg.c
    des.c
    dhm.c
    ecjpake.c
    entropy_poll.c
    error.c
    gcm.c
    havege.c
    hkdf.c
    hmac_drbg.c
    md.c
    md_wrap.c
    md2.c
    md4.c
    md5.c
    memory_buffer_alloc.c
    nist_kw.c
    oid.c
    padlock.c
    pk.c
    pk_wrap.c
    pkcs12.c
    pkcs5.c
    pkparse.c
    pkwrite.c
    platform.c
    platform_util.c
    ripemd160.c
    rsa_internal.c
    sha512.c
    timing.c
    version.c
    version_features.c
    xtea.c
)

nrf_security_debug("Creating list of files for x509 lib")
#
# x509 files
#r
append_with_prefix(src_x509 ${ARM_MBEDTLS_PATH}/library/
    certs.c
    pkcs11.c
    x509.c
    x509_create.c
    x509_crl.c
    x509_crt.c
    x509_csr.c
    x509write_crt.c
    x509write_csr.c
)

nrf_security_debug("Creating list of files for TLS lib")
#
# TLS files
#
append_with_prefix(src_tls ${ARM_MBEDTLS_PATH}/library/
    debug.c
    net_sockets.c
    ssl_cache.c
    ssl_cli.c
    ssl_cookie.c
    ssl_srv.c
    ssl_ticket.c
    ssl_tls.c
)

# ----------  Replacement sources - modified mbedTLS sources  -------------<

nrf_security_debug("Adding replacement files (modified mbed TLS)")

append_with_prefix(src_crypto_replacement
  ${NRF_SECURITY_ROOT}/src/mbedtls/replacements/
  pem.c
)

append_with_prefix(src_tls_replacement
  ${NRF_SECURITY_ROOT}/src/mbedtls/replacements/
  ssl_ciphersuites.c
)

zephyr_include_directories(${common_includes})

#
# Library containing code not in Glue or backend
#
zephyr_library_named(mbedtls_base_vanilla)
zephyr_library_sources(${src_crypto} ${src_crypto_replacement})

#
# Add files if not glued (i.e. when vanilla only is selected)
#
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_AES_C
  ${ARM_MBEDTLS_PATH}/library/aes.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_CCM_C
  ${ARM_MBEDTLS_PATH}/library/ccm.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_CMAC_C
  ${ARM_MBEDTLS_PATH}/library/cmac.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_RSA_C
  ${ARM_MBEDTLS_PATH}/library/rsa.c
)

#
# Files that are either in vanilla or backend
#
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_ECP_C
  ${ARM_MBEDTLS_PATH}/library/ecp.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_ECP_C
  ${ARM_MBEDTLS_PATH}/library/ecp_curves.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_ECDH_C
  ${ARM_MBEDTLS_PATH}/library/ecdh.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_ECDSA_C
  ${ARM_MBEDTLS_PATH}/library/ecdsa.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_ECJPAKE_C
  ${ARM_MBEDTLS_PATH}/library/ecjpake.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_SHA256_C
  ${ARM_MBEDTLS_PATH}/library/sha256.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_SHA1_C
  ${ARM_MBEDTLS_PATH}/library/sha1.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_CHACHA20_C
  ${ARM_MBEDTLS_PATH}/library/chacha20.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_POLY1305_C
  ${ARM_MBEDTLS_PATH}/library/poly1305.c
)
zephyr_library_sources_ifdef(VANILLA_ONLY_MBEDTLS_CHACHAPOLY_C
  ${ARM_MBEDTLS_PATH}/library/chachapoly.c
)
zephyr_library_sources(${ZEPHYR_BASE}/../modules/crypto/mbedtls/zephyr_init.c)

if(CONFIG_SOC_NRF52840 OR CONFIG_SOC_NRF9160)
  zephyr_library_sources(${NRF_SECURITY_ROOT}/src/backend/cc310/replacements/entropy.c)
else()
  zephyr_library_sources(${NRF_SECURITY_ROOT}/src/backend/nrf5x/entropy_nrf5x.c)
endif()
zephyr_library_link_libraries(${IMAGE}mbedtls_common)
nrf_security_debug_list_target_files(${IMAGE}mbedtls_base_vanilla)

#
# Library for x.509
#
if (CONFIG_MBEDTLS_X509_LIBRARY)
  zephyr_library_named(mbedtls_x509_vanilla)
  zephyr_library_sources(${src_x509})
  zephyr_library_link_libraries(${IMAGE}mbedtls_common)
  nrf_security_debug_list_target_files(${IMAGE}mbedtls_x509_vanilla)
endif()

#
# Library for TLS support
#
if (CONFIG_MBEDTLS_TLS_LIBRARY)
  zephyr_library_named(mbedtls_tls_vanilla)
  zephyr_library_sources(${src_tls} ${src_tls_replacement})
  zephyr_library_link_libraries(${IMAGE}mbedtls_common)
  nrf_security_debug_list_target_files(${IMAGE}mbedtls_tls_vanilla)
endif()

if(NOT CONFIG_NRF_CRYPTO_BACKEND_COMBINATION_0)
  #
  # Only single backend is chosen, thus append it to zephyr libraries as
  # no renaming should be carried out.
  #
  if(TARGET ${IMAGE}mbedcrypto_cc310)
    target_link_libraries(${IMAGE}mbedtls_base_vanilla PRIVATE ${IMAGE}mbedcrypto_cc310)
  endif()
  if(TARGET ${IMAGE}platform_cc310)
    target_link_libraries(${IMAGE}mbedtls_base_vanilla PRIVATE ${IMAGE}platform_cc310)
  endif()
endif()


#
# Generate a dummy library called ${IMAGE}mbedtls_external to be able to use
# CONFIG_MBEDTLS and CONFIG_MBEDTLS_LIBRARY in zephyr sockets
#
add_library(${IMAGE}mbedtls_external)
target_sources(${IMAGE}mbedtls_external PRIVATE ${ZEPHYR_BASE}/misc/empty_file.c)

#
# Give the main application access to the generated includes and mbed TLS config
#
# NOTE: This library corresponds to APP_LINK_WITH_MBEDTLS_INCLUDES in Kconfig
#       which is always set to "true".
#
zephyr_interface_library_named(mbedtls_includes)
target_link_libraries(${IMAGE}mbedtls_includes INTERFACE ${IMAGE}mbedtls_common)

#
# Give zephyr libraries access to includes and mbed TLS config
#
zephyr_link_libraries(${IMAGE}mbedtls_common)
