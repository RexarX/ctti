#pragma once

#define CTTI_PP_STR_IMPL(x) #x
#define CTTI_PP_STR(x) CTTI_PP_STR_IMPL(x)

#define CTTI_PP_CAT_IMPL(a, b) a##b
#define CTTI_PP_CAT(a, b) CTTI_PP_CAT_IMPL(a, b)
