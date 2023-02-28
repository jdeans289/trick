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

int __wrap_getaddrinfo (const char *hostname, const char *servname, const struct addrinfo *hints, struct addrinfo **res) {
    return -1;
}

Trick::ClientListener::ClientListener () : _listen_socket(-1), _hostname(""), _port(0), _client_tag("<empty>"), _initialized(false) {}

Trick::ClientListener::~ClientListener () {
    // Clean up our socket if initialized
    if (_initialized) {
        close (_listen_socket);
    }
}

int Trick::ClientListener::initialize(std::string in_hostname, int in_port) {

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    if (in_hostname.size() == 0) {
        in_hostname = "localhost";
    }

    int err;
    if ((err = getaddrinfo(in_hostname.c_str(), std::to_string(in_port).c_str(), &hints, &res)) != 0) {
        std::cerr << "Unable to lookup address: " << gai_strerror(err) << std::endl;
        return LISTENER_ERROR;
    }

    if ((_listen_socket = socket(res->ai_family, res->ai_socktype, 0)) < 0) {
        perror ("Unable to open socket");
        return LISTENER_ERROR;
    }

    int option_val = 1;

    // Allow socket's bound address to be used by others
    if (setsockopt(_listen_socket, SOL_SOCKET, SO_REUSEADDR, (const void *) &option_val, (socklen_t) sizeof(option_val)) != 0) {
        perror("Could not set socket to reuse addr");
        close (_listen_socket);
        return LISTENER_ERROR;
    }
    // Turn off data buffering on the send side
    if (setsockopt(_listen_socket, IPPROTO_TCP, TCP_NODELAY, (const void *) &option_val, (socklen_t) sizeof(option_val)) != 0) {
        perror("Could not turn off data buffering");
        close (_listen_socket);
        return LISTENER_ERROR;
    }

    // Bind to socket
    if (bind(_listen_socket, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Could not bind to socket");
        close (_listen_socket);
        return LISTENER_ERROR;
    } 

    // Check that correct port was bound to
    struct sockaddr_in s_in;
    socklen_t s_in_size =  sizeof(s_in) ;
    getsockname( _listen_socket , (struct sockaddr *)&s_in, &s_in_size) ;
    int bound_port = ntohs(s_in.sin_port);

    if (in_port != 0 && bound_port != in_port) {
        std::cerr << "Could not bind to requested port " << in_port << std::endl;
        close(_listen_socket);
        return LISTENER_ERROR;
    }

    // Save port number
    _port = bound_port;

    // Save printable hostname
    _hostname = inet_ntoa((struct in_addr)((struct sockaddr_in *) res->ai_addr)->sin_addr);

    // Start listening
    if (listen(_listen_socket, SOMAXCONN) < 0) {
        std::string error_message = "Could not listen on port " + std::to_string(_port);
        perror (error_message.c_str());
        close(_listen_socket);
        return LISTENER_ERROR;
    }

    // Done!
    _initialized = true;
    return 0;
}

int Trick::ClientListener::initialize() {
    return initialize("", 0);
}


// int Trick::ClientListener::set_block_mode(TCCommBlocking mode) {
//     // if (!initialized)
//     //     return -1;

//     // return tc_blockio(&_listen_dev, mode);
// }


bool Trick::ClientListener::checkForNewConnections() {
    if (!_initialized)
        return false;

    fd_set rfds;
    struct timeval timeout_time = { 2, 0 };
    FD_ZERO(&rfds);
    FD_SET(_listen_socket, &rfds);
    timeout_time.tv_sec = 2 ;

    // Listen with a timeout of 2 seconds
    int result = select(_listen_socket + 1, &rfds, NULL, NULL, &timeout_time);

    // If there's some kind of error, just ignore it and return false
    if (result != 0) {
        return false;
    }

    return FD_ISSET(_listen_socket, &rfds);
}



std::string Trick::ClientListener::getHostname () {
    if (!_initialized)
        return "";

    return _hostname;
}


int Trick::ClientListener::getPort() {
    if (!_initialized)
        return -1;

    return _port;
}


int Trick::ClientListener::disconnect() {
    // if (!initialized)
    //     return -1;

    // return tc_disconnect(&_listen_dev) ;    
}

bool Trick::ClientListener::validateSourceAddress(std::string requested_source_address) {
    // char hname[80];
    // static struct sockaddr_in s_in;
    // gethostname(hname, (size_t) 80);

    // // Test to see if the restart address is on this machine.  If it is not, it's not an error
    // if ( strcmp( requested_source_address.c_str(), hname )) {
    //     if (! inet_pton(AF_INET, requested_source_address.c_str(), (struct in_addr *)&s_in.sin_addr.s_addr) ) {
    //         return false;
    //     }
    // }

    // return true;
}

int Trick::ClientListener::checkSocket() {
    // if (!initialized)
    //     return -1;

    // struct sockaddr_in s_in;
    // int s_in_size =  sizeof(s_in) ;
    // getsockname( _listen_dev.socket , (struct sockaddr *)&s_in, (socklen_t *)&s_in_size) ;
    // printf("restart variable server message port = %d\n" , ntohs(s_in.sin_port)) ;
    // _listen_dev.port = ntohs(s_in.sin_port);

    // return 0;
}

bool Trick::ClientListener::isInitialized() {
    return _initialized;
}

