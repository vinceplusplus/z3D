
#pragma once

#include "predecl.h"
#include "ResBase.h"
#include "Constants.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace z3D
{
	namespace Core
	{
		template< class T >
		class AnimKey
		{
		public:
			REAL		time;
			T			key;
		public:
						AnimKey();
						AnimKey(REAL t, const T& k);
		public:			// default interpolation, for Vec3 or the like it would be lerp(), for Quat it would be slerp()
			T			lerp(const AnimKey<T>& other, REAL now) const;
		public:
			T			nearest(const AnimKey<T>& other, REAL now) const;
		public:
			T			step(const AnimKey<T>& other, REAL now) const;
		};

		template< class T >
		class AnimData: public ResBase
		{
		private:
			vector< AnimKey<T> >		_keys;
			vector<uint8_t>				_keys_tangent;
		private:
			struct _CONST
			{
				SPtr<RefBase>				opaque;							// provider of the constant data
				const AnimKey<T>*			keys;
				const uint8_t*				keys_tangent;					// NULL pointer -> implicit linear, 0 - linear, 1 - step
				size_t						key_count;
			}							_const;
		public:
										AnimData();
										AnimData(const wcs& name, const SPtr<RefBase>& const_opaque, const AnimKey<T>* const_keys, const uint8_t* const_keys_tangent, size_t const_key_count);
		protected:
			virtual						~AnimData();
		public:
			void						nearestPair(REAL time, const AnimKey<T>*& key0, const AnimKey<T>*& key1) const;
			void						nearestPair(REAL time, AnimKey<T>& key0, AnimKey<T>& key1) const;
		public:
			T							eval_lerp(REAL time) const;
			T							eval_slerp(REAL time) const;
			T							eval_nearest(REAL time) const;
			T							eval_step(REAL time) const;
		public:
			T							sample(REAL time) const;
		public:
			void						addKey(REAL time, const T& key);
			void						addKey(REAL time, const T& key, bool step);
			void						setKey(size_t idx, REAL time, const T& key);
			void						setKey(size_t idx, REAL time, const T& key, bool step);
			void						clear();
		public:
			size_t						locateKey(const AnimKey<T>* key) const;
		public:
			const AnimKey<T>&			getKey(size_t idx) const;
			const AnimKey<T>*			getKeys() const;
			const uint8_t*				getKeysTangent() const;
			uint8_t						getKeyTangent(size_t idx) const;				// 0 - linear, 1 - step
		public:
			size_t						getKeyCount() const;
		public:
			bool						const_use() const;
		};

		template< class T >
		class AnimDataEval
		{
		private:
			SPtr< AnimData<T> >		_anim_data;
			AnimKey<T>				_cached_key0;
			AnimKey<T>				_cached_key1;
			size_t					_tangent_type;
		public:
			AnimDataEval();
			AnimDataEval(const SPtr< AnimData<T> >& anim_data);
			AnimDataEval(const AnimDataEval& right);
		public:
			AnimDataEval&			operator=(const AnimDataEval& right);
			AnimDataEval&			operator=(const SPtr< AnimData<T> >& anim_data);
		public:
			const SPtr< AnimData<T> >&		anim_data() const;
		public:
			T						eval_lerp(REAL time);
			T						eval_nearest(REAL time);
			T						eval_step(REAL time);
		public:
			T						sample(REAL time);
		};
	};
};

#include "AnimKey_Inl.h"

