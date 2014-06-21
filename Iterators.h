
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
	This file is not intended to be included directly, it is part of GQuery and included in the GQ namespace.
	The functionality can be used without directly using GQuery, however the iterations return GQuery interfaces.
	
	This may be merged into a plugin, however it is a built-in feature of GQuery.
---*/

#include <limits.h>

#ifndef HEADER_ITERATORS
	#define HEADER_ITERATORS
/*	
	template< typename T >
		struct IterateEach{
		
		typedef ArrayInfo< T > 				Info;
		typedef typename Info::NextType		Next;
		IterateEach( T &t ) : reft( t ) {}
		
		template< typename FuncT, typename ParamA >
			QueryArrayInterface< T > each( FuncT(*func)( ParamA, Next n ) ){
				for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) func( index, this->reft[ index ] );
				return QueryArrayInterface< T >( this->reft );
		}
		
		template< typename FuncT >
			QueryArrayInterface< T > each( FuncT(*func)( Next n ) ){
				for( index_t( Info::First ) index = 0x00 ; index < Info::First ; func( this->reft[ index++ ] ) );
				return QueryArrayInterface< T >( this->reft );
		}

		template< typename FuncT, typename ParamA >
			QueryArrayInterface< T > each( FuncT(*func)( ParamA, Next &n ) ){
				for( index_t( Info::First ) index = 0x00 ; index < Info::First ; ++index ) func( index, this->reft[ index ] );
				return QueryArrayInterface< T >( this->reft );
		}
		
		template< typename FuncT >
			QueryArrayInterface< T > each( FuncT(*func)( Next &n ) ){
				for( index_t( Info::First ) index = 0x00 ; index < Info::First ; func( this->reft[ index++ ] ) );
				return QueryArrayInterface< T >( this->reft );
		}			
		
		T &reft;
	
	};*/
	
	
	/*--- 																---*\
		This type is used to give commonality to different iterators. 
		It just simplifies detection of an iterator compared to other
		types that may be overloaded by operator [].
	\*---																---*/
	
	template< typename T > struct IterationMode : T{};

	
	/*---																---*\
		Iteration structure.
		
		This type holds the definition for a set of basic iterators.
	\*---																---*/
	
	template< unsigned N >
		struct ISingle{
		
			enum{
				Single = true,
				Index = N,
				Start,
				Step
			};
	};
	
	struct IMulti{
		enum{
			Single = false,
			Index = 0,
		};
	};
	
	struct Iteration{
		struct All : IMulti 		{ enum{ Start, Step }; };
		struct Evens : IMulti		{ enum{ Start, Step = 0x02 }; };
		struct Odds	: IMulti		{ enum{ Start = 0x01, Step }; };	
		struct Every : All{			
			struct Third : IMulti	{ enum{ Start = 0x02, Step }; };
			struct Fourth : IMulti	{ enum{ Start = 0x03, Step }; };
			struct Fifth : IMulti	{ enum{ Start = 0x04, Step }; };
			const IterationMode< Every::Third >					third;
			const IterationMode< Every::Fourth >				fourth;
			const IterationMode< Every::Fifth >					fifth;			
		};
		/*
		struct Single{
			enum{ Start, Step = 0 };
			struct First 	{ enum{ Start, Step = 0 }; };
			struct Last		{ enum{ Start = -1, Step }; };
			const IterationMode< First >						first;
			const IterationMode< Last >							last;
		};*/
		
		const IterationMode< All >								all;
		const IterationMode< Evens >							evens;
		const IterationMode< Odds >								odds;
		const IterationMode< Every >							every;
		
		
	};
	
	
	template< typename Data, typename IdxType >
		struct StaticAssign{
			StaticAssign( const Data &data, const IdxType &index ) : data( data ), index( index ) {}
			const Data &data;
			const IdxType &index;
	};	
	
		
	template< typename D, bool Continue >
		struct RunIteratorHelper{
			DERIVED_TO_SELF;
			template< typename Data, typename IdxType >
				D operator =( const StaticAssign< Data, IdxType > &s ){
					for( index_t( D::NumIterations ) index = D::Iter::Start ; index < D::NumIterations ; index += D::Iter::Step )
						typename D::ObjType( typename D::SubType( this->self().t )[ index ] ) = s;
					return this->self();
			}
			
	};
	
	
	template< typename D >
		struct RunIteratorHelper< D, true >{
			DERIVED_TO_SELF;
			template< typename Data, typename IdxType >
				D operator =( const StaticAssign< Data, IdxType > &s ){
					for( index_t( D::NumIterations ) index = D::Iter::Start ; index < D::NumIterations ; index += D::Iter::Step )
						typename D::ObjType( typename D::SubType( this->self().t )[ index ] )[ s.index ] = s.data;
					return this->self();
			}
	};
	
	
	/*--- Shortcut define for RunIterators assignment operators. ---*/
	#define AssignSingle( op ) RunIterators operator op( const typename Info::UnderlyingType &u ){ \
		for( index_t( NumIterations ) index = Iter::Start ; index < NumIterations ; index += Iter::Step ) \
			ObjType( SubType( this->t )[ index ] ) op u; \
		return *this; \
	}

	template< typename T, typename List >
		struct RunIterators : public RunIteratorHelper< RunIterators< T, List >, IsSameType< typename List::Tail, NullType >::value  > {
		//struct RunIterators {
		
			using RunIteratorHelper< RunIterators< T, List >, IsSameType< typename List::Tail, NullType >::value  >::operator =;

			typedef ArrayInfo< T > Info; //Size will be zero for non-arrays.
			typedef typename List::Head Iter;
			
			/*--- Helper constants. ---*/
			enum{ 
				IsCharType = IsSameType< unsigned char, T >::value || IsSameType< char, T >::value,
				NumIterations = Info::IsAnArray ? Info::First : ( IsCharType ? CHAR_BIT : sizeof( T ) ),
				
				StartIsNeg = ( ( signed ) Iter::Start ) < 0x00,
				
				StartIdx = StartIsNeg ? NumIterations + Iter::Start : Iter::Start
			};			
			
			/*---
				If no array is present, the iterator is acting on a GQuery specialisation like bytes or bits
				of an object which are not normally accessible using [] operators.
			---*/
			typedef typename Select< Info::IsAnArray, T&, typename QueryType< T >::Result >::Result SubType;
			
			typedef typename Select< 
				IsSameType< typename List::Tail, NullType >::value, /*--- Is end of list? Select GQuery type to iterate. ---*/
				typename Select< IsCharType, BitRef::DataRef, typename QueryType< typename Info::NextType >::Result >::Result, 
				RunIterators< typename Info::NextType, typename List::Tail > /*--- Otherwise, continue nesting iteration. ---*/
			>::Result ObjType;

			RunIterators( T &t ) : t( t ) {}
			
			/*--- each methods for iterators. ---*/
			template< typename FuncT >
				RunIterators each( FuncT func ){
					for( index_t( NumIterations ) index = Iter::Start ; index < NumIterations ; index += Iter::Step )
						ObjType( SubType( this->t )[ index ] ).each( func );
					return *this;
			}			
			
			/*--- 
				Copy a single value to the iterator list. 
				These operators are defined using the macro AssignSingle located above IteratorList class.
			---*/
			
	
			
			AssignSingle( = );
			AssignSingle( += );
			AssignSingle( -= );
			AssignSingle( *= );
			AssignSingle( /= );
			AssignSingle( ^= );
			AssignSingle( |= );
			AssignSingle( &= );
			AssignSingle( <<= );
			AssignSingle( >>= );
			
			T &t;
			
			/*--- Static iterator attempt ---*/
			
			/*template< typename Data, typename IdxType >
				RunIterators operator =( const StaticAssign< Data, IdxType > &s ){
				
				
				//Are we at the end of the list?
				if( IsSameType< typename List::Tail, NullType >::value ){
				
					for( index_t( NumIterations ) index = Iter::Start ; index < NumIterations ; index += Iter::Step )
						ObjType( SubType( this->t )[ index ] )[ s.index ] = s.data;				
				
					//ObjType( SubType( this->t )[ s.index ] ) = s.data;
				
				//No, keep iterating.
				}else{
				
					for( index_t( NumIterations ) index = Iter::Start ; index < NumIterations ; index += Iter::Step )
						ObjType( SubType( this->t )[ index ] ).operator =( s );
				}
				return *this;
			}	*/		

			/*RunIterators operator =( const typename Info::UnderlyingType &u ){ 

				if( Iter::Step ){
				
					if( StartIsNeg ){
					
						for( index_t( NumIterations ) index = Iter::Start ; index < NumIterations ; index += Iter::Step ) 
							ObjType( SubType( this->t )[ index ] ) = u; 
							
					}else{
					
						for( index_t( NumIterations ) index = NumIterations + Iter::Start ; index < NumIterations ; index += Iter::Step ) 
							ObjType( SubType( this->t )[ index ] ) = u; 					
					}
				}else{
				
				}
				return *this; 
			}	*/				
		};
		
	#undef AssignIterator
	
	
	/*---
		StaticIterator structure.
		
		This interface is used to add a single access iterator to a dynamic list.
		E.g. _(c)[ _.all ][ 4 ] = 0;
	---*/
	
	
	template< typename T, typename List, typename IdxType >
		struct StaticIterator{

			StaticIterator( T &t, IdxType index ) : t( t ), index( index ) {}
			
			
			typename QueryType< T >::Result operator =( const typename ArrayInfo< T >::UnderlyingType &u ){ 
				RunIterators< T, List >( this->t ) = StaticAssign< typename ArrayInfo< T >::UnderlyingType, IdxType >( u, this->index );
				return typename QueryType< T >::Result( this->t );
			}
			
			T &t;
			IdxType index;
	};
		
	/*---
	
		IteratorList structure.
		
		This type implements a list of iterators or a vector of iterator types.
		Rather than nesting iterators, this type produces a simple list of operations to perform
		on each index of an array ( multi-dimensional array iteration ).
		
		This class does not implement any code. It simply constructs a list of iterators
		to perform an operation on if given one. An action must be invoked on the iterator
		to generate code.
		
		Rules for iterators.
		
		* On completion of an iterator, it must return back to the type first iterated.
			_(c)[4][_.all] returns back to a query using the type of c[4], which can be written as
			_(c)[4], but is actually a direct instantiation of _(c[4])
			
		
	---*/
	
	#define AssignSingle( op ) \
		typename QueryType< T >::Result operator op( const typename ArrayInfo< T >::UnderlyingType &u ){ \
			RunIterators< T, List >( this->t ) op u; \
			return typename QueryType< T >::Result( this->t ); \
		}
	
	template< typename T, typename List >
		struct IteratorList{
			
			IteratorList( T &t ) : t( t ) {}
			
			/*--- Append an iterator to the list. ---*/
			template< typename I >
				IteratorList< T, typename TL::Append< List, I >::Result >	operator []( const IterationMode< I > &i ){ 
					return IteratorList< T, typename TL::Append< List, I >::Result >( this->t ); 
			}
			
			/*--- Append a static iterator to the list ---*/
			StaticIterator< T, List, uint16_t >	operator []( const uint16_t &i ){ 
				return StaticIterator< T, List, uint16_t >( this->t, i ); 
			}			
			
			/*--- Append a single step iterator to the list. ---*/
			/*IteratorList< T, typename TL::Append< List, I >::Result >	operator []( const int &i ){ 
				return IteratorList< T, typename TL::Append< List, I >::Result >( this->t ); 
			}*/		
			
			/*--- each methods for iterators. ---*/
			template< typename FuncT >
				typename QueryType< T >::Result each( FuncT func ){
					RunIterators< T, List >( this->t ).each( func );
					return typename QueryType< T >::Result( this->t );
			}			
			/*template< typename FuncT, typename ParamA >
				typename QueryType< T >::Result each( FuncT(*func)( ParamA, typename ArrayInfo< T >::NextType &n ) ){
				RunIterators< T, List >( this->t ).each( func );
				return typename QueryType< T >::Result( this->t );
			}
			
			template< typename FuncT, typename ParamA >
				typename QueryType< T >::Result each( FuncT(*func)( ParamA, typename ArrayInfo< T >::NextType n ) ){
				RunIterators< T, List >( this->t ).each( func );
				return typename QueryType< T >::Result( this->t );
			}*/	
			/*--- 
				Copy a single value to the iterator list. 
				These operators are defined using the macro AssignSingle located above IteratorList class.
			---*/
			
			AssignSingle( = );
			AssignSingle( += );
			AssignSingle( -= );
			AssignSingle( *= );
			AssignSingle( /= );
			AssignSingle( ^= );
			AssignSingle( |= );
			AssignSingle( &= );
			AssignSingle( <<= );
			AssignSingle( >>= );

			//Reference of type to be iterated.
			T &t;	
	};
	
	#undef AssignSingle
	
	
	
	
	
	
	
	
	
#endif
//EOF
