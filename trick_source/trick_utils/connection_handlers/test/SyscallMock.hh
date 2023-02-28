#include <iostream>
#include <functional>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

class SyscallMock;
static SyscallMock * the_system_context;

extern "C" {
	// Declarations for wrap and real functions

	extern int __real_socket(int _0,  int _1,  int _2);
	int __wrap_socket(int _0,  int _1,  int _2);

	extern int __real_setsockopt(int _0,  int _1,  int _2,  const void * _3,  socklen_t _4);
	int __wrap_setsockopt(int _0,  int _1,  int _2,  const void * _3,  socklen_t _4);

	extern int __real_bind(int _0,  const struct sockaddr * _1, socklen_t _2);
	int __wrap_bind(int _0,  const struct sockaddr * _1, socklen_t _2);

	extern int __real_getsockname(int _0,  struct sockaddr * _1,  socklen_t * _2);
	int __wrap_getsockname(int _0,  struct sockaddr * _1,  socklen_t * _2);

	extern int __real_listen(int _0,  int _1);
	int __wrap_listen(int _0,  int _1);

	extern int __real_select(int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4);
	int __wrap_select(int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4);

}

class SyscallMock {
public:
	SyscallMock () {
		real_socket_impl();
		real_setsockopt_impl();
		real_bind_impl();
		real_getsockname_impl();
		real_listen_impl();
		real_select_impl();
	}

	void set_all_real() {
		real_socket_impl();
		real_setsockopt_impl();
		real_bind_impl();
		real_getsockname_impl();
		real_listen_impl();
		real_select_impl();
	}

	void set_all_noop() {
		noop_socket_impl();
		noop_setsockopt_impl();
		noop_bind_impl();
		noop_getsockname_impl();
		noop_listen_impl();
		noop_select_impl();
	}


	// socket Implementation
	public:
	int socket (int _0,  int _1,  int _2) { return socket_impl( _0,  _1,  _2); }
	void register_socket_impl (std::function<int(int _0,  int _1,  int _2)> impl) { socket_impl = impl; }
	void real_socket_impl () { socket_impl = [](int _0,  int _1,  int _2) -> int { return __real_socket( _0,  _1,  _2); }; }
	void noop_socket_impl () { socket_impl = [](int _0,  int _1,  int _2) -> int { return 0; }; }
	private:
	std::function <int(int,  int,  int)> socket_impl;

	// setsockopt Implementation
	public:
	int setsockopt (int _0,  int _1,  int _2,  const void * _3,  socklen_t _4) { return setsockopt_impl( _0,  _1,  _2,  _3,  _4); }
	void register_setsockopt_impl (std::function<int(int _0,  int _1,  int _2,  const void * _3,  socklen_t _4)> impl) { setsockopt_impl = impl; }
	void real_setsockopt_impl () { setsockopt_impl = [](int _0,  int _1,  int _2,  const void * _3,  socklen_t _4) -> int { return __real_setsockopt( _0,  _1,  _2,  _3,  _4); }; }
	void noop_setsockopt_impl () { setsockopt_impl = [](int _0,  int _1,  int _2,  const void * _3,  socklen_t _4) -> int { return 0; }; }
	private:
	std::function <int(int,  int,  int,  const void *,  socklen_t)> setsockopt_impl;

	// bind Implementation
	public:
	int bind (int _0,  const struct sockaddr * _1, socklen_t _2) { return bind_impl( _0,  _1,  _2); }
	void register_bind_impl (std::function<int(int _0,  const struct sockaddr * _1, socklen_t _2)> impl) { bind_impl = impl; }
	void real_bind_impl () { bind_impl = [](int _0,  const struct sockaddr * _1, socklen_t _2) -> int { return __real_bind( _0,  _1,  _2); }; }
	void noop_bind_impl () { bind_impl = [](int _0,  const struct sockaddr * _1, socklen_t _2) -> int { return 0; }; }
	private:
	std::function <int(int,  const struct sockaddr *, socklen_t)> bind_impl;

	// getsockname Implementation
	public:
	int getsockname (int _0,  struct sockaddr * _1,  socklen_t * _2) { return getsockname_impl( _0,  _1,  _2); }
	void register_getsockname_impl (std::function<int(int _0,  struct sockaddr * _1,  socklen_t * _2)> impl) { getsockname_impl = impl; }
	void real_getsockname_impl () { getsockname_impl = [](int _0,  struct sockaddr * _1,  socklen_t * _2) -> int { return __real_getsockname( _0,  _1,  _2); }; }
	void noop_getsockname_impl () { getsockname_impl = [](int _0,  struct sockaddr * _1,  socklen_t * _2) -> int { return 0; }; }
	private:
	std::function <int(int,  struct sockaddr *,  socklen_t *)> getsockname_impl;

	// listen Implementation
	public:
	int listen (int _0,  int _1) { return listen_impl( _0,  _1); }
	void register_listen_impl (std::function<int(int _0,  int _1)> impl) { listen_impl = impl; }
	void real_listen_impl () { listen_impl = [](int _0,  int _1) -> int { return __real_listen( _0,  _1); }; }
	void noop_listen_impl () { listen_impl = [](int _0,  int _1) -> int { return 0; }; }
	private:
	std::function <int(int,  int)> listen_impl;

	// select Implementation
	public:
	int select (int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4) { return select_impl( _0,  _1,  _2,  _3,  _4); }
	void register_select_impl (std::function<int(int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4)> impl) { select_impl = impl; }
	void real_select_impl () { select_impl = [](int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4) -> int { return __real_select( _0,  _1,  _2,  _3,  _4); }; }
	void noop_select_impl () { select_impl = [](int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4) -> int { return 0; }; }
	private:
	std::function <int(int,  fd_set *,  fd_set *,  fd_set *,  struct timeval *)> select_impl;
};

// Wrap function definitions
int __wrap_socket(int _0,  int _1,  int _2) { return the_system_context->socket( _0,  _1,  _2); }
int __wrap_setsockopt(int _0,  int _1,  int _2,  const void * _3,  socklen_t _4) { return the_system_context->setsockopt( _0,  _1,  _2,  _3,  _4); }
int __wrap_bind(int _0,  const struct sockaddr * _1, socklen_t _2) { return the_system_context->bind( _0,  _1,  _2); }
int __wrap_getsockname(int _0,  struct sockaddr * _1,  socklen_t * _2) { return the_system_context->getsockname( _0,  _1,  _2); }
int __wrap_listen(int _0,  int _1) { return the_system_context->listen( _0,  _1); }
int __wrap_select(int _0,  fd_set * _1,  fd_set * _2,  fd_set * _3,  struct timeval * _4) { return the_system_context->select( _0,  _1,  _2,  _3,  _4); }
