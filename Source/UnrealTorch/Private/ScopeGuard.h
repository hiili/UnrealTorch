

#pragma once




/* ScopeGuard, adapted from http://the-witness.net/news/2012/11/scopeexit-in-c11/ */

template<typename T>
class ScopeGuard
{
	T f;
	bool engaged;

public:
	
	ScopeGuard( T f_ ) : f( f_ ), engaged(true) {}
	~ScopeGuard() { if( engaged ) this->f(); }

	void release() { engaged = false; }
};

template<typename T>
ScopeGuard<T> MakeScopeGuard( T f ) { return ScopeGuard<T>( f ); }
