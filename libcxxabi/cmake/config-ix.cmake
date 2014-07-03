include(CheckLibraryExists)
include(CheckCXXCompilerFlag)

# Check compiler flags
check_cxx_compiler_flag(-std=c++11            LIBCXXABI_HAS_STDCXX11_FLAG)
check_cxx_compiler_flag(-fPIC                 LIBCXXABI_HAS_FPIC_FLAG)
check_cxx_compiler_flag(-nodefaultlibs        LIBCXXABI_HAS_NODEFAULTLIBS_FLAG)
check_cxx_compiler_flag(-nostdinc++           LIBCXXABI_HAS_NOSTDINCXX_FLAG)
check_cxx_compiler_flag(-Wall                 LIBCXXABI_HAS_WALL_FLAG)
check_cxx_compiler_flag(-W                    LIBCXXABI_HAS_W_FLAG)
check_cxx_compiler_flag(-Wno-unused-function  LIBCXXABI_HAS_WNO_UNUSED_FUNCTION_FLAG)
check_cxx_compiler_flag(-Wno-unused-parameter LIBCXXABI_HAS_WNO_UNUSED_PARAMETER_FLAG)
check_cxx_compiler_flag(-Wwrite-strings       LIBCXXABI_HAS_WWRITE_STRINGS_FLAG)
check_cxx_compiler_flag(-Wno-long-long        LIBCXXABI_HAS_WNO_LONG_LONG_FLAG)
check_cxx_compiler_flag(-pedantic             LIBCXXABI_HAS_PEDANTIC_FLAG)
check_cxx_compiler_flag(-Werror               LIBCXXABI_HAS_WERROR_FLAG)
check_cxx_compiler_flag(-Wno-error            LIBCXXABI_HAS_WNO_ERROR_FLAG)
check_cxx_compiler_flag(-fno-rtti             LIBCXXABI_HAS_FNO_RTTI_FLAG)
check_cxx_compiler_flag(/WX                   LIBCXXABI_HAS_WX_FLAG)
check_cxx_compiler_flag(/WX-                  LIBCXXABI_HAS_NO_WX_FLAG)
check_cxx_compiler_flag(/EHsc                 LIBCXXABI_HAS_EHSC_FLAG)
check_cxx_compiler_flag(/EHs-                 LIBCXXABI_HAS_NO_EHS_FLAG)
check_cxx_compiler_flag(/EHa-                 LIBCXXABI_HAS_NO_EHA_FLAG)
check_cxx_compiler_flag(/GR-                  LIBCXXABI_HAS_NO_GR_FLAG)

# Check libraries
check_library_exists(c printf "" LIBCXXABI_HAS_C_LIB)
