/*
 * Copyright (c) 2020 Inria
 * Copyright (c) 2016 Georgia Institute of Technology
 * Copyright (c) 2008 Princeton University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __MEM_RUBY_NETWORK_GARNET_0_ADAPTIVEROUTER_HH__
#define __MEM_RUBY_NETWORK_GARNET_0_ADAPTIVEROUTER_HH__

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <random>
#include <queue>

#include "mem/ruby/common/Consumer.hh"
#include "mem/ruby/network/garnet/CommonTypes.hh"
#include "mem/ruby/network/garnet/flitBuffer.hh"

namespace gem5
{

namespace ruby
{

namespace garnet
{

class Router;

class AdaptiveRouter : public Consumer
{

    typedef std::vector<std::vector<int>> Matrix;

  public:
    AdaptiveRouter(Router *router);
    ~AdaptiveRouter() = default;
    void wakeup() {}
    void init(int src);
    void print(std::ostream& out) const {};

    std::vector<int> decode(int node) {
        std::vector<int> index;
        for (int i = 0; i < m_dimension; i++) {
            index.emplace_back(node % m_arys);
            node /= m_arys;
        }
        return index;
    }

    int encode(std::vector<int> index) {
        int node = 0;
        for (int i = 0; i < m_dimension; i++) {
            node = node * m_arys + index[m_dimension - i - 1];
        }
        return node;
    }

    int dirn_to_outport(int dirn);
    int getValue(int outport);
    void setLoc(int x,int y);
    void setCong(int x,int y);
    int getCong(int x,int y);

    int transport(int src, threeD_dirn dirn);
    bool is_allowed_dirn(int src, PortDirection inport_dirn, PortDirection outport_dirn);

    std::pair<int,int> determinePlane(std::vector<int> src, std::vector<int> dst);
    void computeMinimalRouting(int src, int inport_dirn, Matrix &dist);

    std::pair<std::string,int> findPlanarOutport(int src, int dst);
    std::string findBOEOutport(PortDirection inport_dirn, int src, int dst);

    void set_adaptive(RoutingAlgorithm routing_algorithm, AdaptiveAlgorithm adaptive_algorithm)
    {
        m_adaptive = (routing_algorithm == PLANAR_) || (routing_algorithm == BOE_);
        m_routing_algorithm = routing_algorithm;
        m_adaptive_algorithm = adaptive_algorithm;
        not_init = true;
    }

  private:
    const PortDirection dirn_list[6] = {"East0", "West0", "East1", "West1", "East2", "West2"};
    Router *m_router;
    uint32_t num_routers;
    uint32_t m_dimension, m_arys;
    RoutingAlgorithm m_routing_algorithm;
    AdaptiveAlgorithm m_adaptive_algorithm;
    bool not_init,m_adaptive;
    Matrix weight, dist[7];
    std::vector<int> cong,loc;
    std::mt19937 mt;
};

} // namespace garnet
} // namespace ruby
} // namespace gem5

#endif // __MEM_RUBY_NETWORK_GARNET_0_ADAPTIVEROUTER_HH__
