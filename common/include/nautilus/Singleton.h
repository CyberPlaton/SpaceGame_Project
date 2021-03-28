#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H

namespace nautilus {

	namespace core {

		template <class T>
		class TSingleton
		{
		protected:
			static T* m_pSingleton; // static oebjct
		public:

			/*
			Constr & Destr
			*/
			virtual ~TSingleton() {}

			/*
			Getter
			*/
			inline static T* Get()
			{
#ifdef _DEBUG
				if (!m_pSingleton)
				{
					m_pSingleton = new T;
				}
				return (m_pSingleton);
#else
				return(m_pSingleton = (m_pSingleton == NULL) ? new T : m_pSingleton);
#endif // _DEBUG
			}

			/*
			Delete
			*/
			static void Del()
			{
				if (m_pSingleton)
				{
					delete(m_pSingleton);
					m_pSingleton = 0;
				}
			}

		};

		template <class T>
		T* TSingleton<T>::m_pSingleton = 0;

	}

}

#endif // !SINGLETON_H