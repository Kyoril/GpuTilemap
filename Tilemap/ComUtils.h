#pragma once

#include <Windows.h>
#include <comdef.h>
#include <cassert>

inline void ThrowIfFail(HRESULT hr)
{
    if (FAILED(hr))
    {
		assert(false);
        throw _com_error(hr);
    }
}
