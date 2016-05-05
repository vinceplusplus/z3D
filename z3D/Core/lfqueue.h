
#pragma once

#include "predecl.h"

#include "noncopyable.h"

namespace z3D
{
	namespace Core
	{
		// modification of GPG 1.1
		// T can only be any primitive type whose assignment copy is only one instruction, otherwise, around the copy assignment needs to be enclosed with a global mutex
		template<class T>
		class lfqueue: noncopyable
		{
		private:
			class node
			{
			public:
				node*volatile		next;
				T					value;
			};
		private:
			mem_pool<node>*					_pool;
		private:
			struct Z_ALIGN(8)
			{
				node*volatile					_head;
				volatile uint32_t				_pops;					// version no. to deal with ABA, must be immediately after _head
			};
			struct Z_ALIGN(8)
			{
				node*volatile					_tail;
				volatile uint32_t				_pushes;				// version no. to deal with ABA, must be immediately after _tail
			};
		private:
			volatile uint32_t				_size;
		public:
			lfqueue();
			~lfqueue();
		public:
			bool							create_pool(uint32_t size);					// not thread-safe
		public:
			bool							enq(const T& value);
			bool							deq(T& value);
		public:
			bool							empty() const;								// premature check of empty
			const T&						peep() const;								// if the scenario is that there maybe many threads which enqueue, but only one thread will dequeue, then the thread which will dequeue is safe to call this method (preceded by calling empty() to check first)
			uint32_t						size() const;								// the returned size may not be true
		public:
			void							clear();
		};
	};
};
