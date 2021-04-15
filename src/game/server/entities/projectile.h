/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PROJECTILE_H
#define GAME_SERVER_ENTITIES_PROJECTILE_H

class CProjectile : public CEntity
{
public:
	CProjectile
	(
		CGameWorld *pGameWorld,
		int Type,
		int Owner,
		vec2 Pos,
		vec2 Dir,
		int Span,
		bool Freeeze,
		bool Explosive,
		float Force,
		int SoundImpact,
		int Weapon,
		int Layer = 0,
		int Number = 0
	);

	vec2 GetPos(float Time);
	void FillInfo(CNetObj_Projectile *pProj);

	virtual void Reset();
	virtual void Tick();
	virtual void TickPaused();
	virtual void Snap(int SnappingClient);

	unsigned short m_CollisionsByX;
	unsigned short m_CollisionByY;

private:
	vec2 m_Direction;
	int m_LifeSpan;
	int m_Owner;
	int m_Type;
	//int m_Damage;
	int m_SoundImpact;
	int m_Weapon;
	float m_Force;
	int m_StartTick;
	int m_CreationTick; // real starttick
	bool m_Explosive;

	// DDRace

	int m_Bouncing;
	bool m_Freeze;
	int m_TuneZone;
	
	// headbot
	int m_FootMode;
	int m_FootPickupDistance; //<-foot grenade diff.
	int m_FootBounceLoss;
	bool m_FillExtraInfo;

public:

	void AddDirection(vec2 dir) {m_Direction += dir;}
	void SetBouncing(int Value);
	int GetBouncing() const {return m_Bouncing;}
	void SetFootMode(int value) {m_FootMode = value;}
	int GetFootMode() const {return m_FootMode;}
	void SetBounceLoss(int value) {m_FootBounceLoss = value;}
	int GetBounceLoss() const {return m_FootBounceLoss;}
	void SetFillExtraInfo(bool value) {m_FillExtraInfo = value;}
	bool GetFillExtraInfo() const {return m_FillExtraInfo;}
	void FillExtraInfo(CNetObj_Projectile *pProj);
};

#endif
