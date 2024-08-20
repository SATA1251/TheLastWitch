//
//���۱�(c) 2009 - 2010 Mikko Mononen memon@inside.org
//
// �� ����Ʈ����� ��� ����� �Ǵ� ������ ���� ���� '�ִ� �״��' �����˴ϴ�.
// ��� ��쿡�� �ۼ��ڴ� �� ����Ʈ������ ������� ���� �߻��ϴ� ��� ���ؿ� ���ؼ���
//  å���� ���� �ʽ��ϴ�.
// �������� �� ����Ʈ��� ����� �뵵�� �����Ͽ� ��� �뵵�ε� ����� �� ������,
//  ������ ���� ���� ���׿� ���� �����Ӱ� �����ϰ� ������� �� �ֽ��ϴ� :
// 1. �� ����Ʈ������ ��ó�� ������ ǥ���ؼ��� �� �Ǹ�, ������ ���� ������ �ؼ��� �� �˴ϴ�.
// �ڽ��� ���� ����Ʈ��� �ۼ��ߴٰ� �����ؼ��� �� �˴ϴ�.
// �� ����Ʈ��� ��ǰ�� ����ϴ� ��� ��ǰ ������ �� ����� ����ϸ�
// �����ϰ����� �ʼ��� �ƴմϴ�.
// 2. ����� �ҽ� ������ �� ����� ��Ȯ�� ǥ���ؾ� �ϸ�,
// ���� ����Ʈ������ ��ó�� �߸� ǥ���ؼ��� �� �˴ϴ�.
// 3. �� ������ �ҽ� �������� �����ϰų� ������ �� �����ϴ�.
//
//Translated with DeepL.com(free version)
//

#ifndef RECASTASSERT_H
#define RECASTASSERT_H

#ifdef NDEBUG

// From https://web.archive.org/web/20210117002833/http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
#	define rcAssert(x) do { (void)sizeof(x); } while ((void)(__LINE__==-1), false)

#else

/// ����̼� ���� �Լ��Դϴ�.
//  @param[in]		expression  asserted expression.
//  @param[in]		file  Filename of the failed assertion.
//  @param[in]		line  Line number of the failed assertion.
///  @see rcAssertFailSetCustom
typedef void (rcAssertFailFunc)(const char* expression, const char* file, int line);

/// Recast���� ����� �⺻ ����� ���� ��� ���� �Լ��� �����մϴ�.
/// @param[in] assertFailFunc #dtAssert ���� �� ����� �Լ�.
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
