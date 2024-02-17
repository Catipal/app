#include <iostream>
#include <vector>

// Define a Peer class to represent nodes in the network
class Peer {
private:
    std::vector<Peer*> peers;

public:
    void addPeer(Peer* peer) {
        peers.push_back(peer);
    }

    void sendMessage(const std::string& message) {
        // Send message to all connected peers
        for (Peer* peer : peers) {
            peer->receiveMessage(message);
        }
    }

    virtual void receiveMessage(const std::string& message) = 0; // Pure virtual function
};

// Example implementation of a peer
class ExamplePeer : public Peer {
public:
    void receiveMessage(const std::string& message) override {
        std::cout << "Received message: " << message << std::endl;
    }
};

int main() {
    ExamplePeer peer1, peer2;

    // Connect peers
    peer1.addPeer(&peer2);
    peer2.addPeer(&peer1);

    // Send a message from peer1 to peer2
    peer1.sendMessage("Hello from peer1!");

    return 0;
}
