#include "database/peer.hpp"
#include "database/world.hpp"
#include "network/packet.hpp"
#include "on/RequestWorldSelectMenu.hpp"
#include "quit_to_exit.hpp"

void quit_to_exit(ENetEvent event, const std::string& header) 
{
    if (_peer[event.peer]->lobby == true) return;
    --worlds[_peer[event.peer]->recent_worlds.back()].visitors;
    peers(ENET_PEER_STATE_CONNECTED, [&](ENetPeer& p) 
    {
        if (not _peer[&p]->recent_worlds.empty() && !_peer[event.peer]->recent_worlds.empty() && _peer[&p]->recent_worlds.back() == _peer[event.peer]->recent_worlds.back() && _peer[&p]->user_id != _peer[event.peer]->user_id) 
        {
            gt_packet(p, false, {
                "OnConsoleMessage", 
                std::format("`5<`w{}`` left, `w{}`` others here>``", _peer[event.peer]->ltoken[0], worlds[_peer[event.peer]->recent_worlds.back()].visitors).c_str()
            });
            gt_packet(p, true, {
                "OnRemove", 
                std::format("netID|{}\npId|\n", _peer[event.peer]->netid).c_str()
            });
        }
    });
    if (worlds[_peer[event.peer]->recent_worlds.back()].visitors <= 0) {
        worlds.erase(_peer[event.peer]->recent_worlds.back());
    }
    _peer[event.peer]->post_enter.unlock();
    _peer[event.peer]->lobby = true;
    _peer[event.peer]->netid = -1; // this will fix any packets being sent outside of world
    OnRequestWorldSelectMenu(event);
}