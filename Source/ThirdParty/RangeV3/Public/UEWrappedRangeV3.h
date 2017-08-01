// include Range-v3
#pragma warning( push )
#pragma warning( disable: 4268 4668 )    // UE pollution.. let's hope that this does not break anything
#pragma push_macro("check")
#pragma push_macro("PI")
#undef check
#undef PI
#include <range/v3/all.hpp>
#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma warning( pop )
