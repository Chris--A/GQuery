

	#define index_t( x )		typename BestFitInt< x >::Result
	#define cindex_t( x )		const index_t( x )
	
	#define sindex_t( x )		typename BestFitInt< x, false >::Result
	#define csindex_t( x )		const sindex_t( x )


	/*--- An accessor for CRTP based entities. ---*/
	#define CRTP_SELF			D& self() { return *static_cast< D* >( this ); } \
								const D& self() const { return *static_cast< const D* >( this ); } \
								D* selfPtr() { return static_cast< D* >( this ); } \
								const D& selfPtr() const { return static_cast< const D* >( this ); }


	#define EOL					( IsSameType< typename List::Tail, NullType >::value )