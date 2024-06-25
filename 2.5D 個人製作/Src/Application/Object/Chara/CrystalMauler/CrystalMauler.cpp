﻿#include "CrystalMauler.h"
#include"../../../Scene/SceneManager.h"

void CrystalMauler::Update()
{
	// 召喚中はリターン
	if (d != 0)return;

	// アニメーション更新
	int animeCnt;
	m_animationInfo.count += m_animationInfo.speed;
	animeCnt = m_animationInfo.start + m_animationInfo.count;

	// 最後のコマまで行進し終えたらループさせる
	if (animeCnt > m_animationInfo.end)
	{
		if (m_oldSit & NowCharaSit::Down)
		{
			m_isExpired = true;
			return;
		}
		animeCnt = m_animationInfo.start;
		m_animationInfo.count = 0.0f;
		m_nowSit = NowCharaSit::Idle;
	}
	// UV(切り取るコマ)を設定
	m_polygon.SetUVRect(animeCnt);

	// 攻撃
	if (animeCnt == 19 && m_animationInfo.oldCount != 19)
	{
		KdAudioManager::Instance().Play("Asset/Sound/atack.wav", false)->SetVolume(0.3);
		// 球判定用の変数を設定
		KdCollider::SphereInfo sphere;
		// 球の中心位置を設定(座標)
		sphere.m_sphere.Center = m_pos + Math::Vector3(0.7, 0.7, 0);
		// 球の半径を設定
		sphere.m_sphere.Radius = 0.3f;
		// 当たり判定したいタイプを設定
		sphere.m_type = KdCollider::TypeGround | KdCollider::TypeDamage;

		// デバッグ用
		m_pDebugWire->AddDebugSphere(sphere.m_sphere.Center, sphere.m_sphere.Radius);

		// 球に当たったオブジェクト情報を格納
		std::list<KdCollider::CollisionResult> retSphereList;

		// 当たり判定
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphere, &retSphereList))
			{
				// 当たったのが敵かどうか
				if (obj->GetObjectType() == KdGameObject::ObjectType::Enemy)
				{
					// 敵確定
					obj->OnHit(20.f);
				}
			}
		}
	}

	//if (GetAsyncKeyState(VK_SPACE))OnHit(50.f);


	m_animationInfo.oldCount = animeCnt;
	// キャラの状態が切り替わっていたら変数リセット
	if (m_nowSit != m_oldSit)
	{
		ChangeAnimation();
	}
	m_oldSit = m_nowSit;
	if (m_nowSit & NowCharaSit::Idle)m_nowSit = NowCharaSit::Move;

	if (m_nowSit & NowCharaSit::Idle || m_nowSit & NowCharaSit::Move)
	{
		// 攻撃範囲内の敵を見つける
		// ===============================
		//    球判定
		// ===============================

		// 球判定用の変数を設定
		KdCollider::SphereInfo sphere;
		// 球の中心位置を設定(座標)
		sphere.m_sphere.Center = m_pos + Math::Vector3(0, 0.5, 0);
		// 球の半径を設定
		sphere.m_sphere.Radius = 0.4f;
		// 当たり判定したいタイプを設定
		sphere.m_type = KdCollider::TypeDamage;


		// 球に当たったオブジェクト情報を格納
		std::list<KdCollider::CollisionResult> retSphereList;

		// 当たり判定
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphere, &retSphereList))
			{
				// 当たったのが敵かどうか
				if (obj->GetObjectType() == KdGameObject::ObjectType::Enemy)
				{
					m_nowSit = NowCharaSit::Idle;
					Atack();
				}


			}

		}
	}
	if (cFlg == true)m_nowSit = NowCharaSit::Idle;

	if (m_nowSit & NowCharaSit::Move)
	{
		m_pos.x += 0.015;
	}
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = transMat;
	BaseChara::Update();

	if (GetAsyncKeyState('W') & 0x8000)
	{
		Atack();
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		cFlg = true;
	}
}


void CrystalMauler::Init()
{
	BaseChara::Init();
	// 板ポリにテクスチャ(マテリアル)をロード
	m_polygon.SetMaterial("Asset/Textures/Chara/CrystalMauler.png");

	//　板ポリの基準点 (真ん中下段を指定)
	m_polygon.SetPivot(KdSquarePolygon::PivotType::Center_Bottom);

	m_hp = 250;
	m_polygon.SetSplit(8, 8);
	m_polygon.SetUVRect(0);
	m_polygon.SetScale(3.0);
	m_nowSit = NowCharaSit::Idle;
	ChangeAnimation();

}

void CrystalMauler::OnHit(float _dmg)
{
	m_hp -= _dmg;
	if (m_hp <= 0)
	{
		m_nowSit = NowCharaSit::Down;
	}
}

void CrystalMauler::Atack()
{
	m_MP++;
	if (m_atkCoolTime >= 210)
	{
		m_nowSit = NowCharaSit::Skill;
		if (m_MP > 300)
		{
			//m_nowSit = NowCharaSit::Skill2;
			m_MP = 0;
		}
		m_atkCoolTime = 0;

	}
}

void CrystalMauler::ChangeAnimation()
{
	if (m_nowSit & NowCharaSit::Idle)
	{
		m_animationInfo.start = 0;		// 開始コマ
		m_animationInfo.end = 7;		// 終了コマ	
	}
	if (m_nowSit & NowCharaSit::Move)
	{
		m_animationInfo.start = 8;		// 開始コマ
		m_animationInfo.end = 15;		// 終了コマ
	}
	if (m_nowSit & NowCharaSit::Skill)
	{
		m_animationInfo.start = 16;		// 開始コマ
		m_animationInfo.end = 22;		// 終了コマ
	}
	if (m_nowSit & NowCharaSit::Skill2)
	{
		m_animationInfo.start = 23;		// 開始コマ
		m_animationInfo.end = 37;		// 終了コマ
	}
	if (m_nowSit & NowCharaSit::Hit)
	{
		m_animationInfo.start = 38;		// 開始コマ
		m_animationInfo.end = 43;		// 終了コマ
	}
	if (m_nowSit & NowCharaSit::Down)
	{
		m_animationInfo.start = 44;		// 開始コマ
		m_animationInfo.end = 58;		// 終了コマ
	}
	m_animationInfo.count = 0.0f;	//	現在のコマ数カウント
	m_animationInfo.speed = 0.2f;	// アニメーションの速度
}
