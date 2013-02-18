#pragma once
#ifndef GWEN_CONTROLLIST_H
#define GWEN_CONTROLLIST_H


namespace Gwen
{
	struct Point;
	class TextObject;

	namespace Controls
	{
		class Base;
	}

	namespace Event
	{
		class Handler;
		struct Information;
		struct Packet;

		typedef const Gwen::Event::Information & Info;
	}

	template < typename TYPE >
	class TEasyList
	{
		public:

			typedef std::list<TYPE> List;

			void Add( TYPE pControl )
			{
				if ( Contains( pControl ) ) { return; }

				list.push_back( pControl );
			}

			void Remove( TYPE pControl )
			{
				list.remove( pControl );
			}

			void Add( const List & list )
			{
				for ( typename List::const_iterator it = list.begin(); it != list.end(); ++it )
				{
					Add( *it );
				}
			}

			void Add( const TEasyList<TYPE> & list )
			{
				Add( list.list );
			}

			bool Contains( TYPE pControl ) const
			{
				typename List::const_iterator it = std::find( list.begin(), list.end(), pControl );
				return it != list.end();
			}

			inline void Clear()
			{
				list.clear();
			}

			List list;
	};

	class ControlList : public TEasyList<Gwen::Controls::Base*>
	{
		public:

			void Enable();
			void Disable();

			void Show();
			void Hide();

			Gwen::TextObject GetValue();
			void SetValue( const Gwen::TextObject & value );

			template <typename T>
			void SetAction( Gwen::Event::Handler* ob,
							void ( T::*f )( Gwen::Event::Info ),
							const Gwen::Event::Packet & packet )
			{
				SetActionInternal( ob,
								   static_cast<void ( Gwen::Event::Handler::* )( Gwen::Event::Info ) > ( f ),
								   packet );
			}

			void MoveBy( const Gwen::Point & point );

			void DoAction();

		protected:

			void SetActionInternal( Gwen::Event::Handler* pObject,
									void ( Gwen::Event::Handler::*f )( Gwen::Event::Info ),
									const Gwen::Event::Packet & packet );
	};

};

#endif
