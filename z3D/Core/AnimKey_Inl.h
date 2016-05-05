
#include "AnimKey.h"

namespace z3D
{
	namespace Core
	{
		template<class T>
		AnimKey<T>::AnimKey() {}

		template<class T>
		AnimKey<T>::AnimKey(REAL t, const T& k): time(t), key(k) {}

		template<class T>
		T				AnimKey<T>::nearest(const AnimKey<T>& other, REAL now) const
		{
			if(time == other.time)
				return this->key;
			REAL t = (now - time) / (other.time - time);
			return (t < 0.5f) ? key : other.key;
		}

		template<class T>
		T				AnimKey<T>::step(const AnimKey<T>& other, REAL now) const
		{
			return key;
		}

		template<class T>
		AnimData<T>::AnimData()
			: ResBase(L"")
		{
			_const.keys = NULL;
			_const.keys_tangent = NULL;
			_const.key_count = 0;
		}

		template<class T>
		AnimData<T>::AnimData(const wcs& name, const SPtr<RefBase>& const_opaque, const AnimKey<T>* const_keys, const uint8_t* const_keys_tangent, size_t const_key_count)
			: ResBase(name)
		{
			_const.opaque = const_opaque;
			_const.keys = const_keys;
			_const.keys_tangent = const_keys_tangent;
			_const.key_count = const_key_count;
		}

		template<class T>
		AnimData<T>::~AnimData()
		{}

		template<class T>
		void			AnimData<T>::nearestPair(REAL time, const AnimKey<T>*& key0, const AnimKey<T>*& key1) const
		{
			Z_ASSERT(getKeyCount() && "at least 1 AnimKey in AnimData");

			const AnimKey<T>* data = getKeys();
			size_t key_count = getKeyCount();

			if(time < data->time)
			{
				key0 = data;
				key1 = data;
				return;
			}
			else
			{
				const AnimKey<T>* last = &data[key_count - 1];
				if(time >= last->time)
				{
					key0 = last;
					key1 = last;
					return;
				}
			}
			if(key_count == 1)
			{
				key0 = data;
				key1 = data;
				return;
			}
			size_t s = 0, e = key_count - 1;
			size_t m;
			do
			{
				const AnimKey<T>* tmp = data + (m = ((s + e) >> 1));
				if(time < tmp->time)
					e = m;
				else
				{
					const AnimKey<T>* tmp1 = data + (m + 1);
					if(time >= tmp1->time)
						s = m + 1;
					else
					{
						key0 = tmp;
						key1 = tmp1;
						return;
					}
				}
			}while(s != e);
			Z_ASSERT( false && "key data arranged out of order" );
		}

		template<class T>
		void			AnimData<T>::nearestPair(REAL time, AnimKey<T>& key0, AnimKey<T>& key1) const
		{
			const AnimKey<T>* k0 = NULL;
			const AnimKey<T>* k1 = NULL;

			nearestPair(time, k0, k1);
			key0 = *k0;
			key1 = *k1;
		}

		template<class T>
		T				AnimData<T>::eval_lerp(REAL time) const
		{
			const AnimKey<T>* key0;
			const AnimKey<T>* key1;
			nearestPair(time, key0, key1);
			return key0->lerp(*key1, time);
		}

		template<class T>
		T				AnimData<T>::eval_slerp(REAL time) const
		{
			const AnimKey<T>* key0;
			const AnimKey<T>* key1;
			nearestPair(time, key0, key1);
			return key0->slerp(*key1, time);
		}

		template<class T>
		T				AnimData<T>::eval_nearest(REAL time) const
		{
			const AnimKey<T>* key0;
			const AnimKey<T>* key1;
			nearestPair(time, key0, key1);
			return key0->nearest(*key1, time);
		}

		template<class T>
		T				AnimData<T>::eval_step(REAL time) const
		{
			const AnimKey<T>* key0;
			const AnimKey<T>* key1;
			nearestPair(time, key0, key1);
			return key0->step(*key1, time);
		}

		template<class T>
		T				AnimData<T>::sample(REAL time) const
		{
			const AnimKey<T>* key0;
			const AnimKey<T>* key1;
			nearestPair(time, key0, key1);
			size_t idx = locateKey(key0);
			return key0->sample(key1, time, getKeyTangent(idx));
		}

		template<class T>
		void			AnimData<T>::addKey(REAL time, const T& key)
		{
			addKey(time, key, false);
		}

		template<class T>
		void			AnimData<T>::addKey(REAL time, const T& key, bool step)
		{
			Z_ASSERT( !const_use() );

			_keys.push_back(AnimKey<T>(time, key));
			if(((_keys.size() + 7) >> 3) > _keys_tangent.size())
				_keys_tangent.push_back(0);
			_keys_tangent[_keys.size() >> 3] &= (uint8_t)~(((uint8_t)(!step)) << (_keys.size() & 7));
		}

		template<class T>
		void			AnimData<T>::setKey(size_t idx, REAL time, const T& key)
		{
			setKey(idx, time, key, step);
		}

		template<class T>
		void			AnimData<T>::setKey(size_t idx, REAL time, const T& key, bool step)
		{
			Z_ASSERT( !const_use() );

			_keys[idx] = AnimKey<T>(time, key);
			_keys_tangent[idx >> 3] &= (uint8_t)~(((uint8_t)(!step)) << (idx.size() & 7));
		}

		template<class T>
		void			AnimData<T>::clear()
		{
			Z_ASSERT( !const_use() );

			_keys.clear();
			_keys_tangent.clear();
		}

		template<class T>
		size_t			AnimData<T>::locateKey(const AnimKey<T>* key) const
		{
			return key - &getKey(0);
		}
		template<class T>
		const AnimKey<T>&			AnimData<T>::getKey(size_t idx) const
		{
			if(const_use())
				return _const.keys[idx];
			return _keys[idx];
		}

		template<class T>
		const AnimKey<T>*			AnimData<T>::getKeys() const
		{
			if(const_use())
				return _const.keys;
			return VECTOR_BEGIN_PTR(_keys);
		}

		template<class T>
		const uint8_t*						AnimData<T>::getKeysTangent() const
		{
			if(const_use())
				return _const.keys_tangent;
			return VECTOR_BEGIN_PTR(_keys_tangent);
		}

		template<class T>
		uint8_t								AnimData<T>::getKeyTangent(size_t idx) const
		{
			return getKeysTangent() ? (uint8_t)((getKeysTangent()[idx >> 3] >> (idx & 7)) & 1) : 0;
		}

		template<class T>
		size_t								AnimData<T>::getKeyCount() const
		{
			if(const_use())
				return _const.key_count;
			return _keys.size();
		}

		template<class T>
		bool								AnimData<T>::const_use() const
		{
			return !!_const.keys;
		}

		template<class T>
		AnimDataEval<T>::AnimDataEval()
		{
			_cached_key0.time = -INFINITY;
			_cached_key1.time = -INFINITY;
			_tangent_type = 0;
		}

		template<class T>
		AnimDataEval<T>::AnimDataEval(const SPtr< AnimData<T> >& anim_data)
		{
			_anim_data = anim_data;

			_cached_key0.time = -INFINITY;
			_cached_key1.time = -INFINITY;
			_tangent_type = 0;
		}

		template<class T>
		AnimDataEval<T>::AnimDataEval(const AnimDataEval& right)
		{
			_anim_data = right._anim_data;

			_cached_key0.time = -INFINITY;
			_cached_key1.time = -INFINITY;
			_tangent_type = 0;
		}

		template<class T>
		AnimDataEval<T>&				AnimDataEval<T>::operator=(const AnimDataEval<T>& right)
		{
			this->operator =(right._anim_data);
			return *this;
		}

		template<class T>
		AnimDataEval<T>&				AnimDataEval<T>::operator=(const SPtr< AnimData<T> >& anim_data)
		{
			_anim_data = anim_data;
			_cached_key0.time = -INFINITY;
			_cached_key1.time = INFINITY;
			_tangent_type = 0;

			return *this;
		}


		template<class T>
		const SPtr< AnimData<T> >&		AnimDataEval<T>::anim_data() const
		{
			return _anim_data;
		}

		template<class T>
		T								AnimDataEval<T>::eval_lerp(REAL time)
		{
			if(_cached_key0.time == -INFINITY || _cached_key1.time == -INFINITY || !(time >= _cached_key0.time && time < _cached_key1.time))
				_anim_data->nearestPair(time, _cached_key0, _cached_key1);
			return _cached_key0.lerp(_cached_key1, time);
		}

		template<class T>
		T								AnimDataEval<T>::eval_nearest(REAL time)
		{
			if(_cached_key0.time == -INFINITY || _cached_key1.time == -INFINITY || !(time >= _cached_key0.time && time < _cached_key1.time))
				_anim_data->nearestPair(time, _cached_key0, _cached_key1);
			return _cached_key0.nearest(_cached_key1, time);
		}

		template<class T>
		T								AnimDataEval<T>::eval_step(REAL time)
		{
			if(_cached_key0.time == -INFINITY || _cached_key1.time == -INFINITY || !(time >= _cached_key0.time && time < _cached_key1.time))
				_anim_data->nearestPair(time, _cached_key0, _cached_key1);
			return _cached_key0.step(_cached_key1, time);
		}

		template<class T>
		T								AnimDataEval<T>::sample(REAL time)
		{
			if(_cached_key0.time == -INFINITY || _cached_key1.time == -INFINITY || !(time >= _cached_key0.time && time < _cached_key1.time))
			{
				const AnimKey<T>* key0;
				const AnimKey<T>* key1;
				_anim_data->nearestPair(time, key0, key1);

				size_t idx = _anim_data->locateKey(key0);
				_tangent_type = _anim_data->getKeyTangent(idx);

				_cached_key0 = *key0;
				_cached_key1 = *key1;
			}
			if(_tangent_type)
				return _cached_key0.step(_cached_key1, time);
			return _cached_key0.lerp(_cached_key1, time);
		}
	};
};

