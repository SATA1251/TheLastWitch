// ���� ����ű� ������ �ʹ� ������!!!!!!!
// �ڷ�ƾ
#pragma once
#include <coroutine>
#include <functional>
#include <vector>
#include <map>

// ��� �� ���� �̰͸� ����ص� ��.
// https://www.notion.so/60508ce780814fd2a9e449ea16831a17 ���� ������ �ξ����� ����.

// �ڷ�ƾ ����ڿ��� �߿� �ȳ� ����
// �ڷ�ƾ�� ����� ���� �����ӿ� �����˴ϴ�. �ڷ�ƾ�� ����Ǵ� �����ӿ� ��Ÿ���� �ٲ�� ������ �Լ��� �ۼ��ߴٸ�,
// �׸��� �ڷ�ƾ�� ����� �������� ���������ӿ� �����Ǳ� ������ �� ��Ÿ���� Ȯ���ϰ� �ڷ�ƾ�� ����� �ϴ� �Լ��� �ִٸ�
// �ڷ�ƾ�� ������ �迭�� �����Ƿ� �̹� �ִ� �ڷ�ƾ���� �Ǵ��ϰ� �Ѿ�ϴ�.
// �̷��� �Ǹ� �ڷ�ƾ�� ������ �������� �ʰ� �Ǵ� �����Ͻñ� �ٶ��ϴ�.

#define Waitforsecond(time) co_await KunrealEngine::Coroutine::Coroutine_type::WaitForSeconds(time)
//#define Checkbool(bool_param) co_await KunrealEngine::Coroutine::Coroutine_type::CheckBool(bool_param)
#define Return_null co_await KunrealEngine::Coroutine::Coroutine_type::WaitForSeconds(0.0000001);

/// Coroutine_Func��� _Coroutine(.h�� ����)�� _CoroutineIs(.cpp�� Startcoroutine��� ���� ����) ��� ����
#define Coroutine_Func(coro_func) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func = [this]() ->KunrealEngine::Coroutine::Coroutine_type
#define Coroutine_staticFunc(coro_func, exporter) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func = [exporter]() ->KunrealEngine::Coroutine::Coroutine_type
#define _Coroutine(coro_func) std::function<KunrealEngine::Coroutine::Coroutine_type()> coro_func
#define _CoroutineIs(coro_func) coro_func = [this]() ->KunrealEngine::Coroutine::Coroutine_type
#define Startcoroutine(coro_func) Coroutine::StartCoroutine(&coro_func)
#define Updatecoroutine Coroutine::UpdateCoroutines

namespace KunrealEngine
{
	// ���� �ϱ����� ��ü. �����⿡ �ش� �ϴ� �κ�.
	// �ڷ�ƾ�� �����ϰ� �����ϱ� ���� �κ����� �����̴�.
	class Coroutine
	{
	public:
		Coroutine();
		~Coroutine();

		// �̰� �̱��� ��ü
		// �ʿ��� �μ��߿� duration�� �ִµ� �ð��� ���⿡ ��´�.
		// ������ �Ʒ� ������ �޴� �κ��̶� �װ� Ȯ���ϴ� �κ��� �޶�
		//  �Ѵ� �� �� �ְ� �Ϸ��� ���� �ʿ�������.
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

		// �ڷ�ƾ�� ��ü
		struct Coroutine_type
		{
			struct promise_type	// promise_type�� �⺻������ coroutine ��ü�� �����ϱ� ���� �ʿ��� ��ü.
								// �갡 ������ �ڷ�ƾ���� �ν��� ���ϴ���
			{
				Coroutine_type get_return_object();
				std::suspend_never initial_suspend() noexcept;	// �� �־�� �ϴ� �Լ�
				std::suspend_always final_suspend() noexcept;	// �� �־�� �ϴ� �Լ�
				void return_void() noexcept;					// �� �־�� �ϴ� �Լ�
				void unhandled_exception() noexcept;			// �� �־�� �ϴ� �Լ�
				bool await_ready() noexcept;					// �� �־�� �ϴ� �Լ�

				float timer = 0;
				float duration = 0;
			};


			class WaitForSeconds // Awaitable��ü
			{
			public:
				WaitForSeconds(float seconds);						// �ڷ�ƾ�� co_await�� ���� ���ؼ� �� ��ü�� �־�� �ϴ���.
				bool await_ready();
				void await_suspend(std::coroutine_handle<> handle);
				void await_resume() const noexcept;

			};

			class CheckBool // Awaitable��ü
			{
			public:
				CheckBool(bool& param);						// �ڷ�ƾ�� co_await�� ���� ���ؼ� �� ��ü�� �־�� �ϴ���.
				bool await_ready();
				void await_suspend(std::coroutine_handle<> handle);
				void await_resume() const noexcept;
				bool& flag;
			};

			class ReturnNull // Awaitable��ü
			{
			public:
				ReturnNull();						// �ڷ�ƾ�� co_await�� ���� ���ؼ� �� ��ü�� �־�� �ϴ���.
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

		static void StartCoroutine(std::function<Coroutine_type()>* coro);	// �ڷ�ƾ �Լ��� �޾Ƽ� ���۽�Ŵ
		static void UpdateCoroutines();	// �ڷ�ƾ �Լ����� ������Ʈ ������

	private:
		static std::vector<Coroutine_type*> _coroutines;	// �ڷ�ƾ���� ��Ƶδ� ��
		static std::map<int, std::function<Coroutine_type()>*> _AddedCoroutines; // �ڷ�ƾ�� �ּҸ� �����ϴ� ����
		static int idexKey;
	};
}