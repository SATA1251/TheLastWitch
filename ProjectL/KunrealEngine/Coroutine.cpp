#include "Coroutine.h"
#include "TimeManager.h"

#include "GraphicsSystem.h"
#include <DirectXMath.h>

namespace KunrealEngine
{
	std::vector<KunrealEngine::Coroutine::Coroutine_type*> KunrealEngine::Coroutine::_coroutines;
	std::map<int, std::function<KunrealEngine::Coroutine::Coroutine_type()>*> KunrealEngine::Coroutine::_AddedCoroutines; // 코루틴의 주소를 저장하는 집합
	int KunrealEngine::Coroutine::idexKey;

	Coroutine::Coroutine()
	{

	};

	Coroutine::~Coroutine()
	{

	};

	/////////////////////////////////////////
	/////////////DurationManager/////////////
	/////////////////////////////////////////

	Coroutine::DurationManager& Coroutine::DurationManager::getInstance()
	{
		static DurationManager instance;
		return instance;
	}

	float Coroutine::DurationManager::getDuration() const
	{
		return duration;
	}

	void Coroutine::DurationManager::setDuration(float newDuration)
	{
		duration = newDuration;
	}

	void Coroutine::DurationManager::durationtonull()
	{
		duration = 0;
	}

	/////////////////////////////////
	/////////////Coroutine///////////
	/////////////////////////////////

	Coroutine::Coroutine_type::Coroutine_type()
	{

	}

	Coroutine::Coroutine_type::Coroutine_type(std::coroutine_handle<promise_type> handle) : coro_handle(handle)
	{

	}

	Coroutine::Coroutine_type::~Coroutine_type()
	{
		coro_handle.destroy();
	}

	////////////////////////////////////
	/////////////promise_type///////////
	////////////////////////////////////

	Coroutine::Coroutine_type Coroutine::Coroutine_type::promise_type::get_return_object()
	{
		return Coroutine_type(std::coroutine_handle<promise_type>::from_promise(*this));
	}

	std::suspend_never Coroutine::Coroutine_type::promise_type::initial_suspend() noexcept
	{
		return {};
	}

	std::suspend_always Coroutine::Coroutine_type::promise_type::final_suspend() noexcept
	{
		return {};
	}

	void Coroutine::Coroutine_type::promise_type::return_void() noexcept
	{

	}

	void Coroutine::Coroutine_type::promise_type::unhandled_exception() noexcept
	{

	}

	bool Coroutine::Coroutine_type::promise_type::await_ready() noexcept
	{
		timer += TimeManager::GetInstance().GetDeltaTime();

		if (timer > duration)
		{
			timer = 0;
			duration = 0;
			return true;
		}

		else
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////
	/////////////WaitForSeconds///////////
	//////////////////////////////////////

	// WaitForSeconds 구현
	Coroutine::Coroutine_type::WaitForSeconds::WaitForSeconds(float seconds)
	{
		DurationManager::getInstance().setDuration(seconds);
	}

	bool Coroutine::Coroutine_type::WaitForSeconds::await_ready()
	{
		return false;
	}

	void Coroutine::Coroutine_type::WaitForSeconds::await_suspend(std::coroutine_handle<> handle)
	{

	}

	void Coroutine::Coroutine_type::WaitForSeconds::await_resume() const noexcept {}

	////////////////////////////////////////
	/////////////////CheckBool//////////////
	////////////////////////////////////////

	Coroutine::Coroutine_type::CheckBool::CheckBool(bool& param)
		:flag(param)
	{

	}
	bool Coroutine::Coroutine_type::CheckBool::await_ready()
	{
		return false;
	}
	void Coroutine::Coroutine_type::CheckBool::await_suspend(std::coroutine_handle<> handle)
	{
		if (flag)
		{
			handle.resume();
		}
	}

	void Coroutine::Coroutine_type::CheckBool::await_resume() const noexcept
	{

	}

	////////////////////////////////////////
	/////////////////ReturnNull//////////////	
	////////////////////////////////////////

	Coroutine::Coroutine_type::ReturnNull::ReturnNull()
	{

	}

	bool Coroutine::Coroutine_type::ReturnNull::await_ready()
	{
		return false;
	}

	void Coroutine::Coroutine_type::ReturnNull::await_suspend(std::coroutine_handle<> handle)
	{

	}

	void Coroutine::Coroutine_type::ReturnNull::await_resume() const noexcept
	{
	}

	//////////////////////////////////////
	/////////////StartCoroutine///////////
	//////////////////////////////////////

	void Coroutine::StartCoroutine(std::function<Coroutine_type()>* coro)
	{
		// 이미 추가된 코루틴인지 확인
		for (auto& coron : _AddedCoroutines) // 코루틴들의 배열을 순회
		{
			if (coron.second == coro) // 배열속코루틴의 함수포인터와 coro의 함수 포인터를 비교
			{

				return;
			}
		}

		Coroutine_type* coroutineInstance = new Coroutine_type((*coro)());
		_coroutines.emplace_back(coroutineInstance);
		idexKey++;
		coroutineInstance->mapKey = idexKey;
		coroutineInstance->func_ptr = coro;
		coroutineInstance->coro_handle.promise().duration = DurationManager::getInstance().getDuration();
		DurationManager::getInstance().durationtonull();

		_AddedCoroutines.insert(std::make_pair(idexKey, coro));
	}

	////////////////////////////////////////
	/////////////UpdateCoroutines///////////
	////////////////////////////////////////

	void Coroutine::UpdateCoroutines()
	{
		//GRAPHICS->DrawUIText(100, 100, 20, DirectX::XMFLOAT4(255.0f, 255.0f, 255.0f, 255.0f), "Corotine: %d", _coroutines.size());
		bool ismember = true;

		std::vector<Coroutine_type*> deletethings;

		for (auto coroutine : _coroutines)
		{
			bool isready = coroutine->coro_handle.promise().await_ready();
			bool isdone = coroutine->coro_handle.done();

			if (!isdone && isready)
			{
				// 코루틴이 완료되지 않고, 지정된 시간이 경과하지 않은 경우 resume하지 않음
				coroutine->coro_handle.resume();

				for (auto& coron : _AddedCoroutines) // 코루틴들의 배열을 순회
				{
					if (coroutine->mapKey == coron.first) //  찾아줘요 coro_handle
					{
						if (coroutine->coro_handle.promise().duration == 0 && DurationManager::getInstance().getDuration() != 0)
						{
							coroutine->coro_handle.promise().duration = DurationManager::getInstance().getDuration();
						}
					}
				}
			}

			else if (isdone)
			{
				deletethings.push_back(coroutine);
			}
		}

		for (auto coroutine : deletethings)
		{
			if (_AddedCoroutines.find(coroutine->mapKey)->second != coroutine->func_ptr)
			{
				return;
			}

			auto iter = std::find(_coroutines.begin(), _coroutines.end(), coroutine);
			_AddedCoroutines.erase(coroutine->mapKey);
			delete coroutine;
			coroutine = nullptr;
			_coroutines.erase(iter); // 벡터에서 제거
		}
	}
}