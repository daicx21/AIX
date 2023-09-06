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
    m_adaptive(false),not_init(true)
{
}

int 
AdaptiveRouter::dirn_to_outport(int dirn)
{
    return m_router->ComputeOutportDirn2Idx(dirn_list[dirn]);
}

int 
AdaptiveRouter::getValue(int outport)
{
    if (not_init) {
        init(m_router->get_id());
        not_init = 0;
    }
    if (m_adaptive_algorithm==RANDOM_) return mt();
    else if (m_adaptive_algorithm==LOCAL_) return loc[outport];
    return cong[outport];
}

void
AdaptiveRouter::setLoc(int x,int y)
{
    if (not_init) {
        init(m_router->get_id());
        not_init = 0;
    }
    loc[x]=y;
}

void
AdaptiveRouter::setCong(int x,int y)
{
    if (not_init) {
        init(m_router->get_id());
        not_init = 0;
    }
    cong[x]=y;
}

int
AdaptiveRouter::getCong(int x,int y)
{
    if (not_init) {
        init(m_router->get_id());
        not_init = 0;
    }
    return weight[x][y]*cong[y];
}

int
AdaptiveRouter::transport(int src, threeD_dirn dirn)
{
    std::vector<int> index = decode(src);
    switch (dirn) {
        case East0_:
            index[0] ++; break;
        case West0_:
            index[0] --; break;
        case East1_:
            index[1] ++; break;
        case West1_:
            index[1] --; break;
        case East2_:
            index[2] ++; break;
        case West2_:
            index[2] --; break;
    }

    for (int i = 0; i < m_dimension; i++) {
        if (index[i] < 0 || index[i] >= m_arys) {
            return -1;
        }
    }

    return encode(index);
}

bool
AdaptiveRouter::is_allowed_dirn(int src, PortDirection inport_dirn, PortDirection outport_dirn)
{
    assert(outport_dirn != "Local");

    if (inport_dirn == "Local") {
        return true;
    }

    int in_radix = std::stoi(inport_dirn.substr(4));
    int out_radix = std::stoi(outport_dirn.substr(4));

    std::vector<int> index = decode(src);

    if (in_radix == out_radix && inport_dirn == outport_dirn) {
        return false;
    }

    if (index[2] & 1) {
        if (index[0] & 1) {
            if (in_radix == 1 && outport_dirn == "West0") {
                return false;
            }
        }
        else {
            if (out_radix == 1 && inport_dirn == "West0") {
                return false;
            }
        }

        if ((in_radix != 2 && outport_dirn == "West2") || (out_radix != 2 && inport_dirn == "East2")) {
            return false;
        }
    }
    else {
        if (index[1] & 1) {
            if (in_radix == 0 && outport_dirn == "East1") {
                return false;
            }
        }
        else {
            if (out_radix == 0 && inport_dirn == "East1") {
                return false;
            }
        }

        if ((in_radix != 2 && outport_dirn == "East2") || (out_radix != 2 && inport_dirn == "West2")) {
            return false;
        }
    }

    return true;
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
AdaptiveRouter::init(int src)
{
    num_routers = m_router->get_net_ptr()->getNumRouters();
    m_arys = (uint32_t)round(pow(num_routers, 1.0 / m_dimension));

    int n=m_router->get_num_inports(),m=m_router->get_num_outports();

    loc.clear();
    loc.resize(m);
    for (int i = 0; i < m; i++) {
        loc[i] = 0;
    }

    if (m_adaptive_algorithm == RCA_)
    {
        cong.clear();
        cong.resize(m);
        for (int i = 0; i < m; i++) cong[i] = 0;
        weight.clear();
        weight.resize(n);
        for (int i = 0; i < n; i++)
        {
            weight[i].resize(m);
            for (int j = 0; j < m; j++)
            {
                weight[i][j] = 0;
            }
        }
        std::vector<int> src=decode(m_router->get_id());
        for (int i = 0; i < m_dimension; i++)
        {
            PortDirection Inport1="West"+std::to_string(i),Inport2="East"+std::to_string(i);
            int id1 = m_router->ComputeInportDirn2Idx(Inport1), id2 = m_router->ComputeInportDirn2Idx(Inport2);
            for (int j = 0; j < m_dimension; j++) if (j != i)
            {
                PortDirection Outport1="West"+std::to_string(j),Outport2="East"+std::to_string(j);
                int id3=m_router->ComputeOutportDirn2Idx(Outport1),id4=m_router->ComputeOutportDirn2Idx(Outport2);
                if (src[i]>0&&src[j]>0) weight[id1][id3]=1;
                if (src[i]>0&&src[j]<m_arys-1) weight[id1][id4]=1;
                if (src[i]<m_arys-1&&src[j]>0) weight[id2][id3]=1;
                if (src[i]<m_arys-1&&src[j]<m_arys-1) weight[id2][id4]=1;
            }
            if (src[i]>0&&src[i]<m_arys-1)
            {
                weight[id1][m_router->ComputeOutportDirn2Idx(Inport2)]=2*(m_dimension-1);
                weight[id2][m_router->ComputeOutportDirn2Idx(Inport1)]=2*(m_dimension-1);
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        computeMinimalRouting(src, i, dist[i]);
    }
}

void 
AdaptiveRouter::computeMinimalRouting(int src, int inport_dirn, Matrix &dist) {
    std::queue<std::pair<int,int>> queue;

    dist.clear();
    dist.resize(num_routers);

    for (int router = 0; router < num_routers; router++) {
        dist[router].clear();
        dist[router].resize(6);

        for (int i = 0; i < 6; i++) {
            dist[router][i] = INFINITE_;
        }
    }

    for (int i = 0; i < 6; i++) {
        dist[src][i] = 0;
    }
    
    int dst = transport(src, (threeD_dirn) inport_dirn);
    if (dst != -1) {
        dist[dst][inport_dirn^1] = 1;
        queue.push(std::make_pair(dst, inport_dirn^1));
    }

    while (!queue.empty()) {
        std::pair<int,int> fr = queue.front();
        queue.pop();

        int cur = fr.first, inport = fr.second;

        for (int i = 0; i < 6; i++) {
            if (is_allowed_dirn(cur, dirn_list[inport], dirn_list[i])) {
                int dst = transport(cur, (threeD_dirn) i);
                if (dst != -1 && dist[dst][i^1] == INFINITE_) {
                    dist[dst][i^1] = dist[cur][inport] + 1;
                    queue.push(std::make_pair(dst, i^1));
                }
            }
        } 
    }
}

std::pair<std::string,int>
AdaptiveRouter::findPlanarOutport(int src, int dst) {
    if (not_init) {
        init(src);
        not_init = 0;
    }

    assert(src != dst);

    //std::cout << 1 << std::endl;

    std::vector<int> src_index = decode(src);
    std::vector<int> dst_index = decode(dst);

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

        std::string str1=((src_index[ind] < dst_index[ind]) ? "East" : "West") + std::to_string(ind);
        std::string str2=((src_index[ind + 1] < dst_index[ind + 1]) ? "East" : "West") + std::to_string(ind + 1);

        int pl = m_router->ComputeOutportDirn2Idx(str1);
        int ph = m_router->ComputeOutportDirn2Idx(str2);

        bool label=(src_index[ind]>dst_index[ind]);

        if (m_router->getOutputUnit(pl)->gao(2)>m_router->getOutputUnit(ph)->gao(label)) return std::make_pair(str1,2);
        else return std::make_pair(str2,label);

        //std::cout << 8 << std::endl;

        if (getValue(pl)<getValue(ph)) {
            //std::cout << 9 << std::endl;
            return std::make_pair(str1, 2);
        }
        else {
            //std::cout << 10 << std::endl;
            bool label = (src_index[ind] > dst_index[ind]);
            return std::make_pair(str2, label);
        }
    }
}

std::string
AdaptiveRouter::findBOEOutport(PortDirection inport_dirn, int src, int dst) {
    if (not_init) {
        init(src);
        not_init = 0;
    }

    std::vector<int> candidates, min_dist;
    int total_min = INFINITE_;

    min_dist.resize(6);
    for (int i = 0; i < 6; i++) {
        min_dist[i] = INFINITE_;

        if (is_allowed_dirn(src, inport_dirn, dirn_list[i])) {
            int next_r = transport(src, (threeD_dirn) i);
            if (next_r != -1) {
                for (int j = 0; j < 6; j++) {
                    if (dist[i][dst][j] < min_dist[i]) {
                        min_dist[i] = dist[i][dst][j];
                    }
                }
            }
        }

        if (min_dist[i] < total_min) {
            total_min = min_dist[i];
        }
    }

    if (total_min == INFINITE_)
    {
        std::cout << src << " " << dst << " " << inport_dirn << std::endl;
        std::cout << dist[5][47][4] << std::endl;
        std::cout << dist[5][46][0] << std::endl;
        std::cout << dist[5][45][0] << std::endl;
        std::cout << dist[5][44][0] << std::endl;
        std::cout << dist[5][40][2] << std::endl;
        std::cout << dist[5][36][2] << std::endl;
        std::cout << dist[5][32][2] << std::endl;
        std::cout << dist[5][16][4] << std::endl;
        assert(0);
    }
    
    for (int i = 0; i < 6; i++) {
        if (min_dist[i] == total_min) {
            candidates.emplace_back(i);
        }
    }

    assert(!candidates.empty());
    int pl = dirn_to_outport(candidates[0]), min_value = getValue(pl);

    for (int i = 1; i < candidates.size(); i++) {
        int pn = dirn_to_outport(candidates[i]);
        int val = getValue(pn);

        if (val < min_value) {
            min_value = val;
            pl = pn;
        }
    }

    return m_router->ComputeOutportIdx2Dirn(pl);
}

} // namespace garnet
} // namespace ruby
} // namespace gem5
