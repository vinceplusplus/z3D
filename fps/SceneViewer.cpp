
#include "stdafx.h"

#include "../z3D/Core/functor_inl.h"
#include "../z3D/Core/functor_weak_inl.h"

#include "opcodes.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef min
#undef max

void init_symbols();

void stack_walk();


REAL ATTAIN(REAL x, REAL goal, REAL dt)
{
	if(x < goal)
	{
		if((x += dt) > goal)
			return goal;
		return x;
	}
	if(x > goal)
	{
		if((x -= dt) < goal)
			return goal;
		return x;
	}
	return goal;
}

typedef enum _LOWER_ANIM : uint32_t
{
	LOWER_ANIM_UNKNOWN = 0,
	LOWER_ANIM_IDLE,
	LOWER_ANIM_RUN,
	LOWER_ANIM_RUN_BACKWARD,
	LOWER_ANIM_WALK,
	LOWER_ANIM_WALK_BACKWARD,
	LOWER_ANIM_JUMP,
	LOWER_ANIM_JUMP_BACKWARD,
	LOWER_ANIM_LAND,
	LOWER_ANIM_DIE,
} LOWER_ANIM;

typedef enum _UPPER_ANIM : uint32_t
{
	UPPER_ANIM_UNKNOWN = 0,
	UPPER_ANIM_IDLE,
	UPPER_ANIM_SHOOT,
	UPPER_ANIM_CHANGE_WEAPON,
	UPPER_ANIM_DIE,
} UPPER_ANIM;

typedef enum _BULLET_TYPE: uint32_t
{
	BULLET_TYPE_ROCKET = 0,
	BULLET_TYPE_GRENADE,
	BULLET_TYPE_SLUG,
	BULLET_TYPE_CELL,
} BULLET_TYPE;

typedef enum _WEAPON_TYPE: uint32_t
{
	WEAPON_TYPE_ROCKET_LAUNCHER = 0,
	WEAPON_TYPE_GRENADE_LAUNCHER,
	WEAPON_TYPE_RAILGUN,
	WEAPON_TYPE_PLASMA_GUN,
} WEAPON_TYPE;

template<class Type, class Allocator = std::allocator<Type>>
class ref_list: public RefBase, public std::list<Type, Allocator>
{
public:
	ref_list() {}
protected:
	~ref_list() {}
};

template<class T, class FRIEND>
class LIST_TRACKING
{
private:
	WPtr<ref_list<WPtr<T>>>
						_list;
	typename ref_list<WPtr<T>>::iterator
						_iter;
private:
	~LIST_TRACKING()
	{
		detach();
	}
private:
	void				attach(const SPtr<ref_list<WPtr<T>>>& lst, const SPtr<T>& item)
	{
		_list = lst;
		_iter = lst->insert(lst->end(), item);
	}
	void				detach()
	{
		if(!_list.valid())
			return;
		_list.lock()->erase(_iter);
		_list.reset();
	}
public:
	friend typename T;
	friend typename FRIEND;
};

class Server;
class SrvCli;
class svGame;
class svPlayer;
class clGame;
class clPlayer;

//class SNAP
//{
//public:
//	Vec3					pos;
//	Vec3					vel;
//	uint32_t				touch_ground;
//
//	REAL					yaw;
//	REAL					pitch;
//
//	LOWER_ANIM				lower_anim;
//	UPPER_ANIM				upper_anim;
//
//	Vec3					tilt;
//public:
//	SNAP()
//	{
//		memset(this, 0, sizeof(*this));
//	}
//};
//

typedef enum _PLAYER_FLAGS: uint32_t
{
	PLAYER_FLAG_TELEPORTED = 1,
	PLAYER_FLAG_DEAD = 2,
	PLAYER_FLAG_SPAWNED = 4,
} PLAYER_FLAGS;

class SNAP2
{
public:
	uint32_t				flags;				// one or more VOLATILE_FLAGS

	Vec3					pos;
	Vec3					vel;
	uint32_t				touch_ground;

	REAL					yaw;
	REAL					pitch;

	Vec3					tilt;

	REAL					no_move_time;

	WEAPON_TYPE				weapon;
public:
	SNAP2()
	{
		reset();
	}
	void					reset()
	{
		memset(this, 0, sizeof(*this));
	}
	void					lerp(const SNAP2& other, REAL dt, SNAP2& output) const
	{
		if(flags & PLAYER_FLAG_TELEPORTED)
		{
			output = *this;
			return;
		}
		output.flags = flags;
		output.pos = pos.lerp(other.pos, dt);
		output.vel = vel.lerp(other.vel, dt);
		output.touch_ground = touch_ground;
		output.yaw = LERP_ANGLE(yaw, other.yaw, dt);
		output.pitch = LERP_ANGLE(pitch, other.pitch, dt);
		output.tilt = tilt.lerp(other.tilt, dt);
		output.no_move_time = no_move_time;
		output.weapon = weapon;
	}
};

class PLAYER_INPUT
{
public:
	REAL					yaw;
	REAL					pitch;

	union
	{
		uint32_t				keys;
		struct
		{
			bool					up		:	1;
			bool					down	:	1;
			bool					left	:	1;
			bool					right	:	1;
			bool					jump	:	1;
			bool					fire	:	1;
		};
	};

	WEAPON_TYPE				weapon;
public:
	PLAYER_INPUT()
	{
		memset(this, 0, sizeof(*this));
	}
public:
	friend bool operator == (const PLAYER_INPUT& a, const PLAYER_INPUT& b)		{	return !memcmp(&a, &b, sizeof(a));	}
	friend bool operator != (const PLAYER_INPUT& a, const PLAYER_INPUT& b)		{	return !(a == b);	}
};

template<class T>
class InputHistory
{
private:
	T						_inputs[512];
	size_t					_count;
	size_t					_latest_index;
	uint64_t				_latest_frame;
public:
	InputHistory()
	{
		clear();
	}
public:
	void					clear()
	{
		_count = 0;
		_latest_index = _countof(_inputs) - 1;
		_latest_frame = -1I64;
	}
private:
	void					getLatestInput(uint64_t& frame, T& input)
	{
		if(!_count)
			addInput(0, T());
		frame = _latest_frame;
		input = _inputs[_latest_index];
	}
public:
	void					addInput(uint64_t frame, const T& input)
	{
		if(_count && frame <= _latest_frame)
			return;
		if(!_count || frame == _latest_frame + 1)
		{
			_latest_index = (_latest_index + 1) % _countof(_inputs);
			_inputs[_latest_index] = input;
			_latest_frame = frame;
			_count = min(_count + 1, _countof(_inputs));
			return;
		}
		if(true)
		{
			T li = _inputs[_latest_index];
			size_t stuff = min((size_t)(frame - _latest_frame - 1), _countof(_inputs) - 1);
			for(size_t i = 0; i < stuff; ++i)
			{
				_latest_index = (_latest_index + 1) % _countof(_inputs);
				_inputs[_latest_index] = li;
				_count = min(_count + 1, _countof(_inputs));
			}
		}
		_latest_index = (_latest_index + 1) % _countof(_inputs);
		_latest_frame = frame;
		_inputs[_latest_index] = input;
		_count = min(_count + 1, _countof(_inputs));
	}
	const T&				getInput(uint64_t frame)
	{
		if(!_count || _latest_frame < frame)
		{
			uint64_t f;
			T input;
			getLatestInput(f, input);
			addInput(frame, input);
		}
		uint64_t offset = _latest_frame - frame;
		Z_ASSERT( offset <= (uint64_t)((uint32_t)-1I32) );
		if((size_t)offset >= _count)
			return _inputs[(_latest_index + _countof(_inputs) - (_count - 1)) % _countof(_inputs)];
		return _inputs[(_latest_index + _countof(_inputs) - (size_t)offset) % _countof(_inputs)];
	}
	bool					has(uint64_t frame)
	{
		if(!_count || _latest_frame < frame)
			return false;
		return frame >= (_latest_frame - _count + 1);
	}
};

class Snaps
{
private:
	struct _SNAP
	{
		uint64_t				f;
		SNAP2					s;
	}						_snaps[512];
	size_t					_latest_index;
	size_t					_count;
public:
	Snaps()					{	clear();	}
public:
	void					clear()
	{
		_latest_index = _countof(_snaps) - 1;
		_count = 0;
	}
public:
	void					add(uint64_t frame, const SNAP2& s)
	{
		Z_ASSERT( !_count || frame > _snaps[_latest_index].f );

		_latest_index = (_latest_index + 1) % _countof(_snaps);
		_SNAP s2 = {frame, s};
		_snaps[_latest_index] = s2;
		_count = min(_count + 1, _countof(_snaps));
	}
public:
	size_t					count() const
	{
		return _count;
	}
	const SNAP2&				getLatest() const
	{
		return _snaps[_latest_index].s;
	}
	bool					query(uint64_t f, const SNAP2*& s0, const SNAP2*& s1, REAL& dt) const
	{
		Z_ASSERT( _count );
		if(!_count)
			return false;
		if(f >= _snaps[_latest_index].f)
		{
			s0 = s1 = &_snaps[_latest_index].s;
			dt = 0.0f;
			return true;
		}
		for(size_t i = 1; i < _count; ++i)
		{
			const _SNAP* s = _snaps + ((_latest_index + _countof(_snaps) - i) % _countof(_snaps));
			if(f >= s->f)
			{
				const _SNAP* s2 = _snaps + ((_latest_index + _countof(_snaps) - (i - 1)) % _countof(_snaps));

				s0 = &s->s;
				s1 = &s2->s;
				dt = (REAL)(f - s->f) / (s2->f - s->f);
				return true;
			}
		}
		s0 = s1 = &_snaps[(_latest_index + _countof(_snaps) - (_count - 1)) % _countof(_snaps)].s;
		dt = 0.0f;
		return true;
	}
};

template<class T> class fast_list_item;
template<class T> class fast_list;

template<class T>
class fast_list_tracking
{
private:
	list<SPtr<T>>*		_list;
	typename list<SPtr<T>>::iterator
						_iter;
private:
	fast_list_tracking()
	{
		Z_ASSERT( !(_list = NULL) );						// only debug build will care about the value
	}
private:
	void				attach(list<SPtr<T>>* lst, const SPtr<T>& item)
	{
		Z_ASSERT( !_list );
		_list = lst;
		_iter = lst->insert(lst->end(), item);
	}
	void				detach()
	{
		Z_ASSERT( _list );
		_list->erase(_iter);
		Z_ASSERT( !(_list = NULL) );						// only debug build will care about the value
	}
public:
	friend typename fast_list_item<typename T>;
	friend typename fast_list<typename T>;
};

template<class T>
class fast_list_item: public RefBase
{
public:
	fast_list_tracking<T>	list_tracking;
private:
	uint32_t				_slot;								// the reason for length-crafted type is that it can directly be used to serialize/deserialize, and i don't think you need more than 2 ^ 32 - 1 items
public:
	fast_list_item()
	{
		_slot = -1I32;
	}
protected:
	~fast_list_item()
	{
	}
public:
	uint32_t				getSlot() const
	{
		return _slot;
	}
public:
	friend class fast_list_tracking<typename T>;
	friend class fast_list<typename T>;
};

template<class T>
class fast_list
{
public:
	typedef typename list<SPtr<typename T>>::iterator
										iterator;
	typedef typename list<SPtr<typename T>>::const_iterator
										const_iterator;
private:
	typedef struct _FREE_SLOT_LINK
	{
		uint32_t							p;									// not pointer, so porting to 64-bit won't eat more memory
		uint32_t							n;
	} FREE_SLOT_LINK;
private:
	vector<SPtr<T>>						_pool_items;
	list<SPtr<T>>						_items;
private:
	vector<FREE_SLOT_LINK>				_free_slot_links;
	uint32_t							_free_slot_using;						// head of link list of using
	uint32_t							_free_slot_free;						// head of link list of free
public:
	fast_list()
	{
	}
	~fast_list()
	{
		clear();
	}
public:
	bool								init(uint32_t pool_size)
	{
		Z_ASSERT( pool_size );					// check parameter validity
		Z_ASSERT( !_pool_items.size() || !_items.size() );			// check not initialized or no any item exists

		_pool_items.resize(pool_size);

		_free_slot_links.resize(pool_size);
		for(uint32_t i = 0; i < pool_size; ++i)
		{
			FREE_SLOT_LINK& l = _free_slot_links[i];
			l.p = i - 1;
			l.n = i + 1;
		}
		_free_slot_links[0].p = -1I32;
		_free_slot_links[pool_size - 1].n = -1I32;
		_free_slot_using = -1I32;
		_free_slot_free = 0I32;

		return true;
	}
public:
	bool							add(const SPtr<T>& item)
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		if(_free_slot_free == -1I32)
			return false;

		uint32_t slot = _free_slot_free;
		if(true)
		{
			FREE_SLOT_LINK& n = _free_slot_links[slot];
			_free_slot_free = n.n;
			if(n.n != -1I32)
				_free_slot_links[n.n].p = -1I32;
			n.p = -1I32;

			n.n = _free_slot_using;
			if(_free_slot_using != -1I32)
				_free_slot_links[_free_slot_using].p = slot;
			_free_slot_using = slot;
		}

		_pool_items[slot] = item;

		item->_slot = slot;
		item->list_tracking.attach(&_items, item);

		return true;
	}
	void							replace(uint32_t slot, const SPtr<T>& item)
	{
		if(_pool_items[slot])
			remove(slot);
		if(slot != _free_slot_free)
		{
			FREE_SLOT_LINK& n = _free_slot_links[slot];

			if(n.p != -1I32)							// should be redundant
				_free_slot_links[n.p].n = n.n;
			if(n.n != -1I32)
				_free_slot_links[n.n].p = n.p;

			n.p = -1I32;
			n.n = _free_slot_free;
			if(_free_slot_free != -1I32)
				_free_slot_links[_free_slot_free].p = slot;
			_free_slot_free = slot;
		}
		add(item);
	}
	void							remove(uint32_t slot)
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		SPtr<T>& item = _pool_items[slot];

		Z_ASSERT( item );

		item->list_tracking.detach();
		item.reset();

		if(true)
		{
			FREE_SLOT_LINK& n = _free_slot_links[slot];
			if(n.p != -1I32)
				_free_slot_links[n.p].n = n.n;
			else
				_free_slot_using = n.n;
			if(n.n != -1I32)
				_free_slot_links[n.n].p = n.p;

			n.p = -1I32;
			n.n = _free_slot_free;
			if(_free_slot_free != -1I32)
				_free_slot_links[_free_slot_free].p = slot;
			_free_slot_free = slot;
		}
	}
	const SPtr<T>&							at(uint32_t slot) const
	{
		Z_ASSERT( slot >= 0 && slot < _pool_items.size() );

		return _pool_items[slot];
	}
	void									clear()
	{
		while(_items.size())
			remove((*begin())->getSlot());
	}
	uint32_t								size() const
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return (uint32_t)_items.size();
	}
	uint32_t								capacity() const
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return (uint32_t)_pool_items.size();
	}
	typename fast_list<T>::iterator			begin()
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return _items.begin();
	}
	typename fast_list<T>::const_iterator	begin() const
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return _items.begin()
	}
	typename fast_list<T>::iterator			end()
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return _items.end();
	}
	typename fast_list<T>::const_iterator	end() const
	{
		Z_ASSERT( _pool_items.size() );			// check initialization

		return _items.end()
	}
};

class svProjectile: public fast_list_item<svProjectile>
{
public:
	uint32_t				owner_slot;
public:
	BULLET_TYPE			type;
public:
	Vec3					pos;
	Vec3					vel;
public:
	REAL					age;
public:
	//REAL					start_display;			// client
public:
	svProjectile()
	{
		owner_slot = -1I32;

		type = BULLET_TYPE_ROCKET;
		age = 0.0f;
	}
protected:
	~svProjectile()	{}
};

class clProjectile: public fast_list_item<clProjectile>
{
public:
	uint64_t					cur_frame;
public:
	uint32_t					owner_slot;
public:
	BULLET_TYPE					type;
public:
	Vec3						pos;
	Vec3						vel;
public:
	REAL						age;
public:
	REAL						start_display;
public:
	SPtr<SMeshNode>				mn;
	SPtr<cgPointLightFX::Item>	fake_light_item;
public:
	clProjectile()
	{
		cur_frame = -1I64;

		owner_slot = -1I32;

		type = BULLET_TYPE_ROCKET;
		age = 0.0f;

		start_display = 0.0f;
	}
protected:
	~clProjectile()
	{
		if(mn)
			mn->detachFromParent(false);
	}
public:
	void						loadGfx()
	{
		switch(type)
		{
		case BULLET_TYPE_ROCKET:
			{
				mn = SMeshNode::load(L"meshes/rocket/mesh_rocket.bin", L"meshes/rocket/mtl_rocket.xml");
				mn->loadTextures();
				mn->setVisible(false);

				//if(false)
				//{
				//	SPtr<SLightNode> l = SLightNode::create_point(Vec3(1.0f, 1.0f, 0.0f), 1.5f, 25, false);
				//	l->setPriority(-1);
				//	mn->addChild(l, false);
				//}
			}
			break;
		case BULLET_TYPE_GRENADE:
			{
				mn = SMeshNode::load(L"meshes/grenade/mesh_grenade.bin", L"meshes/grenade/mtl_grenade.xml");
				mn->loadTextures();
				mn->setVisible(false);
			}
			break;
		}
	}
};

class svNodeInfo: public RefBase
{
public:
	WPtr<svPlayer>			p;
protected:
	~svNodeInfo() {}
};

class PLAYER_STATUS
{
public:
	int32_t				hp;
public:
	PLAYER_STATUS(int32_t hp_ = 225)
	{
		reset_for_spawn(hp_);
	}
public:
	void					reset_for_spawn(int32_t hp_ = 225)
	{
		hp = hp_;
	}
};

class PLAYER_STATS
{
public:
	int32_t				score;
public:
	PLAYER_STATS()
	{
		score = 0;
	}
};

class svPlayer: public fast_list_item<svPlayer>
{
public:
	wcs						name;
public:
	SNAP2					snap;
	PLAYER_STATUS			status;
	PLAYER_STATS			stats;
private:
	WPtr<svGame>			_game;
	SPtr<SrvCli>			_srvcli;
public:
	PLAYER_INPUT			last_input;
	InputHistory<PLAYER_INPUT>
							input_history;
private:
	size_t					_pressed_jump;
private:
	SPtr<SMeshNode>			_bound;
private:
	uint64_t				_next_firable_frame;
private:
	size_t					_just_teleported;
	size_t					_just_spawned;
public:
	svPlayer(const SPtr<svGame>& game, const SPtr<SrvCli>& srvcli)
	{
		_game = game;
		_srvcli = srvcli;

		_pressed_jump = false;

		_next_firable_frame = -1I64;

		_just_teleported = false;
		_just_spawned = false;
	}
protected:
	~svPlayer()
	{
		if(_bound)
			_bound->detachFromParent(false);
	}
public:
	const SPtr<svGame>		getGame()
	{
		return _game.lock();
	}
public:
	void				loadBound()
	{
		_bound = SMeshNode::load(L"meshes/player_bound/mesh_player_bound.bin", L"meshes/player_bound/mtl_player_bound.xml");
		SPtr<svNodeInfo> info = new svNodeInfo();
		info->p = SPtr<svPlayer>(this);
		_bound->setUserData(info);
	}
public:
	friend class svGame;
};

class SPAWN_POINT
{
public:
	Vec3								pos;
	REAL								yaw;
};

class svGame: public RefBase
{
public:
	struct GAME_INFO
	{
		REAL								fps;
		uint32_t							snap_interval;			// IN FRAMES

		AABB								PLAYER_BB;
		REAL								PLAYER_MAX_RUN_SPEED;
		Vec3								GRAVITY;
	};
private:
	GAME_INFO							_game_info;
private:
	uint64_t							_frame;
private:
	FPSCtrl								_fps_ctrl;
private:
	SPtr<SNode>							_root_node;
private:
	wcs									_map_dir;
	SPtr<SStructNode>					_map_node;
private:
	vector<SPAWN_POINT>					_spawn_points;
private:
	fast_list<svPlayer>					_players;
private:
	fast_list<svProjectile>				_projectiles;
public:
	svGame()
	{
		_frame = 0;

		_game_info.fps = 60;
		_game_info.snap_interval = 2;

		_game_info.PLAYER_BB = AABB(Vec3(-3, 0, -3), Vec3(3, 17.5f, 3));
		_game_info.PLAYER_MAX_RUN_SPEED = 64.0f;
		_game_info.GRAVITY = Vec3(0, -250.0f, 0);

		_fps_ctrl.setFPS(_game_info.fps);

		_players.init(1024);

		_projectiles.init(1024);

		_root_node = new SNode();
	}
protected:
	~svGame()
	{
	}
public:
	bool					loadMap(const wcs& dir)
	{
		if(!dir.length())
			return false;

		_map_dir = dir;

		if(_map_node)
		{
			_map_node->detachFromParent(false);
			_map_node.reset();
		}

		_map_node = SStructNode::load(dir + L"/scene.xml");
		if(!_map_node)
		{
			syslog << "cannot load the struct file";
			return false;
		}
		_map_node->loadTMCtrl(dir + L"/scene_tm.xml");
		_map_node->loadSkinCtrl(dir + L"/scene_skin.xml");
		_map_node->loadUserText(dir + L"/scene_user.txt");

		_root_node->addChild(_map_node, false);

		for(size_t i = 0;; ++i)
		{
			wcs s = FORMATW(L"spawn_point_%02d", i);
			SPtr<SNode> n = _map_node->getLoadedNode(s);
			if(!n)
				break;

			SPAWN_POINT p;
			p.pos = n->world_pos();
			REAL pitch;
			(Vec3::zNegAxis * n->world_rot().asMat3()).resolveOriRH(p.yaw, pitch);

			_spawn_points.push_back(p);
		}

		if(!_spawn_points.size())
		{
			SPAWN_POINT p = {Vec3(0, 50, 0), 0};

			_spawn_points.push_back(p);
		}

		return true;
	}
public:
	void					update();
private:
	void					procPlayerInput(const SPtr<svPlayer>& p, const PLAYER_INPUT& cur, const PLAYER_INPUT& last, REAL dt);
private:
	void					updatePlayerMovement(const SPtr<svPlayer>& p, REAL dt);
private:
	void					updateProjectiles(REAL dt);
	void					addProjectile(const SPtr<svProjectile>& proj);
	void					removeProjectile(const SPtr<svProjectile>& proj);
	void					issueBulletHit(BULLET_TYPE bullet_type, const Vec3& hit_pos, const Vec3& face_normal);
private:
	void					handleHitScan(const SPtr<svPlayer>& p, BULLET_TYPE bullet_type);
private:
	void					dealDamage(const SPtr<svPlayer>& p, int32_t damage, const SPtr<svPlayer>& attacker);
	void					damageFeedback(const SPtr<svPlayer>& p, const Vec3& dmg_dir, int32_t dmg);
private:
	void					spawnPlayer(const SPtr<svPlayer>& p);
public:
	bool					addPlayer(const SPtr<SrvCli>& srvcli);
	void					removePlayer(uint32_t slot);
public:
	void					sendPlayerStatus(const SPtr<svPlayer>& p);
	void					sendPlayersStats(const SPtr<svPlayer>& p);
	void					sendPlayersStatsToAll();
public:
	const GAME_INFO&		getGameInfo() const				{return _game_info;}
public:
	const wcs&				getMapDir() const				{return _map_dir;}
};

class Server;

class SrvCli: public fast_list_item<SrvCli>
{
private:
	SPtr<NetConn>		_conn;
private:
	WPtr<Server>		_server;
private:
	wcs					_name;
public:
	class PLAYER_LINK
	{
	private:
		WPtr<svPlayer>	_p;
	public:
		friend class svGame;
		friend class SrvCli;
	}					player_link;
public:
	SrvCli(intptr_t s, const SPtr<Server>& server)
	{
		_conn = new NetConn();
		_conn->setSocket(s);

		_server = server;

		_conn->setNMPU(functor2<bool, NetConn*, NetMsg*>(this, &SrvCli::nmpu));
		_conn->setOnDieCallback(functor1<void, NetConn*>(this, &SrvCli::onDie));
	}
protected:
	~SrvCli()
	{
	}
protected:
	void				onDie(NetConn* conn)
	{
	}
	bool				nmpu(NetConn* conn, NetMsg* msg);
public:
	const SPtr<NetConn>&	getConn() const
	{
		return _conn;
	}
	const wcs&				getPlayerName() const
	{
		return _name;
	}
};

void					svGame::update()
{
	_fps_ctrl.update();
	if(!_fps_ctrl.nextFrame())
		return;

	REAL dt = (REAL)(1.0 / _game_info.fps);

	++_frame;

	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter;)
		{
			svPlayer* player = (*iter).get();

			++iter;

			if(player->_srvcli->getConn()->dead())
			{
				// remove the disconnected player
				removePlayer(player->getSlot());
			}
		}
	}
	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		{
			svPlayer* player = (*iter).get();

			if(!player->input_history.has(_frame))
				player->input_history.addInput(_frame, player->last_input);

			PLAYER_INPUT cur = player->input_history.getInput(_frame);
			PLAYER_INPUT last = player->input_history.getInput(_frame - 1);

			procPlayerInput(player, cur, last, dt);
			updatePlayerMovement(player, dt);
		}
	}

	updateProjectiles(dt);

	if(_frame % _game_info.snap_interval == 0)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		{
			svPlayer* player = (*iter).get();

			MsgWriter writer(NetMsg::alloc(), MY_OP::S_PLAYERS_SNAP2);
			writer.write(_frame);

			writer.write((uint32_t)_players.size());
			for(fast_list<svPlayer>::iterator iter2 = _players.begin(); iter2 != end_iter; ++iter2)
			{
				svPlayer* p = (*iter2).get();
				writer.write((uint32_t)(p->getSlot()));

				if(p->_just_teleported)
					p->snap.flags |= PLAYER_FLAG_TELEPORTED;
				if(p->_just_spawned)
					p->snap.flags |= PLAYER_FLAG_SPAWNED;

				writer.write(&(p->snap), sizeof(SNAP2));

				if(p->_just_teleported)
				{
					p->snap.flags &= ~PLAYER_FLAG_TELEPORTED;
					p->_just_teleported = false;
				}
				if(p->_just_spawned)
				{
					p->snap.flags &= ~PLAYER_FLAG_SPAWNED;
					p->_just_spawned = false;
				}
			}

			player->_srvcli->getConn()->send(writer.getMsg());
		}
	}
}

void					svGame::procPlayerInput(const SPtr<svPlayer>& p, const PLAYER_INPUT& cur, const PLAYER_INPUT& last, REAL dt)
{
	if(p->snap.flags & PLAYER_FLAG_DEAD)
	{
		if(p->last_input.fire && (p->_next_firable_frame != -1I64 && _frame >= p->_next_firable_frame))
		{
			spawnPlayer(p);
			return;
		}
		if(p->snap.touch_ground)
		{
			Vec3 v = p->snap.vel;
			v.y = 0;
			REAL spd = v.length();
			if(spd < 0.01f)
			{
				p->snap.vel.x = p->snap.vel.z = 0;
			}
			else
			{
				spd = ATTAIN(spd, 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10);
				v = v.normal_safe() * spd;

				p->snap.vel.x = v.x;
				p->snap.vel.z = v.z;
			}
		}
		return;
	}

	if(!p->_just_spawned)
	{
		p->snap.yaw = cur.yaw;
		p->snap.pitch = min(max(-89.5f, cur.pitch), 89.5f);
	}

	if(true)
	{
		Vec3 dir;
		if(cur.up)
			dir.z -= 1.0f;
		if(cur.down)
			dir.z += 1.0f;
		if(cur.left)
			dir.x -= 1.0f;
		if(cur.right)
			dir.x += 1.0f;

		p->snap.tilt.x = ATTAIN(p->snap.tilt.x, -dir.x, dt * 4);
		p->snap.tilt.z = ATTAIN(p->snap.tilt.z, -dir.z, dt * 4);

		if(dir.length() > 0.1f)
		{
			dir.normalize();
			dir *= Mat3::rotationY(p->snap.yaw);
			dir.normalize();

			if(p->snap.touch_ground)
			{
				REAL spd;
				Vec3 diralong = (spd = p->snap.vel * dir) * dir;
				Vec3 remain = p->snap.vel - diralong;
				remain.y = 0;

				spd = ATTAIN(spd, _game_info.PLAYER_MAX_RUN_SPEED, _game_info.PLAYER_MAX_RUN_SPEED * 0.8f * dt * 10);
				if(remain.length() < 0.01f)
				{
					remain = Vec3::zero;
				}
				else
				{
					remain = remain.normal() * (ATTAIN(remain.length(), 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10));
				}

				Vec3 v = dir * spd + remain;
				p->snap.vel.x = v.x;
				p->snap.vel.z = v.z;
			}
			else
			{
				Vec3 acc = dir * _game_info.PLAYER_MAX_RUN_SPEED * dt * 10 * 0.1f;

				Vec3 hvel = p->snap.vel;
				hvel.y = 0;

				if(hvel * dir < _game_info.PLAYER_MAX_RUN_SPEED)
					hvel += acc;

				p->snap.vel.x = hvel.x;
				p->snap.vel.z = hvel.z;
			}
		}
		else
		{
			if(p->snap.touch_ground)
			{
				Vec3 v = p->snap.vel;
				v.y = 0;
				REAL spd = v.length();
				if(spd < 0.01f)
				{
					p->snap.vel.x = p->snap.vel.z = 0;
				}
				else
				{
					spd = ATTAIN(spd, 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10);
					v = v.normal_safe() * spd;

					p->snap.vel.x = v.x;
					p->snap.vel.z = v.z;
				}
			}
		}
	}
	if(cur.jump && !last.jump)
		p->_pressed_jump = true;
	if(p->snap.touch_ground && p->_pressed_jump && cur.jump)
	{
		p->snap.vel.y = 90.0f;

		p->snap.touch_ground = 0;

		p->_pressed_jump = false;
	}
	if(p->snap.touch_ground)
	{
		if(cur.up || cur.down || cur.left || cur.right)
			p->snap.no_move_time = 0;
		else
			p->snap.no_move_time += dt;
	}
	if(true)	// not dead
	{
		if(p->_next_firable_frame == -1I64 || _frame >= p->_next_firable_frame)
		{
			if(p->snap.weapon != cur.weapon)
			{
				if(cur.weapon >= 0 && cur.weapon <= 2)
				{
					p->snap.weapon = cur.weapon;

					p->_next_firable_frame = _frame + (uint64_t)(0.6 / (1.0 / _game_info.fps));
				}
			}
		}
		if(cur.fire && (p->_next_firable_frame == -1I64 || _frame >= p->_next_firable_frame))
		{
			switch(p->snap.weapon)
			{
			case WEAPON_TYPE_ROCKET_LAUNCHER:
				{
					p->_next_firable_frame = _frame + (uint64_t)((2.0 / 3) / (1.0 / _game_info.fps));

					Vec3 dir = Vec3::zNegAxis * Mat3::rotationX(cur.pitch) * Mat3::rotationY(cur.yaw);

					SPtr<svProjectile> proj = new svProjectile();
					proj->owner_slot = p->getSlot();
					proj->type = BULLET_TYPE_ROCKET;
					proj->pos = p->snap.pos + Vec3(0, _game_info.PLAYER_BB.extent()[1] + 2.8f, 0) + dir * (3.0f - 0.01f);
					proj->vel = dir * 250;
					proj->age = 0.0f;

					addProjectile(proj);
				}
				break;
			case WEAPON_TYPE_GRENADE_LAUNCHER:
				{
					p->_next_firable_frame = _frame + (uint64_t)((2.0 / 3) / (1.0 / _game_info.fps));

					Vec3 dir = Vec3::zNegAxis * Mat3::rotationX(cur.pitch) * Mat3::rotationY(cur.yaw);

					SPtr<svProjectile> proj = new svProjectile();
					proj->owner_slot = p->getSlot();
					proj->type = BULLET_TYPE_GRENADE;
					proj->pos = p->snap.pos + Vec3(0, _game_info.PLAYER_BB.extent()[1] + 2.8f, 0) + dir * (3.0f - 0.01f);
					proj->vel = dir * 150;
					proj->age = 0.0f;

					addProjectile(proj);
				}
				break;
			case WEAPON_TYPE_RAILGUN:
				{
					p->_next_firable_frame = _frame + (uint64_t)(1.5 / (1.0 / _game_info.fps));

					handleHitScan(p, BULLET_TYPE_SLUG);
				}
				break;
			}
		}
	}
}

void					svGame::updatePlayerMovement(const SPtr<svPlayer>& p, REAL dt)
{
	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		{
			const SPtr<svPlayer>& p2 = (*iter);
			if(p2 == p)
				continue;
			if(p2->snap.flags & PLAYER_FLAG_DEAD)
				continue;

			_root_node->addChild(p2->_bound, false);
			p2->_bound->setWorldTransform(Mat4::translation(p2->snap.pos));
		}
	}

	Vec3 box_offset = Vec3(0, _game_info.PLAYER_BB.extent()[1], 0);

	Vec3& pos = p->snap.pos;
	Vec3& vel = p->snap.vel;
	uint32_t& touch_ground = p->snap.touch_ground;

	vel += _game_info.GRAVITY * dt;
	if(vel.length() * dt >= 0.01f)
	{
		SPtr<SNode> contact_node;

		Vec3 disp = vel * dt;

		if(touch_ground || true)
		{
			AABB box(_game_info.PLAYER_BB.minimum() + pos + disp, _game_info.PLAYER_BB.maximum() + pos + disp);

			REAL unpen_dist = 0.0f;
			bool tg = false;
			bool clipped;
			Vec3 collision_axis;
			if(FPSCore::clipGround(box, _root_node, 1.625f + max(0.0f, -disp.y), 30.0f, clipped, unpen_dist, tg, collision_axis, contact_node))
			{
				if(clipped)
				{
					pos = pos + disp + (unpen_dist + 0.001f) * Vec3::yAxis;
					if(tg)
						vel.y = max(0.0f, vel.y);
					else
						vel = vel.slideAlong(collision_axis);

					touch_ground = tg;
				}
				else
				{
					pos += disp;
					touch_ground = false;
				}
			}
			else
			{
				touch_ground = false;

				Vec3 new_box_center, remaining_disp, new_vel, collision_normal;
				bool tg;

				for(size_t i = 0; i < 7; ++i)
				{
					AABB box(_game_info.PLAYER_BB.minimum() + pos, _game_info.PLAYER_BB.maximum() + pos);

					if(FPSCore::clipAir(box, disp, vel, _root_node, 0.01f, 30.0f, new_box_center, remaining_disp, new_vel, tg, collision_normal, contact_node))
					{
						pos = new_box_center - Vec3(0, _game_info.PLAYER_BB.extent()[1], 0) + collision_normal * 0.001f;
						vel = new_vel;
						touch_ground |= (uint32_t)tg;
						disp = remaining_disp;
						if(disp.length() < 0.01f)
							break;
					}
					else
					{
						pos += disp;
						break;
					}
				}
			}
		}
		else
		{
			touch_ground = false;

			Vec3 new_box_center, remaining_disp, new_vel, collision_normal;
			bool tg;

			for(size_t i = 0; i < 7; ++i)
			{
				AABB box(_game_info.PLAYER_BB.minimum() + pos, _game_info.PLAYER_BB.maximum() + pos);

				if(FPSCore::clipAir(box, disp, vel, _root_node, 0.01f, 30.0f, new_box_center, remaining_disp, new_vel, tg, collision_normal, contact_node))
				{
					pos = new_box_center - Vec3(0, _game_info.PLAYER_BB.extent()[1], 0) + collision_normal * 0.001f;
					vel = new_vel;
					touch_ground |= (uint32_t)tg;
					disp = remaining_disp;
					if(disp.length() < 0.01f)
						break;
				}
				else
				{
					pos += disp;
					break;
				}
			}
		}
	}

	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			(*iter)->_bound->detachFromParent(false);
	}
}

void					svGame::updateProjectiles(REAL dt)
{
	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		{
			const SPtr<svPlayer>& p2 = (*iter);
			if(p2->snap.flags & PLAYER_FLAG_DEAD)
				continue;

			_root_node->addChild(p2->_bound, false);
			p2->_bound->setWorldTransform(Mat4::translation(p2->snap.pos));
		}
	}

	if(true)
	{
		fast_list<svProjectile>::iterator end_iter = _projectiles.end();
		for(fast_list<svProjectile>::iterator iter = _projectiles.begin(); iter != end_iter;)
		{
			const SPtr<svProjectile>& proj = (*iter);

			++iter;

			proj->age += dt;

			if(proj->age >= 10.0f)
			{
				removeProjectile(proj);
				continue;
			}

			switch(proj->type)
			{
			case BULLET_TYPE_ROCKET:
				{
					REAL t;
					SPtr<SMeshNode> mn;
					int32_t triidx;
					Vec3 dir = proj->vel.normal();

					const SPtr<svPlayer>& p = _players.at(proj->owner_slot);

					if(p)
						p->_bound->detachFromParent(false);

					bool ret = FPSCore::selectTriangle(Ray(proj->pos, dir), _root_node, t, mn, triidx);

					if(p)
					{
						_root_node->addChild(p->_bound, false);
						p->_bound->setWorldTransform(Mat4::translation(p->snap.pos));
					}

					if(ret)
					{
						Vec3 face_normal = mn->mesh()->getFaceNormal(triidx) * mn->world_rot().asMat3();
						if(t <= proj->vel.length() * dt)
						{
							Vec3 hit_pos = proj->pos + t * dir;

							SPtr<svPlayer> contact_player;
							if(mn->getUserData())
							{
								if(true)	// player info
									contact_player = mn->getUserData<svNodeInfo>()->p.lock();
							}

							if(true)
							{
								fast_list<svPlayer>::iterator end_iter = _players.end();
								for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
								{
									const SPtr<svPlayer>& p2 = (*iter);
									if(p2->snap.flags & PLAYER_FLAG_DEAD)
										continue;

									AABB box(_game_info.PLAYER_BB.minimum() + p2->snap.pos, _game_info.PLAYER_BB.maximum() + p2->snap.pos);

									REAL dist = Distance::PointAABB(hit_pos, box);
									if(dist > 20.0f)
										continue;

									dist = max(0.0f, dist - 1.0f);

									Vec3 closest;
									Closest::PointAABB(hit_pos, box, closest);

									REAL damage_rate = 1.0f;
									if(p2 != contact_player)
										damage_rate = min(max(0.0f, 1.0f - dist / 20.0f), 1.0f);

									if(p2 == contact_player)
										contact_player->snap.vel += proj->vel.normal_safe() * 144.0f;
									else
										p2->snap.vel += (box.center() - closest).normal_safe() * 144.0f * damage_rate;

									int32_t damage = max(0, (int32_t)(100 * damage_rate));

									dealDamage(p2, damage, p);

									if(true)
									{
										Vec3 dir = closest - hit_pos;
										if(dir.length() < 0.001f)
											dir = proj->vel;
										damageFeedback(p2, dir.normal_safe(), damage);
										//hitFeeback(...);
									}
								}
							}

							issueBulletHit(proj->type, hit_pos, face_normal);

							removeProjectile(proj);
							continue;
						}
					}

					proj->pos += proj->vel * dt;
				}
				break;
			case BULLET_TYPE_GRENADE:
				{
					proj->vel += _game_info.GRAVITY * dt;

					bool detonate = false;

					const SPtr<svPlayer>& p = _players.at(proj->owner_slot);

					if(p)
						p->_bound->detachFromParent(false);

					const REAL GRENADE_RADIUS = 1.5f;
					Vec3 radius(GRENADE_RADIUS, GRENADE_RADIUS, GRENADE_RADIUS);
					Vec3 disp = proj->vel * dt;
					Vec3 new_pos, new_vel, remaining_disp, collision_normal = Vec3::yAxis;
					bool touch_ground;
					SPtr<SNode> contact_node;

					// at server side, use _root_node, at client side use _map_node
					bool ret = FPSCore::clipAir(AABB(proj->pos - radius, proj->pos + radius), disp, proj->vel, _root_node, 1.5f, 30.0f, new_pos, remaining_disp, new_vel, touch_ground, collision_normal, contact_node);

					if(p)
					{
						_root_node->addChild(p->_bound, false);
						p->_bound->setWorldTransform(Mat4::translation(p->snap.pos));
					}

					SPtr<svPlayer> contact_player;

					if(ret)
					{
						if(contact_node->getUserData())
						{
							if(true)	// player info
								contact_player = contact_node->getUserData<svNodeInfo>()->p.lock();
						}

						proj->pos = new_pos + collision_normal * 0.01f;
						proj->vel = proj->vel.reflect(collision_normal);
						proj->vel = (proj->vel * collision_normal) * collision_normal * 0.666667f + proj->vel.slideAlong(collision_normal) * 0.8f;

						if(contact_player)
							detonate = true;
					}
					else
					{
						proj->pos += disp;
					}

					if(proj->age >= 2.5f)
						detonate = true;

					if(detonate)
					{
						Vec3 hit_pos = new_pos;
						fast_list<svPlayer>::iterator end_iter = _players.end();
						for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
						{
							const SPtr<svPlayer>& p2 = (*iter);
							if(p2->snap.flags & PLAYER_FLAG_DEAD)
								continue;

							AABB box(_game_info.PLAYER_BB.minimum() + p2->snap.pos, _game_info.PLAYER_BB.maximum() + p2->snap.pos);

							REAL dist = Distance::PointAABB(hit_pos, box);
							dist = max(0.0f, dist - GRENADE_RADIUS);
							if(dist > 20.0f)
								continue;

							dist = max(0.0f, dist - 1.0f);

							REAL damage_rate = 1.0f;
							if(p2 != contact_player)
								damage_rate = min(max(0.0f, 1.0f - dist / 20.0f), 1.0f);

							Vec3 closest;
							Closest::PointAABB(hit_pos, box, closest);

							p2->snap.vel += (box.center() - closest).normal_safe() * 144.0f * damage_rate;

							int32_t damage = max(0, (int32_t)(100 * damage_rate));

							dealDamage(p2, damage, p);

							if(true)
							{
								Vec3 dir = closest - hit_pos;
								if(dir.length() < 0.001f)
									dir = proj->vel;
								if(dir.length() < 0.001f)
									dir = box.center() - closest;
								damageFeedback(p2, dir.normal_safe(), damage);
								//hitFeeback(...);
							}
						}

						issueBulletHit(proj->type, proj->pos, collision_normal);

						removeProjectile(proj);
					}
				}
				break;
			}
		}
	}

	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			(*iter)->_bound->detachFromParent(false);
	}
}

void					svGame::addProjectile(const SPtr<svProjectile>& proj)
{
	if(!_projectiles.add(proj))
		return;

	MsgWriter writer(NetMsg::alloc(), MY_OP::S_ADD_PROJECTILE);
	writer.write(proj->getSlot());
	writer.write(_frame - 1);							// because at client side, it still has one frame owed to simulate
	writer.write(proj->owner_slot);
	writer.write((uint32_t)proj->type);
	writer.write(proj->pos);
	writer.write(proj->vel);

	fast_list<svPlayer>::iterator end_iter = _players.end();
	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

	writer.getMsg()->dealloc();
}

void					svGame::removeProjectile(const SPtr<svProjectile>& proj)
{
	MsgWriter writer(NetMsg::alloc(), MY_OP::S_REMOVE_PROJECTILE);

	writer.write(proj->getSlot());

	fast_list<svPlayer>::iterator end_iter = _players.end();
	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

	writer.getMsg()->dealloc();

	_projectiles.remove(proj->getSlot());
}

void					svGame::issueBulletHit(BULLET_TYPE bullet_type, const Vec3& hit_pos, const Vec3& face_normal)
{
	MsgWriter writer(NetMsg::alloc(), MY_OP::S_BULLET_HIT);

	writer.write((uint32_t)bullet_type);
	writer.write(hit_pos);
	writer.write(face_normal);

	fast_list<svPlayer>::iterator end_iter = _players.end();
	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

	writer.getMsg()->dealloc();
}

void					svGame::handleHitScan(const SPtr<svPlayer>& p, BULLET_TYPE bullet_type)
{
	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		{
			const SPtr<svPlayer>& p2 = (*iter);
			if(p2 == p)
				continue;
			_root_node->addChild(p2->_bound, false);
			p2->_bound->setWorldTransform(Mat4::translation(p2->snap.pos));
		}
	}

	Vec3 dir = Vec3::zNegAxis * Mat3::rotationX(p->snap.pitch) * Mat3::rotationY(p->snap.yaw);

	Vec3 start = p->snap.pos + Vec3(0, _game_info.PLAYER_BB.extent()[1] + 2.8f, 0) + dir * (3.0f - 0.01f);
	Vec3 end = start + dir * 1000.0f;

	bool hit_map = false;

	Vec3 face_normal;

	REAL t;
	SPtr<SMeshNode> mn;
	int32_t triidx;

	while(FPSCore::selectTriangle(Ray(start, dir), _root_node, t, mn, triidx))
	{
		SPtr<svPlayer> contact_player;
		if(mn->getUserData())
		{
			if(true)	// player info
				contact_player = mn->getUserData<svNodeInfo>()->p.lock();
		}
		if(!contact_player)
		{
			end = start + dir * t;

			hit_map = true;

			face_normal = mn->mesh()->getFaceNormal(triidx) * mn->world_rot().asMat3();

			break;
		}

		contact_player->_bound->detachFromParent(false);

		switch(bullet_type)
		{
		case BULLET_TYPE_SLUG:
			{
				contact_player->snap.vel += dir * 144.0f;
				dealDamage(contact_player, 100, p);

				damageFeedback(contact_player, dir, 100);
			}
			break;
		}

		if(bullet_type !=BULLET_TYPE_SLUG)
			break;
	}

	if(true)
	{
		MsgWriter writer(NetMsg::alloc(), MY_OP::S_HIT_SCAN);

		writer.write(p->getSlot());
		writer.write((uint32_t)bullet_type);
		writer.write(start);
		writer.write(end);

		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

		writer.getMsg()->dealloc();

		if(hit_map)
			issueBulletHit(bullet_type, end, face_normal);
	}

	if(true)
	{
		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			(*iter)->_bound->detachFromParent(false);
	}
}

void					svGame::dealDamage(const SPtr<svPlayer>& p, int32_t damage, const SPtr<svPlayer>& attacker)
{
	if(p->snap.flags & PLAYER_FLAG_DEAD)
		return;

	p->status.hp -= damage;

	sendPlayerStatus(p);

	if(p->status.hp <= 0)
	{
		p->snap.flags |= PLAYER_FLAG_DEAD;
		p->_next_firable_frame = _frame + (uint64_t)(_game_info.fps * 2);

		if(attacker && attacker != p)
			++attacker->stats.score;
		else
			--p->stats.score;

		sendPlayersStatsToAll();

		p->_bound->detachFromParent(false);
	}
}

void					svGame::damageFeedback(const SPtr<svPlayer>& p, const Vec3& dmg_dir, int32_t dmg)
{
	Vec3 d = dmg_dir * Mat3::rotationY(-p->snap.yaw) * Mat3::rotationX(-p->snap.pitch);
	REAL angle = d.angle(Vec3::yAxis);
	Vec3 axis;
	if(angle <= 0.5f || angle >= 179.5f)
		axis = Vec3::xAxis;
	else
		axis = (Vec3::yAxis ^ d).normal_safe();
	REAL rate = min(max(0.0f, dmg / 100.0f), 1.0f) * (1.0f - FABS(angle - 90) / 90);

	MsgWriter writer(NetMsg::alloc(), MY_OP::S_DAMAGE_FEEDBACK);

	writer.write(axis);
	writer.write(rate);

	p->_srvcli->getConn()->send(writer.getMsg());
}

void					svGame::spawnPlayer(const SPtr<svPlayer>& p)
{
	p->snap.reset();

	if(true)
	{
		const SPAWN_POINT& sp = _spawn_points[((size_t)((_fps_ctrl.getTimer().cur() & 0xffffffff))) % _spawn_points.size()];
		p->snap.pos = sp.pos;
		p->snap.yaw = sp.yaw;
	}

	p->status.reset_for_spawn(225);
	p->_just_teleported = true;
	p->_just_spawned = true;
	p->_next_firable_frame = _frame + (uint64_t)(_game_info.fps * 0.5);
	//p->_pressed_jump = false;

	sendPlayerStatus(p);
}

bool					svGame::addPlayer(const SPtr<SrvCli>& srvcli)
{
	if(srvcli->player_link._p.valid())
	{
		MsgWriter writer(NetMsg::alloc(), MY_OP::S_JOIN_GAME);
		writer.write(false);
		srvcli->getConn()->send(writer.getMsg());
		return false;
	}

	SPtr<svPlayer> p = new svPlayer(this, srvcli);
	p->name = srvcli->getPlayerName();
	if(!_players.add(p))
	{
		MsgWriter writer(NetMsg::alloc(), MY_OP::S_JOIN_GAME);
		writer.write(false);
		srvcli->getConn()->send(writer.getMsg());
		return false;
	}

	// link
	srvcli->player_link._p = p;

	p->loadBound();

	if(true)
	{
		MsgWriter writer(NetMsg::alloc(), MY_OP::S_JOIN_GAME);
		writer.write(true);
		const svGame::GAME_INFO& info = getGameInfo();
		writer.write(info.fps);
		writer.write(info.snap_interval);
		writer.write(info.PLAYER_BB.minimum());
		writer.write(info.PLAYER_BB.maximum());
		writer.write(info.PLAYER_MAX_RUN_SPEED);
		writer.write(info.GRAVITY);

		writer.write(p->getSlot());

		srvcli->getConn()->send(writer.getMsg());

		sendPlayersStats(p);
	}

	spawnPlayer(p);

	return true;
}
void					svGame::removePlayer(uint32_t slot)
{
	const SPtr<svPlayer>& player = _players.at(slot);

	// unlink
	player->_srvcli->player_link._p.reset();

	player->_game.reset();
	player->_srvcli.reset();

	_players.remove(slot);

	if(true)
	{
		MsgWriter writer(NetMsg::alloc(), MY_OP::S_REMOVE_PLAYER);

		writer.write(slot);

		fast_list<svPlayer>::iterator end_iter = _players.end();
		for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

		writer.getMsg()->dealloc();
	}
}
void					svGame::sendPlayerStatus(const SPtr<svPlayer>& p)
{
	MsgWriter writer(NetMsg::alloc(), MY_OP::S_PLAYER_STATUS);
	writer.write(&p->status, sizeof(p->status));

	p->_srvcli->getConn()->send(writer.getMsg());
}
void					svGame::sendPlayersStats(const SPtr<svPlayer>& p)
{
	MsgWriter writer(NetMsg::alloc(), MY_OP::S_PLAYERS_STATS);

	writer.write(_players.size());

	fast_list<svPlayer>::iterator end_iter = _players.end();
	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
	{
		writer.write((*iter)->getSlot());
		writer.write((*iter)->name);
		writer.write(&((*iter)->stats), sizeof((*iter)->stats));
	}

	p->_srvcli->getConn()->send(writer.getMsg());
}

void					svGame::sendPlayersStatsToAll()
{
	MsgWriter writer(NetMsg::alloc(), MY_OP::S_PLAYERS_STATS);

	writer.write(_players.size());

	fast_list<svPlayer>::iterator end_iter = _players.end();
	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
	{
		writer.write((*iter)->getSlot());
		writer.write((*iter)->name);
		writer.write(&((*iter)->stats), sizeof((*iter)->stats));
	}

	for(fast_list<svPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
		(*iter)->_srvcli->getConn()->send(writer.getMsg()->clone());

	writer.getMsg()->dealloc();
}

class Server: public RefBase
{
private:
	SPtr<Listener>					_listener;
	SPtr<NetIOCP>					_iocp;
private:
	fast_list<SrvCli>				_srvclis;
private:
	SPtr<svGame>					_game;
public:
	Server()
	{
		_srvclis.init(1024);

		// i think the listener instance can exist no longer than the server instance, so the following should be okay
		_listener = new Listener();
		_listener->enableQueuingConnections();
		_iocp = new NetIOCP();
	}
protected:
	~Server()
	{
		Z_ASSERT( _iocp->isFullyStopped() );
	}
public:
	bool							init()
	{
		if(!_listener->create(5000))
			return false;

		_iocp->create(_srvclis.capacity());
		_iocp->setListener(_listener);
		_iocp->setIOCPThreads(4);

		return true;
	}
	void							start(const wcs& map_dir)
	{
		_game = new svGame();
		_game->loadMap(map_dir);

		_iocp->start();
	}
	void							stop()
	{
		_iocp->stop();
		while(!_iocp->isFullyStopped())
			W32Thread::sleep(10);
	}
private:
	void							handleIncomingConnections()
	{
		if(_srvclis.size() >= _iocp->getMaxConnections())
			return;

		intptr_t s;
		while(_listener->dequeueConnection(s))
		{
			stack_walk();

			SPtr<SrvCli> srvcli = new SrvCli(s, this);

			if(!_srvclis.add(srvcli))
			{
				NetSys::destroySocket(s);
				continue;
			}

			srvcli->getConn()->setDeferRecvMsgProc(true);

			Z_VERIFY( _iocp->manage(srvcli->getConn()) );

			MsgWriter msgwriter(NetMsg::alloc(), MY_OP::S_CLIENT_INFO);
			msgwriter.write(srvcli->getSlot());
			msgwriter.write(_game->getMapDir());

			srvcli->getConn()->send(msgwriter.getMsg());
		}
	}
public:
	void							update()
	{
		handleIncomingConnections();
		if(true)
		{
			fast_list<SrvCli>::iterator end_iter = _srvclis.end();
			for(fast_list<SrvCli>::iterator iter = _srvclis.begin(); iter != end_iter; ++iter)
				(*iter)->getConn()->procRecvMsgs();
		}
		_game->update();
		if(true)
		{
			fast_list<SrvCli>::iterator end_iter = _srvclis.end();
			for(fast_list<SrvCli>::iterator iter = _srvclis.begin(); iter != end_iter;)
			{
				SrvCli* srvcli = (*iter).get();
				if(srvcli->getConn()->dead() && srvcli->getConn().unique())
				{
					++iter;

					_srvclis.remove(srvcli->getSlot());
					continue;
				}
				srvcli->getConn()->flush();
				++iter;
			}
		}
	}
public:
	const SPtr<svGame>&				getGame() const {return _game;}
};

bool				SrvCli::nmpu(NetConn* conn, NetMsg* msg)
{
	MsgReader reader(msg);
	switch(msg->opcode)
	{
	case MY_OP::C_JOIN_GAME:
		{
			reader.read(_name);
			bool ret = _server.lock()->getGame()->addPlayer(this);
		}
		break;
	case MY_OP::C_INPUT:
		{
			uint32_t count = reader.read<uint32_t>();

			// TODO: check count

			uint64_t frame;
			PLAYER_INPUT input;
			for(uint32_t i = 0; i < count; ++i)
			{
				reader.read(frame);
				reader.read(&input, sizeof(input));

				// TODO: check frame

				if(player_link._p.valid())
				{
					player_link._p.lock()->last_input = input;
					player_link._p.lock()->input_history.addInput(frame, input);
				}
			}
		}
		break;
	default:
		return false;
	}
	return true;
}

SPtr<Server> g_server;

class clPlayer: public fast_list_item<clPlayer>
{
public:
	struct CL_LOOK
	{
		REAL					yaw;
		REAL					pitch;

		CL_LOOK()								{	reset();	}
		void					reset()			{	memset(this, 0, sizeof(*this));	}

	}						cl_look;

	WEAPON_TYPE				cl_weapon;
public:
	InputHistory<PLAYER_INPUT>
							cl_input_history;
	InputHistory<size_t>	cl_input_history_pressed_jump;
public:
	Snaps					sv_snaps;
public:
	struct PREDICTION
	{
		SNAP2					s;
		uint64_t				f;

		PREDICTION()						{	reset();	}
		void					reset()		{	memset(this, 0, sizeof(*this));	}

	}						cl_prediction;
public:
	SNAP2					gfx_snap;
public:
	SPtr<SStructNode>		gfx;

	SPtr<cgDecalFX::Item>	shadow;
public:
	struct CG_DAMAGE_FEEDBACK
	{
		CountDown				cd;
		Vec3					axis;
		REAL					rate;

		CG_DAMAGE_FEEDBACK()				{reset();}

		void					reset()
		{
			rate = 0.0f;
			cd.setRemaining(-1.0f);
		}
	}						cg_damage_feedback;
public:
	SPtr<SMeshNode>			bound;
public:
	LOWER_ANIM				gfx_lower_anim;
	UPPER_ANIM				gfx_upper_anim;
	WEAPON_TYPE				gfx_weapon;
public:
	WPtr<clGame>			game;
public:
	clPlayer()
	{
		reset();
	}
protected:
	~clPlayer()
	{
		if(gfx)
			gfx->detachFromParent(false);
	}
public:
	void				reset()
	{
		//cl_look.reset();
		//cl_weapon = 0;

		//cl_input_history.clear();
		//cl_input_history_pressed_jump.clear();

		//sv_snaps.clear();

		cl_weapon = WEAPON_TYPE_ROCKET_LAUNCHER;

		gfx_weapon = WEAPON_TYPE_ROCKET_LAUNCHER;

		resetAnim();
	}
	void				resetWeapon(WEAPON_TYPE weapon)
	{
		gfx_weapon = weapon;

		gfx->getLoadedNode(L"rocket_launcher")->setVisible(false);
		gfx->getLoadedNode(L"grenade_launcher")->setVisible(false);
		gfx->getLoadedNode(L"railgun")->setVisible(false);

		switch(gfx_weapon)
		{
		case WEAPON_TYPE_ROCKET_LAUNCHER:
			gfx->getLoadedNode(L"rocket_launcher")->setVisible(true);
			break;
		case WEAPON_TYPE_GRENADE_LAUNCHER:
			gfx->getLoadedNode(L"grenade_launcher")->setVisible(true);
			break;
		case WEAPON_TYPE_RAILGUN:
			gfx->getLoadedNode(L"railgun")->setVisible(true);
			break;
		}

		// it has nothing to do with cl_weapon
	}
public:
	void				loadBound()
	{
		bound = SMeshNode::load(L"meshes/player_bound/mesh_player_bound.bin", L"meshes/player_bound/mtl_player_bound.xml");
	}
public:
	void				resetAnim()
	{
		gfx_lower_anim = LOWER_ANIM_IDLE;
		gfx_upper_anim = UPPER_ANIM_IDLE;

		if(gfx)
			SCtrlOps::setOnlyTM(TimeAnimCfg(16.2f, 17.0666666666f, -1), gfx);
	}
	void				loadGfx()
	{
		wcs dir = L"meshes/char";
		gfx = SStructNode::load(dir + L"/scene.xml");
		gfx->loadTMCtrl(dir + L"/scene_tm.xml");
		gfx->loadSkinCtrl(dir + L"/scene_skin.xml");
		gfx->loadUserText(dir + L"/scene_user.txt");

		gfx->loadTextures();

		resetAnim();
		resetWeapon(WEAPON_TYPE_ROCKET_LAUNCHER);
	}
	void					update_upper_anim(const SNAP2& s)
	{
		if(s.flags & PLAYER_FLAG_DEAD)
		{
			if(gfx_upper_anim != UPPER_ANIM_DIE)
			{
				play_upper_anim(UPPER_ANIM_DIE);
			}
			return;
		}
		else
		{
			if(gfx_upper_anim == UPPER_ANIM_DIE)
			{
				play_upper_anim(UPPER_ANIM_IDLE);
			}
		}

		if(gfx_upper_anim == UPPER_ANIM_SHOOT)
		{
			if(gfx->getLoadedNode(L"Bip01 Spine1")->getTMCtrl()->getAnimCtrl()->current_stage().isStopped())
			{
				play_upper_anim(UPPER_ANIM_IDLE);
			}
		}
		if(gfx_upper_anim == UPPER_ANIM_CHANGE_WEAPON)
		{
			const AnimCtrl::Clip& anim = gfx->getLoadedNode(L"Bip01 Spine1")->getTMCtrl()->getAnimCtrl()->current_stage();
			if(anim.getTime() > (anim.getStart() + anim.getEnd()) * 0.5f)
			{
				gfx->getLoadedNode(L"rocket_launcher")->setVisible(false);
				gfx->getLoadedNode(L"grenade_launcher")->setVisible(false);
				gfx->getLoadedNode(L"railgun")->setVisible(false);

				switch(gfx_snap.weapon)
				{
				case WEAPON_TYPE_ROCKET_LAUNCHER:
					gfx->getLoadedNode(L"rocket_launcher")->setVisible(true);
					break;
				case WEAPON_TYPE_GRENADE_LAUNCHER:
					gfx->getLoadedNode(L"grenade_launcher")->setVisible(true);
					break;
				case WEAPON_TYPE_RAILGUN:
					gfx->getLoadedNode(L"railgun")->setVisible(true);
					break;
				}

				gfx_weapon = gfx_snap.weapon;
			}
			if(gfx->getLoadedNode(L"Bip01 Spine1")->getTMCtrl()->getAnimCtrl()->current_stage().isStopped())
				play_upper_anim(UPPER_ANIM_IDLE);
		}
	}
	void					update_lower_anim(const SNAP2& s)
	{
		if(s.flags & PLAYER_FLAG_DEAD)
		{
			if(gfx_lower_anim != LOWER_ANIM_DIE)
			{
				play_lower_anim(LOWER_ANIM_DIE);
			}
			return;
		}

		if(!s.touch_ground)
		{
			if(gfx_lower_anim != LOWER_ANIM_JUMP && gfx_lower_anim != LOWER_ANIM_JUMP_BACKWARD)
			{
				Vec3 dir = -s.tilt;

				LOWER_ANIM anim = LOWER_ANIM_JUMP;
				if(dir.length())
				{
					dir.normalize();
					if(dir.angle(Vec3::zNegAxis) < 100.0f)
						anim = LOWER_ANIM_JUMP;
					else
						anim = LOWER_ANIM_JUMP_BACKWARD;
				}
				play_lower_anim(anim);
			}
		}
		else
		{
			if(s.touch_ground && (s.no_move_time >= 0.2f && (gfx_lower_anim == LOWER_ANIM_RUN || gfx_lower_anim == LOWER_ANIM_RUN_BACKWARD || gfx_lower_anim == LOWER_ANIM_WALK || gfx_lower_anim == LOWER_ANIM_WALK_BACKWARD)))
				play_lower_anim(LOWER_ANIM_IDLE);
			if(gfx_lower_anim == LOWER_ANIM_JUMP || gfx_lower_anim == LOWER_ANIM_JUMP_BACKWARD)
				play_lower_anim(LOWER_ANIM_LAND);

			if(gfx_lower_anim == LOWER_ANIM_LAND || gfx_lower_anim == LOWER_ANIM_IDLE)
			{
				Vec3 dir = -s.tilt;

				if(dir.length())
				{
					dir.normalize();
					if(dir.angle(Vec3::zNegAxis) < 100.0f)
						play_lower_anim(LOWER_ANIM_RUN);
					else
						play_lower_anim(LOWER_ANIM_RUN_BACKWARD);
				}
			}
			if(gfx_lower_anim == LOWER_ANIM_RUN || gfx_lower_anim == LOWER_ANIM_RUN_BACKWARD)
			{
				Vec3 dir = -s.tilt;

				if(dir.length())
				{
					dir.normalize();
					if(dir.angle(Vec3::zNegAxis) < 100.0f)
					{
						if(gfx_lower_anim != LOWER_ANIM_RUN)
							play_lower_anim(LOWER_ANIM_RUN);
					}
					else
					{
						if(gfx_lower_anim != LOWER_ANIM_RUN_BACKWARD)
							play_lower_anim(LOWER_ANIM_RUN_BACKWARD);
					}
				}
			}
			if(gfx_lower_anim == LOWER_ANIM_LAND)
			{
				if(gfx->getLoadedNode(L"Bip01 Spine")->getTMCtrl()->getAnimCtrl()->current_stage().isStopped())
				{
					play_lower_anim(LOWER_ANIM_IDLE);
				}
			}
		}
	}
	void					syncGfx(uint64_t frame, REAL dt);
public:
	void					play_lower_anim(LOWER_ANIM anim)
	{
		if(!gfx)
			return;

		gfx_lower_anim = anim;

		REAL blend_duration = 0.2f;

		switch(gfx_lower_anim)
		{
		case LOWER_ANIM_IDLE:
			transit_lower_anim(16.2f, 17.0666666666f, -1);
			break;
		case LOWER_ANIM_RUN:
			transit_lower_anim(11.86666666f, 12.4f, -1);
			break;
		case LOWER_ANIM_RUN_BACKWARD:
			transit_lower_anim(12.466666666f, 13.0666666666f, -1);
			break;
		case LOWER_ANIM_JUMP:
			transit_lower_anim(13.8f, 14.4f, 0);
			break;
		case LOWER_ANIM_JUMP_BACKWARD:
			transit_lower_anim(15.2f, 16.1333333333f, 0);
			break;
		case LOWER_ANIM_LAND:
			transit_lower_anim(14.4f, 15.13333333f, 0);
			break;
		case LOWER_ANIM_DIE:
			transit_lower_anim(2, 3.93333333f, 0);
			break;
		}
	}
	void					play_upper_anim(UPPER_ANIM anim)
	{
		if(!gfx)
			return;

		gfx_upper_anim = anim;

		REAL blend_duration = 0.2f;

		switch(gfx_upper_anim)
		{
		case UPPER_ANIM_IDLE:
			transit_upper_anim(16.2f, 17.0666666666f, -1);
			break;
		case UPPER_ANIM_SHOOT:
			transit_upper_anim(8.666666666f, 9, 0, 0.05f);
			break;
		case UPPER_ANIM_CHANGE_WEAPON:
			transit_upper_anim(9.466666666f, 10.0666666666f, 0);
			break;
		case UPPER_ANIM_DIE:
			transit_upper_anim(2, 3.93333333f, 0);
			break;
		}
	}
	void					transit_lower_anim(REAL start, REAL end, int loops, REAL blend_duration = 0.2f)
	{
		if(!gfx)
			return;

		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01 L Thigh"));
		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01 R Thigh"));
		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01 Pelvis"), 0);
		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01 Spine"), 0);
		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01"), 0);
	}
	void					transit_upper_anim(REAL start, REAL end, int loops, REAL blend_duration = 0.2f)
	{
		if(!gfx)
			return;

		SCtrlOps::transitOnlyTM(TimeAnimCfg(start, end, loops), blend_duration, gfx->getLoadedNode(L"Bip01 Spine1"));
	}
public:
	friend class clGame;
};

// we must be using double because accumulative single-precision floating poing round-off error can be horrible
class TimePrediction
{
private:
	double				_samples[256];
	size_t				_sample_count;
	size_t				_latest_sample_index;
private:
	double				_interval;
#ifdef _DEBUG
	size_t				_set_interval;
#endif
private:
	size_t				_num_samples_considered;
	double				_time_range_considered;
	double				_recursive_multiplier;
private:
	QStats<256>			_stats;
public:
	TimePrediction(double interval = 1.0)
	{
		Z_ASSERT( _countof(_samples) == _stats.getNumBuckets() );
		_sample_count = 0;
		_latest_sample_index = _countof(_samples) - 1;

		_interval = interval;
#ifdef _DEBUG
		_set_interval = false;
#endif

		_num_samples_considered = _countof(_samples);
		_time_range_considered = 1000.0;
		_recursive_multiplier = 1.0;
	}
public:
	void				setInterval(double interval)
	{
		_interval = interval;
#ifdef _DEBUG
		_set_interval = true;
#endif
	}
	double				getInterval() const
	{
		return _interval;
	}
public:
	void				clear()
	{
		_sample_count = 0;
		_latest_sample_index = _countof(_samples) - 1;

		_stats.reset();
	}
	bool				any() const
	{
		return !!getSampleCount();
	}
	size_t				getSampleCount() const
	{
		return _sample_count;
	}
public:
	void				setNumSamplesConsidered(size_t num)
	{
		_num_samples_considered = min(max((size_t)1, num), _countof(_samples));
	}
	size_t				getNumSamplesConsidered() const
	{
		return _num_samples_considered;
	}
	void				setTimeRangeConsidered(double range)
	{
		_time_range_considered = max(0.0, range);
	}
	double				getTimeRangeConsidered() const
	{
		return _time_range_considered;
	}
	void				setRecursiveMultiplier(double multiplier)
	{
		_recursive_multiplier = max(0.0, multiplier);
	}
	double				getRecursiveMultiplier() const
	{
		return _recursive_multiplier;
	}
public:
	void				input(double t)
	{
		_latest_sample_index = (_latest_sample_index + 1) % _countof(_samples);
		_samples[_latest_sample_index] = t;
		_sample_count = min(_sample_count + 1, _countof(_samples));
		if(_sample_count > 1)
			_stats.input(_samples[_latest_sample_index] - _samples[(_latest_sample_index + _countof(_samples) - 1) % _countof(_samples)]);
	}
public:
	double				predict() const
	{
#ifdef _DEBUG
		Z_ASSERT( !!_set_interval );
#endif
		Z_ASSERT( any() );
		if(!any())
			return 0.0;

		double sum = 0.0;
		double weight_sum = 0.0;
		double weight = 1.0;
		size_t p = _latest_sample_index;

		double latest = _samples[p];
		for(size_t c = min(_num_samples_considered, _sample_count), c2 = 1; c; --c, ++c2)
		{
			double s = _samples[p];
			if(latest - s > _time_range_considered)
				break;
			sum += (s + c2 * _interval) * weight;
			weight_sum += weight;

			p = (p + _countof(_samples) - 1) % _countof(_samples);
			weight *= _recursive_multiplier;
		}

		return sum / weight_sum;
	}
public:
	double				jitter() const
	{
		return _stats.standard_deviation();
	}
};

class cgRailHitFX: public RefBase
{
public:
	SPtr<SMeshNode>					mn;
protected:
	~cgRailHitFX()
	{
		if(mn)
			mn->detachFromParent(false);
	}
public:
	void							load()
	{
		mn = SMeshNode::load(L"meshes/railhit/mesh_railhit.bin", L"meshes/railhit/mtl_railhit.xml");
		mn->loadTextures();
		mn->buildControllers(L"");
		if(true)
		{
			REAL start, end;
			mn->getCtrlSet().getAnimDataTimeRange(start, end);
			mn->getCtrlSet().set(TimeAnimCfg(start, end, 0).setSpeed(0.5f));
		}
	}
	void							update(REAL dt)
	{
		mn->getCtrlSet().update(CtrlUpdater(dt, false));
	}
};

class cgScorchMarkItem: public RefBase
{
public:
	SPtr<cgDecalFX::Item>	decal;
	REAL					life;
public:
	cgScorchMarkItem(const SPtr<cgDecalFX::Item>& decal_, REAL life_)
	{
		decal = decal_;
		life = life_;
	}
protected:
	~cgScorchMarkItem() {}
};

class PLAYER_EXTRA: public fast_list_item<PLAYER_EXTRA>
{
public:
	wcs								name;
public:
	PLAYER_STATUS					status;
	PLAYER_STATS					stats;
protected:
	~PLAYER_EXTRA() {}
};

class clGame: public RefBase
{
private:
	wcs								_player_name;					// your wanted name
private:
	fast_list<clPlayer>				_players;
	fast_list<clProjectile>			_projectiles;
private:
	SPtr<SNode>						_cg_root_node;
	SPtr<SNode>						_root_node;
private:
	SPtr<SStructNode>				_map_node;
private:
	class CG
	{
	public:
		SPtr<cgPointLightFX>			fake_light;
		SPtr<cgBillBoardFX>				bb;
		SPtr<cgBillBoardFX::Image>		bb_rocket_explosion;
		SPtr<cgBeamFX>					slug;

		list<SPtr<cgBillBoardFX::Item>>	bullet_hits;

		list<SPtr<cgBeamFX::Item>>		slugs;

		list<SPtr<cgRailHitFX>>			railhits;

		SPtr<cgDecalFX>					decal_shadow;

		SPtr<cgDecalFX>					decal_scorch_mark;
		list<SPtr<cgScorchMarkItem>>	scorch_marks;

		SPtr<cgCloudFX>					cloud;

		struct CACHE
		{
			SPtr<SMeshNode>					rocket;
			SPtr<SMeshNode>					grenade;
			SPtr<cgRailHitFX>				railhit;
		}								cache;
	}								_cg;
private:
	Counter							_core_counter;
private:
	struct GAME_INFO
	{
		size_t							valid;
		REAL							fps;
		uint32_t						snap_interval;				// in frames

		AABB							PLAYER_BB;
		REAL							PLAYER_MAX_RUN_SPEED;
		Vec3							GRAVITY;
	public:
		GAME_INFO()						{clear();}
		void							clear()						{memset(this, 0, sizeof(*this));}
	}								_game_info;
private:
	TimePrediction					_next_snap_prediction;
	size_t							_improve_prediction_accuracy;
	Counter							_core_counter_improve_prediction_accuracy;
	mutex_fast						_mx_prediction;
	// local perception filter
	class LPF
	{
	public:
		uint64_t						frame;
		double							at;

		uint64_t						cur_frame;
	public:
		LPF()							{	reset();	}
	public:
		void							reset()
		{
			frame = -1I64;
			at = 0.0;

			cur_frame = -1I64;
		}
		bool							valid() const
		{
			return frame != -1I64;
		}
	}								_lpf;

	class LOCAL_PLAYER_PREDICTION
	{
	public:
		uint64_t						frame;
		double							at;
		uint64_t						cur_frame;
	public:
		LOCAL_PLAYER_PREDICTION()
		{
			reset();
		}
	public:
		void							reset()
		{
			frame = -1I64;
			at = 0.0;

			cur_frame = -1I64;
		}
		bool							valid() const
		{
			return frame != -1I64;
		}
	}								_lpp;
	class BUFFERED_INPUT
	{
	public:
		size_t							num_frames_to_buffer;
		uint64_t						frames[256];
		size_t							count;
	public:
		BUFFERED_INPUT()
		{
			num_frames_to_buffer = 1;				// default 1, we already have some cheap delta compression (no sub input details comparison, just the whole) that we don't need to send input every frame

			clear();
		}
	public:
		void							clear()
		{
			count = 0;
		}
		void							add(uint64_t f)
		{
			if(count >= _countof(frames))
				return;
			frames[count] = f;
			++count;
		}
	}								_buffered_input;
private:
	class NET
	{
	public:
		uint32_t						player_slot;				// svPlayer slot
		SPtr<NetConn>					conn;
		SPtr<NetIOCP>					iocp;
	public:
		NET()
		{
			player_slot = -1I32;
		}
		~NET()
		{
			if(conn)
				conn->setDead();
			iocp->stop();
			while(!iocp->isFullyStopped())
				W32Thread::sleep(1);
		}
	}								_net;
public:
	fast_list<PLAYER_EXTRA>			players_extra;
public:
	clGame(const SPtr<SMgr>& smgr)
	{
		_improve_prediction_accuracy = true;
		_core_counter_improve_prediction_accuracy = _core_counter;

		_players.init(1024);
		_projectiles.init(1024);

		smgr->root()->addChild(_root_node = new SNode(), false);
		smgr->root()->addChild(_cg_root_node = new SNode(), false);

		if(true)
		{
			scoped_lock<mutex_fast> o(_mx_prediction);

			_next_snap_prediction.setInterval(1.0 / 60 * 2);
			_next_snap_prediction.setTimeRangeConsidered(2.0);
		}

		_buffered_input.num_frames_to_buffer = 1;

		players_extra.init(1024);
	}
protected:
	~clGame()
	{
		if(_root_node)
			_root_node->detachFromParent(false);
		if(_cg_root_node)
			_cg_root_node->detachFromParent(false);
	}
public:
	void							setPlayerName(const wcs& name)
	{
		_player_name = name;
	}
public:
	bool							initEssentialGfx()
	{
		_cg.fake_light = new cgPointLightFX(AABB(Vec3(-10000, -10000, -10000), Vec3(10000, 10000, 10000)));
		_cg.fake_light->setLookupTexture(Tex2D::load(L"textures/fake_light.png", false, GfxEnum::PIXELFORMAT_A8R8G8B8, false));
		_cg.fake_light->loadShaders();
		_cg_root_node->addChild(_cg.fake_light, false);

		_cg.bb = new cgBillBoardFX(AABB(Vec3(-10000, -10000, -10000), Vec3(10000, 10000, 10000)));
		_cg.bb->loadShaders();
		_cg_root_node->addChild(_cg.bb, false);

		_cg.bb_rocket_explosion = _cg.bb->createImage(L"textures/explosion.png", 4, 4, 16);

		_cg.slug = new cgBeamFX(AABB(Vec3(-10000, -10000, -10000), Vec3(10000, 10000, 10000)));
		_cg.slug->setBeamTextures(Tex2D::load(L"textures/beam.png", false, GfxEnum::PIXELFORMAT_L8, false), Tex2D::load(L"textures/beam_core.png", false, GfxEnum::PIXELFORMAT_L8, false));
		_cg.slug->loadShaders();
		_cg_root_node->addChild(_cg.slug, false);

		_cg.cache.rocket = SMeshNode::load(L"meshes/rocket/mesh_rocket.bin", L"meshes/rocket/mtl_rocket.xml");
		_cg.cache.rocket->loadTextures();

		_cg.cache.grenade = SMeshNode::load(L"meshes/grenade/mesh_grenade.bin", L"meshes/grenade/mtl_grenade.xml");
		_cg.cache.grenade->loadTextures();

		_cg.cache.railhit = new cgRailHitFX();
		_cg.cache.railhit->load();

		_cg.decal_shadow = new cgDecalFX(AABB(Vec3(-10000, -10000, -10000), Vec3(10000, 10000, 10000)));
		_cg.decal_shadow->setDecalTexture(Tex2D::load(L"textures/shadow_mask.png", false, GfxEnum::PIXELFORMAT_A8R8G8B8, false));
		_cg_root_node->addChild(_cg.decal_shadow, false);

		_cg.decal_scorch_mark = new cgDecalFX(AABB(Vec3(-10000, -10000, -10000), Vec3(10000, 10000, 10000)));
		_cg.decal_scorch_mark->setDecalTexture(Tex2D::load(L"textures/scorch_mark.png", false, GfxEnum::PIXELFORMAT_A8R8G8B8, false));
		_cg_root_node->addChild(_cg.decal_scorch_mark, false);

		return true;
	}
	bool							initNet()
	{
		_net.iocp = new NetIOCP();
		if(!_net.iocp->create(16))
			return false;
		if(!_net.iocp->start())
			return false;

		return true;
	}
	bool							connect(const mbs& ip)
	{
		if(_net.conn)
		{
			// ...
			_net.conn.reset();
		}
		ConnMaker c;
		c.connect(ip.c_str(), 5000);
		while(!c.finished())
			W32Thread::sleep(1);
		if(!c.connected())
			return false;
		intptr_t s;
		c.takeSocket(s);

		_net.conn = new NetConn();
		_net.conn->setSocket(s);
		_net.conn->setDeferRecvMsgProc(true);
		_net.conn->setNMPU(functor2<bool, NetConn*, NetMsg*>(this, &clGame::nmpu));

		_net.conn->enablePreviewRecvMsgs(true);
		_net.conn->setPreviewNMPU(functor2<bool, NetConn*, NetMsg*>(this, &clGame::preview_nmpu));

		_net.iocp->manage(_net.conn);
		_net.conn->enableEncryption();

		return true;
	}
public:
	const GAME_INFO&		getGameInfo() const				{return _game_info;}
public:
	const SPtr<NetConn>&	getConn() const
	{
		return _net.conn;
	}
	REAL					getJitter() const				{return (REAL)_next_snap_prediction.jitter();}
	uint32_t				getClientPlayerSlot() const		{return _net.player_slot;}
	bool					isClientInGame() const			{return getClientPlayerSlot() != -1I32;}
private:
	bool					loadMap(const wcs& dir)
	{
		if(!dir.length())
			return false;

		if(_map_node)
		{
			_map_node->detachFromParent(false);
			_map_node.reset();
		}

		_map_node = SStructNode::load(dir + L"/scene.xml");
		if(!_map_node)
		{
			syslog << "cannot load the struct file";
			return false;
		}
		_map_node->loadTMCtrl(dir + L"/scene_tm.xml");
		_map_node->loadSkinCtrl(dir + L"/scene_skin.xml");
		_map_node->loadUserText(dir + L"/scene_user.txt");

		_map_node->loadTextures();

		_root_node->addChild(_map_node, false);

		if(true)
		{
			_cg.cloud = cgCloudFX::create();
			_cg_root_node->addChild(_cg.cloud, false);

			_cg.cloud->setScale(Vec3(5000, 5000, 5000));
		}

		return true;
	}
private:
	void							updatePlayer(uint32_t slot, uint64_t frame, const SNAP2& s)
	{
		Z_ASSERT( slot >= 0 && slot < (uint32_t)_players.capacity() );
		if(slot >= (uint32_t)_players.capacity())
			return;

		if(!_players.at(slot))
		{
			SPtr<clPlayer> p = new clPlayer();

			p->game = SPtr<clGame>(this);

			p->sv_snaps.add(frame, s);

			_players.replace(slot, p);

			p->loadBound();
			p->loadGfx();
			_cg_root_node->addChild(p->gfx, false);
		}
		else
		{
			const SPtr<clPlayer>& p = _players.at(slot);

			p->sv_snaps.add(frame, s);
		}
	}
public:
	bool							any_lpf_advance()
	{
		if(!_game_info.valid)
			return false;
		if(!_lpf.valid())
			return false;
		_core_counter.update();
		uint64_t f = _lpf.frame + (uint64_t)((_core_counter.elapsedTimeDOUBLE() - _lpf.at) / (1.0 / _game_info.fps));
		if(f > _lpf.frame)
			return false;
		if(_lpf.cur_frame != (uint64_t)(-1I64) && f <= _lpf.cur_frame)
			return false;
		return true;
	}
	void							syncGfx()
	{
		if(!_game_info.valid)
			return;
		if(!_lpf.valid())
			return;
		_core_counter.update();
		uint64_t f = _lpf.frame + (uint64_t)((_core_counter.elapsedTimeDOUBLE() - _lpf.at) / (1.0 / _game_info.fps));
		if(f > _lpf.frame)
			return;
		if(_lpf.cur_frame != (uint64_t)(-1I64) && f <= _lpf.cur_frame)
			return;

		REAL dt = (REAL)(1.0 / _game_info.fps);
		if(_lpf.cur_frame != -1I64)
			dt *= (f - _lpf.cur_frame);

		if(true)
		{
			fast_list<clPlayer>::iterator end_iter = _players.end();
			for(fast_list<clPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			{
				const SPtr<clPlayer>& p = (*iter);
				p->syncGfx(f, dt);

				p->shadow.reset();

				REAL t;
				SPtr<SMeshNode> mn;
				int32_t triidx;
				Ray r(p->gfx_snap.pos + Vec3(0, _game_info.PLAYER_BB.extent()[1], 0), Vec3::yNegAxis);
				if(FPSCore::selectTriangle(r, _map_node, t, mn, triidx))
				{
					REAL opacity = max(0.0f, 1.0f - max(0.0f, t - _game_info.PLAYER_BB.extent()[1]) / (_game_info.PLAYER_BB.extent()[1] * 3.0f));
					if(opacity > 0.0f)
						p->shadow = _cg.decal_shadow->create(_map_node, r.origin() + t * r.direction(), Vec3::yAxis, 7.5f, Vec4(1, 1, 1, opacity));
				}
			}
		}

		syncProjectilesGfx(_lpf.cur_frame);

		if(true)
		{
			for(list<SPtr<cgBillBoardFX::Item>>::iterator iter = _cg.bullet_hits.begin(); iter != _cg.bullet_hits.end();)
			{
				const SPtr<cgBillBoardFX::Item>& bullet_hit = *iter;
				if(bullet_hit->anim >= 1.0f)
				{
					iter = _cg.bullet_hits.erase(iter);
					continue;
				}

				bullet_hit->anim += dt;

				++iter;
			}
		}
		if(true)
		{
			for(list<SPtr<cgRailHitFX>>::iterator iter = _cg.railhits.begin(); iter != _cg.railhits.end();)
			{
				const SPtr<cgRailHitFX>& item = (*iter);

				if(item->mn->getCtrlSet().isStopped())
				{
					iter = _cg.railhits.erase(iter);
					continue;
				}

				_cg_root_node->addChild(item->mn, true);

				item->update(dt);

				++iter;
			}
		}

		if(true)
		{
			for(list<SPtr<cgBeamFX::Item>>::iterator iter = _cg.slugs.begin(); iter != _cg.slugs.end();)
			{
				const SPtr<cgBeamFX::Item>& slug = *iter;
				if(slug->color.w <= 0.0f)
				{
					iter = _cg.slugs.erase(iter);
					continue;
				}

				slug->color.w = max(0.0f, slug->color.w - dt);

				++iter;
			}
		}

		if(true)
		{
			for(list<SPtr<cgScorchMarkItem>>::iterator iter = _cg.scorch_marks.begin(); iter != _cg.scorch_marks.end();)
			{
				const SPtr<cgScorchMarkItem>& item = (*iter);

				if(item->life <= 0.0f)
				{
					iter = _cg.scorch_marks.erase(iter);
					continue;
				}

				item->life -= dt;

				item->decal->color.w = min(max(0.0f, item->life), 1.0f);

				++iter;
			}
		}

		updateMap(dt);

		_lpf.cur_frame = f;
	}
private:
	void							syncProjectilesGfx(uint64_t cur_frame)
	{
		fast_list<clProjectile>::iterator end_iter = _projectiles.end();
		for(fast_list<clProjectile>::iterator iter = _projectiles.begin(); iter != end_iter; ++iter)
		{
			const SPtr<clProjectile>& proj = (*iter);

			if(proj->cur_frame >= cur_frame)
				continue;

			proj->age += (REAL)((double)(cur_frame - proj->cur_frame) / _game_info.fps);

			switch(proj->type)
			{
			case BULLET_TYPE_ROCKET:
				{
					REAL dt = (REAL)((double)(cur_frame - proj->cur_frame) / _game_info.fps);

					proj->pos += proj->vel * dt;

					if(proj->age >= proj->start_display)
					{
						REAL yaw, pitch;
						proj->vel.normal_safe().resolveOriRH(yaw, pitch);

						_cg_root_node->addChild(proj->mn, false);
						proj->mn->setWorldTransform(Mat4::rotationX(pitch) * Mat4::rotationY(yaw) * Mat4::translation(proj->pos));
						proj->mn->setVisible(true);

						proj->fake_light_item = _cg.fake_light->create(_map_node, proj->pos, 20.0f, Vec4(1, 1, 1, 1));
					}
				}
				break;
			case BULLET_TYPE_GRENADE:
				{
					uint64_t num_frames = cur_frame - proj->cur_frame;
					REAL dt = (REAL)(1.0 / _game_info.fps);
					for(uint64_t i = 0; i < num_frames; ++i)
					{
						proj->vel += _game_info.GRAVITY * dt;

						Vec3 radius(1.5f, 1.5f, 1.5f);
						Vec3 disp = proj->vel * dt;
						Vec3 new_pos, new_vel, remaining_disp, collision_normal;
						bool touch_ground;
						SPtr<SNode> contact_node;

						// at server side, use _root_node, at client side use _map_node
						if(FPSCore::clipAir(AABB(proj->pos - radius, proj->pos + radius), disp, proj->vel, _map_node, 1.5f, 30.0f, new_pos, remaining_disp, new_vel, touch_ground, collision_normal, contact_node))
						{
							proj->pos = new_pos + collision_normal * 0.01f;
							proj->vel = proj->vel.reflect(collision_normal);
							proj->vel = (proj->vel * collision_normal) * collision_normal * 0.666667f + proj->vel.slideAlong(collision_normal) * 0.8f;
						}
						else
						{
							proj->pos += disp;
						}
					}
					dt *= num_frames;
					if(proj->age >= proj->start_display)
					{
						_cg_root_node->addChild(proj->mn, false);
						REAL r = max(0.0f, 1.0f - proj->age) * 10;
						proj->mn->setWorldTransform(proj->mn->world_rot().asMat4() * Mat4::rotationX(90 * dt * r) * Mat4::rotationY(90 * dt * r) * Mat4::rotationZ(90 * dt * r) * Mat4::translation(proj->pos));
						proj->mn->setVisible(true);
					}
				}
				break;
			}

			proj->cur_frame = cur_frame;
		}
	}
protected:
	bool							preview_nmpu(NetConn* conn, NetMsg* msg)
	{
		MsgReader reader(msg);
		switch(msg->opcode)
		{
		case MY_OP::S_PLAYERS_SNAP2:
			{
				if(true)
				{
					scoped_lock<mutex_fast> o(_mx_prediction);

					_core_counter_improve_prediction_accuracy.update();

					_next_snap_prediction.input(_core_counter_improve_prediction_accuracy.elapsedTimeDOUBLE());
				}
			}
			break;
		default:
			return false;
		}
		return true;
	}
	bool							nmpu(NetConn* conn, NetMsg* msg)
	{
		MsgReader reader(msg);
		switch(msg->opcode)
		{
		case MY_OP::S_CLIENT_INFO:
			{
				wcs map_dir;

				reader.read(_net.player_slot);
				reader.read(map_dir);

				loadMap(map_dir);

				MsgWriter writer(NetMsg::alloc(MY_OP::C_JOIN_GAME));
				writer.write(_player_name);
				_net.conn->send(writer.getMsg());
			}
			break;
		case MY_OP::S_JOIN_GAME:
			{
				bool ret = false;
				reader.read(ret);
				if(ret)
				{
					_game_info.clear();
					_game_info.valid = true;
					reader.read(_game_info.fps);
					reader.read(_game_info.snap_interval);
					_game_info.PLAYER_BB = AABB(reader.read<Vec3>(), reader.read<Vec3>());
					reader.read(_game_info.PLAYER_MAX_RUN_SPEED);
					reader.read(_game_info.GRAVITY);

					_next_snap_prediction.clear();
					_next_snap_prediction.setInterval(1.0 / 60 * _game_info.snap_interval);

					uint32_t slot = -1I32;

					reader.read(slot);

					_net.player_slot = slot;

					//Vec3 pos;
					//reader.read(pos);
				}
			}
			break;
		case MY_OP::S_REMOVE_PLAYER:
			{
				uint32_t slot;
				if(!reader.read(slot))
					break;
				if(_players.at(slot))
					_players.remove(slot);

				if(players_extra.at(slot))
					players_extra.remove(slot);
			}
			break;
		case MY_OP::S_PLAYERS_SNAP2:
			{
				// for accurate timing, some work is moved to preview nmpu, but still, we need to tackle _next_snap_prediction get clear() inside S_JOIN_GAME after preview

				if(true)
				{
					scoped_lock<mutex_fast> o(_mx_prediction);

					if(!_improve_prediction_accuracy || !_next_snap_prediction.any())
					{
						_core_counter.update();

						_next_snap_prediction.input(_core_counter.elapsedTimeDOUBLE());
					}
				}

				double prediction = _next_snap_prediction.predict();

				uint64_t f = 0;
				reader.read(f);

				_lpf.frame = f;
				_lpf.at = prediction;
				_lpf.at += max(_next_snap_prediction.jitter(), 0.5 / _game_info.fps);				// make it more out-of-date to stablize client side simulation

				_lpp.frame = f + _game_info.snap_interval + (_buffered_input.num_frames_to_buffer - 1);
				_lpp.frame += 1;																	// one frame ahead to ensure input arrival before the server starts the processing of the frame
				_lpp.at = prediction;
				_lpp.at -= _net.conn->getPing();
				_lpp.at -= max(_next_snap_prediction.jitter(), 0.0625 / _game_info.fps);

				// stablize...
				if(true)
				{
					double frame_len = 1.0 / _game_info.fps;
					double desired = _lpf.at + frame_len * 0.5;
					while(_lpp.at < desired)
					{
						_lpp.at += frame_len;
						++_lpp.frame;
					}
					_lpp.at = desired;
				}

				// naive way to solve small lag...
				if(true)
				{
					double frame_len = 1.0 / _game_info.fps;

					size_t lag_frames_allow = (size_t)(1.0 / frame_len);			// allow 1 second go
					for(size_t i = 0; i < lag_frames_allow; ++i)
					{
						_lpf.at += frame_len;
						++_lpf.frame;
					}
				}

				uint32_t count = 0;
				reader.read(count);

				for(uint32_t i = 0; i < count; ++i)
				{
					uint32_t slot = -1I32;
					SNAP2 s;
					reader.read(slot);
					reader.read(&s, sizeof(s));

					updatePlayer(slot, f, s);
				}

				if(true)
				{
					SPtr<clPlayer> p = getClientPlayer();

					if(p)
					{
						if(p->sv_snaps.count() && (p->sv_snaps.getLatest().flags & PLAYER_FLAG_SPAWNED))
						{
							const SNAP2& s = p->sv_snaps.getLatest();
							p->cl_look.yaw = s.yaw;
							p->cl_look.pitch = s.pitch;
							p->cl_weapon = s.weapon;
						}

						const SNAP2* s0 = NULL;
						const SNAP2* s1 = NULL;
						REAL dt = 0.0f;
						if(p->sv_snaps.query(f, s0, s1, dt))
						{
							p->cl_prediction.f = f;
							p->cl_prediction.s = *s1;

							if(true)
								predictPlayer(p);
						}
					}
				}
			}
			break;
		case MY_OP::S_ADD_PROJECTILE:
			{
				uint32_t slot = 0;

				SPtr<clProjectile> proj = new clProjectile();
				reader.read(slot);
				reader.read(proj->cur_frame);
				reader.read(proj->owner_slot);
				reader.read((uint32_t&)proj->type);
				reader.read(proj->pos);
				reader.read(proj->vel);

				_projectiles.replace(slot, proj);

				proj->loadGfx();
				switch(proj->type)
				{
				case BULLET_TYPE_ROCKET:
				case BULLET_TYPE_GRENADE:
					{
						proj->start_display = 3.0f / proj->vel.length();			// 3.0, from bounding box to gun head
					}
					break;
				}

				if(true)
				{
					const SPtr<clPlayer>& p = _players.at(proj->owner_slot);
					if(p)
					{
						p->play_upper_anim(UPPER_ANIM_SHOOT);
					}
				}
			}
			break;
		case MY_OP::S_REMOVE_PROJECTILE:
			{
				uint32_t slot = 0;
				reader.read(slot);
				_projectiles.remove(slot);
			}
			break;
		case MY_OP::S_BULLET_HIT:
			{
				BULLET_TYPE type;
				Vec3 hit_pos;
				Vec3 face_normal;

				if(!reader.read((uint32_t&)type))
					break;
				if(!reader.read(hit_pos))
					break;
				if(!reader.read(face_normal))
					break;

				switch(type)
				{
				case BULLET_TYPE_ROCKET:
					{
						SPtr<cgBillBoardFX::Item> bullet_hit = new cgBillBoardFX::Item();
						bullet_hit->pos = hit_pos + face_normal * 3.0f;
						bullet_hit->color = Vec4(1, 1, 1, 1);
						bullet_hit->extent = Vec2(15, 15);
						bullet_hit->image = _cg.bb_rocket_explosion;
						bullet_hit->blend = cgBillBoardFX::Item::BLEND_ADDITIVE;
						bullet_hit->anim = 0.0f;

						_cg.bullet_hits.push_back(bullet_hit);

						_cg.bb->addBillBoard(bullet_hit);

						_cg.scorch_marks.push_back(new cgScorchMarkItem(_cg.decal_scorch_mark->create(_map_node, hit_pos, face_normal, 10.0f, Vec4(0, 0, 0, 1)), 5.0f));
					}
					break;
				case BULLET_TYPE_GRENADE:
					{
						SPtr<cgBillBoardFX::Item> bullet_hit = new cgBillBoardFX::Item();
						bullet_hit->pos = hit_pos + face_normal * 1.5f;
						bullet_hit->color = Vec4(1, 1, 1, 1);
						bullet_hit->extent = Vec2(15, 15);
						bullet_hit->image = _cg.bb_rocket_explosion;
						bullet_hit->blend = cgBillBoardFX::Item::BLEND_ADDITIVE;
						bullet_hit->anim = 0.0f;

						_cg.bullet_hits.push_back(bullet_hit);

						_cg.bb->addBillBoard(bullet_hit);

						_cg.scorch_marks.push_back(new cgScorchMarkItem(_cg.decal_scorch_mark->create(_map_node, hit_pos, face_normal, 10.0f, Vec4(0, 0, 0, 1)), 5.0f));
					}
					break;
				case BULLET_TYPE_SLUG:
					{
						SPtr<cgRailHitFX> railhit = new cgRailHitFX();
						railhit->load();
						if(true)
						{
							REAL yaw, pitch;
							face_normal.resolveOriRH(yaw, pitch);
							railhit->mn->setWorldTransform(Mat4::rotationX(pitch) * Mat4::rotationY(yaw) * Mat4::translation(hit_pos + face_normal * 0.0f));
						}
						_cg.railhits.push_back(railhit);

						_cg.scorch_marks.push_back(new cgScorchMarkItem(_cg.decal_scorch_mark->create(_map_node, hit_pos, face_normal, 3.0f, Vec4(0.0f, 1, 0, 1)), 5.0f));
					}
					break;
				}
			}
			break;
		case MY_OP::S_HIT_SCAN:
			{
				uint32_t slot;
				BULLET_TYPE bullet_type;
				Vec3 start;
				Vec3 end;

				if(!reader.read(slot))
					break;
				if(!reader.read((uint32_t&)bullet_type))
					break;
				if(!reader.read(start))
					break;
				if(!reader.read(end))
					break;

				switch(bullet_type)
				{
				case BULLET_TYPE_SLUG:
					{
						Vec3 dir = (end - start).normal_safe();
						Vec3 offset;
						if(true)
						{
							REAL yaw, pitch;
							dir.normal_safe().resolveOriRH(yaw, pitch);

							offset = Vec3(0.667f, -0.667f, 0) * Mat3::rotationX(pitch) * Mat3::rotationY(yaw);
						}

						_cg.slugs.push_back(_cg.slug->createBeam(start + dir * 3.0f + offset, end, Vec4(0, 1, 0, 1), 1));

						// need to change animation
					}
					break;
				}
			}
			break;
		case MY_OP::S_PLAYER_STATUS:
			{
				if(_net.player_slot != -1I32)
				{
					if(!players_extra.at(_net.player_slot))
						players_extra.replace(_net.player_slot, new PLAYER_EXTRA());
					reader.read(&players_extra.at(_net.player_slot)->status, sizeof(players_extra.at(_net.player_slot)->status));
				}
			}
			break;
		case MY_OP::S_PLAYERS_STATS:
			{
				uint32_t count;
				if(!reader.read(count))
					break;

				for(uint32_t i = 0; i < count; ++i)
				{
					uint32_t slot;
					if(!reader.read(slot))
						break;
					if(slot >= _players.capacity())
						break;

					if(!players_extra.at(slot))
						players_extra.replace(slot, new PLAYER_EXTRA);

					if(!reader.read(players_extra.at(slot)->name))
						break;

					if(!reader.read(&players_extra.at(slot)->stats, sizeof(players_extra.at(slot)->stats)))
						break;
				}
			}
			break;
		case MY_OP::S_DAMAGE_FEEDBACK:
			{
				SPtr<clPlayer> p = getClientPlayer();
				if(p)
				{
					reader.read(p->cg_damage_feedback.axis);
					reader.read(p->cg_damage_feedback.rate);

					p->cg_damage_feedback.cd.setCountDown(0.5f);
					p->cg_damage_feedback.cd.start();
				}
			}
			break;
		default:
			return false;
		}
		return true;
	}
public:
	void							updateMap(REAL dt)
	{
		if(!_map_node)
			return;
		SCtrlOps::update(CtrlUpdater(dt, false), _map_node);
	}
public:
	void							procRecvMsgs()
	{
		if(_net.conn)
			_net.conn->procRecvMsgs();
	}
public:
	void							sendClientInput(const SPtr<Input>& input)
	{
		SPtr<clPlayer> p = getClientPlayer();
		if(!p)
			return;

		_core_counter.update();

		uint64_t f = _lpp.frame + (uint64_t)((_core_counter.elapsedTimeDOUBLE() - _lpp.at) / (1.0 / _game_info.fps));
		if(_lpp.cur_frame != (uint64_t)-1I64 && f <= _lpp.cur_frame)
		{
			// missed input!!!
			//Z_ASSERT( false );
			return;
		}

		_lpp.cur_frame = f;

		clPlayer::CL_LOOK& cl_look = p->cl_look;

		if(true)
		{
			if(input->getMouseDeltaX())
				cl_look.yaw += -input->getMouseDeltaX() * 0.15f * 2;
			if(input->getMouseDeltaY())
				cl_look.pitch += -input->getMouseDeltaY() * 0.15f * 2;

			while(cl_look.yaw > 360.0f)
				cl_look.yaw -= 360.0f;
			while(cl_look.yaw < 0.0f)
				cl_look.yaw += 360.0f;

			cl_look.pitch = min(max(-89.5f, cl_look.pitch), 89.5f);

			if(input->getMouseDeltaZ() < 0)
				p->cl_weapon = WEAPON_TYPE_ROCKET_LAUNCHER;
			else if(input->getMouseDeltaZ() > 0)
				p->cl_weapon = WEAPON_TYPE_RAILGUN;
			else if(input->isButtonDown(2))
				p->cl_weapon = WEAPON_TYPE_GRENADE_LAUNCHER;
		}

		PLAYER_INPUT i;
		i.up = input->isKeyDown('W');
		i.down = input->isKeyDown('S');
		i.left = input->isKeyDown('A');
		i.right = input->isKeyDown('D');
		i.jump = input->isButtonDown(1) | input->isKeyDown(VK_SPACE);
		i.fire = input->isButtonDown(0);
		i.yaw = cl_look.yaw;
		i.pitch = cl_look.pitch;
		i.weapon = p->cl_weapon;

		p->cl_input_history.addInput(f, i);

		if(p->cl_input_history.getInput(f - 1) != i)
			_buffered_input.add(f);

		if(_buffered_input.count && (f - _buffered_input.frames[0] + 1) >= _buffered_input.num_frames_to_buffer)
		{
			MsgWriter writer(NetMsg::alloc(), MY_OP::C_INPUT);
			writer.write((uint32_t)_buffered_input.count);
			for(size_t i = 0; i < _buffered_input.count; ++i)
			{
				writer.write(_buffered_input.frames[i]);
				const PLAYER_INPUT& inp = p->cl_input_history.getInput(_buffered_input.frames[i]);
				writer.write(&inp, sizeof(inp));
			}
			_buffered_input.clear();

			_net.conn->send(writer.getMsg());
		}

		if(true)
			predictPlayer(p);
	}
public:
	void							flushNet()
	{
		if(_net.conn)
			_net.conn->flush();
	}
	SPtr<clPlayer>					getClientPlayer() const
	{
		if(_net.player_slot != -1I32 && _players.at(_net.player_slot) && _net.conn && true)
			return _players.at(_net.player_slot);
		return NULL;
	}
public:
	void							predictPlayer(const SPtr<clPlayer>& p)
	{
		if(!_lpp.valid())
			return;

		if(_lpp.cur_frame == -1I64)
			return;

		if(p->cl_prediction.f == -1I64)
			return;

		REAL dt = (REAL)(1.0 / _game_info.fps);

		for(uint64_t f = p->cl_prediction.f + 1; f <= _lpp.cur_frame; ++f)
		{
			const PLAYER_INPUT& cur = p->cl_input_history.getInput(f);
			const PLAYER_INPUT& last = p->cl_input_history.getInput(f - 1);

			predict_procPlayerInput(p, cur, last, dt);
			predict_updatePlayerMovement(p, dt);

			p->cl_prediction.f = f;
		}
	}
private:
	void							predict_procPlayerInput(const SPtr<clPlayer>& p, const PLAYER_INPUT& cur, const PLAYER_INPUT& last, REAL dt)
	{
		SNAP2& snap = p->cl_prediction.s;

		if(snap.flags & PLAYER_FLAG_DEAD)
		{
			if(snap.touch_ground)
			{
				Vec3 v = snap.vel;
				v.y = 0;
				REAL spd = v.length();
				if(spd < 0.01f)
				{
					snap.vel.x = snap.vel.z = 0;
				}
				else
				{
					spd = ATTAIN(spd, 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10);
					v = v.normal_safe() * spd;

					snap.vel.x = v.x;
					snap.vel.z = v.z;
				}
			}
			return;
		}

		snap.yaw = cur.yaw;
		snap.pitch = min(max(-89.5f, cur.pitch), 89.5f);

		if(true)
		{
			Vec3 dir;
			if(cur.up)
				dir.z -= 1.0f;
			if(cur.down)
				dir.z += 1.0f;
			if(cur.left)
				dir.x -= 1.0f;
			if(cur.right)
				dir.x += 1.0f;

			snap.tilt.x = ATTAIN(snap.tilt.x, -dir.x, dt * 4);
			snap.tilt.z = ATTAIN(snap.tilt.z, -dir.z, dt * 4);

			if(dir.length() > 0.1f)
			{
				dir.normalize();
				dir *= Mat3::rotationY(snap.yaw);
				dir.normalize();

				if(snap.touch_ground)
				{
					REAL spd;
					Vec3 diralong = (spd = snap.vel * dir) * dir;
					Vec3 remain = snap.vel - diralong;
					remain.y = 0;

					spd = ATTAIN(spd, _game_info.PLAYER_MAX_RUN_SPEED, _game_info.PLAYER_MAX_RUN_SPEED * 0.8f * dt * 10);
					if(remain.length() < 0.01f)
					{
						remain = Vec3::zero;
					}
					else
					{
						remain = remain.normal() * (ATTAIN(remain.length(), 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10));
					}

					Vec3 v = dir * spd + remain;
					snap.vel.x = v.x;
					snap.vel.z = v.z;
				}
				else
				{
					Vec3 acc = dir * _game_info.PLAYER_MAX_RUN_SPEED * dt * 10 * 0.1f;

					Vec3 hvel = snap.vel;
					hvel.y = 0;

					if(hvel * dir < _game_info.PLAYER_MAX_RUN_SPEED)
						hvel += acc;

					snap.vel.x = hvel.x;
					snap.vel.z = hvel.z;
				}
			}
			else
			{
				if(snap.touch_ground)
				{
					Vec3 v = snap.vel;
					v.y = 0;
					REAL spd = v.length();
					if(spd < 0.01f)
					{
						snap.vel.x = snap.vel.z = 0;
					}
					else
					{
						spd = ATTAIN(spd, 0, _game_info.PLAYER_MAX_RUN_SPEED * 0.4f * dt * 10);
						v = v.normal_safe() * spd;

						snap.vel.x = v.x;
						snap.vel.z = v.z;
					}
				}
			}
		}

		//// HELL, if using clPlayer::_pressed_jump will cause a very very hard to find bug, we need clPlayer::cl_input_history_pressed_jump too
		size_t pressed_jump = p->cl_input_history_pressed_jump.getInput(p->cl_prediction.f - 1);
		if(cur.jump && !last.jump)
			pressed_jump = true;
		if(snap.touch_ground && pressed_jump && cur.jump)
		{
			snap.vel.y = 90.0f;

			snap.touch_ground = 0;

			pressed_jump = false;
		}
		p->cl_input_history_pressed_jump.addInput(p->cl_prediction.f, pressed_jump);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(snap.touch_ground)
		{
			if(cur.up || cur.down || cur.left || cur.right)
				snap.no_move_time = 0;
			else
				snap.no_move_time += dt;
		}
	}
	void							predict_updatePlayerMovement(const SPtr<clPlayer>& p, REAL dt)
	{
		if(true)
		{
			fast_list<clPlayer>::iterator end_iter = _players.end();
			for(fast_list<clPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
			{
				const SPtr<clPlayer>& p2 = (*iter);
				if(p2 == p)
					continue;
				if(p2->gfx_snap.flags & PLAYER_FLAG_DEAD)
					continue;

				_root_node->addChild(p2->bound, false);
				p2->bound->setWorldTransform(Mat4::translation(p2->gfx_snap.pos));
			}
		}

		Vec3 box_offset = Vec3(0, _game_info.PLAYER_BB.extent()[1], 0);

		Vec3& pos = p->cl_prediction.s.pos;
		Vec3& vel = p->cl_prediction.s.vel;
		uint32_t& touch_ground = p->cl_prediction.s.touch_ground;

		vel += _game_info.GRAVITY * dt;
		if(vel.length() * dt >= 0.01f)
		{
			SPtr<SNode> contact_node;

			Vec3 disp = vel * dt;

			if(touch_ground || true)
			{
				AABB box(_game_info.PLAYER_BB.minimum() + pos + disp, _game_info.PLAYER_BB.maximum() + pos + disp);

				REAL unpen_dist = 0.0f;
				bool tg = false;
				bool clipped;
				Vec3 collision_axis;
				if(FPSCore::clipGround(box, _root_node, 1.625f + max(0.0f, -disp.y), 30.0f, clipped, unpen_dist, tg, collision_axis, contact_node))
				{
					if(clipped)
					{
						pos = pos + disp + (unpen_dist + 0.001f) * Vec3::yAxis;
						if(tg)
							vel.y = max(0.0f, vel.y);
						else
							vel = vel.slideAlong(collision_axis);

						touch_ground = tg;
					}
					else
					{
						pos += disp;
						touch_ground = false;
					}
				}
				else
				{
					touch_ground = false;

					Vec3 new_box_center, remaining_disp, new_vel, collision_normal;
					bool tg;

					for(size_t i = 0; i < 7; ++i)
					{
						AABB box(_game_info.PLAYER_BB.minimum() + pos, _game_info.PLAYER_BB.maximum() + pos);

						if(FPSCore::clipAir(box, disp, vel, _root_node, 0.01f, 30.0f, new_box_center, remaining_disp, new_vel, tg, collision_normal, contact_node))
						{
							pos = new_box_center - Vec3(0, _game_info.PLAYER_BB.extent()[1], 0) + collision_normal * 0.001f;
							vel = new_vel;
							touch_ground |= (uint32_t)tg;
							disp = remaining_disp;
							if(disp.length() < 0.01f)
								break;
						}
						else
						{
							pos += disp;
							break;
						}
					}
				}
			}
			else
			{
				touch_ground = false;

				Vec3 new_box_center, remaining_disp, new_vel, collision_normal;
				bool tg;

				for(size_t i = 0; i < 7; ++i)
				{
					AABB box(_game_info.PLAYER_BB.minimum() + pos, _game_info.PLAYER_BB.maximum() + pos);

					if(FPSCore::clipAir(box, disp, vel, _root_node, 0.01f, 30.0f, new_box_center, remaining_disp, new_vel, tg, collision_normal, contact_node))
					{
						pos = new_box_center - Vec3(0, _game_info.PLAYER_BB.extent()[1], 0) + collision_normal * 0.001f;
						vel = new_vel;
						touch_ground |= (uint32_t)tg;
						disp = remaining_disp;
						if(disp.length() < 0.01f)
							break;
					}
					else
					{
						pos += disp;
						break;
					}
				}
			}
		}

		if(true)
		{
			fast_list<clPlayer>::iterator end_iter = _players.end();
			for(fast_list<clPlayer>::iterator iter = _players.begin(); iter != end_iter; ++iter)
				(*iter)->bound->detachFromParent(false);
		}
	}
public:
	int64_t							getPredictionAhead() const
	{
		return _lpp.cur_frame - _lpf.cur_frame;
	}
};

void				clPlayer::syncGfx(uint64_t frame, REAL dt)
{
	if(SPtr<clPlayer>(this) != game.lock()->getClientPlayer())
	{
		const SNAP2* s0 = NULL;
		const SNAP2* s1 = NULL;
		REAL dt2 = 0.0f;
		if(!sv_snaps.query(frame, s0, s1, dt2))
			return;

		s0->lerp(*s1, dt2, gfx_snap);
	}
	else
	{
		gfx_snap = cl_prediction.s;
	}

	if(true)	// not dead
	{
		if(gfx_snap.weapon != gfx_weapon)
		{
			if(gfx_upper_anim != UPPER_ANIM_CHANGE_WEAPON)
				play_upper_anim(UPPER_ANIM_CHANGE_WEAPON);
		}
	}

	if(gfx_snap.flags & PLAYER_FLAG_SPAWNED)
	{
		resetAnim();
		resetWeapon(gfx_snap.weapon);
		cg_damage_feedback.reset();
	}

	update_lower_anim(gfx_snap);
	update_upper_anim(gfx_snap);

	gfx->setWorldTransform(Mat4::rotationY(gfx_snap.yaw) * Mat4::translation(gfx_snap.pos));

	CtrlUpdater ctrl_updater(dt, true);
	SCtrlOps::update(ctrl_updater, gfx);

	if(!(gfx_snap.flags & PLAYER_FLAG_DEAD))
	{
		Vec3 tilt = gfx_snap.tilt;

		SPtr<SNode> n;

		Vec3 forward = -Vec3(gfx->world_rot().asMat3()[2]).normal_safe();
		Vec3 right = Vec3(gfx->world_rot().asMat3()[0]).normal_safe();

		n = gfx->getLoadedNode(L"Bip01 Spine");
		n->setWorldRot(n->world_rot().asMat3() * Mat3::rotation(forward, -22.5f * tilt.x));

		n = gfx->getLoadedNode(L"Bip01 Spine1");
		n->setWorldRot(n->world_rot().asMat3() * Mat3::rotation(forward, 22.5f * tilt.x));

		n = gfx->getLoadedNode(L"Bip01 Spine");
		n->setWorldRot(n->world_rot().asMat3() * Mat3::rotation(right, -22.5f * tilt.z));

		n = gfx->getLoadedNode(L"Bip01 Spine1");
		n->setWorldRot(n->world_rot().asMat3() * Mat3::rotation(right, 22.5f * tilt.z));

		if(true)
		{
			REAL p = min(max(-75.0f, gfx_snap.pitch), 75.0f) / 3;

			n = gfx->getLoadedNode(L"Bip01 Spine1");
			n->setWorldRot( n->world_rot().asMat3() * Mat3::rotation(right, p));

			n = gfx->getLoadedNode(L"Bip01 Spine2");
			n->setWorldRot( n->world_rot().asMat3() * Mat3::rotation(right, p));

			n = gfx->getLoadedNode(L"Bip01 Spine3");
			n->setWorldRot( n->world_rot().asMat3() * Mat3::rotation(right, p));
		}
	}

	ctrl_updater.execDeferredUpdate();
}

Vec3 spring_damper(REAL kL, REAL kS, REAL kD, const Vec3& p0, const Vec3& p1, const Vec3& v0, const Vec3& v1)
{
	Vec3 dir = (p0 - p1).normal_safe();
	return ((kL - (p0 - p1).length()) * kS - ((v0 - v1) * dir) * kD) * dir;
}

REAL critical_damping(REAL kS)
{
	return 2 * SQRT(kS);
}

class MainMenu: public uiBase
{
private:
	SPtr<TexFont>			_font;
	SPtr<uiTextField>		_txt_name;
	SPtr<uiButton>			_btn_host;
	SPtr<uiTextField>		_txt_ip;
	SPtr<uiButton>			_btn_connect;
	SPtr<uiButton>			_btn_credits;
public:
	bool					doConnect;
	bool					doHost;
	bool					doCredits;
public:
	MainMenu()
	{
		doConnect = false;
		doHost = false;
		doCredits = false;

		_font = TexFont::load("", 32);
		_font->insertFace("pmingliu", 1);

		_txt_name = new uiTextField(uiMgr::current()->getHWND(), _font);
		_txt_name->setPos(75, 125);
		_txt_name->setExt(300, 40);
		_txt_name->setTextLimit(64);
		addChild(_txt_name);

		_btn_host = new uiButton();
		_btn_host->setPos(80, 420);
		_btn_host->setExt(240, 60);
		_btn_host->setText(L"Host");
		_btn_host->setFont(_font);
		_btn_host->loadImages(L"gfx/ui/common/test_up.png", L"gfx/ui/common/test_down.png", L"", L"");
		_btn_host->setStretchImage(true);
		addChild(_btn_host);

		_txt_ip = new uiTextField(uiMgr::current()->getHWND(), _font);
		_txt_ip->setPos(480, 340);
		_txt_ip->setExt(240, 60);
		_txt_ip->setTextLimit(64);
		addChild(_txt_ip);

		_btn_connect = new uiButton();
		_btn_connect->setPos(480, 420);
		_btn_connect->setExt(240, 60);
		_btn_connect->setText(L"Connect");
		_btn_connect->setFont(_font);
		_btn_connect->loadImages(L"gfx/ui/common/test_up.png", L"gfx/ui/common/test_down.png", L"", L"");
		_btn_connect->setStretchImage(true);
		addChild(_btn_connect);

		_btn_credits = new uiButton();
		_btn_credits->setPos(40, 520);
		_btn_credits->setExt(240, 60);
		_btn_credits->setText(L"View Credits");
		_btn_credits->setFont(_font);
		_btn_credits->loadImages(L"gfx/ui/common/test_up.png", L"gfx/ui/common/test_down.png", L"", L"");
		_btn_credits->setStretchImage(true);
		addChild(_btn_credits);

		_txt_name->setText(L"UnnamedPlayer");
		_txt_ip->setText(L"localhost");

		_btn_connect->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(SPtr<MainMenu>(this), &MainMenu::onConnectClick));
		_btn_host->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(SPtr<MainMenu>(this), &MainMenu::onHostClick));
		_btn_credits->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(SPtr<MainMenu>(this), &MainMenu::onCreditsClick));
	}
protected:
	~MainMenu() {}
protected:
	void					onConnectClick(const SPtr<uiButton>&)
	{
		doConnect = true;
	}
	void					onHostClick(const SPtr<uiButton>&)
	{
		doHost = true;
	}
	void					onCreditsClick(const SPtr<uiButton>&)
	{
		doCredits = true;
	}
public:
	mbs						getIP() const
	{
		return WCSTOMBS(_txt_ip->getText());
	}
	wcs						getPlayerName() const
	{
		return _txt_name->getText();
	}
protected:
	void					onDraw()
	{
		REAL dt = 1.0f / 60;
		static Vec3 pos;
		static Vec3 vel;

		static Vec3 old_anchor_pos = uiMgr::current()->getPointerPos() - getAbsPos();
		Vec3 anchor_pos = uiMgr::current()->getPointerPos() - getAbsPos();

		Vec3 f = spring_damper(10, 1000, critical_damping(1000), pos, anchor_pos, vel, (anchor_pos - old_anchor_pos) * (1.0f / dt));
		if((old_anchor_pos - anchor_pos).length() < 0.1f)
		{
			//f += spring_damper(20, 500, critical_damping(500), pos, anchor_pos + Vec3(0, 10, 0), vel, (anchor_pos - old_anchor_pos) * (1.0f / dt));
			f += spring_damper(SQRT(500), 500, critical_damping(500), pos, anchor_pos + Vec3(10, 10, 0), vel, (anchor_pos - old_anchor_pos) * (1.0f / dt));
			f += spring_damper(SQRT(500), 500, critical_damping(500), pos, anchor_pos + Vec3(-10, 10, 0), vel, (anchor_pos - old_anchor_pos) * (1.0f / dt));
		}
		vel += f * dt;
		pos += vel * dt;


		if((pos - anchor_pos).length() > 20)
		{
			pos = (pos - anchor_pos).normal_safe() * 20 + anchor_pos;
		}
		if(vel.length() > 40.0f)
		{
			vel = vel.normal_safe() * 40.0f;
		}

		old_anchor_pos = anchor_pos;

		draw_rect(0, 0, getDX(), getDY(), Vec4(0.667f, 0.667f, 0.667f, 1));
		draw_rect_outline(5, 5, getDX() - 10, getDY() - 10, Vec4(1, 1, 1, 1));

		draw_rect_outline(70, 120, 310, 50, Vec4(1, 1, 1, 1));

		draw_rect_outline(40, 240, 320, 250, Vec4(1, 1, 1, 1));
		draw_rect_outline(440, 240, 320, 250, Vec4(1, 1, 1, 1));

		draw_rect_outline(480, 340, 240, 60, Vec4(1, 1, 1, 1));

		if(false)
		{
			Vec2 p = uiMgr::current()->getPointerPos() - getAbsPos();
			_font->draw(FORMATW(L"%.0f, %.0f", p.x, p.y), Vec2(100, 0));
		}

		_font->begin().shadow().end(L"Your Name: ", 70, 80);

		_font->begin().ext(320, 40).shadow().center().middle().end(L"Host It!!!", 40, 240);
		_font->begin().ext(320, 40).shadow().center().middle().end(L"Connect!!!", 440, 240);

		_font->begin().shadow().end(L"Server IP: (Port: 5000)", 460, 290);

		draw_line(Vec2(anchor_pos.x, anchor_pos.y), Vec2(pos.x, pos.y), Vec4(0, 0, 0, 0));
	}
};

class Credits: public uiBase
{
private:
	SPtr<TexFont>			_font;
	SPtr<uiButton>			_btn_back;
public:
	bool					doBack;
public:
	Credits()
	{
		_font = TexFont::load("", 16);
		_font->insertFace("pmingliu", 1);

		_btn_back = new uiButton();
		_btn_back->setPos(40, 520);
		_btn_back->setExt(240, 60);
		_btn_back->setText(L"Back");
		_btn_back->setFont(TexFont::load("", 32));
		_btn_back->loadImages(L"gfx/ui/common/test_up.png", L"gfx/ui/common/test_down.png", L"", L"");
		_btn_back->setStretchImage(true);
		addChild(_btn_back);

		_btn_back->setOnButtonClick(wfunctor1<void, const SPtr<uiButton>&>(SPtr<Credits>(this), &Credits::onBackClick));

		doBack = false;
	}
protected:
	~Credits() {}
protected:
	void					onBackClick(const SPtr<uiButton>&)
	{
		doBack = true;
	}
protected:
	void					onDraw()
	{
		draw_rect(0, 0, getDX(), getDY(), Vec4(0.667f, 0.667f, 0.667f, 1));
		draw_rect_outline(5, 5, getDX() - 10, getDY() - 10, Vec4(1, 1, 1, 1));

		//draw_rect_outline(70, 120, 310, 50, Vec4(1, 1, 1, 1));

		//draw_rect_outline(40, 240, 320, 250, Vec4(1, 1, 1, 1));
		//draw_rect_outline(440, 240, 320, 250, Vec4(1, 1, 1, 1));

		//draw_rect_outline(480, 340, 240, 60, Vec4(1, 1, 1, 1));

		if(false)
		{
			Vec2 p = uiMgr::current()->getPointerPos() - getAbsPos();
			_font->draw(FORMATW(L"%.0f, %.0f", p.x, p.y), Vec2(100, 0));
		}

		_font->begin().shadow().end(L"Project Name:   The vc's FPS Project...", 60, 60);
		_font->begin().shadow().end(L"Author:   vc", 60, 80);
		_font->begin().shadow().end(L"Engine:   vc's z3D v5", 60, 100);
		_font->begin().shadow().end(L"Dependent Libraries: ", 60, 120);
		_font->begin().shadow().end(L"  zlib, minizip, crc, expat, tinyxml, FreeImage, FreeType, xyssl (only RSA), bullet physics (only btTriTree)", 60, 140);
		_font->begin().shadow().end(L"Texture Source:   Genetica (www.spiralgraphics.biz), St. Peter's Basilica", 60, 160);
		_font->begin().shadow().end(L"Map Design Origin:   Quake 3 Arena's RA3's RA3MAP4", 60, 180);
		_font->begin().shadow().end(L"Animation:   Quake 3's Hunter Biped by Paul Steed", 60, 200);
		_font->begin().shadow().end(L"vc's Contact:   cupid_vc@hotmail.com", 60, 240);

		//_font->begin().shadow().end(L"Your Name: ", 70, 80);

		//_font->begin().ext(320, 40).shadow().center().middle().end(L"Host It!!!", 40, 240);
		//_font->begin().ext(320, 40).shadow().center().middle().end(L"Connect!!!", 440, 240);

		//_font->begin().shadow().end(L"Server IP: ", 460, 290);
	}
};

class SceneViewer: public GameApp, public SNodeUserTextParser
{
private:
	SPtr<TexFont>		_sys_font;
private:
	QStats<25>			_tri_stats;
	FPSCounter			_fps_counter;
private:
	SPtr<SCameraNode>	_camera_node;
	SPtr<SLightNode>	_camera_light_node;
private:
	REAL				_cam_move_speed;
private:
	wcs					_last_load_path;
private:
	SPtr<phyMgr>		_phy_mgr;
private:
	SPtr<Oal>			_oal;
	//SPtr<SndPlayer>		_player;
	//SPtr<wavSndPlayerSet>	_player;
	//Player				_player;

	bool				_camera_follows_player;
private:
	SPtr<clGame>				_game;
private:
	size_t						_updated;
private:
	SPtr<Img2D>					_img_cross_hair;
	SPtr<Img2D>					_img_weapon_rl;
	SPtr<Img2D>					_img_weapon_gl;
	SPtr<Img2D>					_img_weapon_rg;
	SPtr<TexFont>				_font_big;
	SPtr<Img2D>					_img_screen_blood;
private:
	SPtr<uiMgr>					_ui_mgr;
	SPtr<MainMenu>				_main_menu;
	SPtr<Credits>				_credits;
private:						// threading game update is currently unusable
	size_t						_thread_game_update;
	mutex_fast					_mx_cl_critical;
	W32Thread					_game_update_thread;
	uint32_t					_main_thread_id;
public:
	SceneViewer()
	{
		_cam_move_speed = 100;

		_phy_mgr = new phyMgr();
		_phy_mgr->setGravity(Vec3(0, -10, 0));

		_camera_follows_player = false;

		_updated = false;

		_thread_game_update = false;

		_game_update_thread.setProc(functor0<void>(this, &SceneViewer::game_update_thread_proc));

		Z_STATIC_ASSERT( sizeof(_main_thread_id) == sizeof(GetCurrentThreadId()), WRONG_ASSUMPTION );
		_main_thread_id = (uint32_t)GetCurrentThreadId();
	}
protected:
	~SceneViewer()
	{
	}
	void				initFonts()
	{
		wchar_t buffer[4096];
		GetWindowsDirectory(buffer, _countof(buffer));
		wcs s = buffer;
		TexFont::addFontCache(s + L"\\fonts\\arial.ttf");
		TexFont::addFontCache(s + L"\\fonts\\mingliu.ttc");

		_sys_font = TexFont::load("arial", 12);
		_sys_font->insertFace("pmingliu", 1);

		_font_big = TexFont::load("arial", 32);
		_font_big->insertFace("pmingliu", 1);

		TexFont::removeFontCache(s);
	}
public:
	bool				post_create()
	{
		if(!_gfx->supportsShaderModel3())
		{
			MessageBox(NULL, L"The app only supports Shader Model 3.0", L"Error", 0);
			return false;
		}

		Tex2D::setForce16Bit(false);
		Tex2D::setMaxRes(2048);
		Tex2DGIF::setForce16Bit(false);
		Tex2DGIF::setMaxRes(1024);
		Tex2DAVI::setForce16Bit(false);
		Tex2DAVI::setMaxRes(1024);
		TexCube::setForce16Bit(false);
		TexCube::setMaxRes(1024);

		//_smgr->enableShader(false);
		_smgr->enableShadow(false);

		initFonts();

		_ui_mgr = new uiMgr();
		_ui_mgr->setHWND((intptr_t)this->getGameView());
		_ui_mgr->setRes(800, 600);
		_ui_mgr->setSyncDesktopMouse(true);

		_ui_mgr->getRoot()->addChild(_main_menu = new MainMenu());
		_main_menu->setExt(800, 600);
		_ui_mgr->getRoot()->addChild(_credits = new Credits());
		_credits->setExt(800, 600);
		_credits->setVisible(false);

		_camera_node = new SCameraNode();
		_camera_node->setPerspective(CONVERT_FOV(90.0f, 4.0f / 3));
		_camera_light_node = SLightNode::create_point(Vec3(1, 1, 1), 1.0f, 1000.0f, false);
		_camera_node->addChild(_camera_light_node, false);
		_smgr->root()->addChild(_camera_node, false);
		_smgr->setActiveCamera(_camera_node);

		_img_cross_hair = Img2D::load(L"textures/cross_hair.png");

		_img_weapon_rl = Img2D::load(L"gfx/rocket_launcher.png");
		_img_weapon_gl = Img2D::load(L"gfx/grenade_launcher.png");
		_img_weapon_rg = Img2D::load(L"gfx/railgun.png");
		_img_screen_blood = Img2D::load(L"textures/screen_blood.png");

		return true;
	}
protected:
	void			onGameLoopStart()
	{
		if(_thread_game_update)
			_game_update_thread.start();
	}
	bool			onGameLoopEnd()
	{
		_game_update_thread.stop(0, true);
		return true;
	}
private:
	void			game_update_thread_proc()
	{
		//AttachThreadInput(GetCurrentThreadId(), _main_thread_id, TRUE);
		while(!_game_update_thread.needsStop())
		{
			scoped_lock<mutex_fast> o(_mx_cl_critical);

			game_update(0.0f);
		}
	}
public:
	void			update(REAL dt)
	{
		if(!_thread_game_update)
			game_update(dt);
	}
	void			game_update(REAL dt)
	{
		if(_main_menu->isVisible() || _credits->isVisible())
		{
			setUpdateFPS(60);

			_input->setCenterMouse(false);
			_input->setClipMouse(false);

			_input->update();

			_ui_mgr->update(_input, dt);

			_updated = true;
		}
		if(_main_menu->isVisible())
		{
			if(_main_menu->doCredits)
			{
				_main_menu->doHost = false;
				_main_menu->doConnect = false;
				_main_menu->doCredits = false;

				_main_menu->setVisible(false);
				_credits->setVisible(true);
			}

			if(_main_menu->doHost || _main_menu->doConnect)
			{
				bool host = _main_menu->doHost;

				_main_menu->doHost = false;
				_main_menu->doConnect = false;
				_main_menu->doCredits = false;

				bool okay = false;
				do
				{
					if(host)
					{
						g_server = new Server();
						if(g_server->init())
							g_server->start(L"meshes/level2");
						else
						{
							g_server.reset();
							break;
						}
					}

					_game = new clGame(_smgr);

					_game->setPlayerName(_main_menu->getPlayerName());

					if(!_game->initEssentialGfx())
						break;

					if(!_game->initNet())
						break;

					if(!_game->connect(host ? "localhost" : _main_menu->getIP().c_str()))
						break;

					_camera_follows_player = true;
					_camera_node->setPerspective(CONVERT_FOV(120, 4.0f / 3));

					SetFocus((HWND)getGameView());
					_main_menu->setVisible(false);

					okay = true;
				}while(false);
				if(!okay)
				{
					if(g_server)
						g_server->stop();
					g_server.reset();
					_game.reset();
				}
			}
			return;
		}
		if(_credits->isVisible())
		{
			if(_credits->doBack)
			{
				_credits->doBack = false;

				_credits->setVisible(false);
				_main_menu->setVisible(true);
			}
			return;
		}
		setUpdateFPS(1000);

		_input->setCenterMouse(true);
		_input->setClipMouse(true);

		if(_game)
			_game->procRecvMsgs();

		//////////////////////////////////////////////////////////////////

		bool back_to_menu = false;

		if(_game)
		{
			if(_game->getConn() && _game->getConn()->dead())
			{
				back_to_menu = true;
			}
			if(_game->any_lpf_advance())
			{
				//////////////////////////////////
				dt = (REAL)(1.0 / _game->getGameInfo().fps);
				//////////////////////////////////

				_input->update();

				if((_input->isKeyDown(VK_MENU) || _input->isKeyDown(VK_LMENU) || _input->isKeyDown(VK_RMENU)) && _input->isKeyPressed(VK_RETURN))
					switchFullscreen(!fullscreen());

				if(_input->isKeyPressed(VK_F2))
				{
					back_to_menu = true;
				}
				if(_input->isKeyPressed(VK_TAB))
				{
					_camera_follows_player = !_camera_follows_player;
					if(_camera_follows_player)
						_camera_node->setPerspective(CONVERT_FOV(120, 4.0f / 3));
					else
						_camera_node->setPerspective(CONVERT_FOV(90, 4.0f / 3));
				}
				if(!_camera_follows_player)
				{
					if(_input->isKeyDown(VK_UP))
						_camera_node->pan(Vec3::zNegAxis * _cam_move_speed * dt);
					if(_input->isKeyDown(VK_DOWN))
						_camera_node->pan(Vec3::zAxis * _cam_move_speed * dt);
					if(_input->isKeyDown(VK_LEFT))
						_camera_node->pan(Vec3::xNegAxis * _cam_move_speed * dt);
					if(_input->isKeyDown(VK_RIGHT))
						_camera_node->pan(Vec3::xAxis * _cam_move_speed * dt);
					if(_input->getMouseDeltaX() || _input->getMouseDeltaY())
					{
						_camera_node->rotateYaw(-_input->getMouseDeltaX() * 0.15f * 2);
						_camera_node->setPitch(min(max(_camera_node->pitch() - _input->getMouseDeltaY() * 0.15f * 2, -89.5f), 89.5f));
					}
				}

				_game->sendClientInput(_input);

				_updated = true;
			}

			_game->flushNet();
		}

		if(back_to_menu)
		{
			_game.reset();
			if(g_server)
				g_server->stop();
			g_server.reset();

			_main_menu->setVisible(true);
		}

		/////////////////////////////////////////////////////////////////

		if(g_server)
			g_server->update();
	}
	void			draw(REAL dt)
	{
		if(!_thread_game_update)
		{
			if(!_updated)
				return;
		}

		if(true)
		{
			scoped_lock<mutex_fast> o(_mx_cl_critical);

			if(_camera_node)
			{
				_camera_node->setAspectRatio((REAL)_gfx->getFramebufferDX() / _gfx->getFramebufferDY());
			}

			if(!_thread_game_update)
			{
				_updated = false;
			}

			_gfx->resetAccumPolycount();

			_gfx->beginScene();
			_gfx->clear(true, true, false, Vec4(0, 0, 0, 1), _gfx->getZFarMapping(), 0);

			if(!_main_menu->isVisible())
			{
				if(_game)
				{
					_game->syncGfx();
					SPtr<clPlayer> p = _game->getClientPlayer();
					if(p)
					{
						if(_camera_follows_player && !(p->gfx_snap.flags & PLAYER_FLAG_DEAD))
							p->gfx->setVisible(false);
						else
							p->gfx->setVisible(true);

						if(_camera_follows_player)
						{
							p->cg_damage_feedback.cd.update();
							Mat4 pre_rot = Mat4::identity;
							if(p->cg_damage_feedback.cd.remaining() > 0.0f)
								pre_rot = Mat4::rotation(p->cg_damage_feedback.axis, (p->cg_damage_feedback.cd.remaining() / p->cg_damage_feedback.cd.getCountDown()) * p->cg_damage_feedback.rate * 22.5f);
							_camera_node->setWorldTransform(pre_rot * Mat4::rotationX(p->gfx_snap.pitch) * Mat4::rotationY(p->gfx_snap.yaw) * Mat4::translation(p->gfx_snap.pos + Vec3(0, _game->getGameInfo().PLAYER_BB.extent()[1] + 3.0f, 0)));
						}
					}
					_smgr->drawAll();
				}
			}

			if(_ui_mgr)
			{
				_ui_mgr->drawAll(dt);

				if(true)
				{
					rsView rs;
					rs.init(_gfx);
					_gfx->setViewState(rs);
				}

				if(!_input->getCenterMouse())
				{
					_ui_mgr->setTransforms(Mat4::identity);
					_img_cross_hair->draw((int32_t)_ui_mgr->getPointerPos().x - _img_cross_hair->dx() / 2, (int32_t)_ui_mgr->getPointerPos().y - _img_cross_hair->dy() / 2);
				}
			}

			//_gfx->endScene();

			//return;

			_gfx->setViewProjTransform2D();

			_sys_font->draw(FORMATW(L"fps: %d", _fps_counter.getFPS()), Vec2(0, 0));
			_sys_font->draw(FORMATW(L"polycount: %d", _gfx->getAccumPolycount()), Vec2(0, 20));

			if(_game && _game->getConn())
			{
				_sys_font->draw(FORMATW(L"ping: %.4f ms", _game->getConn()->getPing() * 1000), Vec2(0, 40));
				//_sys_font->draw(FORMATW(L"ping: %.4f ms (jitter: %.4f ms)", _game->getConn()->getPing() * 1000, _game->getJitter() * 1000), Vec2(0, 40));
				_sys_font->draw(FORMATW(L"prediction ahead: %I64d", _game->getPredictionAhead()), Vec2(0, 60));

				_sys_font->draw(L"Press and Hold F1 to show Key Help", Vec2(0, 580));
				if(_input->isKeyDown(VK_F1))
				{
					_sys_font->draw(L"Press TAB to toggle camera following", Vec2(0, 500));
					_sys_font->draw(L"up, down, left, right - camera move when not following", Vec2(0, 520));
					_sys_font->draw(L"Press F2 to quit to main menu", Vec2(0, 540));
					_sys_font->draw(L"Press Alt + F4 to quit to Windows", Vec2(0, 560));

					_sys_font->draw(L"W, S, A, D - movement", Vec2(685, 500));
					_sys_font->draw(L"mouse move - look", Vec2(685, 520));
					_sys_font->draw(L"left click - fire", Vec2(685, 540));
					_sys_font->draw(L"right click - jump", Vec2(685, 560));
					_sys_font->draw(L"wheel down, up, press - rl, rg, gl", Vec2(635, 580));
				}

				if(_game->isClientInGame())
				{
					if(true)
					{
						fast_list<PLAYER_EXTRA>::iterator end_iter = _game->players_extra.end();
						size_t c = 0;
						for(fast_list<PLAYER_EXTRA>::iterator iter = _game->players_extra.begin(); iter != end_iter; ++iter, ++c)
						{
							if((*iter)->getSlot() == _game->getClientPlayerSlot())
								uiBase::draw_rect_outline(650, 300 + 20 * (int32_t)c, 140, 20, Vec4(0, 0, 0, 1));
							_sys_font->begin().ext(0, 20).middle().end((*iter)->name, Vec2(660, 300 + (REAL)(20 * c)));
							_sys_font->begin().ext(0, 20).middle().end(FORMATW(L"%d", (*iter)->stats.score), Vec2(770, 300 + (REAL)(20 * c)));
						}
					}

					SPtr<PLAYER_EXTRA> p = _game->players_extra.at(_game->getClientPlayerSlot());
					if(p)
					{
						_font_big->begin().shadow().end(FORMATW(L"HP: %d", p->status.hp), Vec2(0, 100));
						_font_big->begin().shadow().end(FORMATW(L"Score: %d", p->stats.score), Vec2(0, 132));

						if(_camera_follows_player)
						{
							SPtr<clPlayer> p2 = _game->getClientPlayer();
							if(p2)
							{
								if(p2->cg_damage_feedback.cd.remaining() > 0.0f)
									_img_screen_blood->drawStretch(0, 0, _img_screen_blood->dx(), _img_screen_blood->dy(), 0, 0, 800, 600, Vec4(1, 1, 1, p2->cg_damage_feedback.rate * (p2->cg_damage_feedback.cd.remaining() / p2->cg_damage_feedback.cd.getCountDown())));
							}

							_img_cross_hair->draw(_gfx->getFramebufferDX() / 2 - _img_cross_hair->dx() / 2, _gfx->getFramebufferDY() / 2 - _img_cross_hair->dy() / 2);

							if(true)
							{
								SPtr<clPlayer> player = _game->getClientPlayer();
								if(player)
								{
									switch(player->gfx_snap.weapon)
									{
									case WEAPON_TYPE_ROCKET_LAUNCHER:
										_img_weapon_rl->draw(0, 200);
										break;
									case WEAPON_TYPE_GRENADE_LAUNCHER:
										_img_weapon_gl->draw(0, 200);
										break;
									case WEAPON_TYPE_RAILGUN:
										_img_weapon_rg->draw(0, 200);
										break;
									}
								}
							}
						}
						else
						{
							if(false)
							{
								Vec4 p = (Vec4)(_game->getClientPlayer()->gfx_snap.pos) + Vec4(0, _game->getGameInfo().PLAYER_BB.extent()[1] * 2, 0);
								p *= _camera_node->view_proj();
								if(p.valid() && FABS(p.w) > 0.00001f)
								{
									Vec3 p2 = (Vec4)(p.cartesian());
									if(p2.valid())
									{
										if(min(_gfx->getZNearMapping(), _gfx->getZFarMapping()) <= p2.z && p2.z <= max(_gfx->getZNearMapping(), _gfx->getZFarMapping()))
										{
											p2 *= (_gfx->getViewTransform() * _gfx->getUnadjustedProjectionTransform()).inverse();
											if(p2.valid())
											{
												rsShade rs(false);

												_sys_font->getRespectingShadeState(rs, false);

												rs.enableDepthTest(GfxEnum::CMPFUNC_LEQUAL);

												_sys_font->begin().shadow().center().bottom().end(L"UnnamedPlayer", Vec3((REAL)(ptrdiff_t)p2.x, (REAL)(ptrdiff_t)p2.y, p2.z), &rs);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		//Vec3 vel = _player._vel;
		//vel.y = 0;
		//_sys_font->draw(FORMATW(L"spd: %.2f",vel.length()),Vec2(0,40));

		//static Vec3 player_old_pos;

		//static Timer timer;

		//Vec3 delta = _player._node->world_pos() - player_old_pos;
		//delta.y = 0;

		//timer.update();

		//_sys_font->draw(FORMATW(L"real spd: %.2f",delta.length() / timer.elapsedTime()),Vec2(0,60));

		//player_old_pos = _player._node->world_pos();


		_gfx->endScene();
		_gfx->present();

		//W32Thread::sleep(30);

		//_gfx->setViewport(true);
		//_gfx->set2DClipRect(false);
	}
	void		parseUserText(const wcs& node_name, const SPtr<SNode>& node, const wcs& user_text)
	{
		Config* cfg = Config::fromWCS(user_text);
		if(cfg->exists(L"collision_shape"))
		{
			Z_ASSERT( !node->getPhyBody() );

			wcs shape_name = cfg->getWString(L"collision_shape");
			REAL mass = cfg->getFloat(L"mass");
			int32_t compound = cfg->getInt32(L"compound");

			if(shape_name == L"box")
			{
				if(compound)
				{
					SPtr<phyShape> shape = _phy_mgr->createCompoundWrappedBoxShape(node->local_bound().extent(), node->local_bound().center());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::identity, mass, false);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
				else
				{
					SPtr<phyShape> shape = _phy_mgr->createBoxShape(node->local_bound().extent());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"sphere")
			{
				Vec3 ext = node->local_bound().extent();
				SPtr<phyShape> shape = _phy_mgr->createSphereShape((ext[0] + ext[1] + ext[2]) / 3);
				SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(Vec3(node->local_bound().center())), mass, false);
				_phy_mgr->addBody(body);

				node->setPhyBody(body);
			}
			else if(shape_name == L"mesh")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					SPtr<phyShape> shape = _phy_mgr->createMeshShape(node.cast<SMeshNode>()->mesh());
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::identity, mass, true);
					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"convex_hull")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					Vec3 offset;
					REAL computed_mass;
					Mat3 inertia_tensor;
					SPtr<phyShape> shape = _phy_mgr->createConvexHullShape(node.cast<SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
			else if(shape_name == L"convex_decomp")
			{
				if(node->type_info()->kind_of(SMeshNode::type_info_static()))
				{
					Vec3 offset;
					REAL computed_mass;
					Mat3 inertia_tensor;
					SPtr<phyShape> shape = _phy_mgr->createDecompConvexHullShape(node.cast<SMeshNode>()->mesh(), offset, computed_mass, inertia_tensor);
					SPtr<phyBody> body = _phy_mgr->createBody(node, shape, Mat4::translation(offset), mass, mass / computed_mass * Vec3(inertia_tensor[0][0], inertia_tensor[1][1], inertia_tensor[2][2]));

					_phy_mgr->addBody(body);

					node->setPhyBody(body);
				}
			}
		}
		delete cfg;
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if(true)
	{
		class C {};
		typedef void (C::*M)();

		M kk;
		int s = sizeof(kk);
		int k = sizeof(k);
	}
	if(false)
	{
		wfunctor1<void, int> k, k2;
		k = k2;
		k == k2;
		k != k2;
		k < k2;
		k(0);
		k.reset();
		k.valid();
	}

	//Mat4 proj = Mat4::perspectiveFOV(90, 1, -0.1f, -1000.0f, 0, 1, -1);
	Mat4 proj = Mat4::ortho(-10, -10, 10, 10, -0.1f, -1000.0f, 0, 1);
	REAL znear = -(proj.a43 - 0 * proj.a44) / (proj.a33 - 0 * proj.a34);				// no oblique frustum, otherwise fail
	//REAL znear = Frustum(proj, 0, 1).getPlane(4).origin().z;

	Vec3 k = Vec4(1, 1, -10) * proj;
	Mat4 inv = proj.inverse();
	Vec3 p0 = Vec3(k.x, k.y, 0) * inv;										// assume D3D
	Vec3 p1 = Vec3(k.x, k.y, 0.5f) * inv;									// assume D3D
	Vec3 d = p1 - p0;
	Vec3 o = p0 - d / d.z * znear;
	Vec3 v = o + d / d.z * -10;

	init_symbols();

	if(true)
	{
		Vec3 p(-1, 0, 0);

		Vec3 cam_pos(0, 0, 0);
		Vec3 cam_up(0, 1, 0);
		Vec3 cam_right(1, 0, 0);

		Vec3 right = cam_right;
		Vec3 up = cam_up;
		Vec3 vp = cam_pos - p;
		if(vp.lengthsq() > 0.00001f)
		{
			Vec3 vp_normal = vp.normal();
			REAL dp =  vp_normal * cam_up;
			if(FABS(dp) < 1 - 0.00001f)
				right = (cam_up ^ vp_normal).normal();
			up = (vp_normal ^ right).normal();
		}

		int kk = sizeof(kk);
	}

	if(true)
	{
		REAL v[4][4] = {{0, 0, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {3, 2, 1, 0}};
		Mat4 m = Mat4(v).inverse();

		int k = sizeof(k);
	}

	if(false)
	{
		Vec3 E0(0, 0, 0.5f);
		Vec3 E1(0, 0.25f, 0);
		Vec2 e0(0.1f, 0);
		Vec2 e1(0, 0.05f);
		Mat2 m(e0, e1);
		m = m.inverse();
		Vec3 T = m[0][0] * E0 + m[0][1] * E1;
		Vec3 B = m[1][0] * E0 + m[1][1] * E1;
		Vec3 N = (T ^ B).normal();
		Mat3 t2o(T, B, N, false);
		//t2o = t2o.inverse();

		Vec3 k = Vec3(0.707f, 0.707f, 0) * t2o.inverse().transpose();
		int kk = sizeof(kk);
	}
	else
	{
		Vec3 E0(0.5f, 0, 0);
		Vec3 E1(0, 0.5f, 0);
		Vec2 e0(-0.1f, 0);
		Vec2 e1(0, 0.1f);

		Mat3 t2o = (Mat3(E0.normal(), E1.normal(), (E0^E1).normal()).inverse() * Mat3((Vec3)e0.normal(), (Vec3)e1.normal(), Vec3(0, 0, 1))).transpose();
		Vec3 k = Vec3(0, 0, 1) * t2o.inverse().transpose();
		int kk = sizeof(kk);
	}
#ifdef _DEBUG
	//_CrtSetDbgFlag(_crtDbgFlag | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetDbgFlag(_crtDbgFlag | _CRTDBG_LEAK_CHECK_DF);
#endif

	NetSys::startup();
	//NetSys::createPools(32768, 32768, 4096);		// since its no mmo server, the following setting is okay
	NetSys::createPools(4096, 4096, 4096);
	NetSys::rsa_set_example_private_key();

	FileSystem::insertFileSystem(ZipFileSystem::create(L"pack.zip"));

	SPtr<SceneViewer> app = new SceneViewer();
	if(!app->create(L"The FPS", false, 800, 600, 24, false))
	{
		app.reset();
		return 0;
	}

	app->setUpdateFPS(1000);
	app->setCapDrawFPS(false);
	app->setSleep(true, 0);
	app->gameLoop();

	app.reset();

	if(g_server)
	{
		g_server->stop();
		g_server.reset();
	}

	AVIRenderer::__shutdown();

	return 0;
}

