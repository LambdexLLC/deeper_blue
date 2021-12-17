#pragma once

#define LAMBDEX_EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	// Testing function
	void LAMBDEX_EXPORT foo();

#ifdef __cplusplus
};
#endif // __cplusplus
