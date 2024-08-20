// 내가 만든거긴 하지만 너무 조잡해!!!!!!!
// 코루틴
#pragma once
#include <coroutine>
#include <functional>
#include <vector>
#include <map>

// 사용 할 때는 이것만 기억해도 됨.
// https://www.notion.so/60508ce780814fd2a9e449ea16831a17 사용법 정리해 두었으니 참고.

#define Waitforsecond(time) co_await KunrealEngine::WaitForSeconds(time)
#define Checkbool(bool_param) co_await KunrealEngine::CheckBool(bool_param)
#define Return_null co_await KunrealEngine::ReturnNull()
#define Coroutine_Func(coro_func) std::function<KunrealEngine::Coroutine()> coro_func = [this]() ->KunrealEngine::Coroutine
#define Startcoroutine(coro_func) Coroutine::StartCoroutine(coro_func)
#define Updatecoroutine Coroutine::UpdateCoroutines

namespace KunrealEngine
{
	class WaitForSeconds // Awaitable객체
	{
	public:
		WaitForSeconds(float seconds);						// 코루틴의 co_await를 쓰기 위해선 이 객체가 있어야 하더라.
		bool await_ready();
		void await_suspend(std::coroutine_handle<> handle);
		void await_resume() const noexcept;
		float timer = 0;
		float time = 0;
	};

	class CheckBool // Awaitable객체
	{
	public:
		CheckBool(bool& param);						// 코루틴의 co_await를 쓰기 위해선 이 객체가 있어야 하더라.
		bool await_ready();
		void await_suspend(std::coroutine_handle<> handle);
		void await_resume() const noexcept;
		bool& flag;
	};

	class ReturnNull // Awaitable객체
	{
	public:
		ReturnNull();						// 코루틴의 co_await를 쓰기 위해선 이 객체가 있어야 하더라.
		bool await_ready();
		void await_suspend(std::coroutine_handle<> handle);
		void await_resume() const noexcept;
		bool flag;
	};

	// 포장 하기위한 객체. 껍데기에 해당 하는 부분.
	// 코루틴을 보관하고 구분하기 위한 두변수가 전부이다.
	class Coroutine
	{
	public:

		struct promise_type	// promise_type은 기본적으로 coroutine 객체를 생성하기 위해 필요한 객체.
			// 얘가 없으면 코루틴으로 인식을 안하더라
		{
			Coroutine get_return_object();
			std::suspend_never initial_suspend() noexcept;	// 꼭 있어야 하는 함수
			std::suspend_always final_suspend() noexcept;	// 꼭 있어야 하는 함수
			void return_void() noexcept;					// 꼭 있어야 하는 함수
			void unhandled_exception() noexcept;			// 꼭 있어야 하는 함수
			bool await_ready() noexcept;					// 꼭 있어야 하는 함수
		};

		Coroutine(std::coroutine_handle<promise_type> handle); /*: coro_handle(handle)*/
		~Coroutine();

		std::coroutine_handle<promise_type> coro_handle;
		std::function<Coroutine()> corofunc;
		int mapKey = 0;

		static void StartCoroutine(std::function<Coroutine()> coro);	// 코루틴 함수를 받아서 시작시킴
		static void UpdateCoroutines();	// 코루틴 함수들을 업데이트 돌린다

	private:
		static std::vector<int> keybox;					   // 코루틴 키들을 저장하는 키박스
		static std::map<int, Coroutine*> _AddedCoroutines; // 코루틴의 주소를 저장하는 집합
		static int idexKey;
	};
}