
#include <gtest/gtest.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "trick/ClientListener.hh"
#include "SyscallMock.hh"

class MockedSystemTest : public testing::Test {
    protected:
      SyscallMock * system_context;

      MockedSystemTest() {
        system_context = new SyscallMock();
        the_system_context = system_context;
      }

      ~MockedSystemTest(){
        delete system_context;
        the_system_context = NULL;
      }
};

class ClientListenerTest : public MockedSystemTest {

   protected:
      ClientListenerTest(){}
      ~ClientListenerTest(){}

      Trick::ClientListener listener;
};


TEST_F( ClientListenerTest, initialized ) {
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, initialize_localhost_0 ) {
    // ARRANGE
    // ACT
    listener.initialize("localhost", 0);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getHostname(), std::string("127.0.0.1"));
}

TEST_F( ClientListenerTest, initialize_localhost_54321 ) {
    // ARRANGE
    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getPort(), 54321);
}

TEST_F( ClientListenerTest, initialize_no_args ) {
    // ARRANGE
    // ACT
    listener.initialize();

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getHostname(), std::string("127.0.0.1"));
}

TEST_F( ClientListenerTest, initialize_localhost_numerical_54321 ) {
    // ARRANGE
    // ACT
    listener.initialize("127.0.0.1", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getPort(), 54321);
}

TEST_F( ClientListenerTest, initialize_invalid_hostname ) {
    // ARRANGE
    // ACT
    listener.initialize("some_invalid_hostname", 0);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_socket ) {
    // ARRANGE
    system_context->register_socket_impl([](int a, int b, int c) { 
        errno = EPERM;
        return -1; 
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_setsockopt_reuseaddr ) {
    // ARRANGE
    system_context->register_setsockopt_impl([](int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
        errno = EINVAL;
        return -1; 
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_setsockopt_buffering ) {
    // ARRANGE
    system_context->register_setsockopt_impl([](int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
        if (level == IPPROTO_TCP && optname == TCP_NODELAY) {
            errno = ENOTSOCK;
            return -1; 
        }

        return 0;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_bind ) {
    // ARRANGE
    system_context->register_bind_impl([](int sockfd, const struct sockaddr *addr,socklen_t addrlen) { 
        errno = EADDRINUSE;
        return -1;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_sockname ) {
    // ARRANGE
    system_context->register_getsockname_impl([](int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
        ((struct sockaddr_in *) addr)->sin_port = htons(1234);
        return 0;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_listen ) {
    // ARRANGE
    system_context->register_listen_impl([](int sockfd, int backlog) {
        errno = EADDRINUSE;
        return -1;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, checkForNewConnections_uninitialized ) {
    // ARRANGE
    // ACT
    // ASSERT
    EXPECT_EQ(listener.checkForNewConnections(), false);
}

TEST_F( ClientListenerTest, checkForNewConnections ) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = __real_socket(a, b, c);
        return socket_fd;
    });
    system_context->register_select_impl([&socket_fd](int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout) {
        FD_SET(socket_fd, readfds);
        return 0;
    });
    listener.initialize();

    // ACT
    // ASSERT
    EXPECT_EQ(listener.checkForNewConnections(), true);
}

TEST_F( ClientListenerTest, checkForNewConnections_select_error ) {
    // ARRANGE
    system_context->register_select_impl([](int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout) {
        return -1;
    });
    listener.initialize();

    // ACT
    // ASSERT
    EXPECT_EQ(listener.checkForNewConnections(), false);
}
