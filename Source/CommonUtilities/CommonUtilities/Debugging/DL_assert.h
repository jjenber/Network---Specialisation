#pragma once
#if defined _VCRT_BUILD && !defined _ASSERT_OK
    #error assert.h not for CRT internal use
#endif

#include "DL_StackWalker.h"
#include <corecrt.h>

_CRT_BEGIN_C_HEADER



#undef assert

_ACRTIMP void __cdecl _wassert(
    _In_z_ wchar_t const* _Message,
    _In_z_ wchar_t const* _File,
    _In_   unsigned       _Line
    );

#define assert(expression) (void)(                                                       \
        (!!(expression)) ||                                                              \
        (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
    ); \
	if((expression) == false) { DL_Debug::StackWalker sw; sw.ShowCallstack(); }

_CRT_END_C_HEADER
