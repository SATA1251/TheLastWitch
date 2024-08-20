// 내가 만든거긴 하지만 너무 조잡해!!!!!!!
// 코루틴
#pragma once
#include <coroutine>
#include <functional>
#include <vector>
#include <map>

// 사용 할 때는 이것만 기억해도 됨.
// https://www.notion.so/60508ce780814fd2a9e449ea16831a17 사용법 정리해 두었으니 참고.

// 코루틴 사용자에게 중요 안내 사항
// 코루틴은 종료된 다음 프레임에 삭제됩니다. 코루틴이 종료되는 프레임에 불타입이 바뀌는 형태의 함수를 작성했다면,
// 그리고 코루틴이 종료된 순간부터 다음프레임에 삭제되기 전까지 그 불타입을 확인하고 코루틴을 재실행 하는 함수가 있다면
// 코루틴은 여전히 배열에 있으므로 이미 있는 코루틴으로 판단하고 넘어갑니다.
// 이렇게 되면 코루틴이 영원히 동작하지 않게 되니 참고하시기 바랍니다.

#define Waitforsecond(time) co_await KunrealEngine::Coroutine::Coroutine_type::WaitForSeconds(time)
//#define Checkbool(bool_param) co_await KunrealEngine::Coroutine::Coroutine_type::CheckBool(bool_param)
#define Return_null co_await KunrealEngine::Coroutine::Coroutine_type::WaitForSeconds(0.0000001);

/// Coroutine_Func대신 _Coroutine(.h에 선언)과 _CoroutineIs(.cpp내 Startcoroutine사용 직전 구현) 사용 권장
#define Coroutine_Func(coro_func) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func = [this]() ->KunrealEngine::Coroutine::Coroutine_type
#define Coroutine_staticFunc(coro_func, exporter) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func = [exporter]() ->KunrealEngine::Coroutine::Coroutine_type
#define _Coroutine(coro_func) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func
#define _CoroutineIs(coro_func) coro_func = [this]() ->KunrealEngine::Coroutine::Coroutine_type
#define Startcoroutine(coro_func) Coroutine::StartCoroutine(&coro_func)
#define Updatecoroutine Coroutine::UpdateCoroutines

namespace KunrealEngine
{
	// 포장 하기위한 객체. 껍데기에 해당 하는 부분.
	// 코루틴을 보관하고 구분하기 위한 두변수가 전부이다.
	class Coroutine
	{
	public:
		Coroutine();
		~Coroutine();

		// 이건 싱글턴 객체
		// 필요한 인수중에 duration이 있는데 시간을 여기에 담는다.
		// 문제는 아래 구조상 받는 부분이랑 그걸 확인하는 부분이 달라서
		//  둘다 쓸 수 있게 하려다 보니 필요해졌다.
		class DurationManager
		{
		public:
			static DurationManager& getInstance();
			float getDuration() const;
			void setDuration(float newDuration);
			void durationtonull();

		private:
			float duration = 0;
		};

		// 코루틴의 본체
		struct Coroutine_type
		{
			struct promise_type	// promise_type은 기본적으로 coroutine 객체를 생성하기 위해 필요한 객체.
								// 얘가 없으면 코루틴으로 인식을 안하더라
			{
				Coroutine_type get_return_object();
				std::suspend_never initial_suspend() noexcept;	// 꼭 있어야 하는 함수
				std::suspend_always final_suspend() noexcept;	// 꼭 있어야 하는 함수
				void return_void() noexcept;					// 꼭 있어야 하는 함수
				void unhandled_exception() noexcept;			// 꼭 있어야 하는 함수
				bool await_ready() noexcept;					// 꼭 있어야 하는 함수

				float timer = 0;
				float duration = 0;
			};


			class WaitForSeconds // Awaitable객체
			{
			public:
				WaitForSeconds(float seconds);						// 코루틴의 co_await를 쓰기 위해선 이 객체가 있어야 하더라.
				bool await_ready();
				void await_suspend(std::coroutine_handle<> handle);
				void await_resume() const noexcept;

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
			};

			Coroutine_type();
			Coroutine_type(std::coroutine_handle<promise_type> handle); /*: coro_handle(handle)*/
			~Coroutine_type();

			std::coroutine_handle<promise_type> coro_handle;
			std::function<Coroutine_type()>* func_ptr;
			int mapKey = 0;
		};

		static void StartCoroutine(std::function<Coroutine_type()>* coro);	// 코루틴 함수를 받아서 시작시킴
		static void UpdateCoroutines();	// 코루틴 함수들을 업데이트 돌린다

	private:
		static std::vector<Coroutine_type*> _coroutines;	// 코루틴들을 담아두는 곳
		static std::map<int, std::function<Coroutine_type()>*> _AddedCoroutines; // 코루틴의 주소를 저장하는 집합
		static int idexKey;
	};
}