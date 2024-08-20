#pragma once
#include <iostream>
#include <vector>
#include <string>

#ifdef CREATEDLL_EXPORTS
#define _DECLSPEC __declspec(dllexport)
#else
#define _DECLSPEC __declspec(dllimport)
#endif