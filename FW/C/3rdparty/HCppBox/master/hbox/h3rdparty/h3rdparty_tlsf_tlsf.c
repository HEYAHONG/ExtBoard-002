/***************************************************************
 * Name:      h3rdparty_tlsf.c
 * Purpose:   引入第三方源代码文件
 * Author:    HYH (hyhsystem.cn)
 * Created:   2025-06-27
 * Copyright: HYH (hyhsystem.cn)
 * License:   MIT
 **************************************************************/
#include "hdefaults.h"
#include "h3rdparty.h"

#ifdef printf
#undef printf
#endif
#define printf hprintf

#ifndef  H3RDPARTY_USING_SYSTEM_TLSF

#include "3rdparty/tlsf/tlsf.c"

#endif // H3RDPARTY_USING_SYSTEM_TLSF


