#ifndef CLIENT_LISTENER_HH
#define CLIENT_LISTENER_HH

#include <string>

#define LISTENER_ERROR -1


namespace Trick {
    
    class TCConnection;

    class ClientListener {
        public:
            friend class TCConnection;

            ClientListener ();
            ~ClientListener ();

            // We'll see if we need separate methods for these
            int initialize(std::string hostname, int port);
            int initialize();

            // int set_block_mode(TCCommBlocking mode);

            bool checkForNewConnections();

            std::string getHostname ();

            int getPort();

            int disconnect();

            int checkSocket();

            bool validateSourceAddress(std::string source_address);

            bool isInitialized(); 
            
        private:
        
            int _listen_socket;
            std::string _hostname;
            int _port;
            std::string _client_tag;

            bool _initialized;
    };
}

#endif