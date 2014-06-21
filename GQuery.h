
/*---
	The MIT License (MIT)

	Copyright (c) 2014 Christopher Andrews, genx.biz

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
---*/

/*---

	GQuery, A jQuery inspired C++ Library.
	
	General functions
		- 
		
	General Properties
		- isPointer
		- isArray
		- isReference
		- isConst
		- isVolatile
		
	General types
		- UnderlyingType
		- UnqualifiedType
		- NextType
		- MyType
		
	Set functions
		- each
		- min
		- max
		- average
		- mean
		- median
		- std_dev
		- rotate
		
	

---*/

#include "Arduino.h"
//#include "PrintEx.h"
#include "MPL.h"
#include "BitBool.h"

#ifndef HEADER_GQUERY
	#define HEADER_GQUERY
	
	class  _GQuery;
	
	namespace GQ{
	
		TOKEN EntryPointPlugins;

		struct MissingType{};
		#define Node Typelist

		//#define _ ::_GQuery()
		
		#define index_t( x )		typename BestFitInt< x >::Result
		#define cindex_t( x )		const index_t( x )
		
		
		
		/*--- Forward declaration of standard interfaces. ---*/
		template< typename T > struct 				InterfaceBase;
		template< typename T > struct 				QueryInterface;
		template< typename T > struct 				QueryPointerInterface;
		template< typename T > struct 				QueryArrayInterface;
		//template< typename I, typename T > struct 	QueryIteratorInterface;		//Located in Iterators.h
		
		/*---
			The QueryType type determines the most suitable interface for 
			a particular type to use with GQuery.
		---*/		
		
		template< typename T >
			struct QueryType{ 
				typedef typename Select< 
					IsArrayType< T >::Value, QueryArrayInterface< T >, typename Select< 
						TypeTraits< T >::isPointer, QueryPointerInterface< T >, QueryInterface< T > 
					>::Result
				>::Result Result;
		};	
	
		/*--- Terrible C based helpers ---*/
		#include "Macros.h"
		
		/*--- Built in features. ---*/
		#include "Iterators.h"
		#include "GObject.h"
		
		/*--- Modules for specialised cases. ---*/
		#include "GModule_AVR8.h"
		
		//#undef _
		
	/***********************************************************************************************************
		_GQuery Interface.
			Its main function is a functor which converts all calls to a template object.
			This is to avoid the need to use template parameters in client code.
	***********************************************************************************************************/
	};
	class _GQuery : 
		public GQ::Iteration{
			public:
				
				/*---
					This is the main entry point for GQuery.
					
				---*/
				template< typename T >
					typename GQ::QueryType< T >::Result operator()( T &t ){ return typename GQ::QueryType< T >::Result( t ); }
					
				template< typename T >
					typename GQ::QueryPGMType< T >::Result PGM( T &t ){ 
						typedef typename GQ::QueryPGMType< T > PGMQ;
						return typename PGMQ::Result( *( typename PGMQ::PGMType* ) &t );
				}
				
				template< typename T >
					typename GQ::QueryEEPROMType< T >::Result EEPROM( T &t ){ 
						typedef typename GQ::QueryEEPROMType< T > EEPROMQ;
						return typename EEPROMQ::Result( *( typename EEPROMQ::EEPROMType* ) &t );
				}				
		
		protected:
		private:
	};
	
	//Create short and long access methods.
	::_GQuery _;
	::_GQuery &GQuery = _;
	
	//Or use the alternative define below.
	//#define _ ::_GQuery()
	
	namespace GQ{

		/*---
			This is the interface GQuery uses to keep track of itself during complex designs.
			It is the lowest interface used by a single instantiation of gquery.
			However the actual nested types may go significantly lower.
		---*/	
		template< typename T > 
			struct InterfaceBase : TypeTraits< T >{
			
				template< typename U > const bool isType( U u ) { return CompareType( t, u ); }

				InterfaceBase( T &t ) : t( t ) {}
				typename ObjectAccess< T >::Result operator []( cindex_t( sizeof( T ) ) index ){ return ObjectAccess< T >::Call( this->t, index ); }
				T &data( void ) { return this->t; }
				operator T&(){ return this->t; }
				
				T &t;
		};

		/***********************************************************************************************************
			Generic type extension. 
				This is the original QueryInterface, all others are specializations.
		***********************************************************************************************************/
			
		template< typename T > 
			struct QueryInterface : InterfaceBase< T >{
			
				//This enum extends the typetraits with GQuery extras.
				enum{ isArray = false };	
				
				using InterfaceBase< T >::t;
				using InterfaceBase< T >::operator[];
				
				operator T&(){ return this->t; }
				typedef QueryInterface< T > MyType;
				QueryInterface( T &t ) : InterfaceBase< T >( t ) {}
				
				//template< typename U > QueryInterface( const QueryInterface< U > &u ) : InterfaceBase< U >( u.t ) {}
				
				/*--- Invoke an iterator on this object. ---*/
				template< typename I > 
					IteratorList< T, Typelist< I, NullType > > operator []( const IterationMode< I > &i ){ return IteratorList< T, Typelist< I, NullType > >( t ); }				
				
				QueryPointerInterface< T* >		operator &(){ return QueryPointerInterface< T* >( &t ); }	
				MyType&							operator =( T const &cpy ){ t = cpy; return *this; }
				template< typename U > MyType&	operator =( U const &cpy ){ t = cpy; return *this; }
				MyType&							operator =( const QueryInterface< T > &cpy ){ t = cpy.t; return *this; }
				template< typename U > MyType&	operator =( const QueryInterface< U > &cpy ){ t = cpy.t; return *this; }			
		};
		

		
		/***********************************************************************************************************
			Generic pointer extension. 
				This is the built in pointer extension.
		***********************************************************************************************************/

		template< typename T >
			struct QueryPointerInterface : 
				InterfaceBase< T >, PointerInfo< T >{
				
				//This enum extends the typetraits with GQuery extras.
				enum{ isArray = false };
				
				using 										InterfaceBase< T >::t;
				typedef QueryPointerInterface< T >			MyType;				
				typedef typename QueryType< typename MyType::PointeeType >::Result DereferenceType;
				
				QueryPointerInterface( T &t ) : InterfaceBase< T >( t )	{}
				//template< typename U > QueryPointerInterface( const QueryPointerInterface< U > &u ) : InterfaceBase< U >( u.t ) {}
				
				/*--- index function. Re-types t to an array of size N. ---*/
				template< unsigned N >
					typename QueryType< typename PointerToMeta< T, N >::Result >::Result index( void ){ 
						typedef typename PointerToMeta< T, N >::Result OutType;
						return typename QueryType< OutType >::Result( *( OutType* ) this->t ); //My first instinct was to dereference t, however it failed to work.
				};
				
				/*--- Standard pointer semantics, address of & dereference ---*/
				DereferenceType 			operator *()	{ return DereferenceType( *t ); }
				QueryPointerInterface< T* > operator &()	{ return QueryPointerInterface< T* >( &this->t ); }
		};
		
		/***********************************************************************************************************
			Any dimension array type extension.
				The underlying types allow for 3 dimensions currently, more can be added easily.
				
				Supports copying multi dimensional arrays, this simulates the ISO forbidden array assignment.
		***********************************************************************************************************/
		//3438
		

		template< typename D > class ArrayFromPointer;
		template< typename D > class QAIDummy;
		
		template< typename T >
			struct QueryArrayInterface : 
			
					InterfaceBase< T >, 
					ArrayInfo< T >, 
					
					/*--- Selectively derived Index function for arrays of pointers (only). ---*/
					Select<
						PointerInfo< typename ArrayInfo< T >::UnderlyingType >::IsAPointer,
						ArrayFromPointer< QueryArrayInterface< T > >,
						QAIDummy< QueryArrayInterface< T > >
					>::Result{

				//This enum extends the typetraits with GQuery extras.
				enum{ isArray = true };
				
				/*using Select<
						PointerInfo< typename ArrayInfo< T >::UnderlyingType >::IsAPointer,
						ArrayFromPointer< QueryArrayInterface< T > >,
						QAIDummy< QueryArrayInterface< T > >
					>::Result::index;*/

				//using 										InterfaceBase< T >::t;
				//typedef QueryArrayInterface< T >			MyType;
				typedef ArrayInfo< T >						Info;
				typedef typename Info::ThisType				ThisType;
				typedef typename Info::NextType				Next;
				typedef typename QueryType< Next >::Result 	NextInterface;

				QueryArrayInterface( T &t ) : InterfaceBase< T >( t ) {}
				//QueryArrayInterface( const QueryArrayInterface< T > &u ) : InterfaceBase< T >( u.t ) {}
				//template< typename U > QueryArrayInterface( const QueryArrayInterface< U > &u ) : InterfaceBase< U >( u.t ) {}
				
				NextInterface operator *() { return NextInterface( this->t[ 0 ] ); }
				//NextInterface operator []( cindex_t( Info::First ) index ){ return NextInterface( this->t[ index ] ); }
				//NextInterface operator []( cindex_t( Info::First ) index ) const { return NextInterface( this->t[ index ] ); }
				NextInterface operator []( int index ){ return NextInterface( this->t[ index ] ); }
				NextInterface operator []( int index ) const { return NextInterface( this->t[ index ] ); }
				
				/*--- Invoke an iterator on this object. ---*/
				template< typename I > 
					IteratorList< T, Typelist< I, NullType > > operator []( const IterationMode< I > &i ){ return IteratorList< T, Typelist< I, NullType > >( this->t ); }

				/*--- Copy a single object to the deepest nodes of the array. Implemented using iterators. ---*/
				QueryArrayInterface< T > operator =( const QueryInterface< const typename Info::UnderlyingType > &cpy ){ 
					return _( this->t )[ _.all ] = cpy.t;
					//return ( *this )[ IterationMode< Iteration::All >() ] = cpy.t; //for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) ( *this )[ index ] = cpy;
				}	
				
				/*--- This is an entry point for rvalue arrays, type safety is yet to be implemented. ---*/
				template< typename U >
					QueryArrayInterface< T > &operator =( const QueryArrayInterface< U > &cpy ){
						typedef typename QueryArrayInterface< U >::Next UNext;
						typedef typename Select< IsArrayType< UNext >::Value, QueryArrayInterface< UNext >, QueryInterface< UNext > >::Result UNextInterface;					
						for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) ( *this )[ index ] = UNextInterface( cpy.t[ index ] );
						return *this;
				}	
				
				/*--- These are entry points for non query objects. They simply convert the input to a query object then re-call the operator. ---*/
				template< typename U > 
					QueryArrayInterface< T > operator =( U &tcpy ) 				{ return ( *this ) = QueryArrayInterface< U >( tcpy ); }
					
				//template< typename U > 
				//	QueryArrayInterface< T > operator =( QueryArrayInterface< U > &tcpy ) { return ( *this ) = tcpy.t; }					
					
				QueryArrayInterface< T > operator =( const typename Info::UnderlyingType &cpy )	{ return ( *this ) = QueryInterface< const typename Info::UnderlyingType >( cpy ); }	
				
				/*--- To maintain simplicity ( where possible ) query objects are converted to their const counterparts. ---*/
				QueryArrayInterface< T > operator =( const QueryArrayInterface< T > &cpy )			{ return ( *this ) = QueryArrayInterface< const T >( cpy.t ); }
				
				
				/*---
					each function.
					
					Allows iteration over objects using a callback.
				---*/
				template< typename FuncT, typename ParamA >
					QueryArrayInterface< T > each( FuncT(*func)( ParamA, Next n ) ){
						for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) func( index, this->t[ index ] );
						return QueryArrayInterface< T >( this->t );
				}
				
				template< typename FuncT >
					QueryArrayInterface< T > each( FuncT(*func)( Next n ) ){
						for( index_t( Info::First ) index = 0x00 ; index < Info::First ; func( this->t[ index++ ] ) );
						return QueryArrayInterface< T >( this->t );
				}

				template< typename FuncT, typename ParamA >
					QueryArrayInterface< T > each( FuncT(*func)( ParamA, Next &n ) ){
						for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) func( index, this->t[ index ] );
						return QueryArrayInterface< T >( this->t );
				}
				
				template< typename FuncT >
					QueryArrayInterface< T > each( FuncT(*func)( Next &n ) ){
						for( index_t( Info::First ) index = 0x00 ; index < Info::First ; func( this->t[ index++ ] ) );
						return QueryArrayInterface< T >( this->t );
				}		
		};
		
		template< typename D >
			class ArrayFromPointer{
				public:
				
					/*--- index function. Re-types t to an array of size N. ---*/
					template< unsigned N >
						typename QueryType< typename PointerToMeta< typename D::ThisType, N >::Result >::Result index( void ){ 
							typedef typename PointerToMeta< typename D::ThisType, N >::Result OutType;
							return typename QueryType< OutType >::Result( *( OutType* ) self().t ); //My first instinct was to dereference t, however it failed to work.
					};
				protected:
				private:
					DERIVED_TO_SELF;
		};
		
		template< typename D >
			struct QAIDummy{
				D index( void ){ return D( self().t ); }
				DERIVED_TO_SELF;
		};		
		
		#undef Node
		//#undef _
	};




	
	namespace GQ{
	
		struct char7{
		
			char7( const char c ) :
				a( _(c)[ 6 ] ),
				b( _(c)[ 5 ] ),
				c( _(c)[ 4 ] ),
				d( _(c)[ 3 ] ),
				e( _(c)[ 2 ] ),
				f( _(c)[ 1 ] ),
				g( _(c)[ 0 ] )
			{}
		
			int a : 1;
			int b : 1;
			int c : 1;
			int d : 1;
			int e : 1;
			int f : 1;
			int g : 1;
		};	
	};
#endif

/*--- 																---*\
	
	All features here are intended to be implemented eventually.
	
	--Global--
	Array conversions:		Not implemented.
	
	Array assignment:		Completed.
	Raw byte/bit access: 	Completed.
	LHS Iterators:			Working, incomplete.
	RHS Iterators:			Not implemented.
	Sets:					Not implemented.
	
	
	--AVR--
	PGM built-in:			In development.
	Functional specifics:	Not implemented.

	Compiler version observations.
	
	During the development of this library I have had to design for
	an older compiler. Even though this is a general C++ library I
	wish to fully support the platforms used by Arduino.
	
	As a result of the AVR version of GCC being so outdated, many
	C++11 features are unavailable. Below is a list of each language
	feature that I have had to either avoid or replace entirely with
	a different strategy.
	
	When I refer to GCC I mean the Arduino version GCC 4.3.3
	
	I will provide many C++11 only features though (Lambdas have so much potential here).
	
	SFINAE:
		One of the first problems encountered was using a typical
		EnableIf structure to disable features with incomplete types.
		
		template< bool V, typename T > struct EnableIf{};
		template< typename T > struct EnableIf< true, T >{ typedef T Result; };

		As you can see, if V is not true, then 'Result' is undefined.
		GCC stops compiling and spits out the obvious error.
		
		*	Arrays of pointers features for QueryArrayInterface.
			
	constexpr:
	
		As of writing this I still have not found a way to implement features that
		constexpr would otherwise allow ( without undesired overhead ). Also a number 
		of small template classes are used to overcome the lack of constexpr. This 
		causes complexity in the code that would otherwise be unessecary.
		
	Inheriting Constructors:

		I haven't had to change anything really ( apart from provide a constructor ).
		Although I'd imagine this would allow unknown base classes to be used easily,
		and is probably a situation I may encounter during development.
		
	Variadic templates:
		
		Using std::function(C++11) causes an indirect need for this feature.
	
\*---																---*/

//EOF











		/***********************************************************************************************************
			Supported types for extension.
		***********************************************************************************************************/	
		
		/*---	
			This section will decide what headers to include.
			If the header is not available a place holder will be used.
		---*/
		/*
		#ifdef client_h //From Client.h
			typedef Client GQ_Ardu_Client;
		#else
			typedef MissingType GQ_Ardu_Client;
		#endif
		
		#ifdef Stream_h //From Stream.h
			typedef Stream GQ_Ardu_Stream;
		#else
			typedef MissingType GQ_Ardu_Stream;
		#endif

		#ifdef Print_h //From Print.h
			typedef Print GQ_Ardu_Print;
		#else
			typedef MissingType GQ_Ardu_Print;
		#endif	
		*/
		
		/*---	
			Compatibility type list.
		---*/
		/*
		//Arduino specific types.
		typedef	Typelist< GQ_Ardu_Client,
					Node< GQ_Ardu_Stream,
						Node< GQ_Ardu_Print, NullType > > > 	Ardu_Mid;
						
		//Complete list.								
		typedef	Typelist< Ardu_Mid, NullType >					GQTypes;
		*/
