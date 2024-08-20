#pragma once

///
/// 예전에 썼던 코드 필요할 때 대비해서 냅둠
/// 필요없다고 확신할 수 있을 경우 삭제
///

//ResetIcePos
// 마우스 3D좌표로에 위치시키도록
//DirectX::XMFLOAT3 newPos = GRAPHICS->ScreenToWorldPoint(InputSystem::GetInstance()->GetEditorMousePos().x, InputSystem::GetInstance()->GetEditorMousePos().y);
//_ice->GetComponent<Transform>()->SetPosition
//(
//	newPos.x,
//	this->_playerComp->GetOwner()->GetComponent<Transform>()->GetPosition().y,
//	newPos.z
//);
//
////플레이어 방향 돌리기
//DirectX::XMFLOAT3 playerPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
//DirectX::XMFLOAT3 playerRot = this->GetOwner()->GetComponent<Transform>()->GetRotation();
//DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&playerPos);
//DirectX::XMVECTOR rotPosVec = DirectX::XMLoadFloat3(&newPos);
//
//DirectX::XMVECTOR playerForward = DirectX::XMVectorSet(0.0f, playerRot.y, -1.0f, 0.0f);
//DirectX::XMVECTOR rotDirection = DirectX::XMVectorSubtract(rotPosVec, playerPosVec);
//rotDirection.m128_f32[1] = 0.0f;
//rotDirection = DirectX::XMVector3Normalize(rotDirection);
//
//DirectX::XMVECTOR dotResult = DirectX::XMVector3Dot(playerForward, rotDirection);
//float dotProduct = DirectX::XMVectorGetX(dotResult);
//
//float angle = acos(dotProduct);
//angle = DirectX::XMConvertToDegrees(angle);
//
//if (rotPosVec.m128_f32[0] > playerPosVec.m128_f32[0])
//{
//	angle *= -1;
//}
//
//this->GetOwner()->GetComponent<Transform>()->SetRotation(0.0f, angle, 0.0f);
//
//// 위치 조정
//_ice->GetComponent<BoxCollider>()->Update();
//_ice->GetComponent<MeshRenderer>()->Update();

// void KunrealEngine::PlayerAbility::CreateAbility2()
// {
// 	Ability* ice = new Ability();
// 	ice->Initialize("Ice");
// 
// 	ice->SetTotalData(
// 		"Ice",			// 이름
// 		20.0f,			// 데미지
// 		15.0f,			// 마나
// 		10.0f,			// 무력화 피해량
// 		5.0f,			// 쿨타임
// 		12.0f			// 사거리
// 	);
// 
// 	_ice = ice->_projectile;
// 
// 	// 크기 조정
// 	_ice->GetComponent<Transform>()->SetScale(30.0f, 30.0f, 30.0f);
// 	_ice->GetComponent<Transform>()->SetRotation(90.0f, 0.0f, 0.0f);
// 
// 	_iceParticle1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("IceParticle1");
// 	_iceParticle1->_autoAwake = true;
// 	_iceParticle1->AddComponent<Particle>();
// 	_iceParticle1->GetComponent<Particle>()->SetParticleEffect("Blast3", "Resources/Textures/Particles/fx_Blast3.dds", 1000);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleDuration(3.0f, 2.0f);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleVelocity(8.0f, true);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleSize(5.f, 5.0f);
// 	_iceParticle1->GetComponent<Particle>()->AddParticleColor(0.5f, 3.0f, 5.0f);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleDirection(0.0f, -1.0f, 0.0f);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleAngle(339.0f, 0.0f, 30.0f);
// 	_iceParticle1->GetComponent<Particle>()->SetParticleCameraApply(true);
// 	_iceParticle1->GetComponent<Particle>()->SetActive(false);
// 
// 	_iceParticle2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("IceParticle2");
// 	_iceParticle2->_autoAwake = true;
// 	_iceParticle2->AddComponent<Particle>();
// 	_iceParticle2->GetComponent<Particle>()->SetParticleEffect("Dust3", "Resources/Textures/Particles/fx_Dust3.dds", 1000);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleDuration(3.0f, 2.0f);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleVelocity(8.0f, true);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleSize(5.0f, 5.0f);
// 	_iceParticle2->GetComponent<Particle>()->AddParticleColor(0.5f, 3.0f, 5.0f);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleAngle(339.0f, 0.0f, 30.0f);
// 	_iceParticle2->GetComponent<Particle>()->SetParticleCameraApply(true);
// 	_iceParticle2->GetComponent<Particle>()->SetActive(false);
// 
// 	_iceParticle3 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("IceParticle3");
// 	_iceParticle3->_autoAwake = true;
// 	_iceParticle3->AddComponent<Particle>();
// 	_iceParticle3->GetComponent<Particle>()->SetParticleEffect("Sparks1", "Resources/Textures/Particles/fx_Sparks1.dds", 1000);
// 	_iceParticle3->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.1f);
// 	_iceParticle3->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
// 	_iceParticle3->GetComponent<Particle>()->SetParticleSize(30.f, 30.0f);
// 	_iceParticle3->GetComponent<Particle>()->AddParticleColor(0.0f, 1.0f, 2.0f);
// 	_iceParticle3->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
// 	_iceParticle3->GetComponent<Particle>()->SetParticleCameraApply(true);
// 	_iceParticle3->GetComponent<Particle>()->SetActive(false);
// 
// 	_iceParticleHit1 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("IceParticleHit1");
// 	_iceParticleHit1->_autoAwake = true;
// 	_iceParticleHit1->AddComponent<Particle>();
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleEffect("Dust3", "Resources/Textures/Particles/fx_Dust3.dds", 100);
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleDuration(4.0f, 0.5f);
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleVelocity(10.0f, true);
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleSize(25.0f, 25.0f);
// 	_iceParticleHit1->GetComponent<Particle>()->AddParticleColor(0.5f, 3.0f, 5.0f);
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
// 	_iceParticleHit1->GetComponent<Particle>()->SetParticleAngle(0.0f, 0.0f, 0.0f);
// 	_iceParticleHit1->GetComponent<Particle>()->SetActive(false);
// 
// 	_iceParticleHit2 = SceneManager::GetInstance().GetCurrentScene()->CreateObject("IceParticleHit2");
// 	_iceParticleHit2->_autoAwake = true;
// 	_iceParticleHit2->AddComponent<Particle>();
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleEffect("Sparks1", "Resources/Textures/Particles/fx_Sparks1.dds", 100);
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleDuration(1.0f, 0.1f);
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleVelocity(1.0f, true);
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleSize(30.f, 30.0f);
// 	_iceParticleHit2->GetComponent<Particle>()->AddParticleColor(0.0f, 1.0f, 2.0f);
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleDirection(0.0f, 0.0f, 0.0f);
// 	_iceParticleHit2->GetComponent<Particle>()->SetParticleCameraApply(true);
// 	_iceParticleHit2->GetComponent<Particle>()->SetActive(false);
// 
// 	// 투사체 컴포넌트 추가
// 	_ice->AddComponent<Projectile>();
// 	Projectile* iceProj = _ice->GetComponent<Projectile>();
// 
// 	iceProj->SetMeshObject("Ice/Ice", "Ice/Ice.png");
// 	iceProj->GetCollider()->SetColliderScale(15.0f, 15.0f, 15.0f);
// 	iceProj->SetDestoryCondition([iceProj, this]()->bool
// 		{
// 			if (this->_destroyIce)
// 			{
// 				return true;
// 			}
// 			else
// 			{
// 				return false;
// 			}
// 		});
// 
// 	_ice->SetActive(false);
// 
// 	ice->SetLogic([ice, iceProj, this]()
// 		{
// 			_iceParticle1->GetComponent<Transform>()->SetPosition(_ice->GetComponent<Transform>()->GetPosition());
// 			_iceParticle2->GetComponent<Transform>()->SetPosition(_ice->GetComponent<Transform>()->GetPosition());
// 			_iceParticle3->GetComponent<Transform>()->SetPosition(_ice->GetComponent<Transform>()->GetPosition());
// 
// 			if (iceProj->GetCollider()->GetTargetObject() != nullptr && iceProj->GetCollider()->GetTargetObject()->GetTag() == "Boss" && _isIceHit)
// 			{
// 				EventManager::GetInstance().CalculateDamageToBoss(ice);
// 				_isIceHit = false;
// 			}
// 		});
// 
// 	AddToContanier(ice);
// }

// if (InputSystem::GetInstance()->KeyDown(KEY::W) && this->_isIceReady)
// {
// 	ResetIcePos();											// 투사체 위치 리셋
// 	_isIceDetected = true;
// 	_isIceHit = true;
// 	Startcoroutine(iceCoolDown);
// 	Startcoroutine(iceStandby);								// 얼음 출현 대기	
// 
// 	_iceParticle1->GetComponent<Particle>()->SetActive(true);
// 	_iceParticle2->GetComponent<Particle>()->SetActive(true);
// 	_iceParticle3->GetComponent<Particle>()->SetActive(true);
// 	_iceParticleHit1->GetComponent<Transform>()->SetPosition(_ice->GetComponent<Transform>()->GetPosition());
// 	_iceParticleHit2->GetComponent<Transform>()->SetPosition(_ice->GetComponent<Transform>()->GetPosition());
// 
// 	_iceParticleHit1->GetComponent<Particle>()->Update();
// 	_iceParticleHit2->GetComponent<Particle>()->Update();
// 	_iceParticle1->GetComponent<Particle>()->Update();
// 	_iceParticle2->GetComponent<Particle>()->Update();
// 	_iceParticle3->GetComponent<Particle>()->Update();
// 
// 	_playerComp->_playerStatus = Player::Status::ABILITY;
// 	_playerComp->_abilityAnimationIndex = 2;				// 얼음 소환 애니메이션	
// 
// 	_CoroutineIs(iceDestroy)
// 	{
// 		auto* ability = this;
// 		Waitforsecond(3.0f);
// 		_iceParticle1->GetComponent<Particle>()->SetActive(false);
// 		_iceParticle2->GetComponent<Particle>()->SetActive(false);
// 		_iceParticle3->GetComponent<Particle>()->SetActive(false);
// 		_iceParticleHit1->GetComponent<Particle>()->SetActive(true);
// 		_iceParticleHit2->GetComponent<Particle>()->SetActive(true);
// 		_iceParticleHit1->SetActive(true);
// 		_iceParticleHit2->SetActive(true);
// 		ability->_destroyIce = true;
// 		ability->_isIceReady = false;
// 
// 		float delta = 0.0f;
// 		while (delta < 0.5f)
// 		{
// 			_iceParticleHit1->GetComponent<Particle>()->SetActive(false);
// 			_iceParticleHit2->GetComponent<Particle>()->SetActive(false);
// 			_iceParticleHit1->SetActive(false);
// 			_iceParticleHit2->SetActive(false);
// 			EventManager::GetInstance().CamShake(1.0f);
// 			delta += TimeManager::GetInstance().GetDeltaTime();
// 			Return_null;
// 		}
// 	};
// 	Startcoroutine(iceDestroy);
// }