#include "Coroutine.h"
#include "TimeManager.h"

namespace KunrealEngine
{
	std::vector<int> KunrealEngine::Coroutine::keybox;
	std::map<int, Coroutine*> KunrealEngine::Coroutine::_AddedCoroutines; // 코루틴의 주소를 저장하는 집합
	int KunrealEngine::Coroutine::idexKey;


	/////////////////////////////////
	/////////////Coroutine///////////
	/////////////////////////////////


	Coroutine::Coroutine(std::coroutine_handle<promise_type> handle) : coro_handle(handle)
	{

	};

	Coroutine::~Coroutine()
	{
		coro_handle.destroy();
	};

	////////////////////////////////////
	/////////////promise_type///////////
	////////////////////////////////////

	Coroutine Coroutine::promise_type::get_return_object()
	{
		return Coroutine(std::coroutine_handle<promise_type>::from_promise(*this));
	}

	std::suspend_never Coroutine::promise_type::initial_suspend() noexcept
	{
		return {};
	}

	std::suspend_always Coroutine::promise_type::final_suspend() noexcept
	{
		return {};
	}

	void Coroutine::promise_type::return_void() noexcept
	{

	}

	void Coroutine::promise_type::unhandled_exception() noexcept
	{

	}

	bool Coroutine::promise_type::await_ready() noexcept
	{
		return false;
	}

	//////////////////////////////////////
	/////////////WaitForSeconds///////////
	//////////////////////////////////////

	// WaitForSeconds 구현
	WaitForSeconds::WaitForSeconds(float seconds)
		: time(seconds)
	{

	}

	bool WaitForSeconds::await_ready()
	{
		return false;
	}

	void WaitForSeconds::await_suspend(std::coroutine_handle<> handle)
	{
		timer += TimeManager::GetInstance().GetDeltaTime();

		if (timer > time)
		{
			timer = 0;
			await_resume();
		}

		else
		{
			return;
		}
	}

	void WaitForSeconds::await_resume() const noexcept {}

	////////////////////////////////////////
	/////////////////CheckBool//////////////
	////////////////////////////////////////

	CheckBool::CheckBool(bool& param)
		:flag(param)
	{

	}
	bool CheckBool::await_ready()
	{
		return false;
	}
	void CheckBool::await_suspend(std::coroutine_handle<> handle)
	{
		if (flag)
		{
			handle.resume();
		}
	}

	void CheckBool::await_resume() const noexcept
	{

	}

	////////////////////////////////////////
	/////////////////ReturnNull/////////////	
	////////////////////////////////////////

	ReturnNull::ReturnNull()
		:flag(false)
	{

	}

	bool ReturnNull::await_ready()
	{
		return false;	
	}

	void ReturnNull::await_suspend(std::coroutine_handle<> handle)
	{
		if (!flag)
		{
			flag = true;
		}

		else if (flag)
		{
			await_ready();
		}
	}

	void ReturnNull::await_resume() const noexcept
	{

	}

	//////////////////////////////////////
	/////////////StartCoroutine///////////
	//////////////////////////////////////

	void Coroutine::StartCoroutine(std::function<Coroutine()> coro)
	{
		//이미 추가된 코루틴인지 확인
		for (auto& coron : _AddedCoroutines)
		{
			for (auto key : keybox)
			{
				if (coron.first != 0 && coron.first == key)
				{
					return;
				}
			}
		}

		idexKey++;
		keybox.push_back(idexKey);
		Coroutine* coroutineInstance = new Coroutine(coro());
		coroutineInstance->corofunc = coro;

		_AddedCoroutines.insert(std::make_pair(idexKey, coroutineInstance));
	}

	////////////////////////////////////////
	/////////////UpdateCoroutines///////////
	////////////////////////////////////////

	void Coroutine::UpdateCoroutines()
	{
		std::vector<std::map<int, Coroutine*>::iterator> itervec;

		for (auto& coroutine : _AddedCoroutines)
		{
			std::coroutine_handle<promise_type> handle = coroutine.second->coro_handle;

			if (handle.done())
			{
				auto iter = std::find(_AddedCoroutines.begin(), _AddedCoroutines.end(), coroutine);
				delete coroutine.second;
				coroutine.second = nullptr;
				itervec.push_back(iter);
			}

			else if (handle)
			{
				handle.promise().await_ready();
			}
		}

		for (auto& iter : itervec)
		{
			_AddedCoroutines.erase(iter); // 맵에서 제거
		}
	}
}