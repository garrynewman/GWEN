#pragma once
#ifndef GWEN_USERDATA_H
#define GWEN_USERDATA_H

namespace Gwen
{
	/*

		Allows you to store multiple and various user data

		//
		// Valid
		//
		UserDataStorage.Set( 100.0f );
		UserDataStorage.<float>Get();

		//
		// Invalid - didn't Add a float type!
		//
		UserDataStorage.Set( 100 );
		UserDataStorage.<float>Get();

		//
		// You can pass structs as long as they can be copied safely.
		//
		UserDataStorage.Set( mystruct );
		UserDataStorage.<MyStruct>Get();

		//
		// If you pass a pointer then whatever it's pointing to
		// should be valid for the duration. And it won't be freed.
		//
		UserDataStorage.Set( &mystruct );
		UserDataStorage.<MyStruct*>Get();

	*/
	class UserDataStorage
	{
			struct ValueBase
			{
				virtual ~ValueBase() {}
				virtual void DeleteThis() = 0;
			};

			template<typename T> struct Value : public ValueBase
			{
				T val;

				Value( const T & v )
				{
					val = v;
				}

				virtual void DeleteThis()
				{
					delete this;
				}
			};

		public:

			UserDataStorage()
			{
			}

			~UserDataStorage()
			{
				std::map< Gwen::String, void*>::iterator it = m_List.begin();
				std::map< Gwen::String, void*>::iterator itEnd = m_List.end();

				while ( it != itEnd )
				{
					( ( ValueBase* ) it->second )->DeleteThis();
					++it;
				}
			}

			template<typename T>
			void Set( const Gwen::String & str, const T & var )
			{
				Value<T>* val = NULL;
				std::map< Gwen::String, void*>::iterator it = m_List.find( str );

				if ( it != m_List.end() )
				{
					( ( Value<T>* ) it->second )->val = var;
				}
				else
				{
					val = new Value<T> ( var );
					m_List[ str ] = ( void* ) val;
				}
			};

			bool Exists( const Gwen::String & str )
			{
				return m_List.find( str ) != m_List.end();
			};

			template <typename T>
			T & Get( const Gwen::String & str )
			{
				Value<T>* v = ( Value<T>* ) m_List[ str ];
				return v->val;
			}

			std::map< Gwen::String, void*>	m_List;
	};

};

#endif
