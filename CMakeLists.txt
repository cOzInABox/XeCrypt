include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

#
# source
#

file(GLOB XECRYPT_AES_SOURCES "aes/*.c")
file(GLOB XECRYPT_MD5_SOURCES "md5/*.c")
file(GLOB XECRYPT_RC4_SOURCES "rc4/*.c")
file(GLOB XECRYPT_SHA_SOURCES "sha/*.c")
file(GLOB XECRYPT_BIGN_SOURCES "bign/*.c")
file(GLOB XECRYPT_DES_SOURCES "des/*.c")

#
# headers
#

file(GLOB XECRYPT_AES_HEADERS "aes/*.h")
file(GLOB XECRYPT_MD5_HEADERS "md5/*.h")
file(GLOB XECRYPT_RC4_HEADERS "rc4/*.h")
file(GLOB XECRYPT_SHA_HEADERS "sha/*.h")
file(GLOB XECRYPT_BIGN_HEADERS "bign/*.h")
file(GLOB XECRYPT_DES_HEADERS "des/*.h")

add_library(xecrypt STATIC
    xecrypt.h
    xecryptBn.c
	xecryptHmacSha.c
    xecryptBn.h
    xecryptTypes.h
    xecrypt.c
    ${XECRYPT_AES_SOURCES}
    ${XECRYPT_AES_HEADERS}
    ${XECRYPT_MD5_SOURCES}
    ${XECRYPT_MD5_HEADERS}
    ${XECRYPT_RC4_SOURCES}
    ${XECRYPT_RC4_HEADERS}
    ${XECRYPT_SHA_SOURCES}
    ${XECRYPT_SHA_HEADERS}
    ${XECRYPT_BIGN_SOURCES}
    ${XECRYPT_BIGN_HEADERS}
    ${XECRYPT_DES_SOURCES}
    ${XECRYPT_DES_HEADERS}
)
