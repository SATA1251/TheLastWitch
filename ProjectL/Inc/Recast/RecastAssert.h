//
//저작권(c) 2009 - 2010 Mikko Mononen memon@inside.org
//
// 이 소프트웨어는 어떠한 명시적 또는 묵시적 보증 없이 '있는 그대로' 제공됩니다.
// 어떠한 경우에도 작성자는 이 소프트웨어의 사용으로 인해 발생하는 어떠한 손해에 대해서도
//  책임을 지지 않습니다.
// 누구든지 이 소프트웨어를 상업적 용도를 포함하여 어떠한 용도로든 사용할 수 있으며,
//  다음과 같은 제한 사항에 따라 자유롭게 변경하고 재배포할 수 있습니다 :
// 1. 이 소프트웨어의 출처를 허위로 표시해서는 안 되며, 다음과 같은 행위를 해서는 안 됩니다.
// 자신이 원본 소프트웨어를 작성했다고 주장해서는 안 됩니다.
// 이 소프트웨어를 제품에 사용하는 경우 제품 설명서에 이 사실을 명시하면
// 감사하겠지만 필수는 아닙니다.
// 2. 변경된 소스 버전은 그 사실을 명확히 표시해야 하며,
// 원본 소프트웨어인 것처럼 잘못 표시해서는 안 됩니다.
// 3. 이 공지는 소스 배포에서 제거하거나 변경할 수 없습니다.
//
//Translated with DeepL.com(free version)
//

#ifndef RECASTASSERT_H
#define RECASTASSERT_H

#ifdef NDEBUG

// From https://web.archive.org/web/20210117002833/http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
#	define rcAssert(x) do { (void)sizeof(x); } while ((void)(__LINE__==-1), false)

#else

/// 어설테이션 실패 함수입니다.
//  @param[in]		expression  asserted expression.
//  @param[in]		file  Filename of the failed assertion.
//  @param[in]		line  Line number of the failed assertion.
///  @see rcAssertFailSetCustom
typedef void (rcAssertFailFunc)(const char* expression, const char* file, int line);

/// Recast에서 사용할 기본 사용자 정의 어설션 실패 함수를 설정합니다.
/// @param[in] assertFailFunc #dtAssert 실패 시 사용할 함수.
void rcAssertFailSetCustom(rcAssertFailFunc* assertFailFunc);

/// Gets the base custom assertion failure function to be used by Recast.
rcAssertFailFunc* rcAssertFailGetCustom();

#	include <assert.h> 
#	define rcAssert(expression) \
		{ \
			rcAssertFailFunc* failFunc = rcAssertFailGetCustom(); \
			if (failFunc == NULL) { assert(expression); } \
			else if (!(expression)) { (*failFunc)(#expression, __FILE__, __LINE__); } \
		}

#endif

#endif // RECASTASSERT_H
