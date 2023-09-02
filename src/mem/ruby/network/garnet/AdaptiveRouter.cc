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


#include "mem/ruby/network/garnet/AdaptiveRouter.hh"

#include "debug/RubyNetwork.hh"
#include "mem/ruby/network/garnet/OutputUnit.hh"
#include "mem/ruby/network/garnet/Router.hh"

namespace gem5
{

namespace ruby
{

namespace garnet
{

AdaptiveRouter::AdaptiveRouter(Router *router)
  : Consumer(router), m_router(router), m_dimension(m_router->get_dimension()),
    m_adaptive(0)
{
}

std::pair<int,int>
AdaptiveRouter::determinePlane(std::vector<int> src, std::vector<int> dst)
{
    int diff = 0, low = -1;
    for (int i = m_dimension - 1; i >= 0; i--) {
        if (src[i] != dst[i]) {
            low = i;
            diff ++;
        }
    }
    if (diff == 1) {
        return std::make_pair(1, low);
    }
    else if (diff == 2 && low < m_dimension - 1 && src[low + 1] != dst[low + 1]) {
        return std::make_pair(2, low);
    }
    else {
        return std::make_pair(0, -1);
    }
}

void
AdaptiveRouter::init()
{
    num_routers = m_router->get_net_ptr()->getNumRouters();
    m_arys = (uint32_t)round(pow(num_routers, 1.0 / m_dimension));
    
    //std::cout << num_routers << m_dimension << m_arys << std::endl;

    prob.clear();
    prob.resize(num_routers);
    std::vector<int> src = decode(m_router->get_id());
    for (int router = 0; router < num_routers; router++) {
        prob[router].clear();
        std::vector<int> dst = decode(router);

        auto cur = determinePlane(src, dst);
        //std::cout << router << std::endl;
        if (cur.first == 2) {
            int pl = 2 * cur.second + (src[cur.second] > dst[cur.second]);
            int ph = 2 * (cur.second + 1) + (src[cur.second + 1] > dst[cur.second + 1]);

            prob[router].resize(2*m_dimension);
            for (int i = 0; i < 2*m_dimension; i++) {
                prob[router][i] = 0;
            }
            prob[router][pl] = prob[router][ph] = (1<<4);
        }
        //std::cout << router << std::endl;
    }

    latency.clear();
    latency.resize(2*m_dimension);
    for (int i = 0; i < 2*m_dimension; i++) {
        latency[i] = 1;
    }
}

std::pair<std::string,int>
AdaptiveRouter::findOutport(int src, int dst) {
    if (m_adaptive) {
        init();
        m_adaptive = 0;
    }

    assert(src != dst);

    //std::cout << 1 << std::endl;

    std::vector<int> src_index = decode(src);
    std::vector<int> dst_index = decode(dst);

    //std::cout << 2 << std::endl;

    int ind = -1;
    for (int i = 0; i < m_dimension; i++) {
        if (src_index[i] != dst_index[i]) {
            ind = i;
            break;
        }
    } 
    
    //std::cout << 3 << std::endl;

    assert(ind != -1);

    assert(src_index[ind] != dst_index[ind]);

    if (ind == m_dimension - 1 || src_index[ind + 1] == dst_index[ind + 1]) {
        //std::cout << 4 << std::endl;
        if (src_index[ind] < dst_index[ind]) {
            return std::make_pair("East" + std::to_string(ind), 2);
        }
        else {
            return std::make_pair("West" + std::to_string(ind), 2);
        }
    }
    else {
        //std::cout << 5 << std::endl;
        std::vector<int> realdst_index = src_index;
        realdst_index[ind] = dst_index[ind];
        realdst_index[ind + 1] = dst_index[ind + 1];
        
        //std::cout << 6 << std::endl;

        int realdst = encode(realdst_index);

        //std::cout << 7 << std::endl;

        assert(realdst >= 0);
        assert(realdst < num_routers);
        int pl = 2 * ind + (src_index[ind] > dst_index[ind]);
        int ph = 2 * (ind + 1) + (src_index[ind + 1] > dst_index[ind + 1]);

        //std::cout << 8 << std::endl;

        assert(!prob[realdst].empty());

        assert(prob[realdst][pl] + prob[realdst][ph] == (1<<5));

        if ((mt() & ((1<<5) - 1)) < prob[realdst][pl]) {
            //std::cout << 9 << std::endl;
            return std::make_pair(((src_index[ind] < dst_index[ind]) ? "East" : "West") + std::to_string(ind), 2);
        }
        else {
            //std::cout << 10 << std::endl;
            bool label = (src_index[ind] > dst_index[ind]);
            return std::make_pair(((src_index[ind + 1] < dst_index[ind + 1]) ? "East" : "West") + std::to_string(ind + 1), label);
        }
    }
}

void
AdaptiveRouter::resetStats() {
}

} // namespace garnet
} // namespace ruby
} // namespace gem5
