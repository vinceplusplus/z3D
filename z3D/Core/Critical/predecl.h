
#pragma once

#ifdef Z3DCORECRIT_EXPORTS
#define __Z3DCORECRIT_API __declspec(dllexport)
#else
#define __Z3DCORECRIT_API __declspec(dllimport)
#endif

namespace z3D
{
	namespace Core
	{
		class __Z3DCORECRIT_API Logger;
	};
};
