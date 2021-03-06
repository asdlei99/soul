#include <soul/include/drivers/compiler_lib.h>
#include <soul/include/drivers/compiler_impl.h>

using boost::shared_ptr;

BEGIN_NS_SOUL_DRIVERS();
shared_ptr<compiler> create_compiler()
{
	return shared_ptr<compiler>( new compiler_impl() );
}
END_NS_SOUL_DRIVERS();