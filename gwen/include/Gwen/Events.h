/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#pragma once
#ifndef GWEN_EVENTS_H
#define GWEN_EVENTS_H

#include <list>
#include "Gwen/Exports.h"
#include "Gwen/Structures.h"
#include "Gwen/TextObject.h"
#include "Gwen/ControlList.h"

namespace Gwen
{

	namespace Controls
	{
		class Base;
	}

	namespace Event
	{
		/*

			When adding an event hook you can add a Packet too
			This will be passed in Event::Info when you receive an event

		*/
		struct Packet
		{
			Packet( Gwen::Controls::Base* pControl = NULL ) { Control = pControl; }

			Gwen::Controls::Base*	Control;
			Gwen::String			String;
			Gwen::UnicodeString		UnicodeString;
			int						Integer;
			float					Float;
			unsigned long long		UnsignedLongLong;
		};

		/*

			Passed to an event hook

		*/
		struct Information
		{
			Information() { Init(); }
			Information( Gwen::Controls::Base* pctrl ) { Init(); Control = pctrl; }

			void Init()
			{
				ControlCaller	= NULL;
				Packet			= NULL;
				Control			= NULL;
				Integer			= 0;
			}

			// This is set by the event caller, it will always be
			// the control which is calling the event.
			Gwen::Controls::Base*	ControlCaller;

			// This is set by the event hook
			// ie onDoSomething.Add( this, &ThisClass::MyFunction, Gwen::Event::Packet( "Something" )
			Gwen::Event::Packet*	Packet;

			//
			// These are set by the event and may or may not be set.
			//
			Gwen::Controls::Base*	Control;

			Gwen::ControlList		ControlList;
			Gwen::TextObject		String;
			Gwen::Point				Point;
			int						Integer;

		};

		typedef const Gwen::Event::Information & Info;


		class Caller;

		// A class must be derived from this
		class GWEN_EXPORT Handler
		{
			public:

				Handler();
				virtual ~Handler();

				void RegisterCaller( Caller* );
				void UnRegisterCaller( Caller* );

			protected:

				void CleanLinks();
				std::list<Caller*>	m_Callers;

			public:

				typedef void ( Handler::*Function )( Gwen::Controls::Base* pFromPanel );
				typedef void ( Handler::*FunctionBlank )();
				typedef void ( Handler::*FunctionWithInformation )( Gwen::Event::Info info );

		};



		//
		//
		//
		class GWEN_EXPORT Caller
		{
			public:

				Caller();
				~Caller();

				void Call( Controls::Base* pThis );
				void Call( Controls::Base* pThis, Gwen::Event::Info info );

				template <typename T> void Add( Event::Handler* ob, T f ) {	AddInternal( ob, static_cast<Handler::Function>( f ) ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )( Gwen::Event::Info ) ) { AddInternal( ob, static_cast<Handler::FunctionWithInformation>( f ) ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )( Gwen::Event::Info ), const Gwen::Event::Packet & packet ) { AddInternal( ob, static_cast<Handler::FunctionWithInformation>( f ), packet ); }
				template <typename T> void Add( Event::Handler* ob, void ( T::*f )() ) {	AddInternal( ob, static_cast<Handler::FunctionBlank>( f ) ); }

				void RemoveHandler( Event::Handler* pObject );

			protected:

				void CleanLinks();
				void AddInternal( Event::Handler* pObject, Handler::Function pFunction );
				void AddInternal( Event::Handler* pObject, Handler::FunctionWithInformation pFunction );
				void AddInternal( Event::Handler* pObject, Handler::FunctionWithInformation pFunction, const Gwen::Event::Packet & packet );
				void AddInternal( Event::Handler* pObject, Handler::FunctionBlank pFunction );

				struct handler
				{
					handler()
					{
						fnFunction			= NULL;
						fnFunctionInfo		= NULL;
						fnFunctionBlank		= NULL;
						pObject				= NULL;
					}

					Handler::Function					fnFunction;
					Handler::FunctionWithInformation	fnFunctionInfo;
					Handler::FunctionBlank				fnFunctionBlank;

					Event::Handler*			pObject;
					Gwen::Event::Packet		Packet;
				};

				std::list<handler> m_Handlers;
		};

	}

}
#endif
